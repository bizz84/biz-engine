
uniform sampler2D sTexture;

uniform vec4 Color;

void main()
{
	gl_FragColor = 0.5 * Color + texture2D(sTexture, gl_TexCoord[0].st);
}

