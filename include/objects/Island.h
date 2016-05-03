#ifndef ISLAND_H
#define ISLAND_H

//My includes
#include "Shader.h"
#include "Ground.h"


Ground ground(40.0f, glm::vec3(0.1f, 1.0f, 0.1f));

class Island {
public:

	void instantiate() {
		ground.instantiate();
	}

	void draw(Shader shader) {
		ground.draw(shader);
	}

	void kill() {
		ground.kill();
	}

};

#endif