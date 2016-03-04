import subprocess

args = ['Exec/KVEngine.exe', '-nodraw', '-auto', '-auto_connect', 'login=Guest']
for x in range(0, 1):
    p = subprocess.Popen(args, cwd = './Exec')
#p.kill ()