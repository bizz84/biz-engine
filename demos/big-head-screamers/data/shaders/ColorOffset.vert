
varying vec4 Color;

uniform vec2 Offset;

void main()
{
	Color = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position.xy += Offset * gl_Position.w;
}
