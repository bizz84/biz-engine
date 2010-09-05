/*****************************************************************************
 * Filename			CollisionDetector.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Collision Detector
 *
 *****************************************************************************/

#include "CollisionDetector.h"
#include "Vector.h"
#include "WeaponManager.h"
#include "Bullet.h"
#include "AIManager.h"
#include "Misc.h"
#include "Enemy.h"
#include "Settings.h"
#include "QuadTree.h"

#include "boost/ptr_container/ptr_list.hpp"

using namespace std;
using namespace boost;

/*
General algorithm: A list of bullets can collide with a list of enemies.
Initially all bullets are un-exploded and as the double loop in Execute()
finds collisions, the "hit" flag is set to true in bullet objects and the
health is diminished in monsters.
When results are read back, hit bullets are removed from the array and
originate instances of BloodParticleSystems.
Enemies whose health <= 0 are removed as well and originate instances of
DeathAnimation.
*/

bool CollisionDetector::AboveHeight(float h)
{
	return h > 2.0f * Settings::Instance().EnemyHeight;
}

// Actual processing
unsigned int CollisionDetector::Run()
{
	if (Write())
	{
		unsigned int ret = Execute();
		Read();
		return ret;
	}
	return 0;
}

#define HEIGHT_TEST


unsigned int CPUCollisionDetector::Execute()
{
	ptr_list<Bullet> &bullets = (ptr_list<Bullet> &)GetWM()->GetBullets();
	ptr_vector<Enemy> &enemies = (ptr_vector<Enemy> &)GetAI()->GetData();

	if (bullets.size() == 0 || enemies.size() == 0)
		return 0;
		
	ptr_list<Bullet>::iterator b;
	ptr_vector<Enemy>::iterator e;

	unsigned int comparisons = 0;
	// TODO: Find solution cheaper than O(B * E)
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
#ifdef HEIGHT_TEST
		// discard bullets that are above the enemy height
		if (AboveHeight(b->GetPosition()[1]))
			continue;
#endif
		for (e = enemies.begin(); e != enemies.end(); e++)
		{	
			comparisons++;
			// Bullet has exploded already
			if (b->Impact())
				continue;
				
			// Enemy has been killed already
			if (e->health <= 0)
				continue;

			Point3 target3 = Point3(e->pos[0], Settings::Instance().EnemyHeight, e->pos[1]);
				
			//if (CollisionSphereSphere(curr, target3, Settings::Instance().CollisionRadius))
			if (Collision(b->GetPrevPosition(), b->GetPosition(),
				target3, Settings::Instance().CollisionRadius))
			{
				b->SetImpact();

				e->health -= b->Damage();
				GetAI()->AddParticles(b->GetPosition(), e->health);
			}
		}
	}
	return comparisons;
}


/* CPU optimised write-execute-read template */
bool CPUSegmentSphereCollisionDetector::Collision(const Vector3 &a,
	const Vector3 &b, const Vector3 &s, const float r)
{
	return CollisionSegmentSphere(a, b, s, r);
}


bool CPUSphereSphereCollisionDetector::Collision(const Vector3 &a,
	const Vector3 &b, const Vector3 &s, const float r)
{
	return CollisionSphereSphere(b, s, r);
}



unsigned int QuadTreeCollisionDetector::Execute()
{
	ptr_list<Bullet> &bullets = (ptr_list<Bullet> &)GetWM()->GetBullets();
	ptr_vector<Enemy> &enemies = (ptr_vector<Enemy> &)GetAI()->GetData();

	if (bullets.size() == 0 || enemies.size() == 0)
		return 0;

	// Generate tree
	//static const unsigned int QuadTreeDivisions = 5;
	unsigned int divisions = (unsigned int)(0.5 * sqrt((double)Settings::Instance().NumEnemies));
	QuadTree tree(divisions, enemies);

	ptr_list<Bullet>::iterator b;

	unsigned int comparisons = 0;
	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		Point3 p = b->GetPosition();
#ifdef HEIGHT_TEST
		// discard bullets that are above the enemy height
		if (AboveHeight(p[1]))
			continue;
#endif
		Point2 pos(p[0], p[2]);
		list<Enemy *> *bin = tree.GetBin(pos);
		// discard bullets that are outside the enemy area
		if (bin == NULL)
			continue;

		list<Enemy *>::iterator f;
		for (f = bin->begin(); f != bin->end(); f++)
		{	
			comparisons++;
			Enemy *e = *f;
			// Bullet has exploded already
			if (b->Impact())
				continue;
				
			// Enemy has been killed already
			if (e->health <= 0)
				continue;

			Point3 target3 = Point3(e->pos[0], Settings::Instance().EnemyHeight, e->pos[1]);
				
			//if (CollisionSphereSphere(b->GetPosition(), target3, Settings::Instance().CollisionRadius))
			if (CollisionSegmentSphere(b->GetPrevPosition(), b->GetPosition(),
				target3, Settings::Instance().CollisionRadius))
			{
				b->SetImpact();

				e->health -= b->Damage();
				GetAI()->AddParticles(b->GetPosition(), e->health);
			}
		}
	}	
	return comparisons;
}


