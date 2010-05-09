
__kernel void fountain_kern(
        __global float4* pos,
        __global float4* vel,
        __global float4* hash,
		int elements,
		float t,
	    float dt, 
		float g
)
{       

    int gti = get_global_id(0);	
    
    if (gti >= elements)
		return;

	float f = g * dt;

	vel[gti].y -= f;
	
	pos[gti] += vel[gti] * (float4)(dt, dt, dt, 0.0);
	pos[gti].w += dt;
	
	if (pos[gti].y < 0.0f)
	{
		vel[gti].w += 1.0f;
		if (vel[gti].w > hash[gti].w)
		{
			//float arg = hash[gti];
			vel[gti].x = hash[gti].x;
			vel[gti].y = 5.0 + hash[gti].y;
			vel[gti].z = hash[gti].z;
			vel[gti].w = 0.0;

			pos[gti] = (float4)(0.0f,0.0f,0.0f,0.0f);
		}
		else
		{
			pos[gti].y = -pos[gti].y;
			vel[gti].y *= -0.5f;
		}
	}
}