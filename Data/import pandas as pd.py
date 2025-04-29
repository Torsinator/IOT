# import pandas as pd
# import numpy as np
# import matplotlib.pyplot as plt

# df = pd.read_csv('D:\IOTPROJECT\IOT\Data\lightsens.csv')
# x = df["lux"].to_numpy(dtype=np.float32)
# y = df["PhotoR"].to_numpy(dtype=np.float32)

# print(x)
# print(y)

# c2, c1, c0 = np.polyfit(x, y, 2)

# x2 = np.linspace(min(x), max(x), 100)
# y2 = c2*x2**2 + c1*x2 + c0

# # print(f"y = {m}x + {b}")

# plt.plot(x, y, 'o', x2, y2, 'r')
# plt.xlabel('lux')
# plt.ylabel('photo resistor')
# plt.show()
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# CSV 파일 불러오기
df = pd.read_csv('D:\\IOTPROJECT\\IOT\\Data\\lightsens.csv')
x = df["lux"].to_numpy(dtype=np.float32)
y = df["PhotoR"].to_numpy(dtype=np.float32)

# 지수 함수 모델 정의: y = a * exp(bx)
def exp_func(x, a, b):
    return a * np.exp(b * x)

# curve fitting
popt, _ = curve_fit(exp_func, x, y, maxfev=10000)
a, b = popt

# 예측용 데이터 생성
x_fit = np.linspace(min(x), max(x), 100)
y_fit = exp_func(x_fit, a, b)

# 플로팅
plt.plot(x, y, 'o', label='Original Data')
plt.plot(x_fit, y_fit, 'r-', label=f'Fitted: y = {a:.2f} * exp({b:.2f}x)')
plt.xlabel('lux')
plt.ylabel('photo resistor')
plt.title('Exponential Curve Fitting')
plt.legend()
plt.grid(True)
plt.show()
