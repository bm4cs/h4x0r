#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8888))

#shellcode = "\x8b\x03\xeb\x01\x43\x80\x3b\x00\x75\xfa\x39\xc3\x7f\x0c\x8a\x08\x8a\x2b\x88\x28\x88\x0b\x4b\x40\xeb\xf0"
shellcode = "\x89\xd8\xeb\x01\x43\x80\x3b\x00\x75\xfa\x39\xd8\x7f\x0c\x8a\x08\x8a\x2b\x88\x28\x88\x0b\x4b\x40\xeb\xf0"

print("Sending shellcode (" + str(len(shellcode)) + " bytes)")
s.send(shellcode)