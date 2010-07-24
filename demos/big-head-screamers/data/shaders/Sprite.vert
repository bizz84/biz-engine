
//uniform vec2 NeighborOffset;

//varying vec2 Neighbor[4];

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

	/*Neighbor[0] = gl_TexCoord[0].st + vec2(-NeighborOffset.x,  NeighborOffset.y);
	Neighbor[1] = gl_TexCoord[0].st + vec2( NeighborOffset.x,  NeighborOffset.y);
	Neighbor[2] = gl_TexCoord[0].st + vec2( NeighborOffset.x, -NeighborOffset.y);
	Neighbor[3] = gl_TexCoord[0].st + vec2(-NeighborOffset.x, -NeighborOffset.y);*/

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
