
uniform sampler2D sTexture;

varying vec2 Neighbor[4];
	
void main()
{
	// TODO: Do this cheaply
	vec4 color[4];
	color[0] = texture2D(sTexture, Neighbor[0]);
	color[1] = texture2D(sTexture, Neighbor[1]);
	color[2] = texture2D(sTexture, Neighbor[2]); 
	color[3] = texture2D(sTexture, Neighbor[3]);
		
	gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st);

	if (length(color[0]) < 0.1 || length(color[1]) < 0.1 || length(color[2]) < 0.1 || length(color[3]) < 0.1)
		discard;
}

