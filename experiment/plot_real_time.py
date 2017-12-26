import serial
import numpy as np
from matplotlib import pyplot as plt
ser = serial.Serial('COM3', 9600)   # Your port number
 
plt.ion() # set plot to animated

ydata = [0] * 100    # deque of 100 data point
ax1=plt.axes()
# make plot
line, = plt.plot(ydata)
plt.ylim([10,40])
while True:
    data = []
    data.append(ser.readline())
    data.append(ser.readline())
    index = 0
    if data[0].find("InFlow") != -1:
        index = 1
    inflow = data[index].find("InFlow") +  len("InFlow is:")
    outflow = data[index].find("OutFlow") +  len("OutFlow is:")
    f1 = data[index].find("F1") + len("F1 is:")
    f2 = data[index].find("F2") + len("F2 is:")
    ydata.append(inflow)
    line.set_xdata(np.arange(len(ydata)))
    line.set_ydata(ydata)
    plt.draw()