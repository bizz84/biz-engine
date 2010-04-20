// 


//varying float Intensity;
varying vec3 Normal;
varying vec3 LightDir;

//varying vec2 TexCoord;

void main(void)
{
	vec3 EyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	LightDir = normalize(vec3(gl_LightSource[0].position) - EyePos);
	
	//Intensity = max(0.0, dot(Normal, LightDir));

	gl_TexCoord[0] = gl_MultiTexCoord0;
	//TexCoord = gl_MultiTexCoord0.st;
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
