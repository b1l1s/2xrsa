#include "3ds_utils.11.h"
#include "lib.11.h"
#include "svc.11.h"
#include "pointers.11.h"

u32 *work_buffer = BUFFER_ADR;

Result srvRegisterProcess(Handle *handle, u32 procid, u32 count, const void *serviceaccesscontrol)
{       
	Result rc = 0;

	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04030082; // SRVPM:RegisterProcess for <7.x       
	cmdbuf[1] = procid;     
	cmdbuf[2] = count;      
	cmdbuf[3] = (count << 16) | 2;  
	cmdbuf[4] = (u32)serviceaccesscontrol;

	if((rc = svc_sendSyncRequest(*handle))) return rc;

	return (Result)cmdbuf[1];       
}

Result srvUnregisterProcess(Handle *handle, u32 procid)
{       
	Result rc = 0;

	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x04040040; // SRVPM:UnregisterProcess for <7.x     
	cmdbuf[1] = procid;

	if((rc = svc_sendSyncRequest(*handle))) return rc;

	return (Result)cmdbuf[1];       
}

Result srv_RegisterClient(Handle* handleptr)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x10002; //request header code
	cmdbuf[1]=0x20;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr))) return ret;

	return (Result)cmdbuf[1];
}

Result srv_getServiceHandle(Handle* handleptr, Handle* out, char* server)
{
	u8 l = _strlen(server);
	if(!out || !server || l>8) return -1;

	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0] = 0x50100; //request header code
	_strcpy((char*)&cmdbuf[1], server);
	cmdbuf[3] = l;
	cmdbuf[4] = 0x0;

	Result ret=0;
	if((ret=svc_sendSyncRequest(*handleptr))) return ret;

	*out=cmdbuf[3];

	return (Result)cmdbuf[1];
}

//Only need the pointers to these, determined by linker script
extern const u32 arm9_stage1;
extern const u32 arm9_stage1_end;

Result PS_VerifyRsaSha256(Handle *handle)
{       
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u8 *cmdbuf8 = (u8*)cmdbuf;

	const u32 bufSize = PS_VERIFYRSASHA256_BUFFER_SIZE;

	u32* buffer = work_buffer;
	_memset(buffer, 0, bufSize);

	buffer[0x28/sizeof(u32)] = 0x820002;
	buffer[0x2C/sizeof(u32)] = (u32)(buffer + 0x80/sizeof(u32));
	buffer[0x30/sizeof(u32)] = (bufSize << 4) | 0xA;
	buffer[0x34/sizeof(u32)] = (u32)(buffer + 0x380/sizeof(u32));
	buffer[0x280/sizeof(u32)] = bufSize<<3; //RSA bit-size, for the signature.

	u32 *ptr = buffer+0x380/sizeof(u32);
	const u32 *src = &arm9_stage1;
	const u32 size = (u32)((const u8*)&arm9_stage1_end-(const u8*)&arm9_stage1);
	const u32 nopsled = 0x1000; // FIXME do we need such a  large NOP sled?

	for(u32 i = 0; i < nopsled/sizeof(u32); i++)
	{       
		*ptr++ = 0xE1A00000;
	}

	for(u32 i = 0; i < size; i+=4)
	{       
		*ptr++ = *src++;
	}

	for(u32 i=0; i < bufSize-size-nopsled; i+=4)
	{       
		//FIXME What are these pointers for?
		//*ptr++ = 0x080C3EE0; //4.5
		//*ptr++ = 0x080C4420; //3.x
		//*ptr++ = 0x080C2520; //2.2
		*ptr++ = 0x080C2340; //2.1
		//*ptr++ = 0x080B9620; //1.1
		//*ptr++ = 0x080B95C0; //1.0
	}

	for(u32 i = 0; i < 0x80u; i++)
	{       
		cmdbuf8[i] = ((u8*)(buffer))[i];
	}

	cmdbuf[0] = 0x00020244;

	if((ret = svc_sendSyncRequest(*handle))!=0) return ret;

	//We do not expect to return from the syncRequest
	return (Result)cmdbuf[1];
} 

