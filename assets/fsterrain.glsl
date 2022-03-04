#version 400
uniform vec3 EyePos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D MixTex;
uniform sampler2D DetailTex[4];
uniform vec3 Scaling;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord0;

in vec2 Texcoord1;

out vec4 FragColor;

//Visualisierung: https://thebookofshaders.com/glossary/?search=clamp
//Formel für Nebel aus dem Aufgabenblatt, Werte und Color-Berechnung in der main unten
float sat(in float a)
{
    float dmin = 0;
    float dmax = 150;
    
    return clamp(pow((length(EyePos-Position)-dmin)/dmax-dmin,a), 0.0, 1.0);
}


void main()
{
    vec3 N      = normalize(Normal);
    vec3 L      = normalize(LightPos); // light is treated as directional source
    vec3 D      = EyePos-Position;
    float Dist  = length(D);
    vec3 E      = D/Dist;
    vec3 R      = reflect(-L,N);
    
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);

   
	//https://forum.openframeworks.cc/t/what-does-the-texture-function-in-glsl-do/21196
	//https://thebookofshaders.com/glossary/?search=mix
    vec4 color;
    vec4 MixTextureVec = texture(MixTex,Texcoord0).rgba; 
    //Textures
    vec4 GrassTexture = texture(DetailTex[0], Texcoord1).rgba;
    vec4 RockTexture = texture(DetailTex[1], Texcoord1).rgba;

    //Mixe zusammen
    color = mix(GrassTexture, RockTexture, MixTextureVec);
   
    //Nebel
    float s = sat(0.5);
    vec3 colorFogCalc = vec3(0.45, 0.35, 0.45); //Farbe für sunset skybox
    vec4 colorFog = vec4(colorFogCalc.rgb, 1);
    vec4 newColorFog = (1-s)*color+(s*colorFog);    

    FragColor = vec4(((DiffuseComponent + AmbientColor) + SpecularComponent),1) * newColorFog; //ohne Nebel: * color;

}
