#ifndef ALLCLOUDS
#define ALLCLOUDS

//External Includes
#include <vector>

//My includes
#include "Shader.h"
#include "Cloud.h"

Cloud cloud(40.0f, 26.0f);

class AllClouds {
public:
	
	/*
	* Create the data
	*/
	void instantiate() {
		cloud.instantiate();
		this->addManyClouds();
	}

	void draw(Shader textureShader) {
		for (GLuint singleCloud = 0; singleCloud < myClouds.size(); singleCloud++) {
			textureShader.Use();
			GLint texModelLoc = glGetUniformLocation(textureShader.Program, "model");

			glm::mat4 model;
			glm::vec3 singleCloudLoc = this->myClouds[singleCloud];
			model = glm::translate(model, singleCloudLoc);
			glUniformMatrix4fv(texModelLoc, 1, GL_FALSE, glm::value_ptr(model));

			cloud.draw(textureShader);
		}
	}

	void kill() {
		cloud.kill();
	}



private:
	std::vector<glm::vec3> myClouds;

	void addSingleClump(GLfloat x, GLfloat y, GLfloat z) {
		myClouds.push_back(glm::vec3(x,y,z));
	}
	GLfloat getRand(GLfloat LO, GLfloat HI) {
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}

	void addManyClouds(){
		GLuint maxClouds = 10;

		GLfloat range = groundWidth;
		GLfloat inc = 4 * range / maxClouds;
		
		GLfloat ran_fac = 9.0f;
		GLfloat y = 30.0;
		GLfloat x, z, yn;
		for (GLfloat xn = -range; xn < range; xn+= inc) {
		for (GLfloat zn = -range*1.5; zn < range; zn+= inc) {
				xn += getRand(-ran_fac, ran_fac);
				zn += getRand(-ran_fac, ran_fac);
				
				yn = y;// +getRand(-ran_fac / 2, ran_fac);
				if (zn < 0.0f) {
					yn -= (range*1.5 - zn)/20;
				}
				this->addSingleClump(xn, yn, zn);
			}
		}


	}

};

#endif