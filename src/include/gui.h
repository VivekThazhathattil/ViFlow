#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"
#include "fluidcube.h"

#define kITER 16

class Gui{
	private: 
		FluidCube *cube;

		sf::Font font;
		sf::Text text;
		sf::RenderWindow window;
		Mesh mesh;

		sf::RectangleShape vf[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)]; // for velocity field
	public:
		Gui();
		~Gui();

		void Run();

		static void setBnd(int b, float *x, int nX, int nY);
		static void linSolve(int b, float *x, float *x0, float a, float c, int iter, int nX, int nY);
		void fluidCubeStep(FluidCube *cube);
		static void diffuse (int b, float *x, float *x0, float diff, float dt, int iter, int nX, int nY);
		static void project(float *velocX, float *velocY, float *p, float *div, int iter, int nX, int nY);
		static void advect(int b, float *d, float *d0,  float *velocX, float *velocY, float dt, int nX, int nY);
		int clamp (int val, int min, int max);

		void addDensity(sf::Vector2i coords, int amount);
		void addVelocity(sf::Vector2i coords, int amountX, int amountY);

		void showDetails_stub();
		sf::Vector2i getMousePointedGrid(sf::Vector2i coords);

		void showVelocityField();
};
