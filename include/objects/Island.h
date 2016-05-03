#ifndef ISLAND_H
#define ISLAND_H

//My includes
#include "Shader.h"
#include "Ground.h"


Ground ground(100.0f, 80, glm::vec3(0.1f, 1.0f, 0.1f));

class Island {
public:

	void instantiate() {
		ground.instantiate();
	}

	void draw(Shader shader) {
		shader.Use();
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		ground.draw(shader);
	}

	void kill() {
		ground.kill();
	}

};

#endif