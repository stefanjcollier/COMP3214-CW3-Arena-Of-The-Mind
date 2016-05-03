#ifndef LARGEBLUEBUTTERFLY_H
#define LARGEBLUEBUTTERFLY_H

using namespace std;
#include <stdio.h>

//My includes
#include "Shader.h"
#include "BlueWing.h"


BlueWing largeBWing(15.0f, 9.0f);

class LargeBlueButterfly {
public:
	/*
	* Create the data
	*/
	void instantiate() {
		largeBWing.instantiate();
	}

	void draw(Shader textureShader, glm::vec3 centre, GLfloat speed, GLfloat offsetDegrees) {
		//Find the texture location attribute
		textureShader.Use();
		GLint texModelLoc = glGetUniformLocation(textureShader.Program, "model");

		//Move the butterfly
		glm::mat4 wingModel1;
		glm::mat4 wingModel2;

		wingModel1 = glm::translate(wingModel1, centre);
		wingModel2 = glm::translate(wingModel2, centre);
		wingModel2 = glm::rotate(wingModel2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		GLfloat angle = glm::sin(glfwGetTime() * speed + glm::radians(offsetDegrees));//Keeps it to 0 -> 90deg rather that 0 -> 180deg
		if (angle > 0) {
			angle = angle*-1;
		}

		wingModel1 = glm::rotate(wingModel1, float(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		wingModel2 = glm::rotate(wingModel2, -float(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		//Draw wings
		glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(wingModel1));
		largeBWing.draw(textureShader);

		glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(wingModel2));
		largeBWing.draw(textureShader);


	}

	void kill() {
		largeBWing.kill();
	}

};

#endif