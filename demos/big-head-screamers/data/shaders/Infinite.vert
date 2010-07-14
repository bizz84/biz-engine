

uniform vec2 TexRepeat;
uniform vec2 TexOffset;

void main()
{
	gl_TexCoord[0].xy = TexRepeat * (gl_Vertex.xz - TexOffset);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
