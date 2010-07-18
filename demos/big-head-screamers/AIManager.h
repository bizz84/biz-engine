/*****************************************************************************
 * Filename			AIManager.h
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
#ifndef _AI_MANAGER_H_
#define _AI_MANAGER_H_

#include "Extensions.h"
#include "Vector.h"

#include <vector>

using namespace std;

class AIManager
{
	vector<Vector2> data;
public:
	AIManager(const Vector3 &player);

	void Input(const float t, const float dt, const Vector3 &player);

	void Render(const float height, const float angleCorr);

	static const float ImpactDistance;
	static const float InitialMinDistance;
	static const unsigned int NumEnemies;
};

#endif