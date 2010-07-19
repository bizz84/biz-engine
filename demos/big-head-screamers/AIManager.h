/*****************************************************************************
 * Filename			AIManager.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		AIManager defines the logic for the enemies and (currently)
 *                  the rendering code as well.
 *
 *****************************************************************************/
#ifndef _AI_MANAGER_H_
#define _AI_MANAGER_H_

#include "Extensions.h"
#include "Vector.h"

#include <vector>

using namespace std;

// AIManager defines the logic for the enemies and (currently) the rendering
// code as well (move this somewhere else?)
class AIManager
{
	enum { NUM_SPRITES = 2 };
	GLuint uiSprite[NUM_SPRITES];

	struct SpriteData
	{
		Vector2 pos;
		int texIndex;

		SpriteData(const Vector2 &p, const int index)
			: pos(p), texIndex(index) { }
	};
	vector<SpriteData> data;

	bool LoadSprites();
public:
	AIManager(const Vector3 &player);

	void Input(const float t, const float dt, const Vector3 &player);

	void Render(const float height, const float angleCorr);

	static const float ImpactDistance;
	static const float InitialMinDistance;
	static const unsigned int NumEnemies;
	static const float MaxDistance;
};

#endif