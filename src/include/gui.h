#include <SFML/Graphics.hpp>
#include "consts.h"
#include "mesh.h"

class Gui{
	public: 
		sf::RenderWindow window;
		Mesh mesh;
		Gui();
		~Gui();
		void Run();
};
