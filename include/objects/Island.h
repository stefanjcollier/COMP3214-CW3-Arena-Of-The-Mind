#ifndef ISLAND_H
#define ISLAND_H

//My includes
#include "Shader.h"
#include "GrassClump.h"
#include "Ground.h"

Ground ground(70.0f, 40, glm::vec3(0.1f, 1.0f, 0.1f));

class Island {
public:

	void instantiate() {
		clumps.instantiate();
		ground.instantiate();
	}

	void draw(Shader shader, Shader textureShader) {
		shader.Use();
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		ground.draw(shader);
		clumps.draw(textureShader);
	}

	void kill() {
		clumps.kill();
		ground.kill();
	}

};

#endif