/* Describes the physical properties of a fluid cube 
 * Each individual cube will have uniform properties */
#pragma once

struct FluidCube{
	private:
		float p = 0; // pressure
		float visc = 0; // viscosity
		float density = 0;
		float diff = 0; // amount of diffusion
		float dt = 0.2; // time interval
		float uX = 0; // velocity in x direction
		float uY = 0; // velocity in y direction
		float uX0 = 0; //initial velocity in the x direction
		float uY0 = 0; //initial velocity in the y direction
};