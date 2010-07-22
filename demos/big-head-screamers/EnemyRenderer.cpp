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
#include "Sprite2D.h"
#include "Misc.h"

#include <assert.h>

// Shaders
static const char *Shaders[] = {
	"data/shaders/Sprite.vert", "data/shaders/Sprite.frag",
	"data/shaders/SpriteAttrib.vert", "data/shaders/Sprite.frag",
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

#define MEMBER_OFFSET(s, var) (s + ((ptrdiff_t)&s->texIndex - (ptrdiff_t)s))

// BindTexture happens outside
void EnemyRenderer::Render(const AIManager *ai, const float angle, const float height) const
{
	const vector<Enemy *> &data = ai->GetData();
	if (data.size())
	{
		glGetError();
		vector<Enemy *>::const_iterator iter;

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
			first->rotAngle = angle;
			first->translation = Point3((*iter)->pos[0], 0.5f * height, (*iter)->pos[1]);

		
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

		// Choose program
		GLuint program = UseProgram(P_SPRITE_ATTRIB);

		// Set attributes
		char *attribs = (char *)attrib;
		
		// TODO?
		//	glEnableClientState(GL_VERTEX_ARRAY);

		//glBindBuffer(

		//glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertexData), attribs);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, tex));
		//glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

		//glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertexData), attrib); 

		// todo: link with:
		//void glBindAttribLocation( uint program, uint index, const char *name );
		
		/*glEnableVertexAttribArray(attribLoc[A_VERTEX]);
		glVertexAttribPointer(attribLoc[A_VERTEX], 2, GL_FLOAT, GL_FALSE,
			sizeof(SpriteVertexData), attrib);
		glEnableVertexAttribArray(attribLoc[A_TEX_COORD]);
		glVertexAttribPointer(attribLoc[A_TEX_COORD], 2, GL_FLOAT, GL_FALSE,
			sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, tex));
		glEnableVertexAttribArray(attribLoc[A_TEX_INDEX]);
		glVertexAttribPointer(attribLoc[A_TEX_INDEX], 1, GL_INT,   GL_FALSE,
			sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, texIndex));
		glEnableVertexAttribArray(attribLoc[A_SCALE]);
		glVertexAttribPointer(attribLoc[A_SCALE], 1, GL_FLOAT, GL_FALSE,
			sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, scale));
		//glEnableVertexAttribArray(attribLoc[A_ROT_ANGLE]);
		//glVertexAttribPointer(attribLoc[A_ROT_ANGLE], 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, rotAngle));
		glEnableVertexAttribArray(attribLoc[A_TRANSLATE]);
		glVertexAttribPointer(attribLoc[A_TRANSLATE], 3, GL_FLOAT, GL_FALSE,
			sizeof(SpriteVertexData), MEMBER_OFFSET(attrib, translation));
		
		
		int err = glGetError();
		assert(err != GL_INVALID_VALUE);

		// Render VBO
		glDrawArrays(GL_QUADS, 0, size);
		
		for (unsigned int i = 0; i < P_ATTRIBS; i++)
			glDisableVertexAttribArray(attribLoc[i]);

		delete [] attrib;

		err = glGetError();
		//assert(err == GL_NO_ERROR);*/

		// Choose program
		program = UseProgram(P_SPRITE);
		// TODO: Use instancing here. One draw call, extrinsics passed as attributes
		// More textures handled as texture atlas with texcoords as attributes
		for (iter = data.begin(); iter != data.end(); iter++)
		{
			glBindTexture(GL_TEXTURE_2D, uiSprite[(dynamic_cast<SpriteEnemy *>(*iter))->GetTextureIndex()]);
			Sprite2D::Render((*iter)->pos, 0.5f * height, 20.0f, angle);
		}
	}
}


