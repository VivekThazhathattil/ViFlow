/* Creates RectangleShape objects for the grids
 * Places the rectangleShape objects in the correct positions of the mesh
 * Randomises the colors of each alternate grids
 * TODO: write the lin_solv, diffuse, advect, project, step functions as just individual functions not associated with any class
 * TODO: find out what the purpose of s is
 */
#include "include/gui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define ADD_DENSITY 200
#define DIFFUSION 0.0001
#define VISCOSITY 0.0001
#define DT 0.2

#define IX(x, y) ((x) + (y) * (MESH_SIZE_X/GRID_SIZE_X))

Gui::Gui() : mesh(MESH_SIZE_X, MESH_SIZE_Y, GRID_SIZE_X, GRID_SIZE_Y),\
/*		     vf(sf::LineStrip, (MESH_SIZE_X/GRID_SIZE_X)*(MESH_SIZE_Y/GRID_SIZE_Y)),\ */
			   window(sf::RenderWindow(sf::VideoMode(WINDOW_X, WINDOW_Y), "ViFlow",sf::Style::Close)){}
Gui::~Gui(){}

FluidCube *FluidCubeCreate(int sizeX, int sizeY, float diffusion, float viscosity, float dt) {
	FluidCube *cube = (FluidCube*)malloc(sizeof(*cube));
	cube->nX = sizeX;
	cube->nY = sizeY;
	
	cube ->dt = dt;
	cube->diff = diffusion;
	cube->visc = viscosity;
	
	cube->s = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	cube->density = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	
	cube->uX = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	cube->uY = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	
	cube->uX0 = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	cube->uY0 = (float*)calloc( cube->nX * cube->nY, sizeof(float));
	
	
	return cube;
}

void FluidCubeFree(FluidCube *cube) {
	free(cube->s);
	free(cube->density);
	
	free(cube->uX);
	free(cube->uY);
	
	free(cube->uX0);
	free(cube->uY0);
	
	free(cube);
}

void Gui::setBnd(int b, float *x, int nX, int nY)
{
    for(int i = 1; i < nX - 1; i++) {
        x[IX(i, 0  )] = b == 2 ? -x[IX(i, 1  )] : x[IX(i, 1  )];
        x[IX(i, nY-1)] = b == 2 ? -x[IX(i, nY-2)] : x[IX(i, nY-2)];
    }
    for(int j = 1; j < nY - 1; j++) {
        x[IX(0  , j)] = b == 1 ? -x[IX(1  , j)] : x[IX(1  , j)];
        x[IX(nX-1, j)] = b == 1 ? -x[IX(nX-2, j)] : x[IX(nX-2, j)];
    }
    
    x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, nY-1)] = 0.5 * (x[IX(1, nY-1)] + x[IX(0, nY-2)]);
	x[IX(nX-1, 0)] = 0.5 * (x[IX(nX-2, 0)] + x[IX(nX-1, 1)]);
    x[IX(nX-1, nY-1)] = 0.5 * (x[IX(nX-2, nY-1)] + x[IX(nX-1, nY-2)]);
}

void Gui::linSolve(int b, float *x, float *x0, float a, float c, int iter, int nX, int nY)
{
    float cRecip = 1.0 / c;
    for (int k = 0; k < iter; k++) {
        for (int j = 1; j < nY - 1; j++) {
            for (int i = 1; i < nX - 1; i++) {
                x[IX(i, j)] =
                    (x0[IX(i, j)]
                        + a*(    x[IX(i+1, j  )]
                                +x[IX(i-1, j  )]
                                +x[IX(i  , j+1)]
                                +x[IX(i  , j-1)]
                       )) * cRecip;
            }
        }
        setBnd(b, x, nX, nY);
    }
}

void Gui::diffuse (int b, float *x, float *x0, float diff, float dt, int iter, int nX, int nY)
{
    float a = dt * diff * (nX - 2) * (nY - 2);
    linSolve(b, x, x0, a, 1 + 4 * a, iter, nX, nY);
}

void Gui::project(float *velocX, float *velocY, float *p, float *div, int iter, int nX, int nY)
{
    for (int j = 1; j < nY - 1; j++) {
        for (int i = 1; i < nX - 1; i++) {
            div[IX(i, j)] = -0.5f*(
                     velocX[IX(i+1, j  )]
                    -velocX[IX(i-1, j  )]
                    +velocY[IX(i  , j+1)]
                    -velocY[IX(i  , j-1)]
                )/(0.5*(nX + nY));
            p[IX(i, j)] = 0;
        }
    }
    setBnd(0, div, nX, nY); 
    setBnd(0, p, nX, nY);
    linSolve(0, p, div, 1, 4, iter, nX, nY);
    
    for (int j = 1; j < nY - 1; j++) {
        for (int i = 1; i < nX - 1; i++) {
            velocX[IX(i, j)] -= 0.5f * (  p[IX(i+1, j)]
                                         -p[IX(i-1, j)]) * nX;
            velocY[IX(i, j)] -= 0.5f * (  p[IX(i, j+1)]
                                         -p[IX(i, j-1)]) * nY;
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
            tmp1 = dtx * velocX[IX(i, j)];
            tmp2 = dty * velocY[IX(i, j)];
            x    = ifloat - tmp1; 
            y    = jfloat - tmp2;
            
            if(x < 0.5f) x = 0.5f; 
            if(x > nXfloat + 0.5f) x = nXfloat + 0.5f; 
            i0 = floor(x); 
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
            
            d[IX(i, j)] = s0 * ( t0 * d0[IX(i0i, j0i)]
                              +( t1 * d0[IX(i0i, j1i)]))
                         +s1 * ( t0 * d0[IX(i1i, j0i)]
                              +( t1 * d0[IX(i1i, j1i)]));
        }
    }
    setBnd(b, d, nX, nY);
}


void Gui::fluidCubeStep(FluidCube *cube)
{
    int nX = cube->nX;
    int nY = cube->nY;
    float visc     = cube->visc;
    float diff     = cube->diff;
    float dt       = cube->dt;
    float *uX      = cube->uX;
    float *uY      = cube->uY;
    float *uX0     = cube->uX0;
    float *uY0     = cube->uY0;
    float *s       = cube->s;
    float *density = cube->density;
    
    diffuse(1, uX0, uX, visc, dt, 4, nX, nY);
    diffuse(2, uY0, uY, visc, dt, 4, nX, nY);
    
    project(uX0, uY0, uX, uY, 4, nX, nY);
    
    advect(1, uX, uX0, uX0, uY0, dt, nX, nY);
    advect(2, uY, uY0, uX0, uY0, dt, nX, nY);
    
    project(uX, uY, uX0, uY0, 4, nX, nY);
    
    diffuse(0, s, density, diff, dt, 4, nX, nY);
    advect(0, density, s, uX, uY, dt, nX, nY);
}

int Gui::clamp (int val, int min, int max) {
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

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
	this->cube  = FluidCubeCreate(this->mesh.numI, this->mesh.numJ, DIFFUSION, VISCOSITY, DT); // blue print

	// calculate the spacing required to position the mesh in the screen center
	this->mesh.getMeshLoc(WINDOW_X, WINDOW_Y);
	// make rectangles and put them in the mesh
	std::vector<sf::RectangleShape> shapeGrid(this->mesh.numI * this->mesh.numJ);
	for (int i = 0; i < this->mesh.numI; i++){
		for (int j = 0; j < this->mesh.numJ; j++){

			shapeGrid[this->mesh.IDX(i,j)].setPosition(\
					this->mesh.meshLocX + i*this->mesh.gridSizeX,\
					this->mesh.meshLocY + j*this->mesh.gridSizeY\
					);
			shapeGrid[this->mesh.IDX(i,j)].setOutlineThickness(1);
			shapeGrid[this->mesh.IDX(i,j)].setOutlineColor(sf::Color(255,255,255,20));
			shapeGrid[this->mesh.IDX(i,j)].setFillColor(sf::Color::Black);

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
			this->cube->density[this->mesh.IDX(30,30)] += ADD_DENSITY;
			flag = 0;
		}
		this->fluidCubeStep(this->cube);
		for (int i = 0; i < this->mesh.numI; i++){
			for (int j = 0; j < this->mesh.numJ; j++){
				shapeGrid[this->mesh.IDX(i,j)].setFillColor(\
						sf::Color(255, 255, 255, (this->cube->density[this->mesh.IDX(i,j)] > 255) \
						? 255 : this->cube->density[this->mesh.IDX(i,j)])\
					);
				this->window.draw(shapeGrid[this->mesh.IDX(i,j)]);
			}
		}
		prevMouseCoords = currMouseCoords;
		currMouseCoords = this->getMousePointedGrid(currMouseCoords);
        	this->text.setString("Density: " +  \
				std::to_string((int)this->cube->density[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)]) + \
				"\n" +\
				" uX0: " +\
			       	std::to_string((int)this->cube->uX0[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)]) +\
				"\n" +\
				" uY0: " +\
			       	std::to_string((int)this->cube->uY0[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)]) +\
				"\n" +\
				" uX: " +\
			       	std::to_string((int)this->cube->uX[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)]) +\
				"\n" +\
				" uY: " +\
			       	std::to_string((int)this->cube->uY[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)]) +\
				"\n" +\
				" s: " +\
			       	std::to_string((int)this->cube->s[this->mesh.IDX(currMouseCoords.x,currMouseCoords.y)])\
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
	if (( i != this->mesh.numI - 1 && j != this->mesh.numJ - 1) || \
	 ( i != 0 || j != 0))
		this->cube->density[this->mesh.IDX(i,j)] += amount;
}


void Gui::addVelocity(sf::Vector2i coords, int amountX, int amountY){
	int i,j;
	sf::Vector2i val = this->getMousePointedGrid(coords);
	i = val.x;
	j = val.y;
	if (( i != this->mesh.numI - 1 && j != this->mesh.numJ - 1) ||\
	 ( i != 0 || j != 0)){
		this->cube->uX[this->mesh.IDX(i,j)] += amountX/10;
		this->cube->uY[this->mesh.IDX(i,j)] += amountY/10;
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
			radAngle = atan2(this->cube->uY[this->mesh.IDX(i,j)], this->cube->uX[this->mesh.IDX(i,j)]);
		//	printf("angle = %f\n", 180/M_PI*radAngle);
			this->vf[this->mesh.IDX(i,j)].setSize(sf::Vector2f(10,1));
			this->vf[this->mesh.IDX(i,j)].setPosition(x,y);
			this->vf[this->mesh.IDX(i,j)].setFillColor(sf::Color::Red);
			this->vf[this->mesh.IDX(i,j)].setRotation(180/M_PI*radAngle);
		}

	}
}
