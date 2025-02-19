TTY = /dev/ttyUSB1

all:
	west build --board tinyclunx33@rev2/custom --shield tinyclunx33_devkit_rev2

clean:
	rm -rf build

flash:
	west $@

picocom:
	picocom -b 156200 $(TTY)
