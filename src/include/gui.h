#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"
class Gui{
	private:
		sf::RenderWindow window;
	public: 
		Gui() : mesh(MESH_SIZE_X, MESH_SIZE_Y, hX, hY){};
		Mesh mesh;
		~Gui();
		void Run();
};
