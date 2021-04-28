#include "include/gui.h"

Gui::~Gui(){};
void Gui::Run(){
	this->window(sf::RenderWindow(sf::VideoMode(WINDOW_X, WINDOW_Y), "ViFDM", sf::Style::Titlebar | sf::Style::Close));
	// calculate the spacing required to position the mesh in the screen center
	this->mesh.getMeshLoc(WINDOW_X, WINDOW_Y);
	// make rectangles and put them in the mesh
	// make rectangles:
//	sf::RectangleShape shapeGrid(sf::Vector2f(this->mesh.gridSizeX, this->mesh.gridSizeY)[this->mesh.numI * this->mesh.numJ];
	std::vector<sf::RectangleShape> shapeGrid(this->mesh.numI * this->mesh.numJ);
	// put the rectangles in the mesh: 	
	for (int i = 0; i < this->mesh.numI; i++){
		for (int j = 0; j < this->mesh.numJ; j++){
			shapeGrid[this->mesh.IDX(i,j)].setPosition(\
					this->mesh.meshLocX + i*this->mesh.gridSizeX,\
					this->mesh.meshLocY + j*this->mesh.gridSizeY\
					);
			shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color::White);
		}
	}
		
	while (this->window.isOpen())
	{

		sf::Event e;
		while(window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				this->window.close();
		}

		this->window.clear();

		for (int i = 0; i < this->mesh.numI; i++){
			for (int j = 0; j < this->mesh.numJ; j++){
				this->window.draw(shapeGrid[this->mesh.IDX(i,j)]);
			}
		}
		this->window.display();
	}
}

