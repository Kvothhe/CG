#ifndef GENERATOR_BEZIER_H
#define GENERATOR_BEZIER_H
#include "generator.h"
#include <cstdio>
#include <cstdlib>
#include <vector>

struct bezierPatch
{
    std::vector<std::vector<int>> indicesForPatch;
    std::vector<vertice> controlPoints;
};

bezierPatch parseBezier(char* filePath);
vector<vector<float>> convertIndicesToMatrix(bezierPatch bp, vector<int> indices, int type);
vector<float> multMatrix(vector<float> matrixA, vector<vector<float>> matrixB);
vector<vertice> createPatch(bezierPatch bp, vector<int> indices, int tesselation);
vector<int> ordenaPatch(vector<vertice> patch, int tesselation, vector<int> indices);
void generateBezier(bezierPatch bp, int tesselation);

#endif //GENERATOR_BEZIER_H
