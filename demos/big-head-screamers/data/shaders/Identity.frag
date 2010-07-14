
uniform sampler2D sTexture;
	
uniform vec4 Color;

void main()
{
	//gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st);
	gl_FragColor = Color;
}

