/*****************************************************************************
 * Filename			CollisionDetector.cpp
 * 
 * License			LGPL
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
#include "Weapon.h"
#include "AIManager.h"
#include "Misc.h"

using namespace std;

/*
General algorithm: A list of bullets can collide with a list of enemies.
Initially all bullets are un-exploded and as the double loop in Execute()
finds collisions, the "hit" flag is set to true in Grenade objects and the
health is diminished in monsters.
When results are read back, hit bullets are removed from the array and
originate instances of GrenadeExplosions.
Enemies whose health <= 0 are removed as well and originate instances of
DeathAnimation.
*/

// Actual processing
void CollisionDetector::Run()
{
	if (Write())
	{
		Execute();
		Read();
	}
}


/* CPU optimised write-execute-read template */
bool CPUGrenadeEnemyCollisionDetector::Write()
{
	return true;
}

void CPUGrenadeEnemyCollisionDetector::Execute()
{
	const list<Bullet *> &bullets = GetWS()->GetBullets();
	const vector<Enemy *> &enemies = GetAI()->GetData();
	
	if (bullets.size() == 0 || enemies.size() == 0)
		return;
		
	list<Bullet *>::const_iterator b;
	vector<Enemy *>::const_iterator e;

	Point3 prev, curr, target3;
	Point2 target2;

	for (b = bullets.begin(); b != bullets.end(); b++)
	{
		for (e = enemies.begin(); e != enemies.end(); e++)
		{	
			// Bullet has exploded already
			if ((*b)->Impact())
				continue;
				
			// Enemy has been killed already
			if ((*e)->health <= 0)
				continue;
				
			curr = (*b)->GetPosition();
			prev = (*b)->GetPrevPosition();
			target2 = (*e)->pos;
			target3 = Point3(target2[0], AIManager::EnemyHeight,target2[1]);
				
			if (CollisionSegmentSphere(prev, curr, target3, AIManager::EnemyRadius))
			{
				(*b)->SetImpact();
				//(*b)->SetPosition() here to determine the exact point
				(*e)->health -= Grenade::DAMAGE;
			}				
		}
	}	
	
}


bool CPUGrenadeEnemyCollisionDetector::Read()
{
	return true;
}



/* proper write-execute-read template */
/*bool CPUGrenadeEnemyCollisionDetector::Write()
{
	// get arrays
	const list<Bullet *> &bullets = GetWS()->GetBullets();
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

void CPUGrenadeEnemyCollisionDetector::Execute()
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
			if (CollisionSegmentSphere(bullet[i].prev, bullet[i].curr, enemy[j].pos, AIManager::EnemyRadius))
			{
				bullet[i].hit = true;
				enemy[j].health -= Grenade::DAMAGE;
			}
		}
	}
}


bool CPUGrenadeEnemyCollisionDetector::Read()
{
	// TODO: read & deallocate
	// write back to data structures
	// delete item if hit == true & generate explosion
	const list<Bullet *> &bullets = GetWS()->GetBullets();
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
*/
