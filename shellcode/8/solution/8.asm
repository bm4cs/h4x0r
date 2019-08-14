section .text
	global _start

_start:
	push ecx
	push ebx
	mov eax,0x8048fa6
	call eax
	add esp,8

