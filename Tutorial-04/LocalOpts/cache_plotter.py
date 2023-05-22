import matplotlib.pyplot as plt


stride = []
time = []

with open("cache_exp.txt", "r") as f:
    for line in f:
        stride.append(int(line.split("\t")[0]))
        time.append(float(line.split("\t")[1]))

fig, ax = plt.subplots()
ax.set_xlabel("stride")
ax.set_ylabel("time (seconds)")

plt.plot(stride, time, linewidth=2)

# ax.xaxis.set_ticks()
# ax.xaxis.set_ticklabels()

fig.set_size_inches(10, 6)
plt.savefig("cache_graph.png", dpi = 200)