#ifdef __APPLE__
#include <GLUT/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <list>
#include "catmull.h"

using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <IL/il.h>
#include "tinyxml2.h"

char** fileString;
int ite;
float angle = 0;
float posx = 0;
float posy = 0;
float posz = 0;

float xScale = 1.0f;
float yScale = 1.0f;
float zScale = 1.0f;
int contaIds = 0;

float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;

GLenum drawType = GL_FILL;

struct vertice
{
    int ponto;
    float x;
    float y;
    float z;
};

struct translate
{
    int id;
    bool isStatic = true;
    float x;
    float y;
    float z;
    int time;
    vector<float> vX;
    vector<float> vY;
    vector<float> vZ;
};

struct rotate
{
    int id;
    int time;
    float angle;
    float aX;
    float aY;
    float aZ;
};

struct scale
{
    int id;
    float x;
    float y;
    float z;
};

struct figure
{
    char* model;
    char* texture;
    bool temText;
    GLuint textureID;
    GLuint txtBuff;
    vector<vertice> vertices;
    vector<vertice> normais;
    vector<vertice> textures;
    vector<unsigned int> indices;
    vector<int> id;
    float rgb[3] = {1,0,1};
};

struct vbo
{
    unsigned int indexCount;
    GLuint indices;
    GLuint vertice;
    GLuint normal;
    GLuint texture;
    GLuint verticeCount;
};

struct lights
{
    GLenum num;
    char* type;
    vertice pos;
};

struct figures
{
    vector<figure> figuras;
    vector<scale> escalas;
    vector<rotate> rotacoes;
    vector<translate> translacoes;
    vector<vbo> vbos;
    vector<char*> vbosName;
    vector<lights> luzes;
};
figures* globalFigs;

void loadTexture(int i) {
    if (globalFigs->figuras[i].texture != nullptr) {
        unsigned int t, tw, th;
        unsigned char *texData;

        ilInit();
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
        ilGenImages(1, &t);
        ilBindImage(t);
        //printf("load: %s", f.texture);
        ilLoadImage((ILstring) globalFigs->figuras[i].texture);
        tw = ilGetInteger(IL_IMAGE_WIDTH);
        th = ilGetInteger(IL_IMAGE_HEIGHT);
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        texData = ilGetData();

        glGenTextures(1, &globalFigs->figuras[i].textureID);

        glBindTexture(GL_TEXTURE_2D, globalFigs->figuras[i].textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


int containsVBO(vector<char*> vbos,char* name)
{
    for(int i = 0; i < vbos.size(); i++)
        if(strcmp(name, vbos[i]) == 0)
            return i;
        return -1;
}

void prepareData()
{
    int contador = 1;
    for(int i = 0; i < globalFigs->figuras.size(); i++)
    {
        vector<float> p;
        vector<float> norm;
        vector<float> text;
        //printf("Inicio!\n");
        int j = 1;
        //printf("figs to points, v: %ld\n", globalFigs->figuras[i].vertices.size());
        if(containsVBO(globalFigs->vbosName, globalFigs->figuras[i].model) == -1)
        {
            for(; j < globalFigs->figuras[i].vertices.size(); j++)
            {
                float xf = globalFigs->figuras[i].vertices[j].x;
                float yf = globalFigs->figuras[i].vertices[j].y;
                float zf = globalFigs->figuras[i].vertices[j].z;
                //printf("%.3f %.3f %.3f\n", xf,yf,zf);
                float xN = globalFigs->figuras[i].normais[j].x;
                float yN = globalFigs->figuras[i].normais[j].y;
                float zN = globalFigs->figuras[i].normais[j].z;

                //printf("%.3f %.3f %.3f\n", xN,yN,zN);
                p.push_back(xf);
                p.push_back(yf);
                p.push_back(zf);
                norm.push_back(xf);
                norm.push_back(yf);
                norm.push_back(zf);

            }

            globalFigs->vbosName.push_back(globalFigs->figuras[i].model);
            vbo aux;
            aux.indices = contador;
            globalFigs->vbos.push_back(aux);

            glGenBuffers(1,&(globalFigs->vbos[contador-1].vertice));
            glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[contador-1].vertice);
            glBufferData(GL_ARRAY_BUFFER,sizeof(float)*p.size(),p.data(),GL_STATIC_DRAW);

            glGenBuffers(1,&(globalFigs->vbos[contador-1].normal));
            glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[contador-1].normal);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*norm.size(), norm.data(),GL_STATIC_DRAW);

            glGenBuffers(1,&(globalFigs->vbos[contador-1].indices));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,globalFigs->vbos[contador-1].indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)* globalFigs->figuras[i].indices.size() ,globalFigs->figuras[i].indices.data(),GL_STATIC_DRAW);



            globalFigs->vbos[contador-1].indexCount = globalFigs->figuras[i].indices.size();
            contador++;
        }
        if(globalFigs->figuras[i].temText)
        {
            //printf("%s\n", globalFigs->figuras[i].texture);
            for (int j = 1; j < globalFigs->figuras[i].textures.size(); j++) {
                float xT = globalFigs->figuras[i].textures[j].x;
                float yT = globalFigs->figuras[i].textures[j].y;
                //printf("%.2f %.2f\n", xT, yT);
                text.push_back(xT);
                text.push_back(yT);
                //text.push_back();
            }

            glGenBuffers(1,&(globalFigs->figuras[i].txtBuff));
            glBindBuffer(GL_ARRAY_BUFFER,globalFigs->figuras[i].txtBuff);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*text.size(), text.data(),GL_STATIC_DRAW);

            loadTexture(i);

            /*for (int j = 0; j < text.size(); j++)
                printf("%f\n", text[j]);*/
        }
    }
}

