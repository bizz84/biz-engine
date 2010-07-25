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

#include "EnemyRendererAttrib.h"
#include "GLResourceManager.h"
#include "Enemy.h"
#include "Misc.h"
#include "Settings.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>

/*****************************************************************************
 * EnemyRendererAttrib implementation
 *****************************************************************************/
static const char *AttribShaders[] = {
	"data/shaders/SpriteAttrib.vert", "data/shaders/Sprite.frag",
};

EnemyRendererAttrib::EnemyRendererAttrib()
{
	assert(LoadShaders(AttribShaders, NUM_PROGRAMS));
	assert(LoadSprites());

	GLuint program = UseProgram(P_SPRITE_ATTRIB);

	attribLoc[A_VERTEX] = glGetAttribLocation(program, "inVertex");
	attribLoc[A_TEX_COORD] = glGetAttribLocation(program, "inTexCoord");
	attribLoc[A_SCALE] = glGetAttribLocation(program, "inScale");
	attribLoc[A_ROT_ANGLE] = glGetAttribLocation(program, "inRotAngle");
	attribLoc[A_TRANSLATE] = glGetAttribLocation(program, "inTranslate");

	attrib = new SpriteVertexData[4 * Settings::Instance().NumEnemies];
}
EnemyRendererAttrib::~EnemyRendererAttrib()
{
	delete [] attrib;
}

bool EnemyRendererAttrib::LoadSprites()
{
	GLResourceManager &loader = GLResourceManager::Instance();
	// Load texture atlas
	return loader.LoadTextureFromFile("data/textures/sprites/Atlas.bmp",
			uiAtlas, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
}

Point2 EnemyRendererAttrib::TexCoord(Point2 coord, int index)
{
	coord[0] *= 0.25f;
	coord[1] *= 0.5f;
	coord[0] += (index % 4) * 0.25f;
	coord[1] += (index / 4) * 0.5f;
	return coord;
}

bool EnemyRendererAttrib::Update(const vector<Enemy *> &data, const float angle,
							const float height)
{
	// Render if there's at least one enemy
	if (!data.size())
		return false;

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
		ptr->tex = TexCoord(Point2( 0.0f,  1.0f), texIndex);
		ptr->SetAttributes(20.0f, radAngle, translation);
		ptr++;
		
		ptr->pos = Point2( 0.5f, -1.0f);
		ptr->tex = TexCoord(Point2( 1.0f,  1.0f), texIndex);
		ptr->SetAttributes(20.0f, radAngle, translation);
		ptr++;

		ptr->pos = Point2( 0.5f,  1.0f);;
		ptr->tex = TexCoord(Point2( 1.0f,  0.0f), texIndex);
		ptr->SetAttributes(20.0f, radAngle, translation);
		ptr++;
		
		ptr->pos = Point2(-0.5f,  1.0f);;
		ptr->tex = TexCoord(Point2( 0.0f,  0.0f), texIndex);
		ptr->SetAttributes(20.0f, radAngle, translation);
		ptr++;
	}
	return true;
}

GLuint EnemyRendererAttrib::UseProgram(int index) const
{
	GLuint program = Program(index);
	glUseProgram(program);
	// Set uniforms
	//float offset[] = { 4.0f / 2048.0f, 4.0f / 2048.0f };
	//glUniform2fv(GetUniLoc(program, "NeighborOffset"), 1, offset);
	return program;
}

bool EnemyRendererAttrib::SetAttribPointer(GLint loc, size_t size, GLenum type, void *address) const
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

bool EnemyRendererAttrib::UnsetAttribPointer(GLint loc) const
{
	if (loc != -1)
	{
		glDisableVertexAttribArray(loc);
		return true;
	}
	return false;
}


// BindTexture happens outside
void EnemyRendererAttrib::Render(const vector<Enemy *> &data, const float angle,
							const float height) const
{
	// Render if there's at least one enemy
	if (!data.size())
		return;

	size_t size = data.size() << 2;
	
	glBindTexture(GL_TEXTURE_2D, uiAtlas);

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
}


