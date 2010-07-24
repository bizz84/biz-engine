// Instanced implementation of the sprite renderer
// Allows to render all sprites in one draw call
// Aka this is a vertex-shader implementation of the flyweight design pattern

attribute vec2 inVertex;
attribute vec2 inTexCoord;
// Own sprite transformations
attribute float inScale;
attribute float inRotAngle;
attribute vec3 inTranslate;

//uniform vec2 NeighborOffset;

//varying vec2 Neighbor[4];

void main()
{
	// Find corresponding image in texture atlas
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	/*Neighbor[0] = gl_TexCoord[0].st + vec2(-NeighborOffset.x,  NeighborOffset.y);
	Neighbor[1] = gl_TexCoord[0].st + vec2( NeighborOffset.x,  NeighborOffset.y);
	Neighbor[2] = gl_TexCoord[0].st + vec2( NeighborOffset.x, -NeighborOffset.y);
	Neighbor[3] = gl_TexCoord[0].st + vec2(-NeighborOffset.x, -NeighborOffset.y);*/

	// Instancing transformation
	vec3 Pos = vec3(gl_Vertex.x, gl_Vertex.y, 0.0);
	Pos *= inScale;
	
	float c = cos(inRotAngle);
	float s = sin(inRotAngle);
	vec3 Rot = Pos;
	Rot.x = c * Pos.x + s * Pos.z;
	Rot.z = -s * Pos.x + c * Pos.z;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(Rot + inTranslate, 1.0);
}
