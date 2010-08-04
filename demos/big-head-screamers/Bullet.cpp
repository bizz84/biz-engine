/*****************************************************************************
 * Filename			Weapon.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/

#include "Bullet.h"
#include "BulletRenderer.h"

/*****************************************************************************
 * Bullet implementation
 *****************************************************************************/

 Bullet::Bullet(const Point3 &p,
	 const float yRot, const float xRot, const float speed, const float damage)
	 : xRot(xRot), yRot(yRot), damage(damage)
{
	impact = false;
	pos[1] = -p;
	pos[1][1] -= 10.0f;
	pos[0] = pos[1];
	vel = Matrix3::RotationY(-yRot) * (Matrix3::RotationX(-xRot) *
		Vector3(0.0f, 0.0f, -speed));		
}


		
/*****************************************************************************
 * GravityBullet implementation
 *****************************************************************************/

bool GravityBullet::Update(const float dt)
{
	pos[0] = pos[1];
	float speed = dt * Settings::Instance().BulletSpeed;
	float f = Settings::Instance().BulletGravity * speed;

	vel[1] -= f;

	pos[1] += vel * speed;

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


/*****************************************************************************
 * GravityBullet implementation
 *****************************************************************************/
bool StraightBullet::Update(float dt)
{
	float speed = dt * Settings::Instance().LaserSpeed;
	pos[0] = pos[1];
	pos[1] += vel * speed;

	if (pos[1][1] <= 0.0f)
		impact = true;

	if ((distance += speed) > Settings::Instance().LaserMaxDistance)
		impact = true;

	return !Impact();
}

