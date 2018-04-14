import numpy as np

A = np.matrix([[1, 2, 9], [3000, 2000, 1000], [4/1e6, 3/1e6, 2/1e6]])
b = np.matrix([[1], [2000], [3/1e6]])
r = A.I * b
print('A 条件数：%s' % np.linalg.cond(A))
print('AX=b 解向量x：\n%s' % r)

A[2,2] = 3 / 1e6
b[2,0] = 4 / 1e6
r = A.I * b
print('A~X=b~ 解向量x~：\n%s' % r)


A = np.matrix([[1, 2, 9], [3000, 2000, 1000], [4/1e6, 3/1e6, 2/1e6]])
b = np.matrix([[1], [2000], [3/1e6]])

P = np.matrix(np.diag([1, 1e-3, 1e6]))
PA = P*A
r = PA.I*P*b
print('PAX=Pb 解向量x：\n%s' % r)
print('PA 条件数：%s' % np.linalg.cond(P*A))

PA[2,2] += 1e-6
b[2,0] += 1e-6
r = PA.I*P*b
print('给PA、Pb以+1e-6 扰动的解向量x~：\n%s' % r)
PA[2,2] -= 2*1e-6
b[2,0] -= 2*1e-6
r = PA.I*P*b
print('给PA、Pb以-1e-6 扰动的解向量x~：\n%s' % r)
