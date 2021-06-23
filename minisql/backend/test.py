import subprocess, io
import os

if __name__ == "__main__":
    proc = os.popen('test 123')
    print(proc.read())