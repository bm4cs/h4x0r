#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8888))

shellcode = "\x90\x90\x90\x90\x01\xcb\x89\xd8"

print("Sending shellcode (" + str(len(shellcode)) + " bytes)")
s.send(shellcode)
