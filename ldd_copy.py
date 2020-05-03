#!/usr/bin/python3

import sys
import shutil
import subprocess

if len(sys.argv) < 2:
    exit(1)

lddProc = subprocess.run(['ldd', sys.argv[1]], stdout=subprocess.PIPE)
outputLines = lddProc.stdout.split(b'\n')

res = []
for l in outputLines:
    entry = l.strip().split(b' ')
    if b'=>' in entry:
        res.append(entry[2])

for f in res:
    print(f.decode("utf-8"))
    shutil.copyfile(f, f[f.rfind(b'/')+1:])
