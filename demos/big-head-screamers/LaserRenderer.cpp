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

#include "LaserRenderer.h"
#include "Misc.h"
#include "Bullet.h" // Bullet part of it
#include "GLResourceManager.h"
#include "Mesh.h"

#include <assert.h>

static const char *Textures[] = {

	"data/textures/256_64.bmp"
};

static const char *Shaders[] = {
	"data/shaders/Laser.vert", "data/shaders/Laser.frag",
};

LaserRenderer::LaserRenderer()
{
	assert(LoadShaders(Shaders, NUM_PROGRAMS));

	GLuint shader = Program(P_LASER);
	glUseProgram(shader);

	iColorLoc = GetUniLoc(shader, "Color");
	iTranslateLoc = GetUniLoc(shader, "Translate");
	iRotateLoc = GetUniLoc(shader, "Rotate");

	// Should uniform array locations be non consecutive, use this
	/*char name[15];
	for (unsigned int i = 0; i < BATCH_SIZE; i++)
	{
		sprintf(name, "Translate[%d]", i);
		iTranslateLoc[i] = GetUniLoc(shader, name);
		sprintf(name, "Rotate[%d]", i);
		iRotateLoc[i] = GetUniLoc(shader, "Rotate");
	}*/
	
	GLResourceManager &loader = GLResourceManager::Instance();
	
	// Load texture for ground
	assert(loader.LoadTextureFromFile(Textures[0],
		uiTexture, GL_LINEAR, GL_LINEAR));
	
}

void LaserRenderer::Render(const list<Bullet *> &bullets) const
{
	glDepthMask(0);

	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glUseProgram(Program(P_LASER));

	float color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(iColorLoc, 1, color);

	const float u = 1.0f;
	const float z = 0.0f;

	float afTexAttrib[] = {
		z, u,
		z, z,
		u, z,
	    u, u,
		z, u,
		z, z,
		u, z,
	    u, u
	};
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 2, afTexAttrib);


	list<Bullet *>::const_iterator iter;

	// Load uniforms
	assert(bullets.size() <= 64);

	// The rotation can be calculated as a Matrix3 on the CPU and passed as such,
	// or the two angles can be passed to the GPU as uniforms and the matrix 
	// calculated in the vertex shader.
	// The second approach is used here since the first doesn't seem to work
	unsigned int i = 0;
	//Matrix3 rotUni[64];
	Vector2 rotUni[64];
	Vector3 trUni[64];
	for (iter = bullets.begin(); iter != bullets.end(); iter++, i++)
	{
		trUni[i] = (*iter)->GetPosition();
		//rotUni[i] = (Matrix3::RotationY(-(*iter)->GetAngleY()) * Matrix3::RotationX(-(*iter)->GetAngleX()));
		rotUni[i] = Vector2(-(*iter)->GetAngleX(), -(*iter)->GetAngleY());
	}
	glUniform3fv(iTranslateLoc, 64, (float *)&trUni);
	glUniform2fv(iRotateLoc, 64, (float *)&rotUni);
	//glUniformMatrix3fv(iRotateLoc, 64, GL_TRUE, (float *)&rotUni);

	const float s = 3.0f;
	const float z1 = -s * 8.0f;
	const float z2 =  s * 8.0f;
	const float w = 1.0f;

	i = 0;
	for (iter = bullets.begin(); iter != bullets.end(); iter++, i++)
	{
		float afVertAttrib[] = {
			-s, z, z1, (float)i,
			 s, z, z1, (float)i,
			 s, z, z2, (float)i,
			-s, z, z2, (float)i,
			z, -s, z1, (float)i,
			z,  s, z1, (float)i,
			z,  s, z2, (float)i,
			z, -s, z2, (float)i,
		};
			
		glVertexPointer(4, GL_FLOAT, sizeof(float) * 4, afVertAttrib);

		// TODO: Measure performance improvement of pseudo-instanced approach
		/*glPushMatrix();
		const Point3 &pos = (*iter)->GetPosition();
		glTranslatef(pos[0], pos[1], pos[2]);
		//glScalef(AmmoSize, AmmoSize, AmmoSize);
	
		glRotatef(-(*iter)->GetAngleY() * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
		glRotatef(-(*iter)->GetAngleX() * 180.0f / M_PI, 1.0f, 0.0f, 0.0f);
		*/
		glDrawArrays(GL_QUADS, 0, 4); // 8 to draw vertical one as well
		
		//glPopMatrix();
	}
	glDepthMask(1);
}
