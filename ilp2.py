import re
import argparse as ap
import gurobipy as gp
from gurobipy import GRB
import numpy as np
import time


def solve(filename):
    t_s = time.time()
    with open(filename, "r") as f:
        line = f.readline().strip()
        while re.match(r"c.*", line):
            line = f.readline().strip() # Skip comments

        match = re.match(r"param n := (\d+);", line)
        if not match:
            raise RuntimeError(f"Error when reading {filename}: Expected number of items\n{line}")
        N = int(match.group(1)) # Number of items
        p = np.empty(N, dtype=int)
        w = np.empty(N, dtype=int)
        c = [np.empty(N, dtype=int) for i in range(N)]


        line = f.readline().strip()
        match = re.match(r"param c := (\d+);", line)
        if not match:
            raise RuntimeError(f"Error when reading {filename}: Expected number of constraints\n{line}")
        C = int(match.group(1)) # Number of constraints

        line = f.readline().strip()
        if not re.match(r"param : V : p w :=", line):
            raise RuntimeError(f"Error when reading {filename}: Expected start of weights and profits\n{line}")
        line = f.readline().strip()
        # Read profits and weights
        i = 0
        while not re.match(r";", line) and i < N:
            match = re.match(r"\d+\s(\d+)\s(\d+)", line)
            if not match:
                raise RuntimeError(f"Error when reading {filename}: Expected weight and profit\n{line}")
            p[i] = int(match.group(1))
            w[i] = int(match.group(2))
            i += 1
            line = f.readline().strip()
        if i < N:
            raise RuntimeError(f"Error when reading {filename}: Expected {N} items, read {i}")

        line = f.readline().strip()
        while not re.match(r"set E :=", line):
            line = f.readline().strip()
        line = f.readline().strip()
        while not re.match(r";", line):
            match = re.match(r"(\d+)\s(\d+)", line)
            if not match:
                raise RuntimeError(f"Error when reading {filename}: Expected constraint\n{line}")
            u = int(match.group(1))
            v = int(match.group(2))
            c[u][v] = 1
            c[v][u] = 1
            line = f.readline().strip()


    m = gp.Model("ILP2")
    # m.Params.LogToConsole = 0
    m.Params.Threads = 1
    # m.Params.TimeLimit = 600 # 600s
    # m.Params.MemLimit = 2 # 2GB

    # Add variables
    x = m.addMVar(shape=(N), vtype=GRB.BINARY, name="items")

    # Add contstraints
    m.addConstr(gp.quicksum(x * w) <= C, "weight")
    for i, n in enumerate(c):
        m.addConstr(gp.quicksum(x * n) <= len(n)*(1 - x[i]), f"c{i}")

    # Set objective function
    m.setObjective(gp.quicksum(x * p), GRB.MAXIMIZE)

    time2load = time.time() - t_s

    # Solve
    m.optimize()

    # Print results
    print("\n" + "-"*80 + "\n")
    print(f"Time2Load: {time2load}")
    print(f"Time2Solve: {m.getAttr('Runtime')}")
    print(f"MaxMemUsed: {m.getAttr('MaxMemUsed')}")
    print(f"Selected Items: {[v.index for v in m.getVars()]}")
    print(f"Obj: {m.ObjVal}")


if __name__ == "__main__":
    parser = ap.ArgumentParser("ilp2")
    parser.add_argument("filename", help="File containing KPC AMPL formulation", type=str)
    args = parser.parse_args()

    solve(args.filename)
