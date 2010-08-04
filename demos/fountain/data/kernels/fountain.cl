
typedef struct Particle_tag
{
	float4 pos;
	float4 vel;
	float4 hash;
} Particle;

__kernel void fountain_kern_array(
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
			vel[gti] = hash[gti];
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

__kernel void fountain_kern_stride(
        __global Particle* particle,
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

	particle[gti].vel.y -= f;
	
	particle[gti].pos += particle[gti].vel * (float4)(dt, dt, dt, 0.0);
	particle[gti].pos.w += dt;
	
	if (particle[gti].pos.y < 0.0f)
	{
		particle[gti].vel.w += 1.0f;
		if (particle[gti].vel.w > particle[gti].hash.w)
		{
			particle[gti].vel = particle[gti].hash;
			particle[gti].vel.w = 0.0;

			particle[gti].pos = (float4)(0.0f,0.0f,0.0f,0.0f);
		}
		else
		{
			particle[gti].pos.y = -particle[gti].pos.y;
			particle[gti].vel.y *= -0.5f;
		}
	}
}