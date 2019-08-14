section .text
	global _start

_start:
	push esi
	push edi
	push ecx
	push edx
	pop esi
	pop edi
	pop ecx
	pop edx
