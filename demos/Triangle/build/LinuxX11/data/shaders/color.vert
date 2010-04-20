
varying vec4 Color;

varying float Intensity;

void main()
{
	Color = gl_Color;
	Intensity = 0.4;

    gl_Position = ftransform();
}
