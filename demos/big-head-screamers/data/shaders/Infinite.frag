
uniform sampler2D sTexture;
uniform sampler2D sTexture2;
	
uniform vec2 ScreenInv;

varying float Arg;

// TODO: Can use frag y and horizon line to determine mix value?
float ReflectionMix(float value)
{
	return pow(value, 0.4);
}

void main()
{
	vec2 TexCoord2 = gl_FragCoord.xy * ScreenInv;
	// Note this creates a very cool drunken/"black hole" effect
	//vec2 TexCoord = gl_TexCoord[0].st + TexCoord2;
	// regular mapping
	vec2 TexCoord = gl_TexCoord[0].st; 
	
	gl_FragColor = mix(texture2D(sTexture, TexCoord),
					texture2D(sTexture2, TexCoord2), ReflectionMix(Arg));
}

