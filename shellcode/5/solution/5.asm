
# work in progress as of 2019-08-14

section .text
	global _start

_start:
	mov eax,ebx
	jmp endcount

loop:
	inc ebx
	
endcount:
	cmp byte[ebx],0
	jne loop

reverse:
	cmp eax,ebx
	jg finish
	mov cl,[eax]
	mov ch,[ebx]
	mov [eax],ch
	mov [ebx],cl
	dec ebx
	inc eax
	jmp reverse

finish:
	

