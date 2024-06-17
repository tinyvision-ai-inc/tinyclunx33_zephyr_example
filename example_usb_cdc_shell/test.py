from tinyclunx33_usb_shell import TinyClunx33
clunx = TinyClunx33("/dev/ttyACM0", verbose=True)
print(clunx.run_command('help', timeout=10))
