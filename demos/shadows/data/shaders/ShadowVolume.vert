
uniform float ShadowExtent;
uniform bool  InfiniteShadowVolume;
uniform int   LightIndex;

varying vec3 Normal;
varying vec3 LightDir;

void main(void)
{
	vec3 EyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	vec3 LightPos = vec3(gl_LightSource[LightIndex].position);
	
	LightDir = normalize(LightPos - EyePos);
	
	Normal = normalize(gl_NormalMatrix * gl_Normal);

	if (dot(Normal, LightDir) < 0.0)
	{
		// Extend shadow volume to infinity.
		vec4 vertex;
		if (InfiniteShadowVolume)
			vertex = vec4(-LightDir, 0.0);
		else
			vertex = vec4(EyePos - LightDir * ShadowExtent, 1.0);
		
		gl_Position = gl_ProjectionMatrix * vertex;		
	}
	else
	{	
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	}

}
