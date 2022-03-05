#version 400

uniform vec3 EyePos;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

out vec4 FragColor;

uniform sampler2D DiffuseTexture;

float sat(in float a)
{
    float dmin = 0;
    float dmax = 150;
    
    return clamp(pow((length(EyePos-Position)-dmin)/dmax-dmin,a), 0.0, 1.0);
}

void main()
{

	vec4 DiffTex = texture(DiffuseTexture, Texcoord).rgba;
	vec4 color = DiffTex;

	//Nebel
    float s = sat(1);
    vec3 colorFogCalc = vec3(0.45, 0.35, 0.45); //Farbe für sunset skybox
    vec4 colorFog = vec4(colorFogCalc.rgb, 1);
    vec4 newColorFog = (1-s)*color+(s*colorFog);

	FragColor = vec4(DiffTex.rgb, 0.3) * newColorFog;

}
