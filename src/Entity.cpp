//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Entity.h"


Entity::Entity(const char* tree, const char* flower, Terrain *terrain) {
	this->terrain = terrain;
	loadModels(tree, flower);
}
bool Entity::loadModels(const char* tree, const char* flower) {
	for (int i = 0; i < 100; i++) {
		trees[i] = new Model(tree);
		trees[i]->shader(pShader);
		this->generatePosisition(trees, i);
		flowers[i] = new Model(flower);
		flowers[i]->shader(pShader);
		this->generatePosisition(flowers, i);
	}
	return true;
}
void Entity::draw(const BaseCamera& Cam) {
	for (int i = 0; i < 100; i++) {
		trees[i]->draw(Cam);
		flowers[i]->draw(Cam);
	}
}

void Entity::generatePosisition(Model *arr[], int i) {
	Matrix pos;
	pos = arr[i]->transform();
	pos.translation();
	float x, z, y;
	x = randomFloat(-5.0, 5.0);
	z = randomFloat(-5.0, 5.0);
	float convX, convZ;
	convX = (((x / 24.6) + 24.6 / 2.0) / 0.03075);
	convZ = (((z / 24.6) + 24.6 / 2.0) / 0.03075);
	z = (terrain->heights[static_cast<int>(x)][static_cast<int>(z)]) - 0.01;
	arr[i]->transform(pos);
}

// Quelle: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float Entity::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return  a + r;
}