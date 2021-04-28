#pragma once

class Mesh{
	public:
		int meshSizeX;
		int meshSizeY;
		int gridSizeX;
		int gridSizeY;
		int meshLocX; // X offset location onf the mesh in the container window
		int meshLocY; // Y offset location of the mesh in the container window
		int numI; // number of grids in a row
		int numJ; // number of grids in a column
	
		Mesh(int mX, int mY, int gX, int gY);
		~Mesh();
		void list_values_stub();
		void getMeshLoc(int wX, int wY);
		int IDX(int x, int y);
};
