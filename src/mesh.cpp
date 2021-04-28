#include "include/mesh.h"

#include <stdio.h>

Mesh::Mesh(int mX, int mY, int gX, int gY){
	this->meshSizeX = mX; // size of the mesh in X direction
	this->meshSizeY = mY; // size of the mesh in Y direction
	this->gridSizeY = gX; // size of a single grid in Y direction
	this->gridSizeY = gY; // size of a single grid in X direction
	this->numI = (int) (mX / gX);
	this->numJ = (int) (mY / gY);
}

Mesh::~Mesh(){}

void Mesh::list_values_stub(){
	printf("%d, %d, %d, %d", this->meshSizeX, this->meshSizeY, this->gridSizeX, this->gridSizeY); 	
}

void Mesh::getMeshLoc(int wX, int wY){
	this->meshLocX = (int)((wX - this->meshSizeX)/2);
	this->meshLocY = (int)((wY - this->meshSizeY)/2);
}

int Mesh::IDX(int *x, int *y){
	return *x + this->numI * (*y);
}
