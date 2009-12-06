
uniform float ShadowExtent;
uniform bool  InfiniteShadowVolume;

void main(void)
{
	vec3 EyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	vec3 LightPos = vec3(gl_LightSource[0].position);
	vec3 LightDir = normalize(LightPos - EyePos);
	
	vec3 Normal = normalize(gl_NormalMatrix * gl_Normal);

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
