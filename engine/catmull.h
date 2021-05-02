//
// Created by kvothe on 02/05/21.
//

#ifndef ENGINE_CATMULL_H
#define ENGINE_CATMULL_H
#include <stdlib.h>
#include <math.h>
#include <vector>

void getGlobalCatmullRomPoint(float** p,float gt, float *pos, float *deriv);
void normalize(float *a);
void cross(float *a, float *b, float *res);
void buildRotMatrix(float *x, float *y, float *z, float *m);

#endif //ENGINE_CATMULL_H