/* proper write-execute-read template */
/*bool CPUSegmentSphereCollisionDetector::Write()
{
	// get arrays
	const list<Bullet *> &bullets = GetWM()->GetBullets();
	const vector<Enemy *> &enemies = GetAI()->GetData();

	if (bullets.size() == 0 || enemies.size() == 0)
		return false;

	// Copy pos1 and pos2
	bullet = new BulletState[bullets.size()];
	
	unsigned int i = 0;
	list<Bullet *>::const_iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++, i++)
	{
		bullet[i].prev = (*b)->GetPrevPosition();
		bullet[i].curr = (*b)->GetPosition();
		// Note: we assume that all elements in this array have not
		// exploded yet (in fact, when they do they are removed)
		bullet[i].hit = false;
	}
	SetNumColliders(bullets.size());
	// Copy targets
	
	enemy = new EnemyState[enemies.size()];
	
	i = 0;
	vector<Enemy *>::const_iterator e;
	for (e = enemies.begin(); e != enemies.end(); e++, i++)
	{	
		const Vector2 &pos = (*e)->pos;
		enemy[i].pos = Point3(pos[0], AIManager::EnemyHeight, pos[1]);
		enemy[i].health = (*e)->health;
	}
	SetNumTargets(enemies.size());
	return true;
}

void CPUSegmentSphereCollisionDetector::Execute()
{
	// double loop using CollisionSegmentSphere
	for (int i = 0; i < GetNumColliders(); i++)
	{
		for (int j = 0; j < GetNumTargets(); j++)
		{		
			// Already hit
			if (bullet[i].hit)
				continue;
				
			// Enemy has been killed already
			if (enemy[j].health <= 0)
				continue;
				
			// Once a enemy is hit, the collider does not exist anymore,
			// continue to next one
			if (CollisionSegmentSphere(bullet[i].prev, bullet[i].curr, enemy[j].pos, AIManager::CollisionRadius))
			{
				bullet[i].hit = true;
				enemy[j].health -= GravityBullet::DAMAGE;
			}
		}
	}
}


bool CPUSegmentSphereCollisionDetector::Read()
{
	// TODO: read & deallocate
	// write back to data structures
	// delete item if hit == true & generate explosion
	const list<Bullet *> &bullets = GetWM()->GetBullets();
	const vector<Enemy *> &enemies = GetAI()->GetData();

	unsigned int i = 0;
	list<Bullet *>::const_iterator b;
	for (b = bullets.begin(); b != bullets.end(); b++, i++)
	{	
		if (bullet[i].hit)
		{
			// Generate explosion
			(*b)->SetImpact();
		}
	}
	delete [] bullet;
	bullet = NULL;

	i = 0;
	vector<Enemy *>::const_iterator e;
	for (e = enemies.begin(); e != enemies.end(); e++, i++)
	{	
		(*e)->health = enemy[i].health;
	}
	delete [] enemy;
	enemy = NULL;
	
	return true;
}
*/

/*void OpenCLGrenadeEnemyCollisionDetector::Execute()
{

}



collision kernel
__kernel void fountain_kern(
        __global float3* pos1,
        __global float3* pos2,
        __global bool* hit1,
		int elements,
        __global float3* target,
        __global bool* hit2,
		int targets,
	    float radius, 
)
{       
    int gti = get_global_id(0);	
    
    if (gti >= targets)
		return;
		
	for (int i = 0; i < elements; i++)
	{
		// Already hit
		if (hit1[i])
			continue;
		if (Collision(pos1[i], pos2[i], target[gti], radius)
		{
			hit1[i] = true;
			hit2[gti] = true;
			// let others know about hit1[i]
			barrier(CL_GLOBAL_MEM_FENCE);
			return;
		}
	}
}
*/
