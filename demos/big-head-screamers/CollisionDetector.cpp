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
#include "Misc.h"
#include "Weapon.h"

using namespace std;

/*
General algorithm: A list of grenades can collide with a list of enemies.
Initially all grenades are un-exploded and as the double loop in Execute()
finds collisions, the "hit" flag is set to true in Grenade objects and the
health is diminished in monsters.
When results are read back, hit grenades are removed from the array and
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

bool CPUGrenadeEnemyCollisionDetector::Write()
{
	// get arrays
	const list<Bullet *> &grenades = GetWS()->GetBullets();
	const vector<Enemy *> &enemies = GetAI()->GetData();

	if (grenades.size() == 0 || enemies.size() == 0)
		return false;

	// Copy pos1 and pos2
	bullet = new BulletState[grenades.size()];
	
	unsigned int i = 0;
	list<Bullet *>::const_iterator b;
	for (b = grenades.begin(); b != grenades.end(); b++, i++)
	{
		bullet[i].prev = (*b)->GetPrevPosition();
		bullet[i].curr = (*b)->GetPosition();
		// Note: we assume that all elements in this array have not
		// exploded yet (in fact, when they do they are removed)
		bullet[i].hit = false;
	}
	SetNumColliders(grenades.size());
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
			// Once a enemy is hit, the collider does not exist anymore,
			// continue to next one
			if (CollisionSegmentSphere(bullet[i].prev, bullet[i].curr, enemy[j].pos, AIManager::EnemyRadius))
			{
				bullet[i].hit = true;
				enemy[j].health -= Grenade::DAMAGE;
				continue;
			}
		}
	}
}


bool CPUGrenadeEnemyCollisionDetector::Read()
{
	// TODO: read & deallocate
	// write back to data structures
	// delete item if hit == true & generate explosion
	const list<Bullet *> &grenades = GetWS()->GetBullets();
	const vector<Enemy *> &enemies = GetAI()->GetData();

	unsigned int i = 0;
	list<Bullet *>::const_iterator b;
	for (b = grenades.begin(); b != grenades.end(); b++, i++)
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



/*void OpenCLGrenadeEnemyCollisionDetector::Execute()
{

}
*/
