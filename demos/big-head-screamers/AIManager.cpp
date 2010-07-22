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

#include <stddef.h>

const float AIManager::ImpactDistance = 10.0f;
const float AIManager::InitialMinDistance = 100.0f;
const unsigned int AIManager::NumEnemies = 100;
const float AIManager::MaxDistance = 1000.0f;

const float AIManager::EnemyHeight = 20.0f;
const float AIManager::EnemyRadius = 15.0f;



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
		data.push_back(NewEnemy(pos + target, 100,
			texture << 1, (texture << 1) + 1));
		i++;
	}
	//LoadSprites();
}

AIManager::~AIManager()
{
	vector<Enemy *>::iterator iter;

	for (iter = data.begin(); iter != data.end(); iter++)
	{
		delete *iter;
	}
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
	
/*#define MEMBER_OFFSET(s, var) (s + ((ptrdiff_t)&s->texIndex - (ptrdiff_t)s))

void AIManager::Render(const float angleCorr, GLuint program)
{
	if (data.size())
	{
		glGetError();
		vector<Enemy *>::iterator iter;

		// Prepare attrib data for rendering
		// vertices = # sprites * 4
		size_t size = data.size() << 2;
		SpriteVertexData *attrib = new SpriteVertexData[size];
		
		SpriteVertexData *first, *ptr = attrib;
		for (iter = data.begin(); iter != data.end(); iter++)
		{
			// Set attributes for first vertex of quad and repeat for the other three
			first = ptr;

			first->texIndex = (dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex();
			first->scale = 20.0f;
			first->rotAngle = angleCorr;
			first->translation = Point3((*iter)->pos[0], 0.5f * EnemyHeight, (*iter)->pos[1]);

		
			//-0.5f, -1.0f, 1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f
			
			// positions-texcoords loop
			ptr->pos = Point2(-0.5f, -1.0f);
			ptr->tex = Point2( 0.0f,  1.0f);
			ptr++;
			
			*ptr = *first;
			ptr->pos = Point2( 0.5f, -1.0f);
			ptr->tex = Point2( 1.0f,  1.0f);
			ptr++;

			*ptr = *first;
			ptr->pos = Point2( 0.5f,  1.0f);;
			ptr->tex = Point2( 1.0f,  0.0f);
			ptr++;
			
			*ptr = *first;
			ptr->pos = Point2(-0.5f,  1.0f);;
			ptr->tex = Point2( 0.0f,  0.0f);
			ptr++;
		}

		char *attribs = (char *)attrib;
		//glBindBuffer(

		//glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertexData), attribs);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, tex));
		GLint loc;
		//glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

		//glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertexData), attrib); 

		// todo: link with:
		//void glBindAttribLocation( uint program, uint index, const char *name );
		
		loc = glGetAttribLocation(program, "inVertex");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), attrib);
		loc = glGetAttribLocation(program, "inTexCoord");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, tex));
		loc = glGetAttribLocation(program, "inTexIndex");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 1, GL_INT,   GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, texIndex));
		loc = glGetAttribLocation(program, "inScale");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, scale));
		//loc = glGetAttribLocation(program, "inRotAngle");
		//glEnableVertexAttribArray(loc);
		//glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, rotAngle));
		loc = glGetAttribLocation(program, "inTranslate");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, translation));
		
		
		int err = glGetError();
		assert(err != GL_INVALID_VALUE);

		// Render VBO
		glDrawArrays(GL_QUADS, 0, size);
		
		delete [] attrib;

		assert(glGetError() == GL_NO_ERROR);

		// TODO: Use instancing here. One draw call, extrinsics passed as attributes
		// More textures handled as texture atlas with texcoords as attributes
		for (iter = data.begin(); iter != data.end(); iter++)
		{
			glBindTexture(GL_TEXTURE_2D, uiSprite[(dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex()]);
			Sprite2D::Render((*iter)->pos, 0.5f * EnemyHeight, 20.0f, angleCorr);
		}
	}
}
*/