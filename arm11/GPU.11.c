#include "GPU.11.h"
#include "pointers.11.h"
#include "3ds_utils.11.h"
#include "svc.11.h"

const GX_SetTextureCopy_f GX_SetTextureCopy = (GX_SetTextureCopy_f) GX_SETTEXTURECOPY_ADR;
const GSPGPU_FlushDataCache_f GSPGPU_FlushDataCache = (GSPGPU_FlushDataCache_f) GSPGPU_FLUSHDATACACHE_ADR;
u32* const gspHandle = (u32*)GSP_HANDLE_ADR;

Result _GSPGPU_ReadHWRegs(uint32_t* handle, u32 regAddr, u32* data, u8 size)
{
	if(size>0x80 || !data) return -1;

	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0] = 0x00040080; //request header code
	cmdbuf[1] = regAddr;
	cmdbuf[2] = size;
	cmdbuf[0x40] = ((u32)size<<14) | 2u;
	cmdbuf[0x40+1] = (u32)data;

	Result ret = 0;
	if((ret=svc_sendSyncRequest(*handle)))return ret;

	return (Result)cmdbuf[1];
}

