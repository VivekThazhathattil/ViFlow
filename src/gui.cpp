#include "include/gui.h"
#include <stdio.h>

Gui::Gui() : mesh(MESH_SIZE_X, MESH_SIZE_Y, GRID_SIZE_X, GRID_SIZE_Y),\
		     window(sf::RenderWindow(sf::VideoMode(WINDOW_X, WINDOW_Y), "ViFDM",sf::Style::Close)) {} 
Gui::~Gui(){};
void Gui::Run(){
	// calculate the spacing required to position the mesh in the screen center
	this->mesh.getMeshLoc(WINDOW_X, WINDOW_Y);
	// make rectangles and put them in the mesh
	// make rectangles:
	std::vector<sf::RectangleShape> shapeGrid(this->mesh.numI * this->mesh.numJ);
	// put the rectangles in the mesh: 	
//	printf(" mesh.numI = %d\n mesh.numJ = %d\n WINDOW_X = %d\n WINDOW_Y = %d\n meshLocX = %d\n meshLocY = %d\n", this->mesh.numI,\
			this->mesh.numJ, WINDOW_X, WINDOW_Y, this->mesh.meshLocX, this->mesh.meshLocY);
	for (int i = 0; i < this->mesh.numI; i++){
		for (int j = 0; j < this->mesh.numJ; j++){
			shapeGrid[this->mesh.IDX(i,j)].setPosition(\
					this->mesh.meshLocX + i*this->mesh.gridSizeX,\
					this->mesh.meshLocY + j*this->mesh.gridSizeY\
					);
//			printf("(%d,%d)\n", this->mesh.meshLocX + i*this->mesh.gridSizeX, this->mesh.meshLocY + j*this->mesh.gridSizeY);
//			printf("(%d,%d)\n",i,j);
//			printf(" this->mesh.meshLocX = %d, i = %d, this->mesh.gridSizeX = %d, i*this->mesh.gridSizeX = %d", this->mesh.meshLocX, i, this->mesh.gridSizeX, i*this->mesh.gridSizeX);
			if ( (i+j)%2 == 0) 
				shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color(rand()%255,rand()%255,rand()%255));
				//shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color::Black);
			shapeGrid[this->mesh.IDX(i,j)].setSize(sf::Vector2f(this->mesh.gridSizeX, this->mesh.gridSizeY));
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

