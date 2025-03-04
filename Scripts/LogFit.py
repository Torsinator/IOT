import pandas as pd
import numpy as np

df = pd.read_csv('')
x = df[0].to_numpy()
y = df[1].to_numpy()

params = np.polyfit(np.log(x), y, 1)

print("y = {params[0]}ln(x) + {params[1]}")