import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv('Data\\light_sensor.csv')
x = df["lux-meter"].to_numpy(dtype=np.float32)
y = df["light sensor"].to_numpy(dtype=np.float32)

print(x)
print(y)

params = np.polyfit(np.log(x), y, 1)

print(f"y = {params[0]}ln(x) + {params[1]}")

plt.plot(x, y)
plt.xlabel('ADC value')
plt.ylabel('Lux')
plt.show()