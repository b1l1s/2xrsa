About
==============
Executes arm9 payload in 2.1 firm through spider 2.1

Usage
==============
1. Prepare arm9 payload by using the framebuffer addresses stored in(CakeHax style):
   ```
   top left -> 0x23FFFE00
   top right -> 0x23FFFE04
   bottom -> 0x23FFFE08
   ```
   or using the fixed spider framebuffer addresses:
   ```
   top left 1 -> 0x181E6000
   top left 2 -> 0x18273000
   bottom 1 -> 0x1848F000
   bottom 2 -> 0x184C7800
   ```
2. The arm9 payload will be loaded to 0x23F000000 so modify your linker script if required
3. Name payload as arm9.bin, copy to root as SD card along with arm11.bin(from compiling 2xrsa) and visit http://dukesrg.github.io/2xrsa.html?arm11.bin to execute.

Credits
==============
- Normmatt for the rsa exploit
- dukesrg for rop3ds and hosting
- Gelex for debugging and testing
- dark_samus_, others for testing
- anon contrib for initial version of stager