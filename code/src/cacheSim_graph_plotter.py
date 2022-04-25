import matplotlib.pyplot as plt
import numpy as np 
import pandas as pd
# Initialize the lists for X and Y
data = pd.read_csv('output.csv')
  
df = pd.DataFrame(data)
  
x = list(df.iloc[:, 0])
y = list(df.iloc[:, 1])
z = list(df.iloc[:, 2])
w = list(df.iloc[:, 3])
print(y)
plt.bar(x, y)

plt.title('Simple Example')
plt.xlabel('Width Names')
plt.ylabel('Height Values')

plt.show()

plt.bar(x, z)
plt.title('Cache Performance: Number of hits')
plt.xlabel('Cache number')
plt.ylabel('Number of hits')
plt.show()

X_axis = np.arange(len(x))

plt.bar(X_axis - 0.3, y, 0.2, label = 'Misses')
plt.bar(X_axis - 0.1, z, 0.2, label = 'Hits')
plt.bar(X_axis + 0.1, w, 0.2, label = 'Evictions')
plt.xticks(X_axis, x)
plt.xlabel("Cache number")
plt.ylabel("Cache hits/misses/evictions")
plt.title("Cache performance")
plt.legend()
plt.show()