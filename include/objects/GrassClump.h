#ifndef GRASSCLUMP
#define GRASSCLUMP

//External Includes
#include <vector>

//My includes
#include "Shader.h"
#include "Grass2D.h"

Grass2D grass(0.5f,0.5f);

class GrassClumps {
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
			model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f)+this->clumps[clump]);
			glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(model));
			grass.draw(textureShader);
		}
	}

	void kill() {
		grass.kill();
	}

	void addClump(glm::vec3 pos){
		clumps.push_back(pos);
	}


private:
	std::vector<glm::vec3> clumps;
};

#endif