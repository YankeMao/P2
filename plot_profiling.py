import matplotlib.pyplot as plt
import sys
import os

def plot_results(xvals, yvals, n_cores, xscale='linear', yscale='linear'):
    for i, y in enumerate(yvals): 
        plt.plot(xvals, y, marker='o', linestyle='-', label=f"{n_cores[i]} cores")
    plt.title("Parallel implementation with hashing (linear scale)")
    plt.xlabel("Threads")
    plt.ylabel("Time (s)")
    plt.grid(True)
    plt.xscale(xscale)
    plt.yscale(yscale)
    plt.legend()
    plt.savefig("profiling_output.png")

n_cores = ["1", "4", "16"]
n_threads = ["2", "16", "32"]
yvals = [[] for _ in n_cores]
for i, c in enumerate(n_cores): 
    for t in n_threads:
        path = os.path.join("profiling", f"{c}_cores", f"{t}_threads", "perf_report.txt")
        with open(path) as f: 
            last_line = [line.strip() for line in f if line.strip()][-1]
            time = (last_line.split())[2]
            yvals[i].append(float(time))
            

xvals= [int(t) for t in n_threads]

plot_results(xvals, yvals, n_cores)