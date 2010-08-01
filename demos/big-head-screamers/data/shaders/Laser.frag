uniform sampler2D sTexture;

uniform vec4 Color;

varying float Intensity;

void main()
{
	gl_FragColor = vec4(texture2D(sTexture, gl_TexCoord[0].st).r) * Color/* * Intensity*/;
}
