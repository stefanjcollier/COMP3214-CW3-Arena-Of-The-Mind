#ifndef FLAG_H
#define FLAG_H



// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLFW
#include <GLFW/glfw3.h>
#include <GL/glew.h>

//Externals
#include <vector>
using namespace std;
#include <stdio.h>

//My includes
#include "Shader.h"
#include "Rectangle.h"

Rectangle rect(0.25, 0.5f);


class Flag {
public:

	/*
	* Create the data
	*/
	void instantiate() {
		rect.instantiate();
	}
	void draw(Shader shader, glm::vec3 pos, glm::vec3 color) {
			glm::mat4 model;
			model = glm::translate(model, pos);
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			rect.draw(shader, color);
	}

	void kill() {
		rect.kill();
	}


private:

};

#endif