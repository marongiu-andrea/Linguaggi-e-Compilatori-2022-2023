import matplotlib.pyplot as plt
from math import log10


def read_y(file_name):
    with open(file_name, "r") as f:
        line = f.readline()
        y = [float(i) if i != '' else 0 for i in line.split("\t")]
        y = y[:-8]
        return y

y_loop = read_y("loop.txt")
y_loopopt = read_y("loopopt.txt")

# Test ratio tempi loop/loopopt.
print([y_loop[i]/y_loopopt[i] for i in range(3)])

x = [i for i in range(3)]

fig, ax = plt.subplots()
ax.set_xlabel("number of extern loops (log10)")
ax.set_ylabel("time (seconds)")

plt.plot(x, [i for i in y_loop], label="non-optimized", marker='o', linewidth=2)
plt.plot(x, [i for i in y_loopopt], label="loop fusion", marker='o', linewidth=2)

plt.legend(
        bbox_to_anchor = (0, 1, 1., .102),
        loc = 2,
        ncol = 2,
        borderaxespad = 0.
    )

ax.xaxis.set_ticks(range(3))
ax.xaxis.set_ticklabels(x)

fig.set_size_inches(10, 6)
plt.savefig("graph_1-3.png", dpi = 200)