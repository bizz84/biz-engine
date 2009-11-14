/*****************************************************************************
 * Filename			GlResourceManager.cpp
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Resource manager for loading shaders and textures
 *
 *****************************************************************************/

#include "GLResourceManager.h"
#include "Misc.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <SDL/SDL.h>
#include <string>

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>

/*****************************************************************************
 * GLResourceManager::Shader implementation
 *****************************************************************************/

GLResourceManager::Shader::Shader(const char *vertexShader, const char *fragmentShader, GLuint vs, GLuint fs, GLuint program)
	: uiVS(vs), uiFS(fs), uiProgram(program)
{
	strcpy(szVertShader, vertexShader);
	strcpy(szFragShader, fragmentShader);

}

GLResourceManager::Shader::~Shader()
{
	glDetachShader(uiProgram, uiVS);
	glDetachShader(uiProgram, uiFS);
	glDeleteShader(uiProgram);
}

bool GLResourceManager::Shader::SameAs(const char *vertexShader, const char *fragmentShader)
{
	return !strcmp(szVertShader, vertexShader) && !strcmp(szFragShader, fragmentShader);
}

/*****************************************************************************
 * GLResourceManager::Texture implementation
 *****************************************************************************/

GLResourceManager::Texture::Texture(const char *textureFile, GLuint texture)
	: uiTexture(texture)
{
	strcpy(szTextureFile, textureFile);
}

GLResourceManager::Texture::~Texture()
{
	glDeleteTextures(1, &uiTexture);
}

bool GLResourceManager::Texture::SameAs(const char *textureFile)
{
	return !strcmp(szTextureFile, textureFile);
}

/*****************************************************************************
 * GLResourceManager::File3DS implementation
 *****************************************************************************/


GLResourceManager::File3DS::File3DS(const char *file, Lib3dsFile *obj)
	: f(obj)
{
	strcpy(sz3DSFile, file);
}

GLResourceManager::File3DS::~File3DS()
{
	lib3ds_file_free(f);
}

bool GLResourceManager::File3DS::SameAs(const char *file)
{
	return !strcmp(sz3DSFile, file);
}



/*****************************************************************************
 * GLResourceManager implementation
 *****************************************************************************/

GLResourceManager &GLResourceManager::Instance()
{
	static GLResourceManager manager;
	return manager;
}

GLResourceManager::~GLResourceManager()
{
	Release();
}

bool GLResourceManager::Release()
{
	bool retShaders = ReleaseShaders();

	bool retTextures = ReleaseTextures();
	// TODO: Insert disposal code for other resources and AND them in the return clause

	return retShaders && retTextures;
}

/*****************************************************************************
 * Shader methods
 *****************************************************************************/

GLenum GLResourceManager::PrintShaderError(GLuint obj, const char *szShader, const char *szFunction)
{

    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		if (Verbose(VerboseInfo))
			printf(infoLog);
        free(infoLog);
    }
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("%s %s: GL error %d\n", szShader, szFunction, err);
	}
	return err;
}



bool GLResourceManager::LoadShaderFromFile(const char *vertexShader, const char *fragmentShader, GLuint &program)
{
	for (unsigned int i = 0; i < apShader.size(); i++)
	{
		if (apShader[i]->SameAs(vertexShader, fragmentShader))
		{
			program = apShader[i]->GetProgram();
			return true;
		}
	}
	char *pszVertShader = NULL;
	char *pszFragShader = NULL;

	if (Verbose(VerboseInfo))
		printf("Loading vertex shader %s... ", vertexShader);
	
	if (LoadFileIntoMemory(vertexShader, &pszVertShader) <= 0)
    {
		printf("Unable to load shader %s!\n", vertexShader);
		return false;
	}

	if (Verbose(VerboseInfo))
		printf("and fragment shader %s... ", vertexShader);

	if (LoadFileIntoMemory(fragmentShader, &pszFragShader) <= 0)
    {
		printf("Unable to load shader %s!\n", fragmentShader);
		return false;
	}

	GLuint uiVS, uiFS, uiProgram;

    uiVS = glCreateShader(GL_VERTEX_SHADER);
    uiFS = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(uiVS, 1, (const GLchar **)&pszVertShader, NULL);
    if (PrintShaderError(uiVS, vertexShader, "glShaderSource") != GL_NO_ERROR)
		return false;

    glShaderSource(uiFS, 1, (const GLchar **)&pszFragShader, NULL);
    if (PrintShaderError(uiFS, fragmentShader, "glShaderSource") != GL_NO_ERROR)
		return false;

	FreeFileMemory(&pszVertShader);
	FreeFileMemory(&pszFragShader);

	glCompileShader(uiVS);
    if (PrintShaderError(uiVS, vertexShader, "glCompileShader") != GL_NO_ERROR)
		return false;

    glCompileShader(uiFS);
    if (PrintShaderError(uiFS, fragmentShader, "glCompileShader") != GL_NO_ERROR)
		return false;

    uiProgram = glCreateProgram();

    glAttachShader(uiProgram, uiVS);
    glAttachShader(uiProgram, uiFS);
	// Clear errors
	glGetError();

	if (Verbose(VerboseInfo))
		printf("linking... ");

    glLinkProgram(uiProgram);

	//glGetError();

    //if (PrintShaderError(uiProgram, "", "glLinkProgram") != GL_NO_ERROR)
	//	return false;

	if (Verbose(VerboseInfo))
		printf("Done\n");
	
	apShader.push_back(new Shader(vertexShader, fragmentShader, uiVS, uiFS, uiProgram));

    return true;		
}



