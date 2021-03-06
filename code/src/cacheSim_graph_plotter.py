import matplotlib
import numpy as np 
import pandas as pd
#matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import sys

filename1 = sys.argv[1]
filename2 = sys.argv[2]

# Initialize the lists for X and Y
data = pd.read_csv(filename1)
  
df = pd.DataFrame(data)

#specify the columns to sum
cols = ['Hits']
hits = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Misses']
misses = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Evictions']

evictions = df[cols].sum(axis=0)

x = [1, 2, 3]
y = [hits[0],misses[0],evictions[0]]
ax1 = plt.subplot()
ax1.set_xticks(x)

# plot bar chart

plt.bar(x,[hits[0],misses[0],evictions[0]])

# Define tick labels

ax1.set_xticklabels(["Hits","Misses","Evictions"]) 
plt.title('Cache Stats: Total number of hits/misses/evictions')
plt.ylabel('Number of hits/misses/evictions')
ax1.text(x[0]-0.25, hits[0] + .25, str(hits[0]),
            color = 'red', fontweight = 'bold')
ax1.text(x[1]-0.25, misses[0] + .25, str(misses[0]),
            color = 'red', fontweight = 'bold')
ax1.text(x[2]-0.25, evictions[0] + .25, str(evictions[0]),
            color = 'red', fontweight = 'bold')
plt.show()
  
id = list(df.iloc[:, 0])
hits = list(df.iloc[:, 1])
misses = list(df.iloc[:, 2])
eviction = list(df.iloc[:, 3])

X_axis = np.arange(len(id))

plt.bar(X_axis - 0.3, hits, 0.2, label = 'Hits')
plt.bar(X_axis - 0.1, misses, 0.2, label = 'Misses')
plt.bar(X_axis + 0.1, eviction, 0.2, label = 'Evictions')
plt.xticks(X_axis, id)
plt.xlabel("Thread ID")
plt.ylabel("Cache hits/misses/evictions")
plt.title("Cache performance per thread")
plt.legend()
plt.show()

# Initialize the lists for X and Y
data = pd.read_csv('output_funcs.csv')
  
df = pd.DataFrame(data)
  
func_name = list(df.iloc[:, 0])
hits = list(df.iloc[:, 1])
misses = list(df.iloc[:, 2])
eviction = list(df.iloc[:, 3])

X_axis = np.arange(len(func_name))

plt.bar(X_axis - 0.3, hits, 0.2, label = 'Hits')
plt.bar(X_axis - 0.1, misses, 0.2, label = 'Misses')
plt.bar(X_axis + 0.1, eviction, 0.2, label = 'Evictions')
plt.xticks(X_axis, func_name)
plt.xlabel("Func_names")
plt.ylabel("Cache hits/misses/evictions")
plt.title("Cache performance wrt functions")
plt.legend()
plt.show()

# Initialize the lists for X and Y
data = pd.read_csv(filename2)
  
df = pd.DataFrame(data)

bus_req = df.iloc[0, 0]
busRdX = df.iloc[0, 1]
invalidations = df.iloc[0, 2]

x = [1, 2, 3]
ax1 = plt.subplot()
ax1.set_xticks(x)

# plot bar chart

plt.bar(x,[bus_req,busRdX,invalidations])

# Define tick labels

ax1.set_xticklabels(["Bus Request","busRdX","Invalidations"]) 
plt.title('Cache Stats: Coherence Traffic')
plt.ylabel('Count')
ax1.text(x[0]-0.25, bus_req + .25, str(bus_req),
            color = 'red', fontweight = 'bold')
ax1.text(x[1]-0.25, busRdX + .25, str(busRdX),
            color = 'red', fontweight = 'bold')
ax1.text(x[2]-0.1, invalidations + .25, str(invalidations),
            color = 'red', fontweight = 'bold')
plt.show()