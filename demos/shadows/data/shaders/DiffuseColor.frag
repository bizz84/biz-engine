//
uniform sampler2D sTexture;

uniform vec4 Color;

uniform int NumLights;

#define MAX_LIGHTS 4

varying vec3 Normal;
varying vec3 LightDir[MAX_LIGHTS];


void main()
{
	vec4 texColor = texture2D(sTexture, gl_TexCoord[0].st);

	float Intensity = max(0.0, dot(Normal, LightDir[0]));
	
	for (int i = 1; i < NumLights; i++)
		Intensity += max(0.0, dot(Normal, LightDir[i]));
		
	//Intensity /= NumLights;	
		
	gl_FragColor = Intensity * Color * texColor;
	gl_FragColor.a = 0.5;
}
