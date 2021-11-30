#include "Fish.h"
#define MESH_NAME "models/body2.dae"
#define MESH_NAME3 "models/tail.dae"
#define MESH_NAME4 "models/left.dae"
#define MESH_NAME5 "models/right.dae"
#define TEXTURE1 "textures/fish3.jpg"

	Mesh fish;
	Mesh fishTail;
	Mesh fishFinR;
	Mesh fishFinL;
	Mesh trashDanger[30];

	int N;
	//--------------------
	//Boid Stuff
	vec3 position = vec3(0.0, 0.0, 0.0);
	vec3 velocity = vec3(0.0, 0.0, 0.0);
	vec3 acceleration = vec3(0.0, 0.0, 0.0);
	float perceptionRadius;
	float maxForce;
	float maxSpeed;
	bool isDead = false;
	float steerX = 0.0;
	float steerY = 0.0;

	//------------------------------


	void Fish::create(Mesh main, Mesh finL, Mesh finR, Mesh tail, int n, Mesh trash[30])
	{
		this->fish = main;
		this->fishTail = tail;
		this->fishFinR = finR;
		this->fishFinL = finL;
		this->N = n;

		for (int i = 0; i < 20; i++)
		{
			this->trashDanger[i] = trash[i];
		}

		setInitPosition();
	}

	void Fish::setInitPosition() {
		int rx = (rand() % 300) - 150;
		int ry = (rand() % 30) - 15;
		int rz = (rand() % 130)*-1;
		this->position = vec3(rx*1.5, ry * 1.5, rz * 1.5);

		float rxv = (rand() % 100) * 0.004;
		//float ryv = (rand() % 10) * 0.001;
		float rzv = (rand() % 100) *0.008;
		this->velocity = vec3(rxv, 0.0, rzv);
	}

	void Fish::display(int matrix_location, float delta, Fish boids[]) {

		//Boid
		boidFlock(boids);
		boidUpdate();


		mat4 modelFish = identity_mat4();
		
		if (isDead) {
			modelFish = rotate_x_deg(modelFish, 90.0);//look up to face camera
		}
		else {
			modelFish = rotate_x_deg(modelFish, -90.0);//look up to face camera
			modelFish = rotate_x_deg(modelFish, -steerY / 6);//look at the Y direction it's moving
			modelFish = rotate_y_deg(modelFish, steerX / 6);//look at the X direction it's moving
		}
		modelFish = scale(modelFish, vec3(0.8, 0.8, 0.8));
		modelFish = translate(modelFish, this->position);
		

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelFish.m);
		fish.render();

		//Draw FishTail Hierarchial 1 to 1
		mat4 modelTail = identity_mat4();
		float spin = 1;
		if (!isDead) {
			spin = sin(delta * 5) * 25;
			modelTail = rotate_z_deg(modelTail, spin);
		}

		modelTail = translate(modelTail, vec3(spin / 35, -0.2, 0.0));

		modelTail = modelFish * modelTail;

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelTail.m);
		fishTail.render();

		//Draw FishFins Hierarchial 1 to many(2)
		mat4 modelFinL = identity_mat4();
		mat4 modelFinR = identity_mat4();
		float spin2 = sin(delta * 4);
		modelFinL = rotate_z_deg(modelFinL, spin2);
		modelFinR = rotate_z_deg(modelFinR, -spin2);

		modelFinL = modelFish * modelFinL;
		modelFinR = modelFish * modelFinR;

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelFinL.m);
		fishFinL.render();

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelFinR.m);
		fishFinR.render();
	}

	//--------------------

	void Fish::boidUpdate() {
		
		

		if (this->isDead){
			this->acceleration = vec3(0, 0, 0);
			this->velocity = vec3(0, 0.0, 0);
		}
		else {
			this->velocity = this->velocity + this->acceleration;
			this->velocity = limitForce(this->velocity, 1);
			this->acceleration = vec3(0, 0, 0);
			
		}

		collisionWithTrash();
		boidSteer();
		boidClip();
		this->position = this->position + this->velocity;
		

		
		
	}

	void Fish::boidFlock(Fish boids[]) {
		float aliWeight = 1.0;
		float sepWeight = 1.0;
		float cohWeight = 1.0;

		vec3 alignment = boidAlign(boids) * aliWeight;
		vec3 seperation = boidSeperation(boids) * sepWeight;
		vec3 cohesion = boidCohesion(boids) * cohWeight;
		this->acceleration += alignment;
		this->acceleration += seperation;
		this->acceleration += cohesion;
	}

	vec3 Fish::boidAlign(Fish boids[]) {
		vec3 steering = vec3(0.0, 0.0, 0.0);
		int total = 0;
		for (int i = 0; i < N; i++)
		{
			float d = length(boids[i].position - this->position);
			if (d > 0 && d < 8.0)
			{
				steering += boids[i].velocity;
				total++;
			}
		}

		if (total > 0)
		{
			steering = steering / total;
			steering = normalise(steering);
			steering *= 0.07;//maxSpeed;
			steering -= velocity;
		}
		return limitForce(steering, 0.001);
	}

	vec3 Fish::boidSeperation(Fish boids[]) {
		vec3 steering = vec3(0.0, 0.0, 0.0);
		int total = 0;
		for (int i = 0; i < N; i++)
		{
			float d = length(boids[i].position - this->position);
			if (boids[i].isDead && d < 12.0)//avoid dead fish
			{
				vec3 diff = this->position - boids[i].position;
				diff = diff / (d * d);
				steering = steering + (diff*2);
				total++;
			}
			else if (d > 0 && d < 8.0)//perceptionRadius
			{
				vec3 diff = this->position - boids[i].position;
				diff = diff / (d * d);
				steering = steering + diff;
				total++;
			}
		}
		
		if (total > 0)
		{
			steering = steering / total;
			steering = normalise(steering);
			steering *= 0.3;//maxSpeed;
			//steering -= velocity;
		}
		
		return limitForce(steering, 0.0006);

	}

	vec3 Fish::boidCohesion(Fish boids[]) {
		vec3 steering = vec3(0, 0, 0);
		int count = 0;
		for (int i = 0; i < N; i++){
			float d = length(boids[i].position - this->position);
			//printf("    %f Distance to boid %i\n", d, i);
			if (d > 0.0 && d < 30.0 && !boids[i].isDead)
			{
				steering +=  boids[i].position;
				count= count +1;
			}
			
		}

		if (count > 0) {
			steering = steering / count;
			steering -= position;
			steering = normalise(steering);
			steering *= 0.15;//maxSpeed;
			steering -= velocity;
			return limitForce(steering, 0.0001);
		}
		else {
			return  vec3(0, 0, 0);
		}



		
	}


	//Helper functions

	void Fish::boidSteer() {
		//let xDir = map(this.velocity.x, -this.maxSpeed, this.maxSpeed, -90, 90);
		//let yDir = map(this.velocity.y, -this.maxSpeed, this.maxSpeed, 90, -90);

		float input_start = -0.03;    // The lowest number of the range input.
		float input_end = 0.03;    // The largest number of the range input.

		float output_start = -90; // The lowest number of the range output.
		float output_end = 90;  // The largest number of the range output.

		float inputX = velocity.v[0]; // Input value.
		float inputY = velocity.v[1]; // Input value.

		double slope = 1.0 * (output_end - output_start) / (input_end - input_start);
		steerX = output_start + slope * (inputX - input_start);
		steerY = output_start + slope * (inputY - input_start);
	}

	void Fish::boidClip() {
		float offset = 1.0;
		//z
		if (position.v[2] < -200* offset) position.v[2] = 30 * offset;
		if (position.v[2] >30 * offset) position.v[2] = -180 * offset;
		//y
		if (position.v[1] < -25 * offset) position.v[1] = 30 * offset;
		if (position.v[1] > 35 * offset) position.v[1] = -20 * offset;

		//x
		if (position.v[0] < -200 * offset) position.v[0] = 180 * offset;
		if (position.v[0] > 200 * offset) position.v[0] = -180 * offset;
		
	}

	void Fish::collisionWithTrash() {
		for (int i = 0; i < 30; i++)
		{
			
			vec3 trashPos = vec3(trashDanger[i].x, trashDanger[i].y, trashDanger[i].z);
			float d = length(trashPos - this->position);
			
			if (d < 3)
			{
				/*
				printf("Dead fish found at ->");
				print(position);
				printf("Trash at ->");
				print(trashPos);
				printf("\n");
				printf("\n");
				*/
				
				this->isDead = true;
			}
		}
	}

	
	vec3 Fish::limitForce(vec3 force, float maxForce) {
		float magSq = force.v[0] * force.v[0] + force.v[1] * force.v[1] + force.v[2] * force.v[2];
		vec3 limited = force;
		if (magSq > maxForce* maxForce)
		{
			limited = limited/ sqrt(maxForce);
			limited *= maxForce;
		}
		return limited;
	}