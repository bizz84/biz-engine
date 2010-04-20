
varying vec3 Normal;
varying vec3 LightDir;

void main()
{
	float Intensity = abs(dot(Normal, LightDir));
		
	gl_FragColor =vec4(Intensity);
	
	//gl_FragColor = vec4(0.0);
}
