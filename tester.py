from os import path, name, listdir
from subprocess import run, Popen, check_call, PIPE, STDOUT
from math import ceil
from filecmp import cmp

class Log():
    reset = '\033[0m'
    bold = '\033[1m'

    header = '\033[95m'
    ok_blue = bold + '\033[94m[+]' + reset
    info = bold + '\033[32m[*]' + reset
    warning = bold + '\033[93m[!]' + reset
    fail = bold + '\033[91m[!]' + reset + ' Error:'	

    uline = '\033[4m'

    progress_bar_char = u"\u2501"

    @classmethod 
    def print_info(cls, message: str, sep: str = ' ', end: str = '\n') -> None:
        print(Log.info + ' ' +message, sep=sep, end=end)

    @classmethod
    def print_ok(cls, message: str, sep: str = ' ', end: str = '\n') -> None:
        print(Log.ok_blue + ' ' + message, sep=sep, end=end)

    @classmethod
    def print_err(cls, message: str, sep: str = ' ', end: str = '\n') -> None:
        print(Log.fail + ' ' + message, sep=sep, end=end)

    @classmethod
    def print_progress_bar(cls, progress: int, max: int, length: int, end: str='\r'):
        adj_index = ceil((progress+1)/max * length)
        Log.print_info('', end='')
        print(Log.progress_bar_char*(adj_index) + ' '*(length-adj_index), end=' ')
        print(f'{progress+1} of {max}', end=end)


COMPILE_COMMAND = 'gcc -DEVAL -Wall -Werror -std=gnu11 -O2 -pipe -static -s main.c -lm'
MAX_TEST = 111

def main() -> None:
    try:
        if(name == 'nt'):
            check_call(COMPILE_COMMAND)
        else:
            run(COMPILE_COMMAND.split(' '), check=True)
        Log.print_ok('Compiled successfully')
    except:
        Log.print_err('Failed to compile')
        return
    
    Log.print_info('Testing')
    Log.print_progress_bar(0, MAX_TEST, 40)
    try:
        for i in range(1, MAX_TEST):
            with open('output.txt', 'w') as of:
                with open(path.join('.', 'Test', f'open_{i}.txt'), 'r') as inf:
                    if(name == 'nt'):
                        check_call(r'.\a.exe', stdin=inf, stdout=of, stderr=PIPE)
                    else:
                        run(['chmod', '+x', 'a.out'], check=True)
                        proc = run(['./a.out'], shell=True, stdout=of, stdin=inf, stderr=PIPE)
                        print('\n' + proc.stderr.decode('utf-8'), end='\n')

            Log.print_progress_bar(i, MAX_TEST, 40)

            if(not cmp(path.join('.', 'Test', f'open_{i}.output.txt'), 'output.txt')):
                Log.print_err(f'Output mismatch at test {i}' + 30*' ')
                return
        print()
        Log.print_ok('Done')
    except Exception as e:
        Log.print_err(f'Runtime error: {e}')


if __name__ == '__main__':
    main()
