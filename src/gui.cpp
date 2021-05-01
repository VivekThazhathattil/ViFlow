/* Creates RectangleShape objects for the grids
 * Places the rectangleShape objects in the correct positions of the mesh
 * Randomises the colors of each alternate grids
 * TODO: write the lin_solv, diffuse, advect, project, step functions as just individual functions not associated with any class
 * TODO: find out what the purpose of s is
 */
#define ADD_DENSITY 500
#include "include/gui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

void Gui::fluidCubeStep()
{
    int nX          = this->mesh.numI;
    int nY          = this->mesh.numJ;
    float visc     = this->fluidCube[0].visc;
    float diff     = this->fluidCube[0].diff;
    float dt       = this->fluidCube[0].dt;

    for (int i = 0; i < this->fluidCube.size(); i++){
	    this->uX[i] = this->fluidCube[i].uX;
	    this->uY[i] = this->fluidCube[i].uY;
	    this->uX0[i] = this->fluidCube[i].uX0;
	    this->uY0[i] = this->fluidCube[i].uY0;
	    this->s[i] = this->fluidCube[i].s;
	    this->density[i] = this->fluidCube[i].density;
    }

    for(int i = 0; i < this->mesh.numI; i++){
	    for(int j = 0; j < this->mesh.numJ; j++){
		    this->s[this->mesh.IDX(i,j)] = this->fluidCube[this->mesh.IDX(i,j)].density;
		    this->fluidCube[this->mesh.IDX(i,j)].s = this->fluidCube[this->mesh.IDX(i,j)].density;
		    this->uX0[this->mesh.IDX(i,j)] = this->fluidCube[this->mesh.IDX(i,j)].uX;
		    this->uY0[this->mesh.IDX(i,j)] = this->fluidCube[this->mesh.IDX(i,j)].uY;
	    }
    }

    this->diffuse(1, this->uX0, this->uX, visc, dt, kITER, nX, nY);
    this->diffuse(2, this->uY0, this->uY, visc, dt, kITER, nX, nY);

    this->project(this->uX0, this->uY0, this->uX, this->uY, kITER, nX, nY);

    this->advect(1, this->uX, this->uX0, this->uX0, this->uY0, dt, nX, nY);
    this->advect(2, this->uY, this->uY0, this->uX0, this->uY0, dt, nX, nY);

    this->project(this->uX, this->uY, this->uX0, this->uY0, kITER, nX, nY);

    this->diffuse(0, this->s, this->density, diff, dt, kITER, nX, nY);
    for(int i = 0; i < this->mesh.numI; i++){
	    for(int j = 0; j < this->mesh.numJ; j++){
		    this->fluidCube[this->mesh.IDX(i,j)].density = this->density[this->mesh.IDX(i,j)];
		    this->fluidCube[this->mesh.IDX(i,j)].uX =  this->uX[this->mesh.IDX(i,j)];
		    this->fluidCube[this->mesh.IDX(i,j)].uY =  this->uY[this->mesh.IDX(i,j)];
	    }
    }
    this->advect(0, this->density, this->s, this->uX, this->uY, dt, nX, nY);
}

void Gui::setBnd(int b, float *x, int nX, int nY)
{
    switch(b){
	    case 1: //top and bottom boundaries
		    for (int i = 0; i < nX; i++){
			    x[this->mesh.IDX(i,0)] = -x[this->mesh.IDX(i,0)];
			    x[this->mesh.IDX(i,nY-1)] = -x[this->mesh.IDX(i,nY-1)];
		    }
		    break;
	    case 2: // left and right boundaries
		    for (int j = 0; j < nY; j++){
			    x[this->mesh.IDX(0,j)] = -x[this->mesh.IDX(0,j)];
			    x[this->mesh.IDX(nX-1,j)] = -x[this->mesh.IDX(nX-1,j)];
		    }
		    break;
    }

	// averaging corner values
	x[this->mesh.IDX(0, 0)] 	= 0.5f * (x[this->mesh.IDX(1, 0)] + x[this->mesh.IDX(0, 1)]);
	x[this->mesh.IDX(0, nY-1)]	= 0.5f * (x[this->mesh.IDX(1, nY-1)] + x[this->mesh.IDX(0, nY-2)]);
	x[this->mesh.IDX(nX-1, 0)]	= 0.5f * (x[this->mesh.IDX(nX-2,0)] + x[this->mesh.IDX(nX-1, 1)]);
	x[this->mesh.IDX(nX-1, nY-1)] = 0.5f * (x[this->mesh.IDX(nX-2, nY-1)] + x[this->mesh.IDX(nX-1, nY-2)]);
}

