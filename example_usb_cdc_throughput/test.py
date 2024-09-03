import tty, termios, sys, os, time

fd = os.open(sys.argv[1], os.O_RDONLY)
iflag, oflag, cflag, lflag, ispeed, ospeed, cc = termios.tcgetattr(fd)

iflag &= ~(termios.IGNBRK | termios.BRKINT | termios.PARMRK | termios.ISTRIP)
iflag &= ~(termios.INLCR | termios.IGNCR | termios.ICRNL | termios.IXON)
oflag &= ~(termios.OPOST)
cflag &= ~(termios.CSIZE | termios.PARENB)
cflag |= termios.CS8
lflag &= ~(termios.ECHO | termios.ECHONL | termios.ICANON | termios.ISIG)
lflag &= ~(termios.IEXTEN)

termios.tcsetattr(fd, termios.TCSANOW, [iflag, oflag, cflag, lflag, ispeed, ospeed, cc])

t0 = time.time()
megabits = 1024 * 1024
bytes = megabits // 8
while bytes > 0:
    bytes -= len(os.read(fd, bytes))
t1 = time.time()
seconds_per_megabit = t1 - t0
kilobits_per_second = 1024 / seconds_per_megabit
print(f'{int(kilobits_per_second)} kbit/s')
