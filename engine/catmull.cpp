#include <cstdio>
#include "catmull.h"

#define POINT_COUNT 5
// Points that make up the loop for catmull-rom interpolation
float p[POINT_COUNT][3] = {{-1,-1,0},{-1,1,0},{1,1,0},{0,0,0},{1,-1,0}};

void buildRotMatrix(float *x, float *y, float *z, float *m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {

    float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0]/l;
    a[1] = a[1]/l;
    a[2] = a[2]/l;
}


float length(float *v) {

    float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return res;

}

void multMatrixVector(float *m, float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }

}

void getCatmullRomPoint(float t, float * p0, float * p1, float * p2, float * p3, float * pos, float * deriv) {
    float m[4][4] = { {-0.5f, 1.5f,-1.5f, 0.5f},
                      { 1.0f,-2.5f, 2.0f,-0.5f},
                      {-0.5f, 0.0f, 0.5f, 0.0f},
                      { 0.0f, 1.0f, 0.0f, 0.0f}};

    float a[3][4];
    float p[3][4];
    for(int i = 0; i < 3; i++) {
        p[i][0] = p0[i];
        p[i][1] = p1[i];
        p[i][2] = p2[i];
        p[i][3] = p3[i];
    }
    for(int i = 0; i < 3; i++) {
        printf("p0 = %f,p1 = %f,p2 = %f,p3 = %f, \n", p0[i],p1[i],p2[i],p3[i]);

    }

    // compute A = M * P
    multMatrixVector((float *)m,p[0],a[0]);
    multMatrixVector((float *)m,p[1],a[1]);
    multMatrixVector((float *)m,p[2],a[2]);

    // compute pos = T * A
    float T[4] = { t * t * t, t * t, t, 1};

    for(int i = 0; i < 3; i++) {
        pos[i] = 0;
        for(int j = 0; j < 4; j++)
            pos[i] += T[j] * a[i][j];
    }

    // compute deriv = dT * A
    float dT[4] = { 3 * t * t, 2* t, 1, 0};
    for(int i = 0; i < 3; i++) {
        deriv[i] = 0;
        for(int j = 0; j < 4; j++)
            deriv[i] += dT[j] * a[i][j];
    }
}

void getGlobalCatmullRomPoint(float** p,float gt, float *pos, float *deriv)
{
    int numberOfPoints = 4;
    float t = (gt/1000) * numberOfPoints; // this is the real global t
    int index = floor(t);  // which segment
    t = t - index; // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + numberOfPoints-1)%numberOfPoints;
    indices[1] = (indices[0]+1)%numberOfPoints;
    indices[2] = (indices[1]+1)%numberOfPoints;
    indices[3] = (indices[2]+1)%numberOfPoints;

    printf("gt: %f\nProximo t: %.2f\n",gt, t);
    getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
}

