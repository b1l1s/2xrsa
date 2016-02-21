#include "IFile.11.h"
#include "pointers.11.h"

const IFile_Open_f IFile_Open = (IFile_Open_f) IFILE_OPEN_ADR;
const IFile_Close_f IFile_Close = (IFile_Close_f) IFILE_CLOSE_ADR;
const IFile_Read_f IFile_Read = (IFile_Read_f) IFILE_READ_ADR;
const IFile_Write_f IFile_Write = (IFile_Write_f) IFILE_WRITE_ADR;

