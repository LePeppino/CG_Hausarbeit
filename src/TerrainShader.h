#ifndef TerrainShader_hpp
#define TerrainShader_hpp

#include <stdio.h>
#include <assert.h>
#include "PhongShader.h"


class TerrainShader : public PhongShader
{
public:
    enum {
        DETAILTEX0=0,
        DETAILTEX1,
    };
    
    TerrainShader(const std::string& AssetDirectory);
    virtual ~TerrainShader() {}
    virtual void activate(const BaseCamera& Cam) const;
    virtual void deactivate() const;
    
    const Texture* detailTex(unsigned int idx) const { assert(idx < 2); return DetailTex[idx]; }
	const Texture* normalTex(unsigned int idx) const { assert(idx < 2); return NormalTex[idx]; }
    const Texture* mixTex() const { return MixTex; }

    void detailTex(unsigned int idx, const Texture* pTex) { assert(idx < 2); DetailTex[idx] = pTex; }
	void normalTex(unsigned int idx, const Texture* pTex) { assert(idx < 2); NormalTex[idx] = pTex; }
    void mixTex(const Texture* pTex) { MixTex = pTex; }

    void scaling(const Vector& s) { Scaling = s; }
    const Vector& scaling() const { return Scaling; }

private:
    void activateTex(const Texture* pTex, GLint Loc, int slot) const;

    const Texture* MixTex;
    const Texture* DetailTex[2];
	const Texture* NormalTex[2];
    Vector Scaling;

    // shader locations
    GLint MixTexLoc;
    GLint DetailTexLoc[2];
	GLint NormalTexLoc[2];
    GLint ScalingLoc;
};

#endif /* TerrainShader_hpp */
