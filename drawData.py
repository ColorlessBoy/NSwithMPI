#!/usr/bin/python
import sys
import matplotlib.pyplot as plt
import numpy as np

def readData(path):
    mat = []
    with open(path, 'r') as f:
        while True:
            line = f.readline()
            if not line: break
            datas = []
            for data in line.split():
                datas.append(float(data))
            mat.append(datas)
    f.close()
    return mat

def plot(table):
    plt.figure()
    plt.plot(log[:, 0], log[0,1]/log[:, 1], 'x-')
    plt.plot(log[:, 0], log[:, 0], '--')
    plt.xlabel('CPU nums')
    plt.ylabel('Speedup')
    plt.legend(labels=['test', 'expect'])
    plt.title('Speedup by Using Clusters CPUs(128)')
    plt.savefig('log/log128.png')

    plt.figure()
    plt.plot(log[:6, 0], log[0,1]/log[:6, 1], 'x-')
    plt.plot(log[:6, 0], log[:6, 0], '--')
    plt.xlabel('CPU nums')
    plt.ylabel('Speedup')
    plt.legend(labels=['test', 'expect'])
    plt.title('Speedup by Using Clusters CPUs(32)')
    plt.savefig('log/log32.png')

def draw(vx, vy):
    plt.figure(figsize=(10, 10), dpi=100)
    nx = len(vx)
    ny = len(vx[0])
    x = np.linspace(0, 0.05*(nx-1), nx)
    y = np.linspace(0, 0.05*(ny-1), ny)
    X, Y = np.meshgrid(x, y)
    plt.quiver(Y[::3, ::3], X[::3, ::3], vx[::3, ::3], vy[::3, ::3])
    plt.xlabel('axis-y')
    plt.ylabel('axis-x')
    plt.title('Stream Velocity')
    plt.savefig('log/stream.png')

if __name__ == '__main__':
    log = np.array(readData("log/log.out"))
    pressure = np.array(readData("log/pressure.out"))
    vx = np.array(readData("log/velocity_x.out"))
    vy = np.array(readData("log/velocity_y.out"))
    print(np.max(vx))
    print(np.max(vy))
    plot(log)
    draw(vx, vy)

            
