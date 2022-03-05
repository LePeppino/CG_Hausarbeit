#version 400


const int MAX_LIGHTS=14;
struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
	int LightCount;
	Light lights[MAX_LIGHTS];
};


in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec4 depth;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 DepthColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

float near = 0.1; 
float far  = 10000.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    vec3 R = reflect(-L,N);
    vec3 H = normalize(E+L); //AUFGABE 1
    vec3 DiffuseComponent;
	vec3 SpecularComponent;
	vec3 Color = vec3(0,0,0);
	

	for(int i=0; i<LightCount;i++){
	vec3 distance = lights[i].Position - Position;
	float distanceLength = length(distance);
	float attenuation = 1.0;

	//Point light
	if(lights[i].Type == 0){
	//attenuation /=(lights[i].Attenuation.x + (lights[i].Attenuation.y*distanceLength) + (lights[i].Attenuation.z*distanceLength*distanceLength));
	attenuation = 0.13; //sonne soll keine stärke mit distanz verlieren
	L = normalize(lights[i].Position - Position);
	}

	// Nicht nötig für das projekt
	
	//Directional Light
	else if(lights[i].Type == 1){
	L = normalize(-lights[i].Direction);
	}
	/*//Spotlight
	else if(lights[i].Type == 2){
	attenuation /= (lights[i].Attenuation.x + (lights[i].Attenuation.y*distanceLength) + (lights[i].Attenuation.z*distanceLength*distanceLength));
	L = normalize(lights[i].Position - Position);
	 attenuation *= 1-sat((acos(dot(normalize(L),normalize(-lights[i].Direction)))-lights[i].SpotRadius.x)/(lights[i].SpotRadius.y - lights[i].SpotRadius.x));
	}
	*/

	vec3 lColor = lights[i].Color *attenuation;
	H = normalize(E+L);
	DiffuseComponent = lColor * DiffuseColor* sat(dot(N,L));
    SpecularComponent = lColor * SpecularColor * pow( sat(dot(H,N)), SpecularExp);
	
	Color += DiffuseComponent*DiffTex.rgb + SpecularComponent;
	}
		

    FragColor = vec4(Color + AmbientColor*DiffTex.rgb  ,DiffTex.a);


	// Bild mit hellen stellen für bloom effect. Hier wird Gaussian Blur drauf "addiert" später.
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		
	//godrays	
	vec4 depthTmp = depth;
		depthTmp.z =  depth.z * 2;

		depthTmp = depthTmp*10 / (depthTmp.w * depthTmp.w);
		   
		float depth2 = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration

		if (depth2 > 1)
			depth2 = 1;
		DepthColor = vec4(vec3(depth2), 1.0);

}
