/*****************************************************************************
 * Filename			Timer.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		High resolution timer
 *
 *****************************************************************************/

#include "SkyBox.h"
#include "GLResourceManager.h"
#include "Misc.h"

static const float u = 1.0f;
const float SkyBox::VertexAttrib[3 * 8] = {
	-u, -u, -u, 
	-u, -u,  u, 
	 u, -u,  u,
	 u, -u, -u,
	-u,  u, -u, 
	-u,  u,  u, 
	 u,  u,  u,
	 u,  u, -u
};

// Todo: make triangle-list compatible
const int SkyBox::ElementAttrib[24] = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	0, 1, 5, 4,
	3, 2, 6, 7,
	1, 2, 6, 5,
	0, 3, 7, 4
};

static const char VertexShader[] =
	"varying vec3 TexCoord;" \
	"void main()" \
	"{"\
		"/*gl_TexCoord[0].xyz = gl_Vertex.xyz;*/"\
		"TexCoord = gl_Vertex.xyz;"\
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"\
	"}";

static const char FragmentShader[] = 
	"uniform samplerCube sTexture;"\
	"varying vec3 TexCoord;"\
	"void main()"\
	"{"\
		"gl_FragColor = textureCube(sTexture, TexCoord);"\
	"}";

SkyBox::SkyBox()
	: init(false),
	vboCube(NULL)
{

}

SkyBox::~SkyBox()
{
	delete vboCube;
}

SkyBox &SkyBox::Instance()
{
	static SkyBox skybox;
	return skybox;
}



bool SkyBox::Init()
{
	if (init)
		return true;

	GLResourceManager &loader = GLResourceManager::Instance();

	if (!loader.LoadShaderFromMemory(VertexShader, FragmentShader, uiProgram))
		return false;

	glUniform1i(GetUniLoc(uiProgram, "sTexture"), 0);

	vboCube = new IndexedVBO((void *)VertexAttrib, sizeof(float) * 3, 8, (void *)ElementAttrib, 24);

	return (init = true);
}

void SkyBox::Render(const CubeMap &cubemap) const
{
	assert(init);

	glDepthMask(0);
	glUseProgram(uiProgram);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.Get());

	glEnableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_CULL_FACE);

	vboCube->Render(GL_QUADS);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();

	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	glDepthMask(1);

}


bool CubeMap::Init(const char *textures[])
{
	SDL_Surface *surface;
    GLenum texture_format;
	GLint  nOfColors;

	glGenTextures(1, &uiCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, uiCubeMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (unsigned int i = 0; i < 6; i++)
	{
		if (!LoadImage(textures[i], surface, texture_format, nOfColors))
			return false;

		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 
			0, 
			nOfColors, 
			surface->w,
			surface->h, 
			0, 
			texture_format, 
			GL_UNSIGNED_BYTE, 
			static_cast<const GLubyte *>(surface->pixels)
		);
	}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	return true;
}
