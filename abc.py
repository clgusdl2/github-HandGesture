import matplotlib.pyplot as plt
import numpy as np


#해를구함
def quadratic(a,b,c):
    D = b*b-4*a*c
    r1 = (-b+(b**2-4*a*c)**0.5)/(2*a)
    r2 = (-b-(b**2-4*a*c)**0.5)/(2*a)
    if D>=0:
        print((r1,r2))
    else:
        print("No root")
        
quadratic(1,0,-4)
quadratic(2,-8,6)
quadratic(-5,1,-2)

# 
# 세방정식 그래프
x = np.linspace(-5.0, 5.0, 100)
y1 = x*x-4
y2 = 2*x*x-8*x+6
y3 = -5*x*x+1*x-2

plt.plot(x,y1, x,y2, x,y3)
plt.grid()
plt.axis([-5.0, 5.0, -10.0, 10.0])
plt.show()
