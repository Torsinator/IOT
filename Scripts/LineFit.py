import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv('Data\\light_sensor.csv')
x = df["lux-meter"].to_numpy(dtype=np.float32)
y = df["light sensor"].to_numpy(dtype=np.float32)

print(x)
print(y)

c2, c1, c0 = np.polyfit(x, y, 2)

x2 = np.linspace(min(x), max(x), 100)
y2 = c2*x2**2 + c1*x2 + c0

# print(f"y = {m}x + {b}")

plt.plot(x, y, 'o', x2, y2, 'r')
plt.xlabel('Voltage')
plt.ylabel('ADC value')
plt.show()