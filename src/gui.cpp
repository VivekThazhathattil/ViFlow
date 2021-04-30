/* Creates RectangleShape objects for the grids
 * Places the rectangleShape objects in the correct positions of the mesh
 * Randomises the colors of each alternate grids
 * TODO: write the lin_solv, diffuse, advect, project, step functions as just individual functions not associated with any class
 * TODO: find out what the purpose of s is
 */
#define ADD_DENSITY 200
#include "include/gui.h"
#include <stdio.h>
#include <math.h>

void Gui::fluidCubeStep()
{
    int nX          = this->mesh.numI;
    int nY          = this->mesh.numJ;
    float visc     = this->fluidCube[0].visc;
    float diff     = this->fluidCube[0].diff;
    float dt       = this->fluidCube[0].dt;

/*    std::vector<float> uX, uY, uX0, uY0, s, density;

    for (int i = 0; i < this->fluidCube.size(); i++){
	    uX.push_back(this->fluidCube[i].uX);
	    uY.push_back(this->fluidCube[i].uY);
	    uX0.push_back(this->fluidCube[i].uX0);
	    uY0.push_back(this->fluidCube[i].uY0);
	    s.push_back(this->fluidCube[i].s);
	    density.push_back(this->fluidCube[i].density);
    }*/

    float uX[this->fluidCube.size()], \
	    uY[this->fluidCube.size()], \
	    uX0[this->fluidCube.size()], \
	    uY0[this->fluidCube.size()], \
	    s[this->fluidCube.size()], \
	    density[this->fluidCube.size()];

    for (int i = 0; i < this->fluidCube.size(); i++){
	    uX[i] = this->fluidCube[i].uX;
	    uY[i] = this->fluidCube[i].uY;
	    uX0[i] = this->fluidCube[i].uX0;
	    uY0[i] = this->fluidCube[i].uY0;
	    s[i] = this->fluidCube[i].s;
	    density[i] = this->fluidCube[i].density;
    }

    this->diffuse(1, uX, uX0, visc, dt, kITER, nX, nY);
    this->diffuse(2, uY, uY0, visc, dt, kITER, nX, nY);

    this->project(uX0, uY0, uX, uY, kITER, nX, nY);

    this->advect(1, uX, uX0, uX0, uY0, dt, nX, nY);
    this->advect(2, uY, uY0, uX0, uY0, dt, nX, nY);

    this->project(uX, uY, uX0, uY0, kITER, nX, nY);

    this->diffuse(0, s, density, diff, dt, kITER, nX, nY);
    this->advect(0, density, s, uX, uY, dt, nX, nY);
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
}

void Gui::diffuse (int b, float *x, float *x0, float diff, float dt, int iter, int nX, int nY) 
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
		     window(sf::RenderWindow(sf::VideoMode(WINDOW_X, WINDOW_Y), "ViFDM",sf::Style::Close)) {} 
Gui::~Gui(){};
void Gui::Run(){
	sf::Vector2i currMouseCoords = sf::Mouse::getPosition(this->window);
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
		}

		this->window.clear();

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			printf("Left Mouse Button pressed!");
			currMouseCoords = sf::Mouse::getPosition(this->window);
			this->addDensity(currMouseCoords, ADD_DENSITY);
		}

		this->fluidCubeStep();
		for (int i = 0; i < this->mesh.numI; i++){
			for (int j = 0; j < this->mesh.numJ; j++){
				shapeGrid[this->mesh.IDX(i,j)].setFillColor(\
						sf::Color(255, 255, 255, (this->fluidCube[this->mesh.IDX(i,j)].density > 255) \
						? 255 : this->fluidCube[this->mesh.IDX(i,j)].density)
					);
				this->window.draw(shapeGrid[this->mesh.IDX(i,j)]);
			}
		}
		this->window.display();
	}
}


void Gui::addDensity(sf::Vector2i coords, int amount){
	// get the grid IDX for the mouse click location
	int i,j;
	for (i = 0; i < this->mesh.numI - 1; i++){
		for (j = 0; j < this->mesh.numJ - 1; j++){
		if (coords.x > this->mesh.meshLocX + this->mesh.gridSizeX * i  &&\
				coords.x < this->mesh.meshLocX + this->mesh.gridSizeX * (i+1) &&\
				coords.y > this->mesh.meshLocY + this->mesh.gridSizeY * i &&\
				coords.y < this->mesh.meshLocY + this->mesh.gridSizeY * (i+1))
			break;
		}
	}
	if ( i != this->mesh.numI - 1 && j != this->mesh.numJ - 1)
		this->fluidCube[this->mesh.IDX(i,j)].density += amount;
}
