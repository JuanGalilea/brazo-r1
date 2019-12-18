from matplotlib import pyplot as plt
import sympy as sp
import numpy as np
import time
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D






def get_DH(theta=0, d=0, a=0, alpha=0):  # retorna la matriz DH ^(i-1)A_i y el vector ^(i-1)p_i
    Rz = sp.Matrix(4, 4, [sp.cos(theta), -sp.sin(theta), 0, 0,
                          sp.sin(theta), sp.cos(theta), 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1])

    Dz = sp.Matrix(4, 4, [1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, d,
                          0, 0, 0, 1])

    Dx = sp.Matrix(4, 4, [1, 0, 0, a,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1])

    Rx = sp.Matrix(4, 4, [1, 0, 0, 0,
                          0, sp.cos(alpha1), -sp.sin(alpha), 0,
                          0, sp.sin(alpha), sp.cos(alpha), 0,
                          0, 0, 0, 1])

    return Rz * Dz * Dx * Rx, Rz * Rx, sp.Matrix([a, d * sp.sin(alpha), d * sp.cos(alpha), 1])


def get_NE(Q, Q_d, Q_dd, R1, R2, R3, p1, p2, p3):  #
    aux1 = sp.Matrix(4, 4, [0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1])

    aux2 = sp.Matrix(4, 4, [0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 0, 1])

    aux3 = sp.Matrix(4, 4, [0, 0, 0, 0,
                            0, 0, 0, 0,
                            1, 0, 0, 0,
                            0, 0, 0, 1])

    z0 = sp.Matrix([0, 0, 1, 1])

    omega1 = R1 * (aux1 * Q_d)

    omega2 = R2 * (omega1 + aux2 * Q_d) - sp.Matrix([0, 0, 0, 1])

    omega3 = R3 * (omega2 + aux3 * Q_d) - sp.Matrix([0, 0, 0, 1])

    omega1_d = R1 * (aux1 * Q_dd)

    omega2_d = R2 * (omega1_d + aux2 * Q_dd + cross(omega1, (aux2 * omega2))) - sp.Matrix([0, 0, 0, 2])

    omega3_d = R3 * (omega2_d + aux3 * Q_dd + cross(omega2, (aux3 * omega3))) - sp.Matrix([0, 0, 0, 2])

    v1_d = cross(omega1_d, p1) + cross(omega1, cross(omega1, p1)) - sp.Matrix([0, 0, 0, 1])

    v2_d = cross(omega2_d, p2) + cross(omega2, cross(omega2, p2)) + R2 * v1_d - sp.Matrix([0, 0, 0, 2])

    v3_d = cross(omega3_d, p3) + cross(omega3, cross(omega3, p3)) + R3 * v2_d - sp.Matrix([0, 0, 0, 2])

    a1 = cross(omega1_d, s1) + cross(omega1, cross(omega1, s1)) + v1_d - sp.Matrix([0, 0, 0, 2])
    a2 = cross(omega2_d, s2) + cross(omega2, cross(omega2, s2)) + v2_d - sp.Matrix([0, 0, 0, 2])
    a3 = cross(omega3_d, s3) + cross(omega3, cross(omega3, s3)) + v3_d - sp.Matrix([0, 0, 0, 2])

    f3 = m3 * a3 - m3 * sp.Matrix([0, 0, 0, 1]) + sp.Matrix([0, 0, 0, 1])
    f2 = m2 * a2 + R3 * f3 - m2 * sp.Matrix([0, 0, 0, 1])
    f1 = m1 * a1 + R2 * f2 - m1 * sp.Matrix([0, 0, 0, 1])

    n3 = cross(p3 + s3, m3 * a3) + I3 * omega3_d + cross(omega3, I3 * omega3)
    n2 = R2 * (n3 + cross(R2.T * p2, f3)) + cross(p2 + s2, m2 * a2) + I2 * omega2_d + cross(omega2, I2 * omega2)
    n1 = R1 * (n2 + cross(R1.T * p1, f2)) + cross(p1 + s1, m1 * a1) + I1 * omega1_d + cross(omega1, I1 * omega1)

    t1 = n1.dot(z0) + b1 * q1_d
    t2 = n2.dot(R2.T * z0) + b2 * q2_d
    t3 = n3.dot(R3.T * z0) + b3 * q3_d

    return sp.Matrix([t1, t2, t3]), sp.Matrix([f1, f2, f3])


