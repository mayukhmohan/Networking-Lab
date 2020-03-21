import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from scipy import stats


xmin = 0
xmax = 1100
ymin = 0
ymax = 10000
###################################
x = np.zeros(10)
x[0] = 100
for i in range(1,10):
    x[i] = x[i-1] + 100
    
###################################    
    



###################################
df = pd.read_csv('p_rtt2.csv',header=None)
df = df.values
X = df[:,0]
Y = df[:,1]

ans = np.zeros(10)
for i in range(10):
    sum = 0
    for j in range(50):
        sum += Y[j+i*50]
    ans[i] = sum/50
    
median = np.zeros(10)
mode = np.zeros(10)
np.sort(Y)
for i in range(10):
    temp = Y[i*50:i*50+50]
    median[i] = np.median(temp)
    mode[i] = max(set(list(temp)), key=list(temp).count)

plt.xlim([xmin,xmax])
plt.ylim([ymin,ymax])
plt.xlabel('P')
plt.ylabel('Cumulitive RTT')
plt.title('P vs Cumulitive RTT 2')
plt.scatter(X,Y,marker = '*',color = 'r')
plt.plot(x,ans)
plt.plot(x,median)
plt.plot(x,mode)
###################################



###################################
df = pd.read_csv('p_rtt8.csv',header=None)
df = df.values
X = df[:,0]
Y = df[:,1]


ans = np.zeros(10)
for i in range(10):
    sum = 0
    for j in range(50):
        sum += Y[j+i*50]
    ans[i] = sum/50
    
median = np.zeros(10)
mode = np.zeros(10)
np.sort(Y)
for i in range(10):
    temp = Y[i*50:i*50+50]
    median[i] = np.median(temp)
    mode[i] = max(set(list(temp)), key=list(temp).count)


plt.figure()
plt.xlim([xmin,xmax])
plt.ylim([ymin,ymax])
plt.xlabel('P')
plt.ylabel('Cumulitive RTT')
plt.title('P vs Cumulitive RTT 8')
plt.scatter(X,Y,marker = '*',color = 'b')
plt.plot(x,ans)
plt.plot(x,median)
plt.plot(x,mode)
###################################


###################################
df = pd.read_csv('p_rtt16.csv',header=None)
df = df.values
X = df[:,0]
Y = df[:,1]


ans = np.zeros(10)
for i in range(10):
    sum = 0
    for j in range(50):
        sum += Y[j+i*50]
    ans[i] = sum/50
    
    
median = np.zeros(10)
mode = np.zeros(10)
np.sort(Y)
for i in range(10):
    temp = Y[i*50:i*50+50]
    median[i] = np.median(temp)
    mode[i] = max(set(list(temp)), key=list(temp).count)


plt.figure()
plt.xlim([xmin,xmax])
plt.ylim([ymin,ymax])
plt.xlabel('P')
plt.ylabel('Cumulitive RTT')
plt.title('P vs Cumulitive RTT 16')
plt.scatter(X,Y,marker = '*',color = 'g')
plt.plot(x,ans)
plt.plot(x,median)
plt.plot(x,mode)
###################################