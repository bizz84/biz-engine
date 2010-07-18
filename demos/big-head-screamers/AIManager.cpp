/*****************************************************************************
 * Filename			AIManager.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		AIManager
 *
 *****************************************************************************/

#include "AIManager.h"
#include "Misc.h"
#include "Sprite2D.h"

const float AIManager::ImpactDistance = 10.0f;
const float AIManager::InitialMinDistance = 100.0f;
const unsigned int AIManager::NumEnemies = 200;

AIManager::AIManager(const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	for (unsigned int i = 0; i < NumEnemies; )
	{
		Vector2 pos = Vector2(RandRange(-1000.0f, 1000.0f), RandRange(-1000.0f, 1000.0f));
		if ((pos - target).Length() < InitialMinDistance)
			continue;
		data.push_back(pos + target);
		i++;
	}
}


void AIManager::Input(const float t, const float dt, const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Vector2>::iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// Update position
		Vector2 dir = target - *iter;
		*iter += dir.Normalize() * dt * 20.0f;
		// Check impact
		if ((*iter - target).Length() < ImpactDistance)
		{
			do
			{
				*iter = target + Vector2(RandRange(-1000.0f, 1000.0f), RandRange(-1000.0f, 1000.0f));
			}
			while ((*iter - target).Length() < InitialMinDistance);
		}
	}
}

void AIManager::Render(const float height, const float angleCorr)
{
	vector<Vector2>::iterator iter;

	// TODO: Use instancing here. One draw call, extrinsics passed as attributes
	// More textures handled as texture atlas with texcoords as attributes
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		Sprite2D::Render(*iter, 0.5f * height, 20.0f, angleCorr);
	}
}
