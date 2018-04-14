#!/bin/python
import numpy as np
import matplotlib.pyplot as plt

epsilon = 1e-15
# 列主元高斯消去直接法
class Direct:
    @staticmethod
    def Gauss(A , b):
        det = 1
        n = A.shape[1]
        a = np.column_stack((A, b))
        # print('增广矩阵：\n', a)
        for k in range(0, n - 1):
            # 选取列主元
            ik = np.abs(a[:,k]).argmax()
            if (np.abs(a[ik, k]) < epsilon):
                raise '矩阵A奇异'
            # 换行
            if (ik != k):
                a[[k, ik], k:] = a[[ik, k], k:]
                det *= -1
            # print('换行：\n', a)
            # 计算乘积并消元
            for i in range(k + 1, n):
                a[i, k] = a[i, k] / a[k, k]
                a[i, k+1:] = a[i, k+1:] - a[i, k] * a[k, k+1:]
                a[i, k] = 0
            det *= a[k, k]
            # print('消元：\n', a)
        if (np.abs(a[n - 1, n - 1]) < epsilon):
            raise '矩阵A奇异'
        det *= a[n - 1, n - 1]
        # 回代计算
        a[n - 1, n] = a[n - 1, n] / a[n - 1, n - 1]
        for k in range(n - 2, -1, -1):
            for j in range(k + 1, n):
                a[k, n] -= a[k, j] * a[j, n]
            a[k, n] /= a[k, k]
        return a[:, n]

class Iterating:
    @staticmethod
    def Jacobi(A, b, n=100, x0=np.ones((0,0)), draw=False, plt=None):
        (r, c) = A.shape
        if (x0.shape == (0, 0)):
            x0 = np.ones((r, 1))
        x0 = np.matrix(x0)
        It = A.copy()
        # 对角线元素组成的行向量
        diag = 1 / np.matrix(np.diag(It))
        f = np.matrix(np.diag(diag.flat))
        for i in range(r):
            It[i,:] = It[i,:] / It[i,i]
        It -= np.matrix(np.eye(r))
        It *= -1

        axis = 0
        x = [x0[axis,0]]
        for i in range(n):
            x0 = It * x0 + f * b
            x.append(x0[axis,0])
        if draw:
            plt.plot(x)
        return x0

    @staticmethod
    def GaussSeidel(A, b, n=100, x0=np.ones((0,0)), draw=False, plt=None):
        (r, c) = A.shape
        if (x0.shape == (0, 0)):
            x0 = np.ones((r, 1))
        x0 = np.matrix(x0)
        It = A.copy()
        # 对角线元素组成的行向量
        diag = 1 / np.matrix(np.diag(It))
        f = np.matrix(np.diag(diag.flat))
        for i in range(r):
            It[i,:] = It[i,:] / It[i,i]
        It -= np.matrix(np.eye(r))
        It *= -1

        axis = 0
        x = [x0[axis,0]]
        for i in range(n):
            # 计算 x[i+1]
            for j in range(r):
                x0[j,0] = It[j,:] * x0 + b[j,0]*f[j,j]
            x.append(x0[axis,0])
        if draw:
            plt.plot(x)
        return x0
