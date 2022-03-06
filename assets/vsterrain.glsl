#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord0;
layout(location=3) in vec2 VertexTexcoord1;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord0;
out vec2 Texcoord1;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;

void main()
{
   //Skalierung
    vec4 newVertexPosition = VertexPos;
    newVertexPosition.x *= Scaling.x;
    newVertexPosition.y *= Scaling.y;
    newVertexPosition.z *= Scaling.z;

	//Normale
	//https://stackoverflow.com/questions/29008847/normal-matrix-for-non-uniform-scaling
    vec4 newVertexNormal = VertexNormal;
    newVertexNormal.x /= Scaling.x;
    newVertexNormal.y /= Scaling.y;
    newVertexNormal.z /= Scaling.z;

    newVertexNormal = normalize(newVertexNormal);

    Position = (ModelMat * newVertexPosition).xyz;
    Normal = (ModelMat * vec4(newVertexNormal.xyz,0)).xyz;
    Texcoord0 = VertexTexcoord0;
    Texcoord1 = VertexTexcoord1;
    
    gl_Position = ModelViewProjMat * newVertexPosition; 
}
