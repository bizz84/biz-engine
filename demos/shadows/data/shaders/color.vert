
varying vec4 Color;

void main()
{
	Color = gl_Color;
    gl_Position = ftransform() + vec4(1.0, 0.0, 0.0, 0.0);
}