void Gui::linSolve(int b, float *x, float *x0, float a, float c, int nX, int nY)
{
    float cRecip = 1.0 / c;
    for (int k = 0; k < kITER; k++) {
            for (int j = 1; j < nY - 1; j++) {
                for (int i = 1; i < nX - 1; i++) {
                    x[this->mesh.IDX(i, j)] =
                        (x0[this->mesh.IDX(i, j)]
                            + a*(    x[this->mesh.IDX(i+1, j)]
                                    +x[this->mesh.IDX(i-1, j)]
                                    +x[this->mesh.IDX(i  , j+1)]
                                    +x[this->mesh.IDX(i  , j-1)]
                           )) * cRecip;
                }
            }
        setBnd(b, x, nX, nY);
    }
    /*for (int i = 0; i < this->mesh.numI; i++){
	    for (int j = 0;j < this->mesh.numJ; j++){
		    printf("%f ",this->fluidCube[this->mesh.IDX(i,j)].density);
	    }
	    printf("\n");
    }
    printf("\n\n");*/
}

void Gui::diffuse (int b, float *x0, float *x, float diff, float dt, int iter, int nX, int nY) 
{
    float a = dt * diff * (nX - 2) * (nY - 2);
    linSolve(b, x, x0, a, 1 + 6 * a, nX, nY);
}

void Gui::project(float *velocX, float *velocY, float *p, float *div, int iter, int nX, int nY)
{
        for (int j = 1; j < nY - 1; j++) {
            for (int i = 1; i < nX - 1; i++) {
                div[this->mesh.IDX(i, j)] = -0.5f*(
                         velocX[this->mesh.IDX(i+1, j)]
                        -velocX[this->mesh.IDX(i-1, j)]
                        +velocY[this->mesh.IDX(i  , j+1)]
                        -velocY[this->mesh.IDX(i  , j-1)]
                    )/(0.5f*(nX+nY));
                p[this->mesh.IDX(i, j)] = 0;
            }
        }
    setBnd(0, div, nX, nY); 
    setBnd(0, p, nX, nY);
    linSolve(0, p, div, 1, 6, nX, nY);
    
        for (int j = 1; j < nY - 1; j++) {
            for (int i = 1; i < nX - 1; i++) {
                velocX[this->mesh.IDX(i, j)] -= 0.5f * (  p[this->mesh.IDX(i+1, j)]
                                                -p[this->mesh.IDX(i-1, j)]) * nX;
                velocY[this->mesh.IDX(i, j)] -= 0.5f * (  p[this->mesh.IDX(i, j+1)]
                                                -p[this->mesh.IDX(i, j-1)]) * nY;
            }
        }
    setBnd(1, velocX, nX, nY);
    setBnd(2, velocY, nX, nY);
}

void Gui::advect(int b, float *d, float *d0,  float *velocX, float *velocY, float dt, int nX, int nY)
{
    float i0, i1, j0, j1;
    
    float dtx = dt * (nX - 2);
    float dty = dt * (nY - 2);
    
    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;
    
    float nXfloat = nX; 
    float nYfloat = nY; 
    float ifloat, jfloat;
    int i, j;
    
        for(j = 1, jfloat = 1; j < nY - 1; j++, jfloat++) { 
            for(i = 1, ifloat = 1; i < nX - 1; i++, ifloat++) {
                tmp1 = dtx * velocX[this->mesh.IDX(i, j)];
                tmp2 = dty * velocY[this->mesh.IDX(i, j)];
                x    = ifloat - tmp1; 
                y    = jfloat - tmp2;
                
                if(x < 0.5f) x = 0.5f; 
                if(x > nXfloat + 0.5f) x = nXfloat + 0.5f; 
                i0 = floorf(x); 
                i1 = i0 + 1.0f;
                if(y < 0.5f) y = 0.5f; 
                if(y > nYfloat + 0.5f) y = nYfloat + 0.5f; 
                j0 = floorf(y);
                j1 = j0 + 1.0f; 
                
                s1 = x - i0; 
                s0 = 1.0f - s1; 
                t1 = y - j0; 
                t0 = 1.0f - t1;
                
                int i0i = i0;
                int i1i = i1;
                int j0i = j0;
                int j1i = j1;
                
                d[this->mesh.IDX(i, j)] = 
                
                    s0 * ( t0 * (d0[this->mesh.IDX(i0i, j0i)]
                                + d0[this->mesh.IDX(i0i, j0i)])
                        +( t1 * (d0[this->mesh.IDX(i0i, j1i)]
                                + d0[this->mesh.IDX(i0i, j1i)])))
                   +s1 * ( t0 * (d0[this->mesh.IDX(i1i, j0i)]
                                + d0[this->mesh.IDX(i1i, j0i)])
                        +( t1 * (d0[this->mesh.IDX(i1i, j1i)]
                                + d0[this->mesh.IDX(i1i, j1i)])));
            }
        }
    setBnd(b, d, nX, nY);
}

