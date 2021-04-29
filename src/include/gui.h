#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"
#include "physics.h"
#include "fluidcube.h"
#include <vector.h>

class Gui{
	private: 
		sf::RenderWindow window;
		Mesh mesh;
		Physics physics;
		std::vector<FluidCube> fluidCube;
	public:
		Gui();
		~Gui();
		void Run();
};