def get_LE():  # calcula simbolicamente las ecuaciones dimanicas con el metodo Lagrange Euler
    Ud2 = [[0, 0, 0], [0, 0, 0], [0, 0, 0]]

    Ud3 = [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
           [[0, 0, 0], [0, 0, 0], [0, 0, 0]],
           [[0, 0, 0], [0, 0, 0], [0, 0, 0]]]

    for i in range(3):
        for j in range(3):
            Ud2[i][j] = sp.diff(M0An[i], Q[j])

    for i in range(3):
        for j in range(3):
            for k in range(3):
                Ud3[i][j][k] = sp.diff(Ud2[i][j], Q[k])

    J = [I1, I2, I3]

    D = sp.zeros(3)
    for i in range(3):
        for j in range(3):
            for k in range(max(i, j), 3):
                D[i, j] += sp.Trace(Ud2[k][j] * J[k] * Ud2[k][j].T).doit()

    H_aux = [[[0, 0, 0], [0, 0, 0], [0, 0, 0]], [[0, 0, 0], [0, 0, 0], [0, 0, 0]], [[0, 0, 0], [0, 0, 0], [0, 0, 0]]]

    for i in range(3):
        for j in range(3):
            for m in range(3):
                for k in range(max(i, j, m), 3):
                    H_aux[i][j][m] += sp.Trace(Ud3[j][k][m] * J[j] * Ud2[j][i].T).doit()

    H = sp.zeros(3, 1)
    for i in range(3):
        for k in range(3):
            for m in range(3):
                H[i] += H_aux[i][k][m] * Q_d[k] * Q_d[m]

    C = sp.zeros(3, 1)

    for i in range(3):
        for j in range(3):
            C[i] += (M[j] * G.T * Ud2[j][i] * S[j])[0]

    Tau = D * Q_dd + H + C - K * B * Q_d  # torque en la articulacion
    Tau_a = K_i * Tau  # torque en el motor

    i = K_m_i * Tau_a

    # Delta_Q_dd = D.inv()*(Delta_tau-H-C)

    det_D = sp.simplify(D.det())

    D_a = sp.simplify(D.adjugate())

    D_i = sp.simplify(D_a/det_D)


    return Tau, i, H, C, D, D_i


def q2x(q):  # esta funcion toma un punto en el espacio de estados y lo s tranforma al espacio de tarea
    x = (d2 * sp.sin(q[1]) + d3 * sp.sin(q[1] + q[2])) * sp.cos(q[0])
    y = (d2 * sp.sin(q[1]) + d3 * sp.sin(q[1] + q[2])) * sp.sin(q[0])
    z = (d2 * sp.cos(q[1]) + d3 * sp.cos(q[1] + q[2])) + d1

    return sp.Matrix(3, 1, [x, y, z])


def x2q(x):  # esta funcion toma un punto en el espacio de tarea y lo s tranforma al espacio de estados
    q1 = sp.atan2(x[1], x[0])

    Dy = x[2] - d1
    Dx = sp.sqrt(x[0] ** 2 + x[1] ** 2)

    R1, R2 = d2, d3

    D = sp.sqrt(Dx ** 2 + Dy ** 2)

    chorddistance = (R1 ** 2 - R2 ** 2 + D ** 2) / (2 * D)
    # distance from 1st circle's centre to the chord between intersects
    halfchordlength = sp.sqrt(R1 ** 2 - chorddistance ** 2)
    chordmidpointx = (chorddistance * Dx) / D
    chordmidpointy = (chorddistance * Dy) / D

    interseccion = (chordmidpointx - (halfchordlength * Dy) / D, chordmidpointy + (halfchordlength * Dx) / D)
    theta2 = sp.atan2(interseccion[1], interseccion[0])

    q2 = - theta2 + sp.pi / 2
    q3 = sp.pi / 2 - sp.atan2(Dy - interseccion[1], Dx - interseccion[0]) - q2

    return sp.Matrix(3, 1, [q1, q2, q3])


def c2q(c): # esta funcion toma un punto en el espacio de tarea en coordenadas cilindricas (r, theta, h) y lo s tranforma al espacio de estados
    x = c[0]*sp.cos(c[1])
    y = c[0]*sp.sin(c[1])
    z = c[2]
    q=x2q([x, y, z])

    return q
    


def get_trayect(x1, x2, type='line_X_poli', controler_speed=1000,
                out_ts=False):  # esta funcion toma dos puntos en el espacio de trabajo y genera una trayectoria entre estos
    t = sp.var('t')
    if type == 'line_X_poli':
        p1 = x2q(x1)
        p2 = x2q(x2)

        dist_max = max(abs(p2 - p1))

        r1 = (15 * dist_max) / (q_d_max * 8)
        r2 = sp.sqrt(10) * 3 ** (3 / 4) * sp.sqrt(dist_max / q_dd_max) / 3

        T = max(r1.evalf(), r2.evalf())

        a3 = 10 / T ** 3
        a4 = -15 / T ** 4
        a5 = 6 / T ** 5

        s = a3 * t ** 3 + a4 * t ** 4 + a5 * t ** 5
        trayectoria = p1 + (p2 - p1) * s

        if out_ts:
            trayectoria = q2x(trayectoria)

        pos = sp.lambdify(t, trayectoria, 'numpy')

        t_sampled = np.linspace(0, np.float(T), round(np.float(T) * controler_speed))

        trayectoria = pos(t_sampled)


    elif type == 'line_C_poli':

        p1 = c2q(x1)
        p2 = c2q(x2)

        dist_max = max(abs(p2 - p1))

        r1 = (15 * dist_max) / (q_d_max * 8)
        r2 = sp.sqrt(10) * 3 ** (3 / 4) * sp.sqrt(dist_max / q_dd_max) / 3

        T = max(r1.evalf(), r2.evalf())

        a3 = 10 / T ** 3
        a4 = -15 / T ** 4
        a5 = 6 / T ** 5

        s = a3 * t ** 3 + a4 * t ** 4 + a5 * t ** 5
        trayectoria = p1 + (p2 - p1) * s

        if out_ts:
            trayectoria = q2x(trayectoria)

        pos = sp.lambdify(t, trayectoria, 'numpy')

        t_sampled = np.linspace(0, np.float(T), round(np.float(T) * controler_speed))

        trayectoria = pos(t_sampled)



    return trayectoria, np.float(T)


g = 9.81

G = sp.Matrix(4, 1, [0, 0, -g, 0])

q1 = sp.var('q_1')
q2 = sp.var('q_2')
q3 = sp.var('q_3')
Q = sp.Matrix([q1, q2, q3])

q1_d = sp.var('q\u0307_1')
q2_d = sp.var('q\u0307_2')
q3_d = sp.var('q\u0307_3')
Q_d = sp.Matrix([q1_d, q2_d, q3_d])

q1_dd = sp.var('\u00a8q_1')
q2_dd = sp.var('\u00a8q_2')
q3_dd = sp.var('\u00a8q_3')
Q_dd = sp.Matrix([q1_dd, q2_dd, q3_dd])

q1_max = 3 * sp.pi / 4
q1_min = -3 * sp.pi / 4

q2_max = sp.pi / 2
q2_min = -sp.pi / 2

q3_max = sp.pi / 2
q3_min = -sp.pi / 2

q_d_max = 1  # sp.symbols('q\u0307_max',real =True) #la velocidad maxima de las articulaciones
q_dd_max = 1  # sp.symbols('q\u00a8_max',real =True) #la aceleracion maxima de las articulaciones

# parametros D-H

d1 = 84 / 1000  # sp.var('d_1')
d2 = 225 / 1000  ##sp.var('d_2')
d3 = 225 / 1000  # sp.var('d_3')

a1 = 0  # sp.var('a_1')
a2 = 0  # sp.var('a_2')
a3 = 0  # sp.var('a_3')

alpha1 = sp.pi / 2  # sp.var('\u03B1_1')
alpha2 = 0  # sp.var('\u03B1_2')
alpha3 = 0  # sp.var('\u03B1_3')
Alpha = sp.Matrix([alpha1, alpha2, alpha3])

A01, R1, p1 = get_DH(q1, d1, a1, alpha1)
A12, R2, p2 = get_DH(q2, d2, a2, alpha2)
A23, R3, p3 = get_DH(q3, d3, a3, alpha3)

A02 = A01 * A12
A03 = A02 * A23
A13 = A12 * A23
M0An = [A01, A02, A03]

# parametros N-E

m1 = 0.203  # sp.var('m_1')  # masa del primer eslabon
m2 = 1.22  # sp.var('m_2')  # masa del segundo eslabon
m3 = 1.22  # sp.var('m_3')  # masa del tercer eslabon
M = sp.Matrix(3, 1, [m1, m2, m3])

b1 = 6.5E-3  # sp.var('b_1')  # roce dinamico del primer motor
b2 = 6.5E-3  # sp.var('b_2')  # roce dinamico del segundo motor
b3 = 6.5E-3  # sp.var('b_3')  # roce dinamico del tercer motor
B = sp.diag(b1, b2, b3)

k1 = 43  # sp.var('k_1')  # reduccion del engranaje del primer motor
k2 = 43  # sp.var('k_2')  # reduccion del engranaje del segundo motor
k3 = 43  # sp.var('k_3')  # reduccion del engranaje del tercer motor
K = sp.diag(k1, k2, k3)
K_i = K.inv()

k1_m = 19.9E-3  # sp.var('k_1')  # contante torque-corriente del primer motor
k2_m = 19.9E-3  # sp.var('k_2')  # contante torque-corriente del segundo motor
k3_m = 19.9E-3  # sp.var('k_3')  # contante torque-corriente del tercer motor
K_m = sp.diag(k1_m, k2_m, k3_m)
K_m_i = K_m.inv()

Cmx1 = -18.1  # sp.var('Cmx_1')
Cmy1 = 0  # sp.var('Cmy_1')
Cmz1 = 0  # sp.var('Cmz_1')
Ixx1 = 516.4 / 1000000 + m1 * (Cmy1 ** 2 + Cmz1 ** 2)  # sp.var('Ixx1')
Ixy1 = 0.132 / 1000000 + m1 * Cmx1 * Cmy1  # sp.var('Ixy1')
Ixz1 = 0 + m1 * Cmx1 * Cmz1  # sp.var('Ixz1')
Iyy1 = 462.8 / 1000000 + m1 * (Cmx1 ** 2 + Cmz1 ** 2)  # sp.var('Iyy1')
Iyz1 = 0 + m1 * Cmz1 * Cmy1  # sp.var('Iyz1')
Izz1 = 243 / 1000000 + m1 * (Cmy1 ** 2 + Cmx1 ** 2)  # sp.var('Izz1')

Cmx2 = 108 / 1000  # sp.var('Cmx_2')
Cmy2 = 0  # sp.var('Cmy_2')
Cmz2 = 0  # sp.var('Cmz_2')
Ixx2 = 859.9 / 1000000 + m2 * (Cmy2 ** 2 + Cmz2 ** 2)  # sp.var('Ixx2')
Ixy2 = -17.15 / 1000000 + m2 * Cmx2 * Cmy2  # sp.var('Ixy2')
Ixz2 = 71.71 / 1000000 + m2 * Cmx2 * Cmz2  # sp.var('Ixz2')
Iyy2 = 4196 / 1000000 + m2 * (Cmx2 ** 2 + Cmz2 ** 2)  # sp.var('Iyy2')
Iyz2 = 0.39 / 1000000 + m2 * Cmz2 * Cmy2  # sp.var('Iyz2')
Izz2 = 3715 / 1000000 + m2 * (Cmy2 ** 2 + Cmx2 ** 2)  # sp.var('Izz2')

Cmx3 = 108 / 1000  # sp.var('Cmx_3')
Cmy3 = 0  # sp.var('Cmy_3')
Cmz3 = 0  # sp.var('Cmz_3')
Ixx3 = 859.9 / 1000000 + m3 * (Cmy3 ** 2 + Cmz3 ** 2)  # sp.var('Ixx3')
Ixy3 = -17.15 / 1000000 + m3 * Cmx3 * Cmy3  # sp.var('Ixy3')
Ixz3 = 71.71 / 1000000 + m3 * Cmx3 * Cmz3  # sp.var('Ixz3')
Iyy3 = 4196 / 1000000 + m3 * (Cmx3 ** 2 + Cmz3 ** 2)  # sp.var('Iyy3')
Iyz3 = 0.39 / 1000000 + m3 * Cmz3 * Cmy3  # sp.var('Iyz3')
Izz3 = 3715 / 1000000 + m3 * (Cmy3 ** 2 + Cmx3 ** 2)  # sp.var('Izz3')

s1 = sp.Matrix([Cmx1, Cmy1, Cmz1, 1])  # el centro de masas del primer eslabon referido a sus coordenadas
s2 = sp.Matrix([Cmx2, Cmy2, Cmz2, 1])  # el centro de masas del segundo eslabon referido a sus coordenadas
s3 = sp.Matrix([Cmx3, Cmy3, Cmz3, 1])  # el centro de masas del tercer eslabon referido a sus coordenadas
S = sp.Matrix(3, 1, [s1, s2, s3])

I1 = sp.Matrix(4, 4, [Ixx1, Ixy1, Ixz1, Cmx1, Ixy1, Iyy1, Iyz1, Cmy1, Ixz1, Iyz1, Izz1, Cmz1, Cmx1, Cmy1, Cmz1, m1])
I2 = sp.Matrix(4, 4, [Ixx2, Ixy2, Ixz2, Cmx2, Ixy2, Iyy2, Iyz2, Cmy2, Ixz2, Iyz2, Izz2, Cmz2, Cmx2, Cmy2, Cmz2, m2])
I3 = sp.Matrix(4, 4, [Ixx3, Ixy3, Ixz3, Cmx3, Ixy3, Iyy3, Iyz3, Cmy3, Ixz3, Iyz3, Izz3, Cmz3, Cmx3, Cmy3, Cmz3, m3])

Tau, i, H, C, D, D_i = get_LE()

au = time.time()
sp.pprint(D_i)
print(time.time() - au)
"""


au = time.time()
Trayect, t_max = get_trayect(sp.Matrix(3, 1, [0.05, 0.05, 0]), sp.Matrix(3, 1, [0.1, 0, 0.1]), 'line_SS_poli', out_ts=True)
print('time:', time.time() - au)

fig = plt.figure()
ax = fig.gca(projection='3d')

ax.plot(Trayect[0, :, :][0], Trayect[1, :, :][0], Trayect[2, :, :][0])

plt.show()
"""