from pexpect import replwrap, popen_spawn
from argparse import ArgumentParser
import sys, os, re

DEFAULT_SERIAL_PORT = '/dev/ttyACM0'
DEFAULT_I2C_DEVICE = 'i2c@e0005000'
DEFAULT_I2C_ADDRESS = '0x10'

class TinyClunx33:
    def __init__(self, serial_port, verbose=False, timeout=3):
        self.verbose = verbose

        self.proc = popen_spawn.PopenSpawn(f'picocom -q {serial_port}',
            timeout=timeout)
        self.proc.echo = False
        if verbose:
            self.proc.logfile = sys.stdout.buffer
        esc = '\x1b\[[^A-Za-z]*[A-Za-z]'
        self.prompt = f'{esc}uart:~[$] {esc}'

        # Make the prompt appear, skip previous command if any
        self.proc.sendline('')
        self.proc.expect(self.prompt)

        # Avoid the text being sent back at us
        self.proc.sendline('shell echo off')
        self.proc.expect(self.prompt)

    def run_command(self, text, timeout=3):
        self.proc.sendline(text)
        self.proc.expect(self.prompt, timeout=timeout)
        return self.proc.before.decode('utf8')

    def i2c_scan(self, dev=DEFAULT_I2C_DEVICE, timeout=3):
        return self.run_command(f'i2c scan {dev}')

    def i2c_reg8_read(self, addr, reg, size=1, dev=DEFAULT_I2C_DEVICE, timeout=3):
        """
        Read 'size' bytes from the 8-bit register 'reg' of I2C
        peripheral at address 'addr' on I2C bus 'dev' and return the bytearray
        of values read as a result
        """
        if size > 16:
            raise ValueError("max size is 16")
        hexdump = self.run_command(f'i2c read {dev} {addr:x} {reg:x} {size:x}')
        return bytes(int(s, 16) for s in re.split('[ |.\r\n]+', hexdump)[2:-1])

    def i2c_reg8_write(self, addr, reg, byte, dev=DEFAULT_I2C_DEVICE, timeout=3):
        """
        Write 'byte' to the 8-bit register 'reg' of I2C
        peripheral at address 'addr' on I2C bus 'dev'
        """
        self.run_command(f'i2c write {dev} {addr:x} {reg:x} {byte:x}')

def main():
    ap = ArgumentParser(prog='tinyclunx33_usb_shell',
        description='Access and control of the tinyCLUNX33 shell over USB')
    ap.add_argument('-v', '--verbose', action='store_true', help='debug logging')
    ap.add_argument('-p', '--port', help='serial port to access the Zephyr shell', default=DEFAULT_SERIAL_PORT)
    ap.add_argument('-c', '--controller', help='I2C controller to use', default=DEFAULT_I2C_DEVICE)
    ap.add_argument('-a', '--address', help='I2C bus peripheral address', default=DEFAULT_I2C_ADDRESS)
    ap.add_argument('-r', '--register', help='8-bit register address inside the peripheral', required=True)
    ap.add_argument('-s', '--size', help='number of bits to read', type=int, default=1)
    ap.add_argument('-t', '--timeout', help='read timeout value', type=int, default=1)
    ap.add_argument('byte', help='byte to send: write instead of read', nargs='?')
    args = ap.parse_args()

    clunx = TinyClunx33(args.port, verbose=args.verbose)
    if args.byte is not None:
        clunx.i2c_reg8_write(int(args.address, 0), int(args.register, 0), int(args.byte, 0),
            dev=args.controller, timeout=args.timeout)
    else:
        print(clunx.i2c_reg8_read(int(args.address, 0), int(args.register, 0),
            dev=args.controller, timeout=args.timeout, size=args.size))

if __name__ == "__main__":
    main()
