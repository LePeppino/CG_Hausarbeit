//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Ring.h"

Ring::Ring() {

}
Ring::~Ring() {
	delete ring;
}

bool Ring::loadModels(const char *ringFile, Terrain *terrain) {
	if (ringFile != nullptr) {
		ring = new Model(ringFile);
		ring->shader(pShader);
		boundingBox = ring->boundingBox();

		
		float x, z, y;
		y = 0.0;
		while (y < 10.0) {
			Matrix pos, scale, tree, rot;
			pos = ring->transform();
			pos.translation();
			x = randomFloat(-300.0, 300.0);
			z = randomFloat(-300.0, 300.0);
			float convX, convZ;
			convX = (((x / 24.6) + 24.6 / 2.0) / 0.03075);
			convZ = (((z / 24.6) + 24.6 / 2.0) / 0.03075);
			y = (terrain->heights[static_cast<int>(convX)][static_cast<int>(convZ)] * 20) + 5.0;
			rot.rotationY(randomFloat(0.0, 1.0));
			scale.scale(0.1);
			pos.translation(x, y, z);
			tree = pos * rot * scale;
			ring->transform(tree);
			
		}
		return true;
	}
	return false;
}
void Ring::draw(const BaseCamera& Cam) {
	ring->draw(Cam);
}

// Quelle: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float Ring::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return  a + r;
}

bool Ring::calcCollision(Airplane *pAirplane) {
	Vector airplanePos = pAirplane->chassis->transform().translation();
	Vector ringPos = this->ring->transform().translation();
	float tryOut = this->boundingBox.Max.X;
	return((this->boundingBox.Max.X * 0.1 + ringPos.X  > airplanePos.X)
		&& (this->boundingBox.Min.X * 0.1 + ringPos.X < airplanePos.X)
		&& (this->boundingBox.Max.Y * 0.1 + ringPos.Y > airplanePos.Y)
		&& (this->boundingBox.Min.Y * 0.1 + ringPos.Y < airplanePos.Y)
		&& (this->boundingBox.Max.Z * 0.1 + ringPos.Z > airplanePos.Z)
		&& (this->boundingBox.Min.Z * 0.1 + ringPos.Z < airplanePos.Z));
}
