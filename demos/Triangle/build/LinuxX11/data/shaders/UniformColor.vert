// 

varying float Intensity;

void main(void)
{
	//Intensity = dot(vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0));
	//Intensity = dot(normalize(gl_Normal), vec3(1.0, 0.0, 0.0));

	Intensity = 1.0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
