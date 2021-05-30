#include <cstring>
#include <valarray>
#include "bezier.h"

bezierPatch parseBezier(char* filePath)
{
    bezierPatch bp;

    FILE *fp = fopen(filePath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    int number_of_patches = atoi(line);
    printf("Number of patches: %d\n", number_of_patches);
    for(int i = 0; i < number_of_patches; i++)
    {
        getline(&line, &len, fp);
        char* token = strtok(line, ",");
        int j = 0;
        std::vector<int> v;
        while(token != NULL) {
            int iValue = atoi(token);
            v.push_back(iValue);
            token = strtok(NULL, ",");
            j++;
        }
        bp.indicesForPatch.push_back(v);
    }

    getline(&line, &len, fp);
    int number_of_cP = atoi(line);
    printf("Number of Control Points: %d\n", number_of_cP);

    for(int i = 0; i < number_of_cP; i++)
    {
        getline(&line, &len, fp);
        char* token = strtok(line, ",");
        vertice v;
        v.x = atof(token);
        token = strtok(NULL, ",");
        v.y = atof(token);
        token = strtok(NULL, ",");
        v.z = atof(token);

        bp.controlPoints.push_back(v);
    }

    //for(int i = 0; i < 290; i++)
      //  printf("X=%f, Y=%f, Z=%f\n", bp.controlPoints[i].x,bp.controlPoints[i].y,bp.controlPoints[i].z);

    return bp;
}

vector<vector<float>> convertIndicesToMatrix(bezierPatch bp, vector<int> indices, int type)
{
   vector<vector<float>> res;

   for(int i = 0; i < 16; i+= 4)
   {
       vector<float> aux;
       for(int j = 0; j < 4 ;j++)
       {
           if(type == 0)
               aux.push_back(bp.controlPoints[indices[i+j]].x);
           if(type == 1)
               aux.push_back(bp.controlPoints[indices[i+j]].y);
           if(type == 2)
               aux.push_back(bp.controlPoints[indices[i+j]].z);
       }

       res.push_back(aux);
   }

    return res;
}

vector<float> multMatrix(vector<float> matrixA, vector<vector<float>> matrixB)
{
    vector<float> res;
    for(int mul = 0; mul < 4; mul++)
    {
        float aux = 0.0;
        for(int res = 0; res < 4; res++)
            aux += matrixA[res] * matrixB[res][mul];
        res.push_back(aux);
    }

    return res;
}

vector<vertice> createPatch(bezierPatch bp, vector<int> indices, int tesselation)
{
    vector<vector<float>> mMatrix = {{ -1.0f, 3.0f, -3.0f, 1.0f},
                                     { 3.0f, -6.0f, 3.0f, 0.0f},
                                     { -3.0f, 3.0f, 0.0f, 0.0f},
                                     { 1.0f, 0.0f, 0.0f, 0.0f}};
    vector<vector<float>> cpX = convertIndicesToMatrix(bp,indices,0);
    vector<vector<float>> cpY = convertIndicesToMatrix(bp,indices,1);
    vector<vector<float>> cpZ = convertIndicesToMatrix(bp,indices,2);

    vector<vertice> res;

    for(int i = 0; i <= tesselation; i++)
    {
        //própio ponto:
        //printf("%f %f %f", );
        for(int j = 0; j <= tesselation; j++)
        {

            float u = float (i) / float(tesselation);
            float v = float (j) / float(tesselation);
            vector<float> uMatrix = {u*u*u, u*u,u,1.0};
            vector<float> vMatrix = {v*v*v, v*v,v,1.0};
            //U plus M Matrix
            vector<float> um_Matrix = multMatrix(uMatrix,mMatrix);
            //U plus M plus CpX/CpY/CpZ Matrix
            vector<float> umcpX_Matrix = multMatrix(um_Matrix,cpX);
            vector<float> umcpY_Matrix = multMatrix(um_Matrix,cpY);
            vector<float> umcpZ_Matrix = multMatrix(um_Matrix,cpZ);
            //U plus M plus CpX plus Mt Matrix
            vector<float> umcpXm_Matrix = multMatrix(umcpX_Matrix,mMatrix);
            vector<float> umcpYm_Matrix = multMatrix(umcpY_Matrix,mMatrix);
            vector<float> umcpZm_Matrix = multMatrix(umcpZ_Matrix,mMatrix);

            vertice ver;
            ver.x = umcpXm_Matrix[0]*vMatrix[0] + umcpXm_Matrix[1]*vMatrix[1] +
                    umcpXm_Matrix[2]*vMatrix[2] + umcpXm_Matrix[3]*vMatrix[3];
            ver.y = umcpYm_Matrix[0]*vMatrix[0] + umcpYm_Matrix[1]*vMatrix[1] +
                      umcpYm_Matrix[2]*vMatrix[2] + umcpYm_Matrix[3]*vMatrix[3];
            ver.z = umcpZm_Matrix[0]*vMatrix[0] + umcpZm_Matrix[1]*vMatrix[1] +
                      umcpZm_Matrix[2]*vMatrix[2] + umcpZm_Matrix[3]*vMatrix[3];

            res.push_back(ver);
            //printf("%f %f %f\n", ver.x, ver.y, ver.z);

        }
    }

    return res;
}

vector<int> ordenaPatch(vector<vertice> patch, int tesselation, vector<int> indices)
{
    int t = tesselation + 1;
    for(int j = 0; j < tesselation;j++)
    {
        for(int i = 0; i < tesselation;i++)
        {
            indices.push_back(i+(j*t));
            indices.push_back(i+t+(j*t));
            indices.push_back(i+1+(j*t));
            indices.push_back(i+t+(j*t));
            indices.push_back(i+t+1+(j*t));
            indices.push_back(i+1+(j*t));
            //printf("Is: %d %d %d\n", i+(j*t), i+t+(j*t), i+1+(j*t));

            /*fprintf(file,"%f %f %f\n", patch[i+(j*t)].x,patch[i+(j*t)].y,patch[i+(j*t)].z);
            fprintf(file,"%f %f %f\n", patch[i+t+(j*t)].x,patch[i+t+(j*t)].y,patch[i+t+(j*t)].z);
            fprintf(file,"%f %f %f\n", patch[i+1+(j*t)].x,patch[i+1+(j*t)].y,patch[i+1+(j*t)].z);*/

            //printf("Is: %d %d %d\n", i+t+(j*t), i+t+1+(j*t), i+1+(j*t));

            /*fprintf(file,"%f %f %f\n", patch[i+t+(j*t)].x,patch[i+t+(j*t)].y,patch[i+t+(j*t)].z);
            fprintf(file,"%f %f %f\n", patch[i+t+1+(j*t)].x,patch[i+t+1+(j*t)].y,patch[i+t+1+(j*t)].z);
            fprintf(file,"%f %f %f\n", patch[i+1+(j*t)].x,patch[i+1+(j*t)].y,patch[i+1+(j*t)].z);*/

        }
    }

    return indices;
}

vertice normalizeV(vertice v1, vertice v2)
{
    vertice res;
    float l1, l2;
    l1 = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
    l2 = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);
    v1.x = v1.x/l1;
    v1.y = v1.y/l1;
    v1.z = v1.z/l1;
    v2.x = v2.x/l2;
    v2.y = v2.y/l2;
    v2.z = v2.z/l2;

    res.x = v1.y*v2.z - v1.z*v2.y;
    res.y = v1.z*v2.x - v1.x*v2.z;
    res.z = v1.x*v2.y - v1.y*v2.x;

    return res;
}

