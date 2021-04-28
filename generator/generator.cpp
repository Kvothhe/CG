using namespace std;

#include <string>
#include "generator.h"
#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstring>
#include "bezier.h"

void write(int counter, struct vertice arr[], std::string path) {
	ofstream myfile;
	myfile.open(path);

	myfile << counter << endl;

	for (int count = 0; count < counter; count++) {
		myfile << arr[count].x << " " << arr[count].y << " " << arr[count].z << endl;
	}

	myfile.close();

}

void plane(float size, std::string path) {
	float side = size / 2;

	struct vertice arr[6]{};

	float x1, y1, z1;


	arr[0].x = side; arr[0].y = 0.0f; arr[0].z = side;
	arr[1].x = side; arr[1].y = 0.0f; arr[1].z = -side;
	arr[2].x = -side; arr[2].y = 0.0f; arr[2].z = -side;

	arr[3].x = side; arr[3].y = 0.0f; arr[3].z = side;
	arr[4].x = -side; arr[4].y = 0.0f; arr[4].z = -side;
	arr[5].x = -side; arr[5].y = 0.0f; arr[5].z = side;

	//printf("%f", arr[0].x);

	//escreve x,y,z para o ficheiro path

	write(6, arr,path);
	
}

void box(float length, float width, float height,std::string path) {
	float x = length / 2;
	float y = height / 2;
	float z = width / 2;

	struct vertice arr[36]{};

	//aponta para +x
	arr[0].x = x; arr[0].y = -y; arr[0].z = z;
	arr[1].x = x; arr[1].y = -y; arr[1].z = -z;
	arr[2].x = x; arr[2].y = y; arr[2].z = -z;

	arr[3].x = x; arr[3].y = y; arr[3].z = z;
	arr[4].x = x; arr[4].y = -y; arr[4].z = z;
	arr[5].x = x; arr[5].y = y; arr[5].z = -z;

	//aponta para -x
	arr[6].x = -x; arr[6].y = -y; arr[6].z = -z;
	arr[7].x = -x; arr[7].y = -y; arr[7].z = z;
	arr[8].x = -x; arr[8].y = y; arr[8].z = -z;

	arr[9].x = -x; arr[9].y = -y; arr[9].z = z;
	arr[10].x = -x; arr[10].y = y; arr[10].z = z;
	arr[11].x = -x; arr[11].y = y; arr[11].z = -z;

	//aponta para +z
	arr[12].x = -x; arr[12].y = -y; arr[12].z = z;
	arr[13].x = x; arr[13].y = -y; arr[13].z = z;
	arr[14].x = x; arr[14].y = y; arr[14].z = z;

	arr[15].x = -x; arr[15].y = -y; arr[15].z = z;
	arr[16].x = x; arr[16].y = y; arr[16].z = z;
	arr[17].x = -x; arr[17].y = y; arr[17].z = z;

	//aponta para -z
	arr[18].x = x; arr[18].y = -y; arr[18].z = -z;
	arr[19].x = -x; arr[19].y = -y; arr[19].z = -z;
	arr[20].x = x; arr[20].y = y; arr[20].z = -z;

	arr[21].x = x; arr[21].y = y; arr[21].z = -z;
	arr[22].x = -x; arr[22].y = -y; arr[22].z = -z;
	arr[23].x = -x; arr[23].y = y; arr[23].z = -z;

	//aponto para +y
	arr[24].x = -x; arr[24].y = y; arr[24].z = z;
	arr[25].x = x; arr[25].y = y; arr[25].z = z;
	arr[26].x = x; arr[26].y = y; arr[26].z = -z;

	arr[27].x = -x; arr[27].y = y; arr[27].z = z;
	arr[28].x = x; arr[28].y = y; arr[28].z = -z;
	arr[29].x = -x; arr[29].y = y; arr[29].z = -z;

	//aponto para -y
	arr[30].x = x; arr[30].y = -y; arr[30].z = z;
	arr[31].x = -x; arr[31].y = -y; arr[31].z = z;
	arr[32].x = x; arr[32].y = -y; arr[32].z = -z;

	arr[33].x = x; arr[33].y = -y; arr[33].z = -z;
	arr[34].x = -x; arr[34].y = -y; arr[34].z = z;
	arr[35].x = -x; arr[35].y = -y; arr[35].z = -z;

	write(36, arr,path);
}