Gui::Gui() : mesh(MESH_SIZE_X, MESH_SIZE_Y, GRID_SIZE_X, GRID_SIZE_Y),\
/*		     vf(sf::LineStrip, (MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)),\ */
			   window(sf::RenderWindow(sf::VideoMode(WINDOW_X, WINDOW_Y), "ViFlow",sf::Style::Close)){}
Gui::~Gui(){}
void Gui::Run(){

	//f key pressed
	bool fpressed = false;

	// load font 
        if (!this->font.loadFromFile("res/arial.ttf"))
        {
            // error...
        }
	this->text.setFont(this->font);
	this->text.setCharacterSize(14); // in pixels, not points!
        this->text.setPosition(10, this->window.getSize().y - 14 - 100);

	int amountX = 0, amountY = 0;
	int flag = 1;
	sf::Vector2i currMouseCoords = sf::Mouse::getPosition(this->window);
	sf::Vector2i prevMouseCoords = currMouseCoords;
	FluidCube fC; // blue print
	// calculate the spacing required to position the mesh in the screen center
	this->mesh.getMeshLoc(WINDOW_X, WINDOW_Y);
	// make rectangles and put them in the mesh
	std::vector<sf::RectangleShape> shapeGrid(this->mesh.numI * this->mesh.numJ);
	for (int i = 0; i < this->mesh.numI; i++){
		for (int j = 0; j < this->mesh.numJ; j++){

			// make a fluidCube for each grid
			fluidCube.push_back(fC);
			// match each rectangle with its grid position
			shapeGrid[this->mesh.IDX(i,j)].setPosition(\
					this->mesh.meshLocX + i*this->mesh.gridSizeX,\
					this->mesh.meshLocY + j*this->mesh.gridSizeY\
					);
			shapeGrid[this->mesh.IDX(i,j)].setOutlineThickness(1);
			shapeGrid[this->mesh.IDX(i,j)].setOutlineColor(sf::Color(255,255,255,20));
			shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color::Black);

//			 debug snippet to visualize the raw grid  */
/*			if ( (i+j)%2 == 0) 
				shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color(rand()%255,rand()%255,rand()%255)); */

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
			else if(e.type == sf::Event::KeyPressed)
				if(e.key.code == sf::Keyboard::F){
					if(!fpressed)
						fpressed = true;
					else
						fpressed = false;
				}
		}

		this->window.clear();

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			//printf("Left Mouse Button pressed!\n");
			currMouseCoords = sf::Mouse::getPosition(this->window);
			this->addDensity(currMouseCoords, ADD_DENSITY);
		}
		currMouseCoords = sf::Mouse::getPosition(this->window);
		amountX = currMouseCoords.x - prevMouseCoords.x;
		amountY = currMouseCoords.y - prevMouseCoords.y;
		this->addVelocity(currMouseCoords, amountX, amountY);
		this->showDetails_stub();
		if (flag == 1){
			this->fluidCube[this->mesh.IDX(30,30)].density += ADD_DENSITY;
			flag = 0;
		}
		this->fluidCubeStep();
		for (int i = 0; i < this->mesh.numI; i++){
			for (int j = 0; j < this->mesh.numJ; j++){
				shapeGrid[this->mesh.IDX(i,j)].setFillColor(\
						sf::Color(255, 255, 255, (this->fluidCube[this->mesh.IDX(i,j)].density > 255) \
						? 255 : this->fluidCube[this->mesh.IDX(i,j)].density)\
					);
				this->window.draw(shapeGrid[this->mesh.IDX(i,j)]);
			}
		}
		prevMouseCoords = currMouseCoords;
		currMouseCoords = this->getMousePointedGrid(currMouseCoords);
        	this->text.setString("Density: " +  \
				std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].density) + \
				"\n" +\
				" uX0: " +\
			       	std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].uX0) +\
				"\n" +\
				" uY0: " +\
			       	std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].uY0) +\
				"\n" +\
				" uX: " +\
			       	std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].uX) +\
				"\n" +\
				" uY: " +\
			       	std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].uY) +\
				"\n" +\
				" s: " +\
			       	std::to_string((int)this->fluidCube[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)].s)\
				);
		this->window.draw(this->text);
		if(fpressed){
			this->showVelocityField();
			for (auto& arrow : this->vf)
			    this->window.draw(arrow);
		}
		this->window.display();
	}
}

