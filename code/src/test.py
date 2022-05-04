import matplotlib
import numpy as np 
import pandas as pd
#matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt

# Initialize the lists for X and Y
data = pd.read_csv('output_MI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum
cols = ['Hits']
hits_mi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Misses']
misses_mi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Evictions']

evictions_mi = df[cols].sum(axis=0)

print("Here")
# Initialize the lists for X and Y
data = pd.read_csv('output_MSI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum
cols = ['Hits']
hits_msi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Misses']
misses_msi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Evictions']

evictions_msi = df[cols].sum(axis=0)

# Initialize the lists for X and Y
data = pd.read_csv('output_MESI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum
cols = ['Hits']
hits_mesi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Misses']
misses_mesi = df[cols].sum(axis=0)
#specify the columns to sum
cols = ['Evictions']

evictions_mesi = df[cols].sum(axis=0)
hits = [hits_mi[0], hits_msi[0], hits_mesi[0]]
misses = [misses_mi[0], misses_msi[0], misses_mesi[0]]
evictions = [evictions_mi[0], evictions_msi[0], evictions_mesi[0]]

id = [0, 1, 2]
X_axis = np.arange(len(id))

plt.bar(X_axis - 0.3, hits, 0.2, label = 'Hits')
plt.bar(X_axis - 0.1, misses, 0.2, label = 'Misses')
plt.bar(X_axis + 0.1, evictions, 0.2, label = 'Evictions')

plt.xticks(X_axis, ["MI","MSI","MESI"])
plt.xlabel("Protocols")
plt.ylabel("Cache hits/misses/evictions")
plt.title("Cache performance for all protocols")
plt.legend()
plt.show()