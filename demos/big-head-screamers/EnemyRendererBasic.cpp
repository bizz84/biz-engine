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

#include "EnemyRendererBasic.h"
#include "GLResourceManager.h"
#include "Misc.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>


/*****************************************************************************
 * EnemyRendererBasic implementation
 *****************************************************************************/
static const char *BasicShaders[] = {
	"data/shaders/Sprite.vert", "data/shaders/Sprite.frag",
};

static const char *BaseSprites[] = {
	"data/textures/Sprites/BER01.bmp",
	"data/textures/Sprites/BER02.bmp",
	"data/textures/Sprites/OBI01.bmp",
	"data/textures/Sprites/OBI02.bmp",
	"data/textures/Sprites/PUTIN01.bmp",
	"data/textures/Sprites/PUTIN02.bmp",
	"data/textures/Sprites/SARKO01.bmp",
	"data/textures/Sprites/SARKO02.bmp",
};

EnemyRendererBasic::EnemyRendererBasic()
{
	assert(LoadShaders(BasicShaders, NUM_PROGRAMS));
	assert(LoadSprites());
}

bool EnemyRendererBasic::LoadSprites()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	for (unsigned int i = 0; i < NUM_SPRITES; i++)
	{
		// Load texture for ground
		if (!loader.LoadTextureFromFile(BaseSprites[i],
			uiSprite[i], GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR))
			return false;
	}
	return true;
}

void EnemyRendererBasic::Render(const vector<Enemy *> &data, const float angle,
		const float height) const
{
	vector<Enemy *>::const_iterator iter;
	// Alternative method: render each sprite one by one
	glUseProgram(Program(P_SPRITE));
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		glBindTexture(GL_TEXTURE_2D,
			uiSprite[(dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex()]);

		glPushMatrix();

		glTranslatef((*iter)->pos[0],  0.5f * height, (*iter)->pos[1]);
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		glScalef(20.0f, 20.0f, 20.0f);

		RenderQuad2D(-0.5f, -1.0f, 1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		glPopMatrix();
	}
}
