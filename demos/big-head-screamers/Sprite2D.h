/*****************************************************************************
 * Filename			Sprite2D.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Sprite2D
 *
 *****************************************************************************/
#ifndef _SPRITE_2D_H_
#define _SPRITE_2D_H_

#include "Extensions.h"

#include "Vector.h"

#include <iostream>

class Sprite2D
{

public:
	Sprite2D();

	static void Render(const Vector2 &planePos, const float height,
					  const float scale, const float angleCorr);
};

struct SpriteVertexData
{
	Point2 pos;
	Point2 tex;
	
	int texIndex;
	// TODO: Generate a world matrix instead?
	float scale;
	float rotAngle;
	Vector3 translation;
	
	void SetAttributes(int index, float s, float rot, const Vector3 &t)
	{
		texIndex = index;
		scale = s;
		rotAngle = rot;
		translation = t;
	}

	void DebugPrint()
	{	
		cout << "POS[" << pos[0] << "," << pos[1] << "] TEX[" << tex[0] << "," << tex[1]
			<< "] I[" << texIndex << "] R[" << rotAngle << "] T[" << translation[0]
			<< "," << translation[1] << "," << translation[2] << "]\n";
		//printf("POS[%.2f,%.2f] TEX[%.2f,%.2f] I[%.2f] S[%.2f] R[%.2f] T[%.2f,%.2f,%.2f]\n",
		//	pos[0], pos[1], tex[0], tex[1], texIndex, scale, rotAngle, translation[0], translation[1], translation[2]);
	}
};

#endif