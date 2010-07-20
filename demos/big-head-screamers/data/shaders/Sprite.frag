
uniform sampler2D sTexture;

varying vec2 Neighbor[4];

const float sqrt3 = 1.7320508;
const float MinIntensity = 0.0005;
const float MaxIntensity = sqrt3 * 0.975;

void main()
{
	// TODO: Do this cheaply
	vec3 color[4];
	color[0] = texture2D(sTexture, Neighbor[0]).rgb;
	color[1] = texture2D(sTexture, Neighbor[1]).rgb;
	color[2] = texture2D(sTexture, Neighbor[2]).rgb; 
	color[3] = texture2D(sTexture, Neighbor[3]).rgb;
		
	gl_FragColor = texture2D(sTexture, gl_TexCoord[0].st);
	
	//if (length(gl_FragColor.xyz) >= MaxIntensity)
	//	discard;

	//if (length(color[0] + color[1] + color[2] + color[3]) <= MinIntensity)
	//if (length(color[0]) <= MinIntensity || length(color[1]) <= MinIntensity ||
	 //   length(color[2]) <= MinIntensity || length(color[3]) <= MinIntensity)
	 
	//if (length(color[0]) + length(color[1]) + length(color[2]) + length(color[3]) >= 4.0 * MaxIntensity)
	if (length(color[0]) >= MaxIntensity || length(color[1]) >= MaxIntensity ||
	    length(color[2]) >= MaxIntensity || length(color[3]) >= MaxIntensity)
		discard;
}