vector<vertice> createDUPatch(bezierPatch bp, vector<int> indices, int tesselation)
{
    vector<vector<float>> mMatrix = {{ -1.0f, 3.0f, -3.0f, 1.0f},
                                     { 3.0f, -6.0f, 3.0f, 0.0f},
                                     { -3.0f, 3.0f, 0.0f, 0.0f},
                                     { 1.0f, 0.0f, 0.0f, 0.0f}};
    vector<vector<float>> cpX = convertIndicesToMatrix(bp,indices,0);
    vector<vector<float>> cpY = convertIndicesToMatrix(bp,indices,1);
    vector<vector<float>> cpZ = convertIndicesToMatrix(bp,indices,2);

    vector<vertice> res;

    for(int i = 0; i <= tesselation; i++)
    {
        //própio ponto:
        //printf("%f %f %f", );
        for(int j = 0; j <= tesselation; j++)
        {

            float u = float (i) / float(tesselation);
            float v = float (j) / float(tesselation);
            vector<float> uMatrix = {u*u*u, u*u,u,1.0};
            vector<float> duMatrix = {3*u*u, 2*u,1.0,0.0};
            vector<float> vMatrix = {v*v*v, v*v,v,1.0};
            vector<float> dvMatrix = {3*v*v, 2*v,1.0,0.0};
            //dU plus M Matrix
            vector<float> dum_Matrix = multMatrix(duMatrix,mMatrix);
            //U plus M Matrix
            vector<float> um_Matrix = multMatrix(uMatrix,mMatrix);
            //dU plus M plus CpX/CpY/CpZ Matrix
            vector<float> dumcpX_Matrix = multMatrix(dum_Matrix,cpX);
            vector<float> dumcpY_Matrix = multMatrix(dum_Matrix,cpY);
            vector<float> dumcpZ_Matrix = multMatrix(dum_Matrix,cpZ);
            //U plus M plus CpX/CpY/CpZ Matrix
            vector<float> umcpX_Matrix = multMatrix(um_Matrix,cpX);
            vector<float> umcpY_Matrix = multMatrix(um_Matrix,cpY);
            vector<float> umcpZ_Matrix = multMatrix(um_Matrix,cpZ);

            //dU plus M plus CpX plus Mt Matrix
            vector<float> dumcpXm_Matrix = multMatrix(dumcpX_Matrix,mMatrix);
            vector<float> dumcpYm_Matrix = multMatrix(dumcpY_Matrix,mMatrix);
            vector<float> dumcpZm_Matrix = multMatrix(dumcpZ_Matrix,mMatrix);

            //U plus M plus CpX plus Mt Matrix
            vector<float> umcpXm_Matrix = multMatrix(umcpX_Matrix,mMatrix);
            vector<float> umcpYm_Matrix = multMatrix(umcpY_Matrix,mMatrix);
            vector<float> umcpZm_Matrix = multMatrix(umcpZ_Matrix,mMatrix);


            //dU plus M plus CpX plus Mt plux V Matrix
            vertice ver1, ver2;
            ver1.x = dumcpXm_Matrix[0]*vMatrix[0] + dumcpXm_Matrix[1]*vMatrix[1] +
                    dumcpXm_Matrix[2]*vMatrix[2] + dumcpXm_Matrix[3]*vMatrix[3];
            ver1.y = dumcpYm_Matrix[0]*vMatrix[0] + dumcpYm_Matrix[1]*vMatrix[1] +
                    dumcpYm_Matrix[2]*vMatrix[2] + dumcpYm_Matrix[3]*vMatrix[3];
            ver1.z = dumcpZm_Matrix[0]*vMatrix[0] + dumcpZm_Matrix[1]*vMatrix[1] +
                    dumcpZm_Matrix[2]*vMatrix[2] + dumcpZm_Matrix[3]*vMatrix[3];

            ver2.x = umcpXm_Matrix[0]*dvMatrix[0] + umcpXm_Matrix[1]*dvMatrix[1] +
                     umcpXm_Matrix[2]*dvMatrix[2] + umcpXm_Matrix[3]*dvMatrix[3];
            ver2.y = umcpYm_Matrix[0]*dvMatrix[0] + umcpYm_Matrix[1]*dvMatrix[1] +
                     umcpYm_Matrix[2]*dvMatrix[2] + umcpYm_Matrix[3]*dvMatrix[3];
            ver2.z = umcpZm_Matrix[0]*dvMatrix[0] + umcpZm_Matrix[1]*dvMatrix[1] +
                     umcpZm_Matrix[2]*dvMatrix[2] + umcpZm_Matrix[3]*dvMatrix[3];

            vertice vect = normalizeV(ver1, ver2);
            //printf("%f %f %f\n", vect.x, vect.y, vect.z);
            res.push_back(vect);

        }
    }

    return res;
}