void sphere(float r, int slices, int stacks,std::string path) {

	float alpha = -(M_PI / 2); // [-pi/2,pi/2] angulo vertical

	float deslAlpha = M_PI / slices;
	float deslBeta = 2 * M_PI / stacks;

	int count = ((stacks - 2) * slices * 6) + (2 * slices * 3);

	struct vertice* arr = (vertice *) malloc(sizeof(struct vertice) * count);

	int aux = 0;

	// desenhar os triangulos do fundo

	float beta = 0; // [0,2pi] angulo horizontal
	for (beta; beta < (2 * M_PI); beta += deslBeta) {
		arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta + deslBeta)); arr[aux].y = r * sin(alpha + deslAlpha); arr[aux].z = ((r * cos(alpha + deslAlpha))* sin(beta + deslBeta));
		aux++;
		arr[aux].x = ((r * cos(alpha)) * cos(beta + deslBeta)); arr[aux].y = -r; arr[aux].z = (r * cos(alpha))* sin(beta + deslBeta);
		aux++;
		arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta)); arr[aux].y = r * sin(alpha + deslAlpha); arr[aux].z = (r * cos(alpha + deslAlpha))* sin(beta);
		aux++;
	}

	alpha += deslAlpha;

	// desenhar os triangulos das laterais
	 
	for (alpha; alpha < (M_PI / 2) - deslAlpha; alpha += deslAlpha) {
		float beta = 0; // [0,2pi] angulo horizontal
		for (beta; beta < (2 * M_PI); beta += deslBeta) {
			arr[aux].x = ((r * cos(alpha)) * cos(beta + deslBeta)); arr[aux].y = r * sin(alpha); arr[aux].z = (r * cos(alpha))* sin(beta + deslBeta);
			aux++;
			arr[aux].x = ((r * cos(alpha)) * cos(beta)); arr[aux].y = r * sin(alpha); arr[aux].z = (r * cos(alpha)) * sin(beta);
			aux++;
			arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta + deslBeta)); arr[aux].y = r * sin(alpha + deslAlpha); arr[aux].z = (r * cos(alpha + deslAlpha)) * sin(beta + deslBeta);
			aux++;

			arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta + deslBeta)); arr[aux].y = r * sin(alpha + deslAlpha); arr[aux].z = (r * cos(alpha + deslAlpha)) * sin(beta + deslBeta);
			aux++;
			arr[aux].x = ((r * cos(alpha)) * cos(beta)); arr[aux].y = r * sin(alpha); arr[aux].z = (r * cos(alpha)) * sin(beta);
			aux++;
			arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta)); arr[aux].y = r * sin(alpha + deslAlpha); arr[aux].z = (r * cos(alpha + deslAlpha)) * sin(beta);
			aux++;
		}
	}

	// desenhar os triangulos do topo

	beta = 0; // [0,2pi] angulo horizontal
	for (beta; beta < (2 * M_PI); beta += deslBeta) {
		arr[aux].x = ((r * cos(alpha)) * cos(beta)); arr[aux].y = r * sin(alpha); arr[aux].z = (r * cos(alpha)) * sin(beta);
		aux++;
		arr[aux].x = ((r * cos(alpha + deslAlpha)) * cos(beta + deslBeta)); arr[aux].y = r; arr[aux].z = (r * cos(alpha + deslAlpha)) * sin(beta + deslBeta);
		aux++;
		arr[aux].x = ((r * cos(alpha)) * cos(beta + deslBeta)); arr[aux].y = r * sin(alpha); arr[aux].z = (r * cos(alpha)) * sin(beta + deslBeta);
		aux++;

	}

	write(count, arr, path);
}

