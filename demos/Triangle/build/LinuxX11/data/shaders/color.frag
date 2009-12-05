
varying vec4 Color;

varying float Intensity;

void main()
{
	gl_FragColor = Intensity * Color;
}
