import numpy as np
import scipy.linalg
class Inverse:
    @staticmethod
    def L(L, b):
        '''
        下三角回代求解 Ly = b
        '''
        n = b.shape[0]
        y = b.copy()
        for i in range(1, n):
            y[i,0] = b[i,0] - sum([L[i,r]*y[r,0] for r in range(i)])
        return y

    @staticmethod
    def U(U, y):
        '''
        上三角回代求解 Ux = y
        '''
        x = y.copy()
        n = y.shape[0]
        x[n-1,0] = y[n-1,0] / U[n-1,n-1]
        for i in range(n - 2, -1, -1):
            x[i,0] = (y[i,0] - sum([U[i,r]*x[r,0] for r in range(i + 1, n)])) / U[i,i]
        return x

n = 5
A = np.matrix(np.eye(n) * n, dtype='float64')
for i in range(1, n):
    A += np.diag([i] * i, i - n)
    A += np.diag([i] * i, n - i)
b = np.matrix(np.zeros((n, 1)), dtype='float64')
b[0,0] = 1

(P,L,U) = scipy.linalg.lu(A)
L = np.matrix(L, dtype='float64')
U = np.matrix(U, dtype='float64')

# LUx1 = b
x1 = Inverse.U(U, Inverse.L(L, b))
print('利用LU分解解Ax=b：\n%s' % x1)

# LULUx2 = b, LUx2 = x1
x2 = Inverse.U(U, Inverse.L(L, x1))
print('利用LU分解解A^2x=b：\n%s' % x2)

# LULULUx3 = b, LUx3 = x2
x3 = Inverse.U(U, Inverse.L(L, x2))
print('利用LU分解解A^3x=b：\n%s' % x3)

an = U.I * L.I
print('利用LU分解求A^-1：\n%s' % an)

r = None
for i in range(n):
    e = np.zeros((n, 1))
    e[i,0] = 1.0
    x = Inverse.U(U, Inverse.L(L, e))
    if r is None:
        r = x
    else:
        r = np.column_stack((r, x))

print('与A^-1准确解的比较，所有元素中差值绝对值最大为：%s' % np.max(np.abs(r-an)))
