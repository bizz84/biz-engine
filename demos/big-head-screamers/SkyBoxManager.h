/*****************************************************************************
 * Filename			Weapon.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/
#ifndef _SKYBOX_MANAGER_H_
#define _SKYBOX_MANAGER_H_

#include <stdio.h>
#include "Extensions.h"
#include "SkyBox.h"
#include "Misc.h"

class SkyBoxManager
{
	enum { NUM_CUBEMAPS = 5 };

	bool init;
	// Cubemaps data
	float fCubemapTransitionStart;
	float fCubemapTransitionCur;
	unsigned int uiPrevCubemap;
	unsigned int uiCurCubemap;
	CubeMap cubemap[NUM_CUBEMAPS];

	const CubeMap &PrevCubemap() const { return cubemap[uiPrevCubemap]; }
	const CubeMap &CurrCubemap() const { return cubemap[uiCurCubemap]; }
	void NextCubemap() { uiCurCubemap = Next(uiCurCubemap, NUM_CUBEMAPS); }
	void PrevCubemap() { uiCurCubemap = Prev(uiCurCubemap, NUM_CUBEMAPS); }
	bool CubemapTransition() const;
	float CubemapTransitionTime() const;
public:
	SkyBoxManager();

	void CubemapUpdate(bool next, const float time);
	void Update(const float time);

	void Render() const;
};

#endif
