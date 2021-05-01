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

using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
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
    float x;
    float y;
    float z;
};

struct rotate
{
    int id;
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
    vector<vertice> vertices;
    vector<int> id;
    float rgb[3] = {1,0,1};
};

struct vbo
{
    GLuint vertice;
    GLuint verticeCount;
};

struct figures
{
    vector<figure> figuras;
    vector<scale> escalas;
    vector<rotate> rotacoes;
    vector<translate> translacoes;
    vector<vbo> vbos;
    vector<char*> vbosName;
};
figures* globalFigs;

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
        printf("Inicio!\n");
        int j = 1;
        printf("figs to points, v: %ld\n", globalFigs->figuras[i].vertices.size());
        if(containsVBO(globalFigs->vbosName, globalFigs->figuras[i].model) == -1)
        {
            for(; j < globalFigs->figuras[i].vertices.size(); j++)
            {
                float xf = globalFigs->figuras[i].vertices[j].x;
                float yf = globalFigs->figuras[i].vertices[j].y;
                float zf = globalFigs->figuras[i].vertices[j].z;
                //printf("%.3f %.3f %.3f\n", xf,yf,zf);
                p.push_back(xf);
                p.push_back(yf);
                p.push_back(zf);
            }

            printf("globalFigs->figuras[i].model: %s\n",globalFigs->figuras[i].model);
            globalFigs->vbosName.push_back(globalFigs->figuras[i].model);
            vbo aux;
            aux.vertice = contador;
            aux.verticeCount = j-1;
            globalFigs->vbos.push_back(aux);
            printf("Img: %d\n", i);
            glGenBuffers(1, &(globalFigs->vbos[contador-1].vertice));
            glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[contador-1].vertice);
            glBufferData(GL_ARRAY_BUFFER,sizeof(float) * p.size(), &p[0],GL_STATIC_DRAW);
            printf("contador: %d\n", contador);
            contador++;
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
    for(int i = 0; i < figs->figuras.size(); i++)
    {
        printf("Fig: %s\n",figs->figuras[i].model);
        for(int j = 0; j < figs->figuras[i].id.size(); j++)
            printTransf(figs,figs->figuras[i].id[j]);

        printf("----------------------\n");
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

vector<vertice> ler(const char *ficheiro) {

    FILE *fp = fopen(ficheiro, "r");
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
    while ((getline(&line, &len, fp)) != -1) {
        std::string lineS(line);
        tokens = split(lineS, " ");
        vertice vAux{0,stof(tokens[0]),stof(tokens[1]),stof(tokens[2])};
        v.push_back(vAux);
    }
    fclose(fp);
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
        f.model = strdup(file);
        for(int i = 0; i < ids.size(); i++)
            vc.push_back(ids[i]);
        f.id = vc;

        char* auxDest = (char*) malloc(sizeof(char)*20);
        strcpy(auxDest,"./");
        strcat(auxDest,f.model);
        f.vertices = ler(auxDest);

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

    if(strcmp(titleElement->Value(),"rotate") == 0)
    {
        rotate r{contaIds};

        const char* str = (char*) malloc(sizeof(20));
        titleElement->QueryStringAttribute("angle",&str);
        if(str)
            r.angle = atof(str),str = nullptr;

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

void drawVBOs(int i)
{
    //drawAxis();
    glBindBuffer(GL_ARRAY_BUFFER,globalFigs->vbos[i].vertice);
    glVertexPointer(3,GL_FLOAT,0,0);
    printf("vc: %d\n", globalFigs->vbos[i].verticeCount);
    glDrawArrays(GL_TRIANGLES,0,globalFigs->vbos[i].verticeCount);
}

void applyTransf(int i)
{
    for(int ids = 0 ; ids < globalFigs->figuras[i].id.size() ;ids++) {
        for (int t = 0; t < globalFigs->translacoes.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->translacoes[t].id)
                glTranslatef(globalFigs->translacoes[t].x, globalFigs->translacoes[t].y, globalFigs->translacoes[t].z);


        for (int t = 0; t < globalFigs->rotacoes.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->rotacoes[t].id)
                glRotatef(globalFigs->rotacoes[t].angle, globalFigs->rotacoes[t].aX, globalFigs->rotacoes[t].aY,
                          globalFigs->rotacoes[t].aZ);


        for (int t = 0; t < globalFigs->escalas.size(); t++)
            if (globalFigs->figuras[i].id[ids] == globalFigs->escalas[t].id)
                glScalef(globalFigs->escalas[t].x, globalFigs->escalas[t].y, globalFigs->escalas[t].z);

    }
}

void renderScene() {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printf("Teste\n");

    // set the camera
    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);

    // put the geometric transformations here


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
        printf("a: %d\n", a);
        drawVBOs(a);
        glPopMatrix();
    }

    // End of frame
    glutSwapBuffers();
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
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(reactKeyboard);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_VERTEX_ARRAY);
    glPolygonMode(GL_FRONT, GL_LINE);
    spherical2Cartesian();

    prepareData();
    printf("okk\n");
    // enter GLUT's main cycle
    glutMainLoop();

    return 0;

}
