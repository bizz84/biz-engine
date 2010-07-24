

varying float Intensity;

void main()
{
	Intensity = max(0.2, dot(gl_Normal, vec3(0.0, 1.0, 0.0)));
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
