//attribute float inArg;

uniform float TexRepeat;
uniform vec2 PosOffset;
uniform float ZFar;

varying float Arg;

void main()
{
	gl_TexCoord[0].st = TexRepeat * (gl_Vertex.xz + PosOffset) / ZFar;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	//Arg = inArg;
	// FIXME: why does this not work?
	//Arg = -(gl_ModelViewMatrix[2] * gl_Vertex) / ZFar;
	Arg = -(gl_ModelViewMatrix * gl_Vertex).z / ZFar;
}
