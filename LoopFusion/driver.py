#!/usr/bin/env python

import json
import typing
import subprocess

from sys import stdout

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

ARRAY_SIZES = [10, 50, 100, 1000]

no_opt_cache_misses_plot_data = []
no_opt_runtime_plot_data = []
opt_cache_misses_plot_data = []
opt_runtime_plot_data = []

for size in ARRAY_SIZES:
    make(size)

    no_opt_stats = get_stats(optimized=False)
    opt_stats = get_stats(optimized=True)

    no_opt_cache_misses_plot_data.append((size, no_opt_stats[0]))
    no_opt_runtime_plot_data.append((size, no_opt_stats[1]))

    opt_cache_misses_plot_data.append((size, opt_stats[0]))
    opt_runtime_plot_data.append((size, opt_stats[1]))

