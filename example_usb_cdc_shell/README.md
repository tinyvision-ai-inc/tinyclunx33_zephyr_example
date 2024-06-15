# USB CDC Shell Example

This uses the CDC ACM USB class, a slightly modified `usbd_cdc_acm.c` class
from Zephyr to get it to work with the tinyCLUNX33 USB driver.

After programming the device and power cycling the board, a serial interface
would show-up, such as on `/dev/ttyACM0` on Linux, exposing the
[Zephyr shell](https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example/blob/main/example_usb_shell/tinyclunx33_usb_shell.py):

```
picocom /dev/ttyACM0
```

```
uart:~$ help
Please press the <Tab> button to see all available commands.
You can also use the <Tab> button to prompt or auto-complete all commands or its subcommands.
You can try to call commands with <-h> or <--help> parameter for more information.

Shell supports following meta-keys:
  Ctrl + (a key from: abcdefklnpuw)
  Alt  + (a key from: bf)
Please refer to shell documentation for more details.

Available commands:
  clear    : Clear screen.
  device   : Device commands
  devmem   : Read/write physical memory
            Usage:
            Read memory at address with optional width:
            devmem address [width]
            Write memory at address with mandatory width and value:
            devmem address <width> <value>
  help     : Prints the help message.
  history  : Command history.
  i2c      : I2C commands
  kernel   : Kernel commands
  rem      : Ignore lines beginning with 'rem '
  resize   : Console gets terminal screen size or assumes default in case the
            readout fails. It must be executed after each terminal width change
            to ensure correct text display.
  retval   : Print return value of most recent command
  shell    : Useful, not Unix-like shell commands.
  usbd     : USB device support commands
uart:~$
```

The baudrate can be ignored: there is no physical UART involved.
