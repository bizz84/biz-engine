/*****************************************************************************
 * Filename			Weapon.cpp
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Weapon
 *
 *****************************************************************************/

#include "SkyBoxManager.h"
#include "GLResourceManager.h"
#include "Settings.h"
	
// Credit for cubemaps:
// http://www.alusion-fr.com/
static const char *CubemapFaesky[] = {
	"data/textures/Skybox_faesky/faesky02left.bmp",
	"data/textures/Skybox_faesky/faesky02right.bmp",
	"data/textures/Skybox_faesky/faesky02up.bmp",
	"data/textures/Skybox_faesky/faesky02down.bmp",
	"data/textures/Skybox_faesky/faesky02back.bmp",
	"data/textures/Skybox_faesky/faesky02front.bmp",
};

static const char *CubemapAlpine[] = {
	"data/textures/Skybox_alpine/alpine_east.bmp",
	"data/textures/Skybox_alpine/alpine_west.bmp",
	"data/textures/Skybox_alpine/alpine_up.bmp",
	"data/textures/Skybox_alpine/alpine_down.bmp",
	"data/textures/Skybox_alpine/alpine_north.bmp",
	"data/textures/Skybox_alpine/alpine_south.bmp",
};

static const char *CubemapCanyon[] = {
	"data/textures/Skybox_canyon/lnhcanyons1_fr.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_bk.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_up.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_dn.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_rt.bmp",
	"data/textures/Skybox_canyon/lnhcanyons1_lt.bmp",
};

static const char *CubemapTaosky[] = {
	"data/textures/Skybox_taosky/skybox3_right.bmp",
	"data/textures/Skybox_taosky/skybox3_back.bmp",
	"data/textures/Skybox_taosky/skybox3_top.bmp",
	"data/textures/Skybox_taosky/skybox3_bottom.bmp",
	"data/textures/Skybox_taosky/skybox3_front.bmp",
	"data/textures/Skybox_taosky/skybox3_left.bmp",
};

static const char *CubemapMntsea[] = {
	"data/textures/Skybox_mntsea/lnhmtns1_fr.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_bk.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_up.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_dn.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_rt.bmp",
	"data/textures/Skybox_mntsea/lnhmtns1_lt.bmp",
};


static const char **Cubemaps[6] = {
	CubemapFaesky,
	CubemapAlpine,
	CubemapCanyon,
	CubemapTaosky,
	CubemapMntsea,
};


SkyBoxManager::SkyBoxManager() :
	fCubemapTransitionStart(0.0f),
	fCubemapTransitionCur(0.0f),
	uiPrevCubemap(0),
	uiCurCubemap(0),
	init(false)
{
	// Initialize skybox singleton
	if (!SkyBox::Instance().Init())
		return;

	if (!SkyBoxTransition::Instance().Init())
		return;

	for (unsigned int i = 0; i < NUM_CUBEMAPS; i++)
	{
		if (!cubemap[i].Init(Cubemaps[i]))
			return;
	}
	init = true;
	assert(init);
}


void SkyBoxManager::CubemapUpdate(bool next, const float time)
{
	fCubemapTransitionStart = time;
	uiPrevCubemap = uiCurCubemap;
	if (next)
		NextCubemap();
	else
		PrevCubemap();
}
float SkyBoxManager::CubemapTransitionTime() const
{
	return fCubemapTransitionCur / Settings::Instance().SkyBoxTransitionTime;
}

bool SkyBoxManager::CubemapTransition() const
{
	return fCubemapTransitionCur < Settings::Instance().SkyBoxTransitionTime;
}

void SkyBoxManager::Update(const float time)
{
	fCubemapTransitionCur = time - fCubemapTransitionStart;
}

void SkyBoxManager::Render() const
{
	if (!init)
		return;

	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	if (CubemapTransition())
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CurrCubemap().Get());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, PrevCubemap().Get());

		SkyBoxTransition::Instance().Render(CubemapTransitionTime());
	}
	else
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, CurrCubemap().Get());

		SkyBox::Instance().Render();
	}		
	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
}
