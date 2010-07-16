
uniform sampler2D sTexture;
uniform sampler2D sTexture2;
	
uniform vec2 ScreenInv;

varying float Arg;

float ReflectionMix()
{
	return pow(Arg, 0.4);
}

// Note this creates a very cool drunken/"black hole" effect
//vec2 TexCoord2 = gl_FragCoord.xy * ScreenInv;
//gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st + TexCoord2);

void main()
{
	vec2 TexCoord2 = gl_FragCoord.xy * ScreenInv;
	//gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st + TexCoord2);
	
	gl_FragColor = mix(texture2D(sTexture, gl_TexCoord[0].st),
					texture2D(sTexture2, TexCoord2), ReflectionMix());
}

