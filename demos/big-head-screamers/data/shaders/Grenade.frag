
uniform vec4 Color;

varying float Intensity;

void main()
{
	gl_FragColor = Color * Intensity;
}
