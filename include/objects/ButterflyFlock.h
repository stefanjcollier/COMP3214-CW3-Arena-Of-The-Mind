#ifndef BUTTERFLYFLOCK_H
#define BUTTERFLYFLOCK_H

//My includes
#include "Shader.h"
#include "Ground.h"
#include "SmallOrangeButterfly.h"



class ButterflyFlock {
public:
	ButterflyFlock(GLfloat r, GLfloat h, glm::vec3 centre, GLuint number){
		this->radius = r;
		this->height = h;
		this->origin = centre;
		this->flockSize = number;

		this->assignWeights();
	}

	void instantiate() {
		this->orange.instantiate();
		this->blue.instantiate();
	}

	void draw(Shader textureShader) {
		GLuint n = this->flockSize;
		GLfloat  theta = glm::radians(360.0f / n);
		for (GLuint insect = 0; insect < n; insect++) {
			glm::vec3 pos = this->origin;
			pos.x += this->radius * glm::cos(glfwGetTime() + theta*insect + this->xweights[insect]);
			pos.y += this->height * glm::sin(glfwGetTime() + theta*insect + this->yweights[insect]);
			pos.z += this->radius * glm::sin(glfwGetTime() + theta*insect + this->zweights[insect]);
			if (this->useBlues[insect]) {
				this->blue.draw(textureShader, pos, glfwGetTime() + insect*theta, glm::vec3(0.0f, 1.0f, 0.0f), 7, 0);
			} else {
				this->orange.draw(textureShader, pos, glfwGetTime() + insect*theta, glm::vec3(0.0f, 1.0f, 0.0f), 7, 0);
			}
		}
	}

	void kill() {
		this->orange.kill();
		this->blue.kill();

	}


private:
	GLfloat radius, height;
	GLuint flockSize;
	vector<GLfloat> xweights, yweights, zweights;
	vector<GLboolean> useBlues;
	glm::vec3 origin;
	SmallOrangeButterfly orange;
	SmallBlueButterfly blue;

	GLfloat getRand(GLfloat LO, GLfloat HI) {
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}

	void assignWeights() {
		this->xweights.clear();
		this->yweights.clear();
		this->zweights.clear();
		this->useBlues.clear();

		for (GLuint insect = 0; insect < this->flockSize; insect++) {
			this->xweights.push_back(this->getRand(0,glm::pi<GLfloat>()));
			this->yweights.push_back(this->getRand(0, glm::pi<GLfloat>()));
			this->zweights.push_back(this->getRand(0, glm::pi<GLfloat>()));
			
			this->useBlues.push_back(getRand(0, 2) > 1);
		}
	}


};

#endif