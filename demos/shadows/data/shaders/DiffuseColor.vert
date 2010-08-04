// 

uniform int NumLights;

#define MAX_LIGHTS 4

varying vec3 Normal;
varying vec3 LightDir[MAX_LIGHTS];

void main(void)
{
	vec3 EyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	
	for (int i = 0; i < NumLights; i++)
		LightDir[i] = normalize(vec3(gl_LightSource[i].position) - EyePos);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