void generateBezier(bezierPatch bp, int tesselation)
{
    FILE *fp = fopen("bezier.3d", "w");
    vector<int> indices;
    vector<vertice> normal;
    int points_to_write = 6*tesselation*tesselation*bp.indicesForPatch.size();
    fprintf(fp,"%ld\n", bp.indicesForPatch.size()*(tesselation+1)*(tesselation+1));
    for(int i = 0; i < bp.indicesForPatch.size(); i ++)
    {
        //printf("Patch: %d\n", i);
        vector<vertice> patch = createPatch(bp,bp.indicesForPatch[i], tesselation);
        vector<vertice> dudv = createDUPatch(bp, bp.indicesForPatch[i], tesselation);
        for(int j = 0; j < patch.size(); j++)
            fprintf(fp, "%f %f %f\n", patch[j].x,patch[j].y,patch[j].z);
        indices = ordenaPatch(patch,tesselation,indices);
        normal.insert(normal.end(), dudv.begin(), dudv.end());
    }
    int number_of_points_by_patch = (tesselation+1)*(tesselation+1);
    int aux = 0;
    fprintf(fp,"%d\n", points_to_write);
    for(int i = 0; i < indices.size(); i++)
    {
        if(i >=(tesselation*tesselation*2*3) && i % (tesselation*tesselation*2*3) == 0)
            aux += (tesselation+1) * (tesselation+1);

        fprintf(fp,"%d\n", indices[i]+aux);
    }

    fprintf(fp,"%ld\n", normal.size());
    for(int i = 0; i < normal.size(); i++)
        fprintf(fp,"%f %f %f\n", normal[i].x,normal[i].y,normal[i].z);

    fclose(fp);
}

