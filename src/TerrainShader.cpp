#include "TerrainShader.h"
#include <string>

TerrainShader::TerrainShader(const std::string& AssetDirectory) : PhongShader(false), Scaling(1,1,1), MixTex(NULL)
{
    std::string VSFile = AssetDirectory + "vsterrain.glsl";
    std::string FSFile = AssetDirectory + "fsterrain.glsl";
    if( !load(VSFile.c_str(), FSFile.c_str()))
        throw std::exception();
    PhongShader::assignLocations();
    specularColor(Color(0,0,0));
     
    MixTexLoc = getParameterID( "MixTex");
    ScalingLoc = getParameterID( "Scaling");
    
    for(int i = 0; i < 2; i++)
    {
		std::string s;

		//Diffuse Textures
        DetailTex[i] = NULL;
        s += "DetailTex[" + std::to_string(i) + "]";
        DetailTexLoc[i] = getParameterID( s.c_str());

		//Normal Textures
		NormalTex[i] = NULL;
		s += "NormalTex[" + std::to_string(i) + "]";
		NormalTexLoc[i] = getParameterID( s.c_str());
    }

}

void TerrainShader::activate(const BaseCamera& Cam) const
{
    PhongShader::activate(Cam);

    int slot=0;
    activateTex(MixTex, MixTexLoc, slot++);

	for (int i = 0; i < 2; i++) {
		activateTex(DetailTex[i], DetailTexLoc[i], slot++);
	}
	for (int i = 0; i < 2; i++) {
		activateTex(NormalTex[i], NormalTexLoc[i], slot++);
	}

    setParameter(ScalingLoc, Scaling);
}

void TerrainShader::deactivate() const
{
    PhongShader::deactivate();
	for (int i = 2 - 1; i >= 0; i--) {
		if (DetailTex[i] && DetailTexLoc[i] >= 0) DetailTex[i]->deactivate();
		if (NormalTex[i] && NormalTexLoc[i] >= 0) NormalTex[i]->deactivate();
	}
        
    if(MixTex) MixTex->deactivate();
}

void TerrainShader::activateTex(const Texture* pTex, GLint Loc, int slot) const
{
    if(pTex && Loc>=0)
    {
        pTex->activate(slot);
        setParameter(Loc, slot);
    }
    
}
