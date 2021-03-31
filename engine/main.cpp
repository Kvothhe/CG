#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <cstdio>
#include <iostream>
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
int posIds = 0;
int contaT = 0;
int contaR = 0;
int contaS = 0;

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
    vertice* vertices; 
    int* id;
};

struct figures
{
    figure* figuras;
    scale* escalas;
    rotate* rotacoes;
    translate* translacoes;
};

void readModel(tinyxml2::XMLElement *titleElement)
{
    const char* file = (char*) malloc(20);
    titleElement = titleElement->FirstChildElement();

    while(titleElement)
    {
        titleElement->QueryStringAttribute("file",&file);
        printf("file: %s\n", file);
        titleElement = titleElement->NextSiblingElement();
    }

}


void readGroup(tinyxml2::XMLElement *titleElement, figures* figs, int* ids)
{
    printf("Group:\n");
    int id = 0;

    titleElement = titleElement->FirstChildElement();
    if(strcmp(titleElement->Value(),"translate") == 0)
    {
        figs->translacoes = (translate*) malloc(sizeof(translate*));
        figs->translacoes->id = contaIds;

        float xT,yT,zT;
        titleElement->QueryFloatAttribute("X",&xT);
        if(xT)
            figs->translacoes[contaT].x = xT;

        titleElement->QueryFloatAttribute("Y",&yT);
        printf("ok1\n");

        if(yT)
            figs->translacoes[contaT].y = yT;
        printf("ok2\n");
        titleElement->QueryFloatAttribute("Z",&zT);
        if(zT)
            figs->translacoes[contaT].z = zT;
        titleElement = titleElement->NextSiblingElement();
        contaT++;
    }

    if(strcmp(titleElement->Value(),"rotate") == 0)
    {
        figs->rotacoes = (rotate*) malloc(sizeof(rotate*));
        figs->rotacoes->id = contaIds;

        float angle, axisX, axisY, axisZ;
        titleElement->QueryFloatAttribute("angle",&angle);
        if(angle)
            figs->rotacoes[contaR].angle = angle;

        titleElement->QueryFloatAttribute("axisX",&axisX);
        if(axisX)
            figs->rotacoes[contaR].aX = axisX;

        titleElement->QueryFloatAttribute("axisY",&axisY);
        if(axisY)
            figs->rotacoes[contaR].aY = axisY;

        titleElement->QueryFloatAttribute("axisZ",&axisZ);
        if(axisZ)
            figs->rotacoes[contaR].aZ = axisZ;

        titleElement = titleElement->NextSiblingElement();
        contaR++;
    }

    if(strcmp(titleElement->Value(),"scale") == 0)
    {
        figs->escalas = (scale*) malloc(sizeof(scale*));
        figs->escalas->id = contaIds;

        float xS,yS,zS;
        titleElement->QueryFloatAttribute("X",&xS);
        if(xS)
            figs->escalas[contaS].x = xS;

        titleElement->QueryFloatAttribute("Y",&yS);
        if(yS)
            figs->escalas[contaS].y = yS;

        titleElement->QueryFloatAttribute("Z",&zS);
        if(zS)
            figs->escalas[contaS].z = zS;
        titleElement = titleElement->NextSiblingElement();
        contaS++;
    }

    if(strcmp(titleElement->Value(),"models") == 0)
    {
        //ids = (int *) (realloc(ids, sizeof(ids) * 2));
        //ids[posIds] = contaIds;
        printf("1\n");
        readModel(titleElement);
        printf("2\n");
        posIds++;
        contaIds++;
    }

    titleElement = titleElement->NextSiblingElement();
    while (titleElement && strcmp(titleElement->Value(),"group") == 0)
    {
        readGroup(titleElement, figs,ids);
        posIds--;
        titleElement = titleElement->NextSiblingElement();
    }
    printf("End Group\n");
}




