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
const unsigned int AIManager::NumEnemies = 50;
const float AIManager::MaxDistance = 1000.0f;

const float AIManager::EnemyHeight = 20.0f;
const float AIManager::EnemyRadius = 20.0f;



static const char *Sprites[] = {
	"data/textures/Sprites/BER01.bmp",
	"data/textures/Sprites/BER02.bmp",
	"data/textures/Sprites/OBI01.bmp",
	"data/textures/Sprites/OBI02.bmp",
	"data/textures/Sprites/PUTIN01.bmp",
	"data/textures/Sprites/PUTIN02.bmp",
	"data/textures/Sprites/SARKO01.bmp",
	"data/textures/Sprites/SARKO02.bmp",
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
		int texture = (rand() % (NUM_SPRITES >> 1));
		data.push_back(EnemyFactory::NewEnemy(pos + target, 100,
			texture << 1, (texture << 1) + 1));
		i++;
	}
	LoadSprites();
}

AIManager::~AIManager()
{
	vector<Enemy *>::iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
	{
		delete *iter;
	}
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
	return true;
}

void AIManager::Spawn(vector<Enemy *>::iterator &iter, const Vector2 &player)
{
	do
	{
		(*iter)->pos = player + Vector2(RandRange(-MaxDistance, MaxDistance),
                                     RandRange(-MaxDistance, MaxDistance));
	}
	while (((*iter)->pos - player).Length() < InitialMinDistance);
	(*iter)->health = 100;
}


void AIManager::Input(const float t, const float dt, const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Enemy *>::iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// Update position
		Vector2 dir = target - (*iter)->pos;
		(*iter)->pos += dir.Normalize() * dt * 20.0f;
		// Check impact
		if (((*iter)->pos - target).Length() < ImpactDistance)
		{
			Spawn(iter, target);
		}
	}
}

void AIManager::UpdateState(const Vector3 &player)
{
	const Vector2 target = Vector2(player[0], player[2]);
	vector<Enemy *>::iterator iter;
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		if ((*iter)->Dead())
		{
			Spawn(iter, target);
		}
	}
}


void AIManager::Render(const float angleCorr)
{
	vector<Enemy *>::iterator iter;

	// TODO: Use instancing here. One draw call, extrinsics passed as attributes
	// More textures handled as texture atlas with texcoords as attributes
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		glBindTexture(GL_TEXTURE_2D, uiSprite[(dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex()]);
		Sprite2D::Render((*iter)->pos, 0.5f * EnemyHeight, 20.0f, angleCorr);
	}
}
