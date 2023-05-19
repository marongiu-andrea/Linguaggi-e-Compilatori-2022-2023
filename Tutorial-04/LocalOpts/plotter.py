import matplotlib.pyplot as plt
from math import log10


def read_y(file_name):
    with open(file_name, "r") as f:
        line = f.readline()
        y = [float(i) if i != '' else 0 for i in line.split("\t")]
        y = y[:-1]
        return y

y_loop = read_y("loop.txt")
y_loopopt = read_y("loopopt.txt")
x = [i for i in range(10)]

fig, ax = plt.subplots()
ax.set_xlabel("number of extern loops (log10)")
ax.set_ylabel("time (log10 of seconds)")

plt.plot(x, [log10(i) for i in y_loop], label="non-optimized", marker='o', linewidth=2)
plt.plot(x, [log10(i) for i in y_loopopt], label="loop fusion", marker='o', linewidth=2)

plt.legend(
        bbox_to_anchor = (0, 1, 1., .102),
        loc = 2,
        ncol = 2,
        borderaxespad = 0.
    )

ax.xaxis.set_ticks(range(10))
ax.xaxis.set_ticklabels(x)

fig.set_size_inches(10, 6)
plt.savefig("graph_log.png", dpi = 200)