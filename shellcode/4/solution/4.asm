section .text
	global _start

_start:
	mov eax,0
	jmp end

begin:
	inc eax
	inc ebx
	
end:
	cmp byte[ebx],0
	jnz begin
	ret
