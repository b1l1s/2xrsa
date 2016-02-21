.global svc_sleepThread
svc_sleepThread:
	svc 0x0A
	bx  lr

.global svc_connectToPort
svc_connectToPort:
	str r0, [sp, #-0x4]!
	svc 0x2D
	ldr r3, [sp], #4
	str r1, [r3]
	bx  lr

.global svc_sendSyncRequest
svc_sendSyncRequest:
	svc 0x32
	bx lr

.global svc_getProcessId
svc_getProcessId:
	str r0, [sp, #-0x4]!
	svc 0x35
	ldr r3, [sp], #4
	str r1, [r3]
	bx  lr