void cone(float r, float h, int slices, int stacks, std::string path) {
	float alpha = 0;
	float deslAlpha = 2 * M_PI / slices;
	float deslHeight = h / stacks;
	float deslRadius = r / stacks;

	int count = ((stacks - 1) * slices * 6) + (2 * slices * 3);

	struct vertice* arr = (vertice*)malloc(sizeof(struct vertice) * count);

	int aux = 0;

	// desenhar a base
	for (;alpha < 2 * M_PI;alpha += deslAlpha) {
		arr[aux].x = r * cos(alpha); arr[aux].y = 0; arr[aux].z = r* sin(alpha);
		aux++;
		arr[aux].x = r * cos(alpha + deslAlpha); arr[aux].y = 0; arr[aux].z = r * sin(alpha + deslAlpha);
		aux++;
		arr[aux].x = 0; arr[aux].y = 0; arr[aux].z = 0;
		aux++;
	}

	float auxR = r;
	float auxH = 0;
	for (int i = 0; i < stacks - 1; i++) {
		auxR -= deslRadius;
		auxH += deslHeight;

		alpha = 0;
		for (;alpha < 2 * M_PI;alpha += deslAlpha) {
			arr[aux].x = (auxR + deslRadius) * cos(alpha + deslAlpha); arr[aux].y = auxH - deslHeight; arr[aux].z = (auxR + deslRadius) * sin(alpha + deslAlpha);
			aux++;
			arr[aux].x = (auxR + deslRadius) * cos(alpha); arr[aux].y = auxH - deslHeight; arr[aux].z = (auxR + deslRadius) * sin(alpha);
			aux++;
			arr[aux].x = auxR * cos(alpha + deslAlpha); arr[aux].y = auxH; arr[aux].z = auxR * sin(alpha + deslAlpha);
			aux++;

			arr[aux].x = auxR * cos(alpha + deslAlpha); arr[aux].y = auxH; arr[aux].z = auxR * sin(alpha + deslAlpha);
			aux++;
			arr[aux].x = (auxR + deslRadius) * cos(alpha); arr[aux].y = auxH - deslHeight; arr[aux].z = (auxR + deslRadius) * sin(alpha);
			aux++;
			arr[aux].x = auxR * cos(alpha); arr[aux].y = auxH; arr[aux].z = auxR * sin(alpha);
			aux++;
		}
	}
	auxR -= deslRadius;
	auxH += deslHeight;
	alpha = 0;

	for (;alpha < 2 * M_PI;alpha += deslAlpha) {
		arr[aux].x = (auxR + deslRadius) * cos(alpha + deslAlpha); arr[aux].y = auxH - deslHeight; arr[aux].z = (auxR + deslRadius) * sin(alpha + deslAlpha);
		aux++;
		arr[aux].x = (auxR + deslRadius) * cos(alpha); arr[aux].y = auxH - deslHeight; arr[aux].z = (auxR + deslRadius) * sin(alpha);
		aux++;
		arr[aux].x = 0; arr[aux].y = h; arr[aux].z = 0;
		aux++;
	}

	write(count, arr, path);
}

int main(int argc, char** argv) {

    if(argc == 4 && strcmp(argv[1],"plane") == 0)
    {
        char* destFile =(char*) malloc(sizeof (char) * 20);
        strcat(destFile, "./");
        strcat(destFile,argv[3]);
        plane(atof(argv[2]),destFile);
    }

    if(argc == 6 && strcmp(argv[1],"sphere") == 0)
    {
        char* destFile =(char*) malloc(sizeof (char) * 20);
        strcat(destFile, "./");
        strcat(destFile,argv[5]);
        sphere(atof(argv[2]),atoi(argv[3]),atoi(argv[4]),destFile);
    }

    if(argc == 6 && strcmp(argv[1],"box") == 0)
    {
        char* destFile =(char*) malloc(sizeof (char) * 20);
        strcat(destFile, "./");
        strcat(destFile,argv[5]);
        box(atof(argv[2]),atof(argv[3]),atof(argv[4]),destFile);
    }

	if(argc == 7 && strcmp(argv[1],"cone") == 0)
	{
	    char* destFile =(char*) malloc(sizeof (char) * 20);
	    strcat(destFile, "./");
	    strcat(destFile,argv[6]);
	    cone(atof(argv[2]),atof(argv[3]), atoi(argv[4]),atoi(argv[5]), destFile);
	}

    if(argc == 4 && strcmp(argv[1],"bezier") == 0)
    {
        bezierPatch bp =  parseBezier(argv[2]);
        generateBezier(bp,atoi(argv[3]));
    }
}

	

