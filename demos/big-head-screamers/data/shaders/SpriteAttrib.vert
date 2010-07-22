// Instanced implementation of the sprite renderer
// Allows to render all sprites in one draw call
// Aka this is a vertex-shader implementation of the flyweight design pattern

attribute vec2 inVertex;
attribute vec2 inTexCoord;
// Texture indices go from 0 to 7 and 
// are mapped to the corresponding 2x4 texture atlas
attribute int inTexIndex;
// Own sprite transformations
attribute float inScale;
attribute float inRotAngle;
attribute vec3 inTranslate;

uniform vec2 NeighborOffset;

varying vec2 Neighbor[4];

void main()
{
	// Find corresponding image in texture atlas
	gl_TexCoord[0].xy = inTexCoord * vec2(0.25, 0.5);
	gl_TexCoord[0].x += inTexIndex % 4;
	if (inTexIndex >= 4)
		gl_TexCoord[0].y += 0.5;
	
	Neighbor[0] = gl_TexCoord[0].st + vec2(-NeighborOffset.x,  NeighborOffset.y);
	Neighbor[1] = gl_TexCoord[0].st + vec2( NeighborOffset.x,  NeighborOffset.y);
	Neighbor[2] = gl_TexCoord[0].st + vec2( NeighborOffset.x, -NeighborOffset.y);
	Neighbor[3] = gl_TexCoord[0].st + vec2(-NeighborOffset.x, -NeighborOffset.y);
	
	vec3 Pos = vec3(inVertex.x, 0.0, inVertex.y);
	Pos *= inScale;
	// TODO rotate
	
	Pos += inTranslate;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(Pos, 1.0);
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x, 10.0, gl_Vertex.y, 1.0);
}
