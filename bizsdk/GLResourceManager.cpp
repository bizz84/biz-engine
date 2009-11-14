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



