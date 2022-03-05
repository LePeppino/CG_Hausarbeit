#include "WaterShader.h"
#include <string>

WaterShader::WaterShader(const std::string& AssetDirectory) : PhongShader(true)
{
    std::string VSFile = AssetDirectory + "vswater.glsl";
    std::string FSFile = AssetDirectory + "fswater.glsl";
    if( !load(VSFile.c_str(), FSFile.c_str()))
        throw std::exception();

    PhongShader::assignLocations();
    
}

void WaterShader::activate(const BaseCamera& Cam) const
{
    PhongShader::activate(Cam);
}

void WaterShader::deactivate() const
{
    PhongShader::deactivate();
}
