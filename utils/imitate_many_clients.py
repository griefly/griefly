import subprocess
import sys
import time
import random
from typing import List

TOTAL_BOTS = 5
MAX_BOTS = 20


def get_args() -> List[str]:
    if sys.platform.startswith("linux"):
        exe = "./KVEngine"
    else:
        exe = "exec/KVEngine.exe"
    return [exe, 'ip=grief.ly', '-nodraw', '-auto', '-auto_connect', 'login=Guest']


def get_bots_count() -> int:
    count = TOTAL_BOTS

    if len(sys.argv) == 2:
        try:
            count = int(sys.argv[1])
        except ValueError:
            raise Exception("Invalid bots count param")

    if count > MAX_BOTS:
        raise Exception("Too much bots")

    return count


def create_process() -> subprocess.Popen:
    return subprocess.Popen(get_args(), cwd='./exec')


def populate_processes(count: int) -> List[subprocess.Popen]:
    processes = []
    for x in range(0, count):
        processes.append(create_process())

    print("Amount of bots: {}".format(count))
    return processes[1:]  # Have no idea why did you slice this list but I decided to left it unchanged.


def main():
    processes = populate_processes(get_bots_count())

    while True:
        time.sleep(5)
        new_processes = []
        for process in processes:
            if random.randint(0, 10) == 0:
                process.kill()
                time.sleep(1)
                process.communicate()
                new_processes.append(create_process())
            else:
                new_processes.append(process)
        processes = new_processes

if __name__ == "__main__":
    main()
