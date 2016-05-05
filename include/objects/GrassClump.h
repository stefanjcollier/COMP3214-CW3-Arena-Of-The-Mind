#ifndef GRASSCLUMP
#define GRASSCLUMP

//External Includes
#include <vector>

//My includes
#include "Shader.h"
#include "Grass2D.h"

Grass2D grass(0.5f,0.5f);

class GrassClump {
public:
	
	/*
	* Create the data
	*/
	void instantiate() {
		grass.instantiate();
	}

	void draw(Shader textureShader) {
		for (GLuint clump = 0; clump < clumps.size(); clump++) {
			textureShader.Use();
			GLint texModelLoc = glGetUniformLocation(textureShader.Program, "model");

			glm::mat4 model;
			glm::vec3 clumpLoc = this->clumps[clump];
			model = glm::translate(model, glm::vec3(0.0f, -0.10f, 0.0f)+clumpLoc);

			//model = glm::rotate(model, this->findAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
			//model = glm::translate(model, glm::vec3(-grass.getWidth()/2, 0.0f, 0.0f));
			glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(model));
			grass.draw(textureShader);

			model = glm::rotate(model, glm::half_pi<GLfloat>(), glm::vec3(0.0f, 1.0f, 0.0f));
			GLfloat offset = grass.getWidth() ;
			model = glm::translate(model, glm::vec3(-offset/2, 0.0f, offset/2));

			glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(model));
			grass.draw(textureShader);
		}
	}

	void kill() {
		grass.kill();
	}

	void addClump(glm::vec3 pos, glm::vec3 oldX, glm::vec3 oldZ){
		GLuint clumpsPerFunc = 1;
		for (int clump = 0; clump < clumpsPerFunc; clump++) {
			this->addSingleClump(pos, oldX, oldZ);
		}
	}


private:
	std::vector<glm::vec3> clumps;

	GLfloat getRand(GLfloat LO, GLfloat HI) {
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}

	void addSingleClump(glm::vec3 pos, glm::vec3 oldX, glm::vec3 oldZ) {
		//Interpolate a random point between the two
		GLfloat ran = getRand(0.0f, 1.0f);
		glm::vec3 ran_mid = oldZ + (pos - oldZ) * ran;
		ran_mid.x = oldX.x + (pos.x - oldX.x) * ran;
		clumps.push_back(ran_mid);
	}

	GLfloat findAngle() {
		GLfloat deg = 15.0f;
		return glm::radians(deg*glm::sin(glfwGetTime()/2)-deg);
	}
};

#endif