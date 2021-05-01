/* Describes the physical properties of a fluid cube 
 * Each individual cube will have uniform properties */
#pragma once

struct FluidCube{
		float p = 10; // pressure
		float visc = 0.0001f; // viscosity
		float density = 100;
		float diff = 0.0001f; // amount of diffusion
		float dt = 0.02; // time interval
		float uX = 200; // velocity in x direction
		float uY = 2000; // velocity in y direction
		float uX0 = 200; //initial velocity in the x direction
		float uY0 = 2000; //initial velocity in the y direction
		float s = 0;
};
