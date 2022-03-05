#ifndef WaterShader_hpp
#define WaterShader_hpp

#include <stdio.h>
#include "PhongShader.h"


class WaterShader : public PhongShader
{
public:
    
    WaterShader(const std::string& AssetDirectory);
    virtual ~WaterShader() {}
    virtual void activate(const BaseCamera& Cam) const;
    virtual void deactivate() const;

private:
    
	// shader locations
    //GLint DetailTexLoc;
    
};

#endif /* WaterShader_hpp */
