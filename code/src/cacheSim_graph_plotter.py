import matplotlib
import numpy as np 
import pandas as pd
#matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt

# Initialize the lists for X and Y
data = pd.read_csv('output.csv')
  
df = pd.DataFrame(data)
  
id = list(df.iloc[:, 0])
hits = list(df.iloc[:, 1])
misses = list(df.iloc[:, 2])
eviction = list(df.iloc[:, 3])

plt.bar(id, hits)
plt.title('Cache Performance: Number of hits')
plt.xlabel('Cache ID')
plt.ylabel('Number of hits')

plt.show()

plt.bar(id, misses)
plt.title('Cache Performance: Number of misses')
plt.xlabel('Cache ID')
plt.ylabel('Number of misses')
plt.show()

X_axis = np.arange(len(id))

plt.bar(X_axis - 0.3, hits, 0.2, label = 'Hits')
plt.bar(X_axis - 0.1, misses, 0.2, label = 'Misses')
plt.bar(X_axis + 0.1, eviction, 0.2, label = 'Evictions')
plt.xticks(X_axis, id)
plt.xlabel("Cache ID")
plt.ylabel("Cache hits/misses/evictions")
plt.title("Cache performance")
plt.legend()
plt.show()