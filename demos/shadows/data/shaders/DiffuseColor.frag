//
uniform sampler2D sTexture;
//varying vec2 TexCoord;

uniform vec4 Color;

//varying float Intensity;
varying vec3 Normal;
varying vec3 LightDir;


void main()
{
	vec4 texColor = texture2D(sTexture, gl_TexCoord[0].st);

	float Intensity = max(0.0, dot(Normal, LightDir));
		
	gl_FragColor = Color * texColor * vec4(Intensity);
	gl_FragColor.a = 0.5;
}
