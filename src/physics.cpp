#include "include/physics.h"

Physics::Physics()
Physics::~Physics(){}
void Physics::step(){
	this->diffuse();	
	this->project();
}
Physics::step(){
	this->diffuse();
	this->project();

	this->advect();
	this->advect();
	this->project();

	this->diffuse();
	this->advect();
}

Physics::diffuse(){
}

    def diffuse(self, x, x0, diff):
        if diff != 0:
            a = self.dt * diff * (self.size - 2) * (self.size - 2)
            self.lin_solve(x, x0, a, 1 + 6 * a)
        else:  # equivalent to lin_solve with a = 0
            x[:, :] = x0[:, :]
