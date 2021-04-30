#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"
#include "fluidcube.h"

#define kITER 2

class Gui{
	private: 
		sf::RenderWindow window;
		Mesh mesh;
		std::vector<FluidCube> fluidCube;
	public:
		Gui();
		~Gui();
		void Run();
		void setBnd(int b, float *x, int nX, int nY);
		void linSolve(int b, float *x, float *x0, float a, float c, int nX, int nY);
		void fluidCubeStep();
		void diffuse (int b, float *x, float *x0, float diff, float dt, int iter, int nX, int nY);
		void project(float *velocX, float *velocY, float *p, float *div, int iter, int nX, int nY);
		void advect(int b, float *d, float *d0,  float *velocX, float *velocY, float dt, int nX, int nY);
		
		void addDensity(sf::Vector2i coords, int amount);
};
