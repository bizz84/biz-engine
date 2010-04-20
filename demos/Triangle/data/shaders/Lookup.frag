
uniform sampler2D sTexture;
	
void main()
{
	vec4 color = texture2D(sTexture, gl_TexCoord[0].st);
	gl_FragColor = color;
}

