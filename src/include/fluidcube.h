/* Describes the physical properties of a fluid cube 
 * Each individual cube will have uniform properties */
#include <stdlib.h> // malloc, free 
#pragma once

struct FluidCube{
	int nX;
	int nY;
	float dt;   // timestep
	float diff; // diffusion
	float visc; // viscosity
	
	float *s;   
	float *density;
	
	float *uX;
	float *uY;
	
	// previous velocities
	float *uX0;
	float *uY0;

}; 

typedef struct FluidCube FluidCube;

