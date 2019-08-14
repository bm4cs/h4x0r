section .text
	global _start

_start:
	mov eax,ebx

foo:
	cmp ecx,1
	je ready
	mul ebx
	dec ecx
	jmp foo
	
ready:
	
