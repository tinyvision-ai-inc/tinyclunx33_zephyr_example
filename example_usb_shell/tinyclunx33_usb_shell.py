from pexpect import replwrap, fdpexpect
from argparse import ArgumentParser
import sys, os

class TinyClunx33:
    def __init__(self, serial_port, verbose):
        self.verbose = verbose

        # Connect to the serial port
        tty = os.open(serial_port, os.O_RDWR)
        pipe = fdpexpect.fdspawn(tty, timeout=5)

        # Make the prompt appear
        pipe.sendline('')

        # Use the REPL wrapper built to facilitate the use of a REPL
        esc = '[^m]*m'
        self.repl = replwrap.REPLWrapper(pipe, '^' + esc + 'uart:~\\$ ' + esc, None)

    def log(self, msg):
        if self.verbose:
            print(msg)

    def command(self, text, timeout=5):
        self.log(f'uart:~$ {text}')
        self.pipe.sendline(text)
        self.pipe.expect('', timeout=timeout)

    def close():
        self.pipe.close()

def main():
    ap = ArgumentParser(prog='tinyclunx33_usb_shell',
        description='Access and control of the tinyCLUNX33 shell over USB')
    ap.add_argument('-v', '--verbose', action='store_true', help='debug logging')
    ap.add_argument('serial_port', help='COM# on Windows, path in /dev/ on others')
    args = ap.parse_args()

    clunx = TinyClunx33(args.serial_port, args.verbose)
    clunx.command('help', timeout=10)

if __name__ == "__main__":
    main()
