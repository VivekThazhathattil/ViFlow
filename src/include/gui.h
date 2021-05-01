#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"
#include "fluidcube.h"

#define kITER 16

class Gui{
	private: 
		float uX[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];
		float uY[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];
		float uX0[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];
		float uY0[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];
		float s[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];
		float density[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)];

		sf::Font font;
		sf::Text text;
		sf::RenderWindow window;
		Mesh mesh;
		std::vector<FluidCube> fluidCube;

		sf::RectangleShape vf[(MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)]; // for velocity field
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
		void addVelocity(sf::Vector2i coords, int amountX, int amountY);

		void showDetails_stub();
		sf::Vector2i getMousePointedGrid(sf::Vector2i coords);

		void showVelocityField();
};
