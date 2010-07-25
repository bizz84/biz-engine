/*****************************************************************************
 * Filename			Weapon.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/

#include "Bullet.h"

/*****************************************************************************
 * Bullet implementation
 *****************************************************************************/

 Bullet::Bullet(const Point3 &p, const float yRot, const float xRot,
	 const float speed) : xRot(xRot), yRot(yRot)
{
	impact = false;
	pos[1] = -p;
	pos[1][1] -= 10.0f;
	pos[0] = pos[1];
	vel = Matrix3::RotationY(-yRot) * (Matrix3::RotationX(-xRot) *
		Vector3(0.0f, 0.0f, -speed));		
}
		
/*****************************************************************************
 * Grenade implementation
 *****************************************************************************/

Grenade::Grenade(const Point3 &p, const float yRot, const float xRot,
				 const float speed) : Bullet(p, yRot, xRot, speed)
{
	bounces = 0;
}


bool Grenade::Update(float dt)
{
	pos[0] = pos[1];
	dt *= Settings::Instance().BulletSpeed;
	float f = Settings::Instance().BulletGravity * dt;

	vel[1] -= f;

	pos[1] += vel * dt;

	if (pos[1][1] < 0.0f)
	{
		bounces++;
		pos[1][1] = -pos[1][1];
		vel[0] *= Settings::Instance().GrenadeBounceSlowDown;
		vel[1] *= -Settings::Instance().GrenadeBounceSlowDown;
		vel[2] *= Settings::Instance().GrenadeBounceSlowDown;
	}
	if (bounces >= Settings::Instance().GrenadeMaxBounces)
		impact = true;

	xRot = -atan2(vel[1], Point2(vel[0], vel[2]).Length());
	
	return !Impact();

}
