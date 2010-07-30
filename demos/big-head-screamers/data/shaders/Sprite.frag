
uniform sampler2D sTexture;

void main()
{
	gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st);

	if (gl_FragColor.a == 0.0)
		discard;
}

