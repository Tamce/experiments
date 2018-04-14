#!/bin/python
import numpy as np
import matplotlib.pyplot as plt
import solution1 as solution

a = np.matrix(np.diag([5] * 20) + np.diag([-2] * 19, 1) + np.diag([-2] * 19, -1) + np.diag([1] * 18, 2) + np.diag([1] * 18, -2), dtype='float64')
b = np.matrix(np.ones((20, 1), dtype='float64'))


gauss = solution.Direct.Gauss(a, b)
p1 = plt.subplot(211)
p2 = plt.subplot(212)
jacobi = solution.Iterating.Jacobi(a, b, 20, np.zeros((20,1)), True, p1)
gs = solution.Iterating.GaussSeidel(a, b, 20, np.ones((20,1)), True, p2)
plt.show()
print('%s\n%s\n%s' % (gauss, jacobi, gs))
