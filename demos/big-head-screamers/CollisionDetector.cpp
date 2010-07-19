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

// Actual processing
void CollisionDetector::Run()
{
	Write();
	Execute();
	Read();
}



void CPUGrenadeEnemyCollisionDetector::Write()
{
	// TODO: allocate & write
	// pos1, pos2, target = new []
	// copy
	const list<Grenade> &grenades = GetLauncher()->GetGrenades();
	
	pos = new PosState[grenades.size()];
	
	unsigned int i;
	list<Grenade>::const_iterator iter;
	for (iter = grenades.begin(); iter != grenades.end(); iter++)
	{
		//pos[i].prev = 
		//pos[i].curr =
		pos[i].hit = false;
	}	
	// Same thing for targets
}

void CPUGrenadeEnemyCollisionDetector::Execute()
{
	// double loop using CollisionSegmentSphere
    /*int gti = get_global_id(0);	
    
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
	}*/	
}
void CPUGrenadeEnemyCollisionDetector::Read()
{
	// TODO: read & deallocate
	// write back to data structures
	// delete item if hit == true & generate explosion
	
	delete [] pos;
	pos = NULL;
}



/*void OpenCLGrenadeEnemyCollisionDetector::Execute()
{

}
*/
