
uniform vec3 Translate[64];
uniform vec2 Rotate[64];
//uniform mat3 Rotate[64];

const float u = 1.0;
const float z = 0.0;

mat3 RotationX(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return mat3(
		u, z, z,
		z, c, s,
		z,-s, c);
}

mat3 RotationY(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return mat3(
		 c, z,-s,
		 z, u, z,
		 s, z, c);
}

void main(void)
{
	vec3 pos = gl_Vertex.xyz;
	int id = int(gl_Vertex.w);
	
	pos = Translate[id] + RotationY(Rotate[id].y) * RotationX(Rotate[id].x) * pos;
	//pos = Translate[id] + Rotate[id] * pos;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);

	/*vec4 pos = vec4(gl_Vertex.xyz, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
	
	vec4 pos = vec4(gl_Vertex.xyz, 1.0);*/
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);

	gl_TexCoord[0] = gl_MultiTexCoord0;
}
