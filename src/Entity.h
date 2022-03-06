//
//  Tank.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "model.h"
#include "PhongShader.h"
#include "Terrain.h"

class Entity : public BaseModel
{
public:
    Entity(const char* tree, const char* flower, Terrain *terrain);
    bool loadModels(const char* trees, const char* flowers);
    virtual void draw(const BaseCamera& Cam);
	float randomFloat(float a, float b);
protected:
	Model *trees[100];
	Model *flowers[100];
	void generatePosisition(Model *arr[],int i);
	Terrain *terrain;
};

#endif /* Entity_hpp */