bool GLResourceManager::ReleaseShaders()
{
	for (unsigned int i = 0; i < apShader.size(); i++)
	{
		delete apShader[i];
	}
	apShader.clear();
	return true;
}


/*****************************************************************************
 * Texture methods
 *****************************************************************************/

bool GLResourceManager::LoadTextureFromFile(const char *textureFile, GLuint &texture, GLint minFilter, GLint magFilter)
{
	for (unsigned int i = 0; i < apTexture.size(); i++)
	{
		if (apTexture[i]->SameAs(textureFile))
		{
			if (Verbose(VerboseAll))
				printf("Texture %s already loaded\n", textureFile);
			texture = apTexture[i]->GetTexture();
			return true;
		}
	}

    SDL_Surface *surface;
    GLenum texture_format;
	GLint  nOfColors;

	if (LoadImage(textureFile, surface, texture_format, nOfColors))	{
 
		// Check that the image's width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) {
			printf("warning: %s's width is not a power of 2\n", textureFile);
		}
	
		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) {
			printf("warning: %s's height is not a power of 2\n", textureFile);
		}
	 
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
	 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
	 
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	 
		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
		                  texture_format, GL_UNSIGNED_BYTE, surface->pixels );

		SDL_FreeSurface( surface );
	
		apTexture.push_back(new Texture(textureFile, texture));

		return true;
		
	}
	return false;
}

bool GLResourceManager::ReleaseTextures()
{
	for (unsigned int i = 0; i < apTexture.size(); i++)
	{
		delete apTexture[i];
	}
	apTexture.clear();
	return true;
}

/*****************************************************************************
 * 3DS files methods
 *****************************************************************************/


bool GLResourceManager::Load3DSFile(const char *file, unsigned int &index)
{
	for (unsigned int i = 0; i < ap3DS.size(); i++)
	{
		if (ap3DS[i]->SameAs(file))
		{
			if (Verbose(VerboseAll))
				printf("3DS file %s already loaded\n", file);
			index = i;
			return true;
		}
	}

	Lib3dsFile *f = lib3ds_file_load(file);
	if (f == NULL)
	{
		printf("Unable to load %s\n", file);
		return false;
	}

	ap3DS.push_back(new File3DS(file, f));
	index = ap3DS.size() - 1;

	return true;
}

bool GLResourceManager::LoadMeshVBO(unsigned int index3DS, const char *name, IndexedVBO *&vbo)
{
	Lib3dsFile *f = ap3DS[index3DS]->GetFile();
	//int index = lib3ds_file_mesh_by_name(f, name);
	//if (index < 0)
	//	return false;

	Lib3dsMesh *mesh;

	if (Verbose(VerboseAll))
	{
		printf("Listing meshes:\n");
		mesh = f->meshes;
		while (mesh)
		{
			printf(" %s\n", mesh->name);
			mesh = mesh->next;
		}
	}

	mesh = f->meshes;
	while (mesh)
	{
		if (!strcmp(mesh->name, name))
			break;
		mesh = mesh->next;
	}
	if (!mesh)
		return false;

	if (Verbose(VerboseAll))
	{
		printf("Mesh has %d faces\n", mesh->faces);
	}

	int *indices = new int[mesh->faces * 3];
	for (unsigned int i = 0; i < mesh->faces; i++)
	{
		indices[3 * i + 0] = mesh->faceL[i].points[0];
		indices[3 * i + 1] = mesh->faceL[i].points[1];
		indices[3 * i + 2] = mesh->faceL[i].points[2];
	}

	vbo = new IndexedVBO(mesh->pointL, sizeof(Lib3dsPoint), mesh->points, indices, mesh->faces * 3);
	vbo->AddEntry(glVertexPointer, 3, GL_FLOAT, 0);

	delete [] indices;

	return true;
}

bool GLResourceManager::Release3DSFiles()
{
	for (unsigned int i = 0; i < ap3DS.size(); i++)
	{
		delete ap3DS[i];
	}
	ap3DS.clear();
	return true;
}


