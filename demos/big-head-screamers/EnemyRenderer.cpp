/*****************************************************************************
 * Filename			Weapon.cpp
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

#include "EnemyRenderer.h"
#include "GLResourceManager.h"
#include "Enemy.h"
#include "Sprite2D.h"
#include "Misc.h"
#include "AIManager.h"
#include "UnitTest.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>

// Shaders
static const char *Shaders[] = {
	"data/shaders/Sprite.vert", "data/shaders/Sprite.frag",
	"data/shaders/SpriteAttrib.vert", "data/shaders/Sprite.frag",
	"data/shaders/TestAttrib.vert", "data/shaders/Lookup.frag"
};



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

EnemyRenderer::EnemyRenderer()
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));
	assert(LoadSprites());

	GLuint program = UseProgram(P_SPRITE_ATTRIB);

	attribLoc[A_VERTEX] = glGetAttribLocation(program, "inVertex");
	attribLoc[A_TEX_COORD] = glGetAttribLocation(program, "inTexCoord");
	attribLoc[A_TEX_INDEX] = glGetAttribLocation(program, "inTexIndex");
	attribLoc[A_SCALE] = glGetAttribLocation(program, "inScale");
	attribLoc[A_ROT_ANGLE] = glGetAttribLocation(program, "inRotAngle");
	attribLoc[A_TRANSLATE] = glGetAttribLocation(program, "inTranslate");

	attrib = new SpriteVertexData[4 * AIManager::NumEnemies];
}
EnemyRenderer::~EnemyRenderer()
{
	delete [] attrib;
}

bool EnemyRenderer::LoadSprites()
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


GLuint EnemyRenderer::UseProgram(EProgram index) const
{
	GLuint program = Program(index);
	glUseProgram(program);
	// Set uniforms
	float offset[] = { 4.0f / 512.0f, 4.0f / 1024.0f };
	glUniform2fv(GetUniLoc(program, "NeighborOffset"), 1, offset);
	return program;
}

bool EnemyRenderer::SetAttribPointer(GLint loc, size_t size, GLenum type, void *address) const
{
	if (loc != -1)
	{
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, size, type, GL_FALSE,
			sizeof(SpriteVertexData), address);
		return true;
	}
	return false;
}

bool EnemyRenderer::UnsetAttribPointer(GLint loc) const
{
	if (loc != -1)
	{
		glDisableVertexAttribArray(loc);
		return true;
	}
	return false;
}


// BindTexture happens outside
void EnemyRenderer::Render(const vector<Enemy *> &data, const float angle,
							const float height) const
{
	// Render if there's at least one enemy
	if (!data.size())
		return;


	// Create attribute array to be passed to GL
	// vertices = # sprites * 4
	size_t size = data.size() << 2;

	// Use this if # of enemies is not capped
	//SpriteVertexData *attrib = new SpriteVertexData[size];
	
	float radAngle = angle * M_PI / 180.0f;
	// Loop through sprites: Sprites are made by groups of four vertices
	// sharing the same attributes
	vector<Enemy *>::const_iterator iter;
	SpriteVertexData *ptr = attrib;
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		int texIndex = (dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex();
		Vector3 translation = Point3((*iter)->pos[0], 0.5f * height, (*iter)->pos[1]);
	
		// positions-texcoords loop
		ptr->pos = Point2(-0.5f, -1.0f);
		ptr->tex = Point2( 0.0f,  1.0f);
		ptr->SetAttributes(texIndex, 20.0f, radAngle, translation);
		ptr++;
		
		ptr->pos = Point2( 0.5f, -1.0f);
		ptr->tex = Point2( 1.0f,  1.0f);
		ptr->SetAttributes(texIndex, 20.0f, radAngle, translation);
		ptr++;

		ptr->pos = Point2( 0.5f,  1.0f);;
		ptr->tex = Point2( 1.0f,  0.0f);
		ptr->SetAttributes(texIndex, 20.0f, radAngle, translation);
		ptr++;
		
		ptr->pos = Point2(-0.5f,  1.0f);;
		ptr->tex = Point2( 0.0f,  0.0f);
		ptr->SetAttributes(texIndex, 20.0f, radAngle, translation);
		ptr++;
	}
	
	glBindTexture(GL_TEXTURE_2D, uiSprite[0]);

	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Choose program
	GLuint program = UseProgram(P_SPRITE_ATTRIB);

	// Bind regular vertex array and pass it to GL. MUST be used in the shader
	glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertexData), &attrib->pos);
	glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertexData), &attrib->tex);

	// Pass in attributes
	// This ones shouldn't be necessary if glVertexPointer and
	// glTexCoordPointer are issued
	SetAttribPointer(attribLoc[A_VERTEX], 2, GL_FLOAT, &attrib->pos);
	SetAttribPointer(attribLoc[A_TEX_COORD], 2, GL_FLOAT, &attrib->tex);
	// Custom attributes
	SetAttribPointer(attribLoc[A_TEX_INDEX], 1, GL_INT, &attrib->texIndex);
	SetAttribPointer(attribLoc[A_SCALE], 1, GL_FLOAT, &attrib->scale);
	SetAttribPointer(attribLoc[A_ROT_ANGLE], 1, GL_FLOAT, &attrib->rotAngle);
	SetAttribPointer(attribLoc[A_TRANSLATE], 3, GL_FLOAT, &attrib->translation);

	// Render VBO
	glDrawArrays(GL_QUADS, 0, size);
	
	// Once finished, disable arrays
	for (unsigned int i = 0; i < P_ATTRIBS; i++)
		UnsetAttribPointer(attribLoc[i]);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	

	//delete [] attrib;


	// Alternative method: render each sprite one by one
	/*program = UseProgram(P_SPRITE);
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		glBindTexture(GL_TEXTURE_2D,
			uiSprite[(dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex()]);
		Sprite2D::Render((*iter)->pos, 0.5f * height, 20.0f, angle);
	}*/
}