sf::Vector2i Gui::getMousePointedGrid( sf::Vector2i coords){
	bool flag = false;
	sf::Vector2i i;
	for (i.x = 0; i.x < this->mesh.numI - 1; i.x++){
		for (i.y = 0; i.y < this->mesh.numJ - 1; i.y++){
		if (coords.x >= this->mesh.meshLocX + this->mesh.gridSizeX * i.x  &&\
				coords.x < this->mesh.meshLocX + this->mesh.gridSizeX * (i.x+1) &&\
				coords.y >= this->mesh.meshLocY + this->mesh.gridSizeY * i.y &&\
				coords.y < this->mesh.meshLocY + this->mesh.gridSizeY * (i.y+1)){
			flag = true;
			break;
		}
		}
		if(flag)
			break;
	}
	return i;
}

void Gui::addDensity(sf::Vector2i coords, int amount){
	int i,j;
	sf::Vector2i val = this->getMousePointedGrid(coords);
	i = val.x;
	j = val.y;
/*	printf("coordx = %d, coordy = %d\n", coords.x, coords.y);
	printf("numI = %d, numJ = %d, meshLocX = %d, meshLocY = %d, gridSizeX = %d, gridSizeY = %d\n",this->mesh.numI, this->mesh.numJ, this->mesh.meshLocX, this->mesh.meshLocY, this->mesh.gridSizeX, this->mesh.gridSizeY);
	printf("i = %d, j = %d \n",i,j);*/
	if ( i != this->mesh.numI - 1 && j != this->mesh.numJ - 1)
		this->fluidCube[this->mesh.IDX(i,j)].density += amount;

//	this->fluidCube[this->mesh.IDX(30,30)].density += amount;
}


void Gui::addVelocity(sf::Vector2i coords, int amountX, int amountY){
	int i,j;
	sf::Vector2i val = this->getMousePointedGrid(coords);
	i = val.x;
	j = val.y;
	if ( i != this->mesh.numI - 1 && j != this->mesh.numJ - 1){
		this->fluidCube[this->mesh.IDX(i,j)].uX += amountX;
		this->fluidCube[this->mesh.IDX(i,j)].uY += amountY;
//		printf("u : (%f,%f)\n",this->fluidCube[this->mesh.IDX(i,j)].uX0,this->fluidCube[this->mesh.IDX(i,j)].uY0);
	}
}

void Gui::showDetails_stub(){
}

void Gui::showVelocityField(){
	float x = 0.0, y = 0.0;
	float radAngle = 0;
	for(int i = 0; i < this->mesh.numI; i++){
		for (int j = 0; j < this->mesh.numJ; j++){
			// get the center location for each rectangle grid
			x = this->mesh.meshLocX + (this->mesh.gridSizeX * i) + this->mesh.gridSizeX/2;
			y = this->mesh.meshLocY + (this->mesh.gridSizeY * j) + this->mesh.gridSizeY/2;
			radAngle = atan2(this->fluidCube[this->mesh.IDX(i,j)].uY, this->fluidCube[this->mesh.IDX(i,j)].uX);
		//	printf("angle = %f\n", 180/M_PI*radAngle);
			this->vf[this->mesh.IDX(i,j)].setSize(sf::Vector2f(10,1));
			this->vf[this->mesh.IDX(i,j)].setPosition(x,y);
			this->vf[this->mesh.IDX(i,j)].setFillColor(sf::Color::Red);
			this->vf[this->mesh.IDX(i,j)].setRotation(180/M_PI*radAngle);
		}

	}
}
