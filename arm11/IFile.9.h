#ifndef IFILE_9_H_
#define IFILE_9_H_

#include <inttypes.h>

typedef struct IFILE
{
	uint32_t unk[4];
	uint64_t fptr;
	uint64_t size;
} IFILE;

typedef int(*IFile_Open9_f)(IFILE* file, const wchar_t* path, uint32_t flags);
typedef void(*IFile_Write9_f)(IFILE* file, uint32_t* written, const void* buffer, uint32_t size, uint32_t blockSize);

extern const IFile_Open9_f IFile_Open9;
extern const IFile_Write9_f IFile_Write9;

#endif//IFILE_9_H_

