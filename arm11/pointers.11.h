#ifndef POINTERS_11_H_
#define POINTERS_11_H_

#if !defined (FW_MAJOR) || !defined(FW_MINOR) || ((FW_MAJOR == 21) && FW_MINOR == 1)

#define GX_SETTEXTURECOPY_ADR 0x002c565c
#define GSPGPU_FLUSHDATACACHE_ADR 0x00345ec8

#define GSP_HANDLE_ADR 0x003B9438

#define IFILE_OPEN_ADR 0x0025BC00
#define IFILE_CLOSE_ADR 0x0025BD20 // From testing, this seems wrong, FIXME. Program hangs upon execution
#define IFILE_READ_ADR 0x002FA864
#define IFILE_WRITE_ADR 0x00310190

#define PS_VERIFYRSASHA256_BUFFER_SIZE 0xD9B8
#define UNKNOWN_POINTER_USED_BY_RSA_EXPLOIT 0x080C2340

#define BUFFER_ADR ((void*)0x18400000) // FIXME is this 2.1 specific?
#define BUFFER_ADR_SIZE 0x20000

//FIXME we're missing Framebuffer pointers
//0x1F48F000
//0x1F4C7800

#endif //FW 2.1 or default

#if defined(FW_MAJOR) && (FW_MAJOR > 2)

#define PS_VERIFYRSASHA256_BUFFER_SIZE 0x7440

#endif// FW > 2

// The function of the following values is unknown
//*ptr++ = 0x080C3EE0; //4.5
//*ptr++ = 0x080C4420; //3.x
//*ptr++ = 0x080C2520; //2.2
//*ptr++ = 0x080B9620; //1.1
//*ptr++ = 0x080B95C0; //1.0

#endif//POINTERS_11_H_