void printTransf(figures* figs, int id)
{
    for(int j = 0 ; j < figs->translacoes.size() ;j++)
        if(figs->translacoes[j].id == id)
            printf("Translate: X=%f, Y=%f, Z=%f\n",figs->translacoes[j].x,figs->translacoes[j].y,figs->translacoes[j].z);

    for(int j = 0 ; j < figs->rotacoes.size() ;j++)
        if(figs->rotacoes[j].id == id)
            printf("Rotate: angle=%f, axisX=%f, axisY=%f, axisZ=%f\n",figs->rotacoes[j].angle,figs->rotacoes[j].aX,figs->rotacoes[j].aY,figs->rotacoes[j].aZ);

    for(int j = 0 ; j < figs->escalas.size() ;j++)
        if(figs->escalas[j].id == id)
            printf("Scale: X=%f, Y=%f, Z=%f\n",figs->escalas[j].x,figs->escalas[j].y,figs->escalas[j].z);
}

void printFigures(figures* figs)
{
    printf("%s x=%f, y=%f, z=%f\n",figs->luzes[0].type, figs->luzes[0].pos.x, figs->luzes[0].pos.y,figs->luzes[0].pos.z);
    for(int i = 0; i < figs->figuras.size(); i++)
    {
        printf("Fig: %s\n",figs->figuras[i].model);
        for(int j = 0; j < figs->figuras[i].id.size(); j++)
            printTransf(figs,figs->figuras[i].id[j]);

        printf("----------------------\n");
    }
}

void printTransNotStatic()
{
    for(int i = 0; i < globalFigs->translacoes.size(); i++)
    {
        if(!globalFigs->translacoes[i].isStatic)
        {
            printf("I: %d\n", i);
            printf("Time: %d\n", globalFigs->translacoes[i].time);
            for(int j = 0;j < globalFigs->translacoes[i].vX.size(); j++)
                printf("P: %f %f %f\n",globalFigs->translacoes[i].vX[j], globalFigs->translacoes[i].vY[j], globalFigs->translacoes[i].vZ[j]);
        }
    }
}

