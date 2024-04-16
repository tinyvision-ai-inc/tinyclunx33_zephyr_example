from subprocess import Popen, PIPE
from math import pi
import matplotlib.pyplot as plt
import sys

proc = Popen(['./throughput', sys.argv[1], str(1024 * 1024 * 32)], stdout=PIPE)
max = 2048

fig = plt.figure(figsize=(10, 10))
ax = fig.add_subplot(projection='polar')
txt = ax.annotate(text="50", xy=[1.5*pi, 0.4], fontsize=40, ha='center')
ax.set_axis_off();
bar = ax.bar(x=pi, height=0.2, width=pi, bottom=0.8, align='edge')[0]
fig.show()

for line in proc.stdout:
    bytes_per_sec = int(line)
    mbit_per_sec = bytes_per_sec * 8 / 1024 / 1024

    val = mbit_per_sec / max
    bar.set_width(pi * val)
    bar.set_x(pi * (1 - val))
    txt.set_text('{:.02f} Mbit/s'.format(bytes_per_sec * 8 / 1024 / 1024))

    fig.canvas.draw() 
    fig.canvas.flush_events() 
