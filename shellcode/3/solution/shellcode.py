#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8888))

#shellcode = "\x90\x90\x90\x90\x31\xd2\x49\x74\x06\xd1\xe3\xd1\xd2\xeb\xf7"

#\x31\xd2\x49\x74\x06\xd1\xe3\xd1\xd2\xeb\xf7


shellcode = "\x89\xd8\x83\xf9\x01\x74\x05\xf7\xe3\x49\xeb\xf6"

print("Sending shellcode (" + str(len(shellcode)) + " bytes)")
s.send(shellcode)