void spherical2Cartesian() {

    camX = radius * cos(beta) * sin(alfa);
    camY = radius * sin(beta);
    camZ = radius * cos(beta) * cos(alfa);
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

vector<vertice> ler(const char *ficheiro, FILE* fp) {

    //FILE *fp = fopen(ficheiro, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    int point = stoi(line);
    vector<vertice> v;
    vertice vet{point};
    v.push_back(vet);
    std::string delimiter = " ";
    std::string token;
    vector<string> tokens;
    int cont = 0;
    while (cont < point && (getline(&line, &len, fp)) != -1) {
        std::string lineS(line);
        tokens = split(lineS, " ");
        vertice vAux{0,stof(tokens[0]),stof(tokens[1]),stof(tokens[2])};
        v.push_back(vAux);
        cont++;
    }
    //fclose(fp);
    if (line)
        free(line);

    return v;
}


void readModel(tinyxml2::XMLElement *titleElement, figures* figs,vector<int> ids)
{
    const char* file = (char*) malloc(20);
    const char* rgb = (char*) malloc(20);
    titleElement = titleElement->FirstChildElement();

    while(titleElement)
    {
        figure f{};
        vector<int> vc;
        titleElement->QueryStringAttribute("file",&file);

        int read;
        read = titleElement->QueryStringAttribute("diffR",&rgb);
        if(!read)
            f.rgb[0] = atof(rgb);
        read = titleElement->QueryStringAttribute("diffG",&rgb);
        if(!read)
            f.rgb[1] = atof(rgb);
        read = titleElement->QueryStringAttribute("diffB",&rgb);
        if(!read)
            f.rgb[2] = atof(rgb);
        f.temText = false;
        read = 0;
        read = titleElement->QueryStringAttribute("texture",&rgb);
        if(!read)
        {
            f.texture = strdup(rgb);
            f.temText = true;
        }

        f.model = strdup(file);
        for(int i = 0; i < ids.size(); i++)
            vc.push_back(ids[i]);
        f.id = vc;

        char* auxDest = (char*) malloc(sizeof(char)*20);
        strcpy(auxDest,"./");
        strcat(auxDest,f.model);
        FILE* fp = fopen(auxDest, "r");
        f.vertices = ler(auxDest, fp);
        char *line = NULL;
        size_t len = 0;
        vector<GLuint> indices;
        if(getline(&line, &len, fp) != -1)
        {
            int nV = atoi(line);
            getline(&line, &len, fp);
            //printf("nV: %d\n", nV);
            for(int i = 0; i < nV; getline(&line, &len, fp),i++)
            {
                indices.push_back(atoi(line));
                //printf("indices[%d]: %d\n", i,indices[i]);
            }
        }
        else
        {
            for(int i = 0; i < f.vertices.size(); i++)
                indices.push_back(i);
        }
        //Tem Normais
        vector<vertice> normais;
        if(line != nullptr)
        {
            int nN = atoi(line);
            getline(&line, &len, fp);
            vertice vP{0,0,0,0};
            normais.push_back(vP);
            for(int i = 0; i < nN - 1; getline(&line, &len, fp),i++)
            {
                vector<string> tokens;
                std::string lineS(line);
                tokens = split(lineS, " ");
                vertice vAux{0,stof(tokens[0]),stof(tokens[1]),stof(tokens[2])};
                normais.push_back(vAux);
            }

        }
        line = nullptr;
        getline(&line, &len, fp);
        vector<vertice> texts;
        if (line != nullptr)
        {
            int nT = atoi(line);
            getline(&line, &len, fp);
            vertice vP{0,0,0,0};
            texts.push_back(vP);
            for(int i = 0; i < nT; getline(&line, &len, fp),i++)
            {
                vector<string> tokens;
                std::string lineS(line);
                tokens = split(lineS, " ");
                //printf("tokens[0]: %s, tokens[1]: %s\n", tokens[0].c_str(), tokens[1].c_str());
                vertice vAux{0,stof(tokens[0]),stof(tokens[1]),0};
                texts.push_back(vAux);
            }
        }
        fclose(fp);
        f.indices = indices;
        f.normais = normais;
        f.textures = texts;
        figs->figuras.push_back(f);
        titleElement = titleElement->NextSiblingElement();
    }

}


void readGroup(tinyxml2::XMLElement *titleElement, figures* figs, vector<int> ids)
{
    int id = 0;

    titleElement = titleElement->FirstChildElement();
    if(strcmp(titleElement->Value(),"translate") == 0)
    {
        tinyxml2::XMLElement *auxTElement = titleElement;
        auxTElement = auxTElement->FirstChildElement();
        if(auxTElement && strcmp(auxTElement->Value(),"point") == 0)
        {
            translate t{contaIds};
            t.isStatic = false;
            const char* str = (char*) malloc(sizeof(20));
            int time;
            titleElement->QueryStringAttribute("time", &str);
            if(str)
                t.time = atoi(str),str = nullptr;

            while(auxTElement)
            {
                auxTElement->QueryStringAttribute("X", &str);
                if(str)
                    t.vX.push_back(atof(str)),str = nullptr;
                auxTElement->QueryStringAttribute("Y", &str);
                if(str)
                    t.vY.push_back(atof(str)),str = nullptr;
                auxTElement->QueryStringAttribute("Z", &str);
                if(str)
                    t.vZ.push_back(atof(str)),str = nullptr;
                auxTElement = auxTElement->NextSiblingElement();
            }

            figs->translacoes.push_back(t);
            titleElement = titleElement->NextSiblingElement();
        }
        else
        {
            translate t{contaIds};

            const char* str = (char*) malloc(sizeof(20));
            titleElement->QueryStringAttribute("X",&str);
            if(str)
                t.x = atof(str),str = nullptr;

            titleElement->QueryStringAttribute("Y",&str);
            if(str)
                t.y = atof(str),str = nullptr;


            titleElement->QueryStringAttribute("Z",&str);
            if(str)
                t.z = atof(str);

            figs->translacoes.push_back(t);
            titleElement = titleElement->NextSiblingElement();
        }
    }

    if(strcmp(titleElement->Value(),"rotate") == 0)
    {
        rotate r{contaIds};

        const char* str = (char*) malloc(sizeof(20));
        titleElement->QueryStringAttribute("angle",&str);
        if(str)
            r.angle = atof(str),str = nullptr;

        titleElement->QueryStringAttribute("time",&str);
        if(str)
            r.time = atoi(str),str = nullptr;

        titleElement->QueryStringAttribute("axisX",&str);
        if(str)
            r.aX = atof(str),str = nullptr;

        titleElement->QueryStringAttribute("axisY",&str);
        if(str)
            r.aY = atof(str),str = nullptr;


        titleElement->QueryStringAttribute("axisZ",&str);
        if(str)
            r.aZ = atof(str);

        figs->rotacoes.push_back(r);
        titleElement = titleElement->NextSiblingElement();
    }

    if(strcmp(titleElement->Value(),"scale") == 0)
    {
        scale s{contaIds};

        const char* str = (char*) malloc(sizeof(20));
        titleElement->QueryStringAttribute("X",&str);
        if(str)
            s.x = atof(str),str = nullptr;

        titleElement->QueryStringAttribute("Y",&str);
        if(str)
            s.y = atof(str),str = nullptr;


        titleElement->QueryStringAttribute("Z",&str);
        if(str)
            s.z = atof(str);

        figs->escalas.push_back(s);
        titleElement = titleElement->NextSiblingElement();
    }

    if(strcmp(titleElement->Value(),"models") == 0)
    {
        ids.push_back(contaIds);
        readModel(titleElement, figs,ids);
    }

    titleElement = titleElement->NextSiblingElement();
    while (titleElement && strcmp(titleElement->Value(),"group") == 0)
    {
        contaIds++;
        readGroup(titleElement, figs,ids);
        if(ids.size() > 1)
            ids.pop_back();
        titleElement = titleElement->NextSiblingElement();
    }
}


void readLights(tinyxml2::XMLElement *titleElement, figures* figs)
{
    tinyxml2::XMLElement *elem = titleElement;
    elem = elem->FirstChildElement();
    const char* str = (char*) malloc(sizeof(20));

    if(strcmp(elem->Value(),"light") == 0)
    {
        unsigned int n = 1;
        while(elem) {
            lights luz{};
            luz.type = (char *) malloc(sizeof(20));
            vertice v{};
            elem->QueryStringAttribute("type", &str);
            strcpy(luz.type, str);
            elem->QueryStringAttribute("posX", &str);
            luz.pos.x = atof(str);
            elem->QueryStringAttribute("posY", &str);
            luz.pos.y = atof(str);
            elem->QueryStringAttribute("posZ", &str);
            luz.pos.z = atof(str);
            luz.num = n;
            figs->luzes.push_back(luz);
            elem = elem->NextSiblingElement();
            n++;
        }
    }
}

figures* readXml()
{
    figures* figs = (figures*) malloc(sizeof( figures));
    vector<int> ids;

    tinyxml2::XMLDocument doc;
    doc.LoadFile( "./teste.xml" );

    tinyxml2::XMLElement *titleElement = nullptr;
    titleElement = doc.FirstChildElement();

    if(strcmp(titleElement->Value(),"scene") == 0)
    {
        titleElement = titleElement->FirstChildElement();
        if(strcmp(titleElement->Value(),"lights") == 0)
        {
            readLights(titleElement,figs);
            titleElement = titleElement->NextSiblingElement();
        }
        if(strcmp(titleElement->Value(),"group") == 0)
            readGroup(titleElement,figs,ids);
    }

    //printf("The end!\n");

    //printFigures(figs);

    return figs;
}



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawAxis() {
	// put the geometric transformations here
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}

void draw(vector<vertice> arr, float* rgb){
    //drawAxis();
    int points = arr[0].ponto;
    glBegin(GL_LINES);

    for(int i = 1; i<=points;i+=3){
        glColor3f(rgb[0]/255,rgb[1]/255,rgb[2]/255);

        glVertex3f(arr[i].x,arr[i].y,arr[i].z);
        glVertex3f(arr[i+1].x,arr[i+1].y,arr[i+1].z);

        glVertex3f(arr[i+1].x,arr[i+1].y,arr[i+1].z);
        glVertex3f(arr[i+2].x,arr[i+2].y,arr[i+2].z);

        glVertex3f(arr[i+2].x,arr[i+2].y,arr[i+2].z);
        glVertex3f(arr[i].x,arr[i].y,arr[i].z);
    }
    glEnd();
}

void drawVBOs(int i,float* rgb, int fig)
{
    //materials
    float dark[]={0.2,0.2,0.2,1.0};
    float white[]={0.8,0.8,0.8,1.0};
    float color[]={rgb[0]/255,rgb[1]/255,rgb[2]/255,1.0};

    if(globalFigs->figuras[fig].temText == false)
    {
        glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,color);
        glMaterialfv(GL_FRONT,GL_SPECULAR,color);
        glMaterialf(GL_FRONT,GL_SHININESS,128);
    }

    int iC =  globalFigs->vbos[i].indexCount;
    glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[i].vertice);
    glVertexPointer(3,GL_FLOAT,0,0);
    glColor3f(rgb[0]/255,rgb[1]/255,rgb[2]/255);

    glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[i].normal);
    glNormalPointer(GL_FLOAT,0,0);

    if(globalFigs->figuras[fig].temText)
    {
        //printf("fig: %s\n", globalFigs->figuras[i].texture);
        glBindBuffer(GL_ARRAY_BUFFER,globalFigs->figuras[fig].txtBuff);
        glTexCoordPointer(2,GL_FLOAT,0,0);
        glBindTexture(GL_TEXTURE_2D,globalFigs->figuras[fig].textureID);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,globalFigs->vbos[i].indices);
    glDrawElements(GL_TRIANGLES,globalFigs->vbos[i].indexCount,GL_UNSIGNED_INT,0);


    glClearColor(0,0,0,0);
    if (globalFigs->figuras[i].temText)
        glBindTexture(GL_TEXTURE_2D,0);

}

