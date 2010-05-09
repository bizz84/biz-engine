
varying vec4 Color;

void main()
{
	Color.r = exp(-gl_Vertex.w * 3.5);
	Color.g = exp(-gl_Vertex.w * 1.5);
	Color.b = exp(-gl_Vertex.w * 0.5);
	
	/*Color.r = 0.5 + 0.5 * cos(gl_Vertex.w);
	Color.g = 0.5 + 0.5 * sin(gl_Vertex.w);
	Color.b = 0.5 + 0.5 * cos(2.0 * gl_Vertex.w);*/
	
	/*Color.r = mod(gl_Vertex.w, 4.0) / 4.0f;
	Color.g = mod(gl_Vertex.w, 2.0) / 2.0f;
	Color.b = mod(gl_Vertex.w, 1.0) / 1.0f;*/
	
	Color.a = 1.0;
	//gl_Vertex.w
	//Color = gl_Color;

	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
}