void readXml()
{
    int* ids = (int*) malloc(sizeof(int*));
    ids[0] = -1;

    figures* figs = (figures*) malloc(sizeof(figures*));
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

    printf("The end!\n");
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

vertice* ler(const char *ficheiro) {

    FILE *fp = fopen(ficheiro, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    int point = stoi(line);
    struct vertice* arr = (vertice *) malloc(sizeof(struct vertice) * (point+1));
    arr[0].ponto = point;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    vector<string> tokens;
    int i = 1;
    while ((getline(&line, &len, fp)) != -1) {
        std::string lineS(line);
        tokens = split(lineS, " ");
        arr[i].x = stof(tokens[0]);
        arr[i].y = stof(tokens[1]);
        arr[i].z = stof(tokens[2]);
        //printf("%f %f %f %d %d\n",arr[i].x,arr[i].y,arr[i].z,i,arr[0].ponto);
        i++;
    }
    fclose(fp);
    if (line)
        free(line);
    return arr;
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

void draw(int points,vertice* arr){
    drawAxis();

    glBegin(GL_LINES);

    for(int i = 1; i<=points;i+=3){
        glColor3f(i,0,i);

        glVertex3f(arr[i].x,arr[i].y,arr[i].z);
        glVertex3f(arr[i+1].x,arr[i+1].y,arr[i+1].z);

        glVertex3f(arr[i+1].x,arr[i+1].y,arr[i+1].z);
        glVertex3f(arr[i+2].x,arr[i+2].y,arr[i+2].z);

        glVertex3f(arr[i+2].x,arr[i+2].y,arr[i+2].z);
        glVertex3f(arr[i].x,arr[i].y,arr[i].z);
    }
    glEnd();
}

void renderScene() {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(5.0,5.0,5.0,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);

    // put the geometric transformations here


    // put drawing instructions here
    glTranslatef(posx, posy, posz);
    glRotatef(angle, 0, 1, 0);
    glScalef(xScale, yScale, zScale);

    vertice* array = ler(fileString[ite]);
    ite++;
    draw(array[0].ponto,array);

    // End of frame
    glutSwapBuffers();
}

// write function to process keyboard events
void reactKeyboard(unsigned char c, int x, int y) {

	switch (c) {
	case 'a':
		posx -= 0.1f;
		break;
	case 'd':
		posx += 0.1f;
		break;
	case 'w':
		posy += 0.1f;
		break;
	case 's':
		posy -= 0.1f;
		break;
	case 32:
		angle += 1.5;
		break;
	case 'z':
		posz += 0.1f;
		break;
	case 'x':
		posz -= 0.1f;
		break;
	case '+':
		yScale += 0.1f;
		break;
	case '-':
		yScale -= 0.1f;
		break;
	case '1':
		drawType = GL_LINE;
		break;
	case '2':
		drawType = GL_FILL;
		break;
	case '3':
		drawType = GL_POINT;
	}

	glutPostRedisplay();
}


int main(int argc, char **argv) {

    /*
    glutInit(&argc, argv);
    char** listas = readXml();
    int len;
    for(len = 0; listas[len]; len++);
    char filesToOpen[len][30];
    for(int i = 0; i < len; i++)
    {
        char aux[30] = "./";
        strcat(aux,listas[i]);
        strcpy(filesToOpen[i],aux);
    }

    fileString = (char**) malloc(sizeof(char*) * len);
    GLint windows[len];
    for(int i = 0; i < len; i++)
    {
        fileString[i] = (char*) malloc(sizeof(char) * 30);
        strcpy(fileString[i],filesToOpen[i]);


        // init GLUT and the window
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowPosition(100, 100);
        glutInitWindowSize(800, 800);
        windows[i] = glutCreateWindow("CG");

        // Required callback registry
        glutDisplayFunc(renderScene);
        glutReshapeFunc(changeSize);

        // put here the registration of the keyboard callbacks
        //glutKeyboardFunc(reactKeyboard);

        //  OpenGL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        // enter GLUT's main cycle

    }
    ite = 0;
    glutMainLoop();*/

    readXml();

    return 0;
}