void applyDynamicTranslate(translate t)
{
    const float NUM_STEPS = 100;
    float pos[3];
    float deriv[3];
    float* anterior = (float*) malloc(sizeof (float) * 4);

    anterior[0] = 0;
    anterior[1] = 1;
    anterior[2] = 0;

    std::vector<std::vector<float>> points;
    float partialTime = glutGet(GLUT_ELAPSED_TIME) / t.time;

    float** p = (float**) malloc(sizeof (float*) * 4);
    for(int i = 0; i < 4; i++) {
        p[i] = (float*) malloc(sizeof (float*)*3);
        p[i][0] = t.vX[i];
        p[i][1] = t.vY[i];
        p[i][2] = t.vZ[i];
    }


    getGlobalCatmullRomPoint(p, partialTime, (float *) pos, (float *) deriv);
    glTranslatef(pos[0],pos[1],pos[2]);

    normalize((float *)deriv);

    float z[3];
    cross((float *) deriv, anterior, (float * ) z);
    normalize((float *) z);

    cross((float *) z, (float *) deriv, anterior);
    normalize(anterior);

    float rotateMatrix[4][4];
    buildRotMatrix((float *) deriv,anterior,(float *) z,(float *) rotateMatrix);

    glMultMatrixf((float *) rotateMatrix);
    glEnd();
}

