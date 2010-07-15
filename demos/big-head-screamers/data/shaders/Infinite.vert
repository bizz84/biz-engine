

uniform float TexRepeat;
uniform vec2 PosOffset;
uniform float ZFar;

varying vec2 TexCoord;

void main()
{
	//TexCoord = TexRepeat * (gl_Vertex.xz + PosOffset) / ZFar;
	gl_TexCoord[0].st = TexRepeat * (gl_Vertex.xz + PosOffset) / ZFar;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
