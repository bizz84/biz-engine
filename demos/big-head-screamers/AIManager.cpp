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
#include "GLResourceManager.h"

const float AIManager::ImpactDistance = 10.0f;
const float AIManager::InitialMinDistance = 100.0f;
const unsigned int AIManager::NumEnemies = 200;
const float AIManager::MaxDistance = 1000.0f;


static const char *Sprites[] = {
	"data/textures/Sprites/berlusconi.bmp",
	"data/textures/Sprites/obama.bmp"
};

AIManager::AIManager(const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	for (unsigned int i = 0; i < NumEnemies; )
	{
		Vector2 pos = Vector2(RandRange(-MaxDistance, MaxDistance),
		                      RandRange(-MaxDistance, MaxDistance));
		if ((pos - target).Length() < InitialMinDistance)
			continue;
		data.push_back(SpriteData(pos + target, rand() % NUM_SPRITES));
		i++;
	}
	LoadSprites();
}

bool AIManager::LoadSprites()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < NUM_SPRITES; i++)
	{
		// Load texture for ground
		if (!loader.LoadTextureFromFile(Sprites[i],
			uiSprite[i], GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR))
			return false;
	}
}




void AIManager::Input(const float t, const float dt, const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<SpriteData>::iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// Update position
		Vector2 dir = target - iter->pos;
		iter->pos += dir.Normalize() * dt * 20.0f;
		// Check impact
		if ((iter->pos - target).Length() < ImpactDistance)
		{
			do
			{
				iter->pos = target + Vector2(RandRange(-MaxDistance, MaxDistance),
		                                     RandRange(-MaxDistance, MaxDistance));
			}
			while ((iter->pos - target).Length() < InitialMinDistance);
		}
	}
}

void AIManager::Render(const float height, const float angleCorr)
{
	vector<SpriteData>::iterator iter;

	// TODO: Use instancing here. One draw call, extrinsics passed as attributes
	// More textures handled as texture atlas with texcoords as attributes
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		glBindTexture(GL_TEXTURE_2D, uiSprite[iter->texIndex]);
		Sprite2D::Render(iter->pos, 0.5f * height, 20.0f, angleCorr);
	}
}
