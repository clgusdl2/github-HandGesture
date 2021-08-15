import numpy as np
import matplotlib.pyplot as plt

np.a=[224, 217, 54, 24, 179]
l=['Gyoungi', 'Seoul', 'Pusan', 'Gyoungnam', 'Others']


plt.pie(np.a, labels=l, autopct='%.1d%%')
plt.show()