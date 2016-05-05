#ifndef SHOWCASE_H
#define SHOWCASE_H



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
#include "Painting.h"
#include "Case.h"

GLfloat paint_width = 1.9f;
GLfloat paint_height = 1.4f;
GLfloat border = 0.15f;

Painting painting(paint_width, paint_height);
Case holder(paint_width, paint_height, border, glm::vec3(0.5f, 0.25f, 0.0f));

class Showcase {
public:

	void instantiate() {
		painting.instantiate();
		holder.instantiate();
	}

	void draw(Shader shader, Shader textureshader, glm::vec3 pos) {
		GLint texModelLoc = glGetUniformLocation(textureshader.Program, "model");
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");

		shader.Use();
		glm::mat4 model;
		model = glm::translate(model, pos);
		model = glm::translate(model, glm::vec3( -holder.getWidth()/2, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		holder.draw(shader);

		textureshader.Use();
		model = glm::translate(model, glm::vec3(holder.getBorder()/2, holder.getBorder() / 2, 0.1f));
		glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		painting.draw(textureshader);

	}

	void kill() {
		painting.kill();
		holder.kill();
	}


private:



};

#endif