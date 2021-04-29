Class Physics{
	private:
		void advect();
		void diffuse();
		void source();
		void project();
		void linSolve();
		void setBnd();
	public:
		Physics();
		~Physics();
		void step();
}	
