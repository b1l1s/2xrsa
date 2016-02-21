#ifndef IFILE_11_H_
#define IFILE_11_H_

#include "types.h"
#include <assert.h>

typedef struct {
	s32 s;
	u32 pos;
	u32 size;
	u32 unk[5];
} IFILE;

static_assert(sizeof(wchar_t) == sizeof(short), "Assumption about length of wchar_t is wrong!");
typedef int (*IFile_Open_f)(IFILE *f, const wchar_t *path, int flags);
typedef int (*IFile_Close_f)(IFILE *f);
typedef int (*IFile_Read_f)(IFILE *f, unsigned int *read, void *buffer, unsigned int size);
typedef int (*IFile_Write_f)(IFILE *f, uint32_t *written, void *src, uint32_t size);

const IFile_Open_f IFile_Open;
const IFile_Close_f IFile_Close;
const IFile_Read_f IFile_Read;
const IFile_Write_f IFile_Write;

#endif//IFILE_11_H_

