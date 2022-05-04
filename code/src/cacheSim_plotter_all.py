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

# Initialize the lists for X and Y
data = pd.read_csv('output_traffic_MI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum

bus_req_mi = df.iloc[0, 0]
busRdX_mi = df.iloc[0, 1]
invalidations_mi = df.iloc[0, 2]

# Initialize the lists for X and Y
data = pd.read_csv('output_traffic_MSI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum

bus_req_msi = df.iloc[0, 0]
busRdX_msi = df.iloc[0, 1]
invalidations_msi = df.iloc[0, 2]

# Initialize the lists for X and Y
data = pd.read_csv('output_traffic_MESI.csv')
  
df = pd.DataFrame(data)
#specify the columns to sum

bus_req_mesi = df.iloc[0, 0]
busRdX_mesi = df.iloc[0, 1]
invalidations_mesi = df.iloc[0, 2]

bus_req = [bus_req_mi, bus_req_msi, bus_req_mesi]
busRdX = [busRdX_mi, busRdX_msi, busRdX_mesi]
invalidations = [invalidations_mi, invalidations_msi, invalidations_mesi]
id = [0, 1, 2]
X_axis = np.arange(len(id))

plt.bar(X_axis - 0.3, bus_req, 0.2, label = 'Bus Request')
plt.bar(X_axis - 0.1, busRdX, 0.2, label = 'busRdX')
plt.bar(X_axis + 0.1, invalidations, 0.2, label = 'Invalidations')

# Define tick labels
plt.xticks(X_axis, ["MI","MSI","MESI"])
plt.title('Cache Stats: Coherence Traffic for all Protocols')
plt.ylabel('Count')
plt.legend()
plt.show()