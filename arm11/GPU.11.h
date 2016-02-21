#ifndef GPU_11_H_
#define GPU_11_H_

#include "types.h"

//GPU stuffs
typedef int (*GX_SetTextureCopy_f)(void *input_buffer, void *output_buffer, u32 size, int in_x, int in_y, int out_x, int out_y, int flags);
typedef int (*GSPGPU_FlushDataCache_f)(void *address, u32 length);

extern const GX_SetTextureCopy_f GX_SetTextureCopy;
extern const GSPGPU_FlushDataCache_f GSPGPU_FlushDataCache;

Result _GSPGPU_ReadHWRegs(uint32_t* handle, u32 regAddr, u32* data, u8 size);

extern u32* const gspHandle;


#endif//GPU_11_H_