void applyTransf(int i)
{
    for(int ids = 0 ; ids < globalFigs->figuras[i].id.size() ;ids++) {
        for (int t = 0; t < globalFigs->translacoes.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->translacoes[t].id)
            {
                if(globalFigs->translacoes[t].isStatic)
                    glTranslatef(globalFigs->translacoes[t].x, globalFigs->translacoes[t].y, globalFigs->translacoes[t].z);
                else
                    applyDynamicTranslate(globalFigs->translacoes[t]);
            }


        for (int t = 0; t < globalFigs->rotacoes.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->rotacoes[t].id)
            {
                if(globalFigs->rotacoes[t].time)
                    glRotatef(glutGet(GLUT_ELAPSED_TIME)/ globalFigs->rotacoes[t].time, globalFigs->rotacoes[t].aX, globalFigs->rotacoes[t].aY,
                              globalFigs->rotacoes[t].aZ);
                else
                glRotatef(globalFigs->rotacoes[t].angle, globalFigs->rotacoes[t].aX, globalFigs->rotacoes[t].aY,
                          globalFigs->rotacoes[t].aZ);
            }


        for (int t = 0; t < globalFigs->escalas.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->escalas[t].id)
                glScalef(globalFigs->escalas[t].x, globalFigs->escalas[t].y, globalFigs->escalas[t].z);

    }
}



