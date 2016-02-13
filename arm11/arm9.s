.section ".text"
.arm
.align 4
.global arm9_main
.global arm9_main_end

arm9_main:
	dump_arm9:
	ldr sp,=0x22140000
		
		@Disable IRQ
		mrs r0, cpsr
		orr r0, #(1<<7)|(1<<6)
		msr cpsr_c, r0
		
		@adr r0, dump_regions
		@.word 0xEF00007B ;svc 0x7B
		
		b StartLoader

crap:		
		ldr r11, =0x01010101 @val
		
		mov r10, #3
		
				dump_fcram:
				@FCRAM
				adr r0, this
				adr r1, filename9_1 @filename
				ldr r2, =0x6 @openflags
				
				mov r3, #0
				str r3, [r0,#0]
				str r3, [r0,#4]
				str r3, [r0,#16]
				str r3, [r0,#20]
				str r3, [r0,#24]
				str r3, [r0,#28]
				
				@ldr r5, =0x0805FAB5 @fopen 1.0 (thumb)
				@ldr r5, =0x08055545 @fopen 1.1 (thumb)
				ldr r5, =0x08065AA5 @fopen 2.1 (thumb)
				@ldr r5, =0x08065C51 @fopen 2.2 (thumb)
				blx r5

				@ldr r2, =0x01FF8000
				@mvn r1, #0
				@str r1, [r2,#0x108]
				
				@MRC p15, 0, R2,c9,c1, 0
				@sub r2, #0xC
			
				adr r0, this
				sub r1, sp, #0x750 @_this
				ldr r2, =0x20000000 @ src
				ldr r3, =0x08000000 @ size
				mov r4, #1 @Flush?
				str r4, [sp] @Flush?

				@ldr r5, =0x080628C3 @fwrite 1.0 (thumb)
				@ldr r5, =0x08055599 @fwrite 1.1 (thumb)
				ldr r5, =0x08068719 @fwrite 2.1 (thumb)
				@ldr r5, =0x080688CD @fwrite 2.2 (thumb)
				blx r5
				
				sub r10, #1
				cmp r10, #0
				bhi dump_fcram
		
clear_screen:
				ldr r0, =0x20000000
				mov r11, r11, lsl #2
				add r2, r0, #0x8000000
				
				cpl:
					str r11, [r0], #4

					cmp r0, r2
					blt cpl
					
				b clear_screen
				
StartLoader:
				ldr r0, =0x22000000
				ldr r2, =0xDEADBABE
				ldr r3, =0xBABEDADA-1
				add r3, #1
				
				check:
					ldr r1, [r0], #4
					cmp r1, r2 @0xDEADBABE
					bne check
					ldr r1, [r0], #4
					cmp r1, r3 @0xBABEDADA
					bne check
					
				@r0 points to the memory I need to copy
				ldmia r0!, {r1-r9}
				ldr r10, =0x23FFFE00
				stmia r10!, {r1-r9}
				
				mov r10, #5
				@load loader
load_launcher:
				adr r0, this
				adr r1, filename9 @filename
				ldr r2, =0x1 @openflags
				
				mov r3, #0
				str r3, [r0,#0]
				str r3, [r0,#4]
				str r3, [r0,#16]
				str r3, [r0,#20]
				str r3, [r0,#24]
				str r3, [r0,#28]
				
				@ldr r5, =0x0805FAB5 @fopen 1.0 (thumb)
				@ldr r5, =0x08055545 @fopen 1.1 (thumb)
				ldr r5, =0x08065AA5 @fopen 2.1 (thumb)
				@ldr r5, =0x08065C51 @fopen 2.2 (thumb)
				blx r5
				
				adr r0, this
				sub r1, sp, #0x750 @read_size
				ldr r2, =0x23F00000 @ dest
				ldr r3, =0x00003000 @ size

				@ldr r5, =0x08053E2D @fread 1.0 (thumb)
				@ldr r5, =0x08053E2D @fread 1.1 (thumb)
				ldr r5, =0x0805825D @fread 2.1 (thumb)
				@ldr r5, =0x0805830D @fread 2.2 (thumb)
				blx r5
				
				sub r10, #1
				cmp r10, #0
				bhi load_launcher
				
				@b crap
				
				@jump to my loader start point
				adr r0, boot_launcher
				.word 0xEF00007B ;svc 0x7B

boot_launcher:
	LDR     R3, =0x33333333 @R/W
	MCR     p15, 0, R3,c5,c0, 2
	MCR     p15, 0, R3,c5,c0, 3
	ldr r0, =0x23F00000
	bx r0

	.align 4
	filename9:
	.string16 "sdmc:/load.bin"
	
	.align 4
	filename9_1:
	.string16 "sdmc:/fcram.bin"
		
		.align 4
	this:
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0

.align 4
.pool

arm9_main_end:
.word 0
	