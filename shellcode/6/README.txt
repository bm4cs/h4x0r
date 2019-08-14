Write some shellcode that checks if the string pointed to by ecx occurs in the string pointed to by ebx.
The function should return the index at which the substring occurs or -1 if it doesn't occur.

e.g.
ebx="hello world", ecx="hello" => returns 0
ebx="hello world", ecx="l"     => returns 2
ebx="hello world", ecx="world" => returns 6
ebx="hello world", ecx="nope"  => returns -1
