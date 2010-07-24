// Instanced implementation of the sprite renderer
// Allows to render all sprites in one draw call
// Aka this is a vertex-shader implementation of the flyweight design pattern

attribute vec2 inVertex;
attribute vec2 inTexCoord;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	//gl_TexCoord[0].xy = inTexCoord;
	
    gl_Position = vec4(gl_Vertex.xy, -1.0, 1.0);
    //gl_Position = vec4(inVertex, -1.0, 1.0);
}
