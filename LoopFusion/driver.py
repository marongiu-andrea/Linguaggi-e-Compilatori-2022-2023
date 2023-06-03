#!/usr/bin/env python

import json
import typing
import subprocess
import locale
import os
import matplotlib.pyplot as plt

from sys import stdout

locale.setlocale(locale.LC_NUMERIC, 'C')

def get_stats(optimized=False) -> tuple:
    if optimized:
        stat_file_name = 'stat/stat.opt.json'
    else:
        stat_file_name = 'stat/stat.json'
    
    with open(stat_file_name, 'r') as stat_file:
        stats = json.loads(stat_file.read())

        cache_misses = int(float(stats['counter-value']))
        runtime = stats['event-runtime'] / (10 ** 9)

        return (cache_misses, runtime)

def make(array_size: int) -> None:
    subprocess.run(['make', 'clean-test', 'all-test', f'ARRAY_SIZE={array_size}'], stdout=stdout)

ARRAY_SIZES = [10000+i*50000 for i in range(0,20)]

no_opt_cache_misses_plot_data = []
no_opt_runtime_plot_data = []
opt_cache_misses_plot_data = []
opt_runtime_plot_data = []
sizes = []

if not os.path.exists('diagrams'):
     os.makedirs('diagrams')


for size in ARRAY_SIZES:
    make(size)

    no_opt_stats = get_stats(optimized=False)
    opt_stats = get_stats(optimized=True)
    sizes.append(size)
    opt_cache_misses_plot_data.append(opt_stats[0])
    opt_runtime_plot_data.append(opt_stats[1])
    no_opt_cache_misses_plot_data.append(no_opt_stats[0])
    no_opt_runtime_plot_data.append(no_opt_stats[1])

plt.plot(sizes, opt_cache_misses_plot_data)
plt.plot(sizes, no_opt_cache_misses_plot_data)
plt.title("Difference in cache misses")
plt.xlabel("Dimensione array")
plt.ylabel("Cache misses")
plt.legend(['Optimized version', 'Unoptimized version',])
plt.savefig('diagrams/diagram_cache',format='svg')
plt.clf()
plt.plot(sizes, opt_runtime_plot_data)
plt.plot(sizes, no_opt_runtime_plot_data)
plt.title("Difference in execution time")
plt.xlabel("Dimensione array")
plt.ylabel("Execution time (in seconds)")
plt.legend(['Optimized version', 'Unoptimized version',])
plt.savefig('diagrams/diagram_exec',format='svg')

'''vecchio metodo:
    no_opt_cache_misses_plot_data.append((size, no_opt_stats[0]))
    no_opt_runtime_plot_data.append((size, no_opt_stats[1]))

    opt_cache_misses_plot_data.append((size, opt_stats[0]))
    opt_runtime_plot_data.append((size, opt_stats[1]))'''

