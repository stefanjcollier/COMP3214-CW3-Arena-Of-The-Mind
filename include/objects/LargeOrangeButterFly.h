#ifndef LARGEORANGEBUTTERFLY_H
#define LARGEORANGEBUTTERFLY_H

using namespace std;
#include <stdio.h>

//My includes
#include "Shader.h"
#include "OrangeWing.h"


OrangeWing largeWing(5.0f, 3.0f);

class LargeOrangeButterfly {
public:
	/*
	* Create the data
	*/
	void instantiate() {
		largeWing.instantiate();
	}

	void draw(Shader textureShader, glm::vec3 centre, GLfloat rads, glm::vec3 axis, GLfloat speed, GLfloat offsetDegrees) {
		//Find the texture location attribute
		textureShader.Use();
		GLint texModelLoc = glGetUniformLocation(textureShader.Program, "model");

		//Move the butterfly
		glm::mat4 wingModel1;
		glm::mat4 wingModel2;

		wingModel1 = glm::translate(wingModel1, centre);
		wingModel2 = glm::translate(wingModel2, centre);
		if (rads != 0) {
			wingModel1 = glm::rotate(wingModel1, rads, axis);
			wingModel2 = glm::rotate(wingModel2, rads, axis);
		}

		wingModel2 = glm::rotate(wingModel2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		GLfloat angle = glm::sin(glfwGetTime() * speed + glm::radians(offsetDegrees));//Keeps it to 0 -> 90deg rather that 0 -> 180deg
		if (angle > 0) {
			angle = angle*-1;
		}

		wingModel1 = glm::rotate(wingModel1, float(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		wingModel2 = glm::rotate(wingModel2, -float(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		//Draw wings
		glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(wingModel1));
		largeWing.draw(textureShader);

		glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(wingModel2));
		largeWing.draw(textureShader);


	}

	void kill() {
		largeWing.kill();
	}

};

#endif