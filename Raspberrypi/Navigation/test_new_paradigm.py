import math
import sys
import numpy as np

D = 0
prevCheckPoint_x = 10
prevCheckPoint_y = 20

checkPoint_x = 10
checkPoint_y = 10

try:
    gradientTanLine = ( prevCheckPoint_x - checkPoint_x ) / ( checkPoint_y - prevCheckPoint_y )
except ZeroDivisionError:
    gradientTanLine = sys.maxint if ( prevCheckPoint_x - checkPoint_x > 0 ) else - sys.maxint - 1

print gradientTanLine

verticalLine = False
if ( gradientTanLine == sys.maxint or gradientTanLine == (- sys.maxint - 1) ):
    D = checkPoint_x
    gradientTanLine = 0
    verticalLine = True
    print 'equation of tangent is X = %f' % ( D )
else:
    D = checkPoint_y - gradientTanLine * checkPoint_x
    print 'equation of tangent is Y = %f X + %f' % ( gradientTanLine, D )

##dist = 50
##m = - ( 1 / gradientTanLine )
##a = 1 + ( 1 / m**2 )
##b = - ( 2 * checkPoint_x ) - ( 2 * ( D - checkPoint_y ) / m )
##c = checkPoint_x**2 + ( D - checkPoint_y )**2 - dist**2

dist = 1
a = 1 + gradientTanLine**2
b = 2*gradientTanLine*D - 2*checkPoint_x - 2*gradientTanLine*checkPoint_y
c = checkPoint_x**2 + checkPoint_y**2 - 2*checkPoint_y*D + D**2 - dist**2

def solveQuadEq(a, b, c):
    print 'a, b, c:', a, b, c
    det = b**2 - 4*a*c
    print 'det: ', det
    if det < 0:
        print 'no real roots'
        return -1, -1
    elif det == 0:
        x = ( -b + math.sqrt(det) ) / (2 * a)
        return x, x
    else:
        x1 = ( -b + math.sqrt(det) ) / (2 * a)
        x2 = ( -b - math.sqrt(det) ) / (2 * a)
        return x1, x2

if not verticalLine:
    x1, x2 = solveQuadEq(a, b, c)
    y1 = gradientTanLine * x1 + D
    y2 = gradientTanLine * x2 + D
else:
    x1 = checkPoint_x
    x2 = checkPoint_x
    y1 = D + dist
    y2 = D - dist
    
print checkPoint_x, checkPoint_y
print x1, x2

print y1, y2

v = np.matrix([x1, y1])
w = np.matrix([x2, y2])
p = np.matrix([checkPoint_x - 10, checkPoint_y - 1])

def dist(a, b):
    return math.sqrt((a[0,0] - b[0,0])**2 + (a[0,1] - b[0,1])**2)

def minDist(v, w, p):
    l_sq = (v[0,0]-w[0,0])**2 + (v[0,1]-w[0,1])**2
    if l_sq == 0:
        return dist(p, v)
    a = p - v
    b = w - v
    t = np.dot(a, b.transpose()) / l_sq
    if t < 0:
        return dist(p, w)       # beyond the v end of the segment
    elif t > 1:
        return dist(p, w)       # beyond the w end of the segment
    else:
        projection = v + t * (w - v)    # projection falls on the segment
        return dist(p, projection)

distance = minDist(v, w, p)
print 'distance: ', distance
