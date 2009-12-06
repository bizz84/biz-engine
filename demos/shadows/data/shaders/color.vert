
varying vec4 Color;

void main()
{
	Color = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