void turnOnTheLights(lights l)
{
    glEnable(GL_LIGHT0);

    GLfloat dark[4]={0.2,0.2,0.2,1.0};
    GLfloat white[4]={0.5,0.5,0.5,1.0};

    //light colors
    glLightfv(GL_LIGHT0,GL_AMBIENT,white);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
    glLightfv(GL_LIGHT0,GL_SPECULAR,dark);


}

void renderScene() {
    static float t = 0;
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);



    // put drawing instructions here
    glTranslatef(posx, posy, posz);
    glRotatef(angle, 0, 1, 0);
    glScalef(xScale, yScale, zScale);




    for(int i = 0; i < globalFigs->figuras.size();i++)
    {
        glPushMatrix();
        applyTransf(i);
        //draw(globalFigs->figuras[i].vertices,globalFigs->figuras[i].rgb);
        int a = containsVBO(globalFigs->vbosName, globalFigs->figuras[i].model);
        //printf("a: %d\n", a);
        drawVBOs(a,globalFigs->figuras[i].rgb,i);
        glPopMatrix();
    }

    // End of frame
    glutSwapBuffers();
    t += 0.00001;
}

// write function to process keyboard events
void reactKeyboard(unsigned char c, int x, int y) {

    switch (c) {

        case 'd':
            alfa -= 0.1; break;

        case 'a':
            alfa += 0.1; break;

        case 'w':
            beta += 0.1f;
            if (beta > 1.5f)
                beta = 1.5f;
            break;

        case 's':
            beta -= 0.1f;
            if (beta < -1.5f)
                beta = -1.5f;
            break;

        case 'z': radius -= 1.0f;
            if (radius < 1.0f)
                radius = 1.0f;
            break;

        case 'x': radius += 1.0f; break;
    }
    spherical2Cartesian();
    glutPostRedisplay();
}


int main(int argc, char **argv) {

    globalFigs = readXml();
    //printFigures(globalFigs);
    //printTransNotStatic();


    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG");

    #ifndef __APPLE__
    glewInit();
    #endif


    // Required callback registry
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(reactKeyboard);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_VERTEX_ARRAY);
    glPolygonMode(GL_FRONT, GL_LINES);
    spherical2Cartesian();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_LIGHTING);


    int luzes = globalFigs->luzes.size();
    for (int i = 0;i < luzes; ++i) {
        turnOnTheLights(globalFigs->luzes[i]);
    }

    prepareData();
    glEnable(GL_TEXTURE_2D);
    // enter GLUT's main cycle
    glutMainLoop();

    return 0;

}
