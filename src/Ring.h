//
//  Tank.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Ring_hpp
#define Ring_hpp

#include <stdio.h>
#include "model.h"
#include "PhongShader.h"
#include "Terrain.h"
#include "Tank.h"
class Ring : public BaseModel
{
public:
    Ring();
    virtual ~Ring();
    bool loadModels(const char* ringFile, Terrain *terrain);
	bool calcCollision(Tank *boundingBox);
    virtual void draw(const BaseCamera& Cam);
	float randomFloat(float a, float b);
	bool getActivated(){ return activated; }
	void activate() { this->activated = true; }
protected:
	Model *ring;
	AABB boundingBox;
	bool activated = false;
};

#endif /* Ring_hpp */
