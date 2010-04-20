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

#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include <string>

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>


/*****************************************************************************
 * GLResourceManager::Shader implementation
 *****************************************************************************/



GLResourceManager::Shader::Shader(const char *vertexShader,
	const char *fragmentShader, GLuint vs, GLuint fs, GLuint program)
	: uiVS(vs), uiFS(fs), uiProgram(program)
{
	strcpy(szVertShader, vertexShader);
	strcpy(szFragShader, fragmentShader);

}

GLResourceManager::Shader::~Shader()
{
	//glDetachShader(uiProgram, uiVS);
	//glDetachShader(uiProgram, uiFS);
	glDeleteShader(uiVS);
	glDeleteShader(uiFS);
	glDeleteProgram(uiProgram);
}

bool GLResourceManager::Shader::SameAs(const char *vertexShader,
	const char *fragmentShader)
{
	return !strcmp(szVertShader, vertexShader) &&
	       !strcmp(szFragShader, fragmentShader);
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

	bool ret3DS = Release3DSFiles();

	return retShaders && retTextures && ret3DS;
}

/*****************************************************************************
 * Shader methods
 *****************************************************************************/



GLenum GLResourceManager::PrintShaderError(GLuint obj, bool bCompile)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		if (Verbose(VerboseAll))
			printf ("%s GL error %d\n", bCompile ? "Compile" : "Link", err);
	}
	
	int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	if (bCompile)
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	else
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        if (bCompile)
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		else
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		if (Verbose(VerboseAll))
			printf ("%s", infoLog);
        free(infoLog);
    }

	return err;
}

bool GLResourceManager::LoadShaderFromMemory(const char *vertexShader,
	const char *fragmentShader, GLuint &program)
{
	for (unsigned int i = 0; i < apShader.size(); i++)
	{
		if (apShader[i]->SameAs(vertexShader, fragmentShader))
		{
			program = apShader[i]->GetProgram();
			return true;
		}
	}

	GLint vertCompiled, fragCompiled;//, linked;
	GLuint uiVS, uiFS;

    uiVS = glCreateShader(GL_VERTEX_SHADER);
    uiFS = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vs = vertexShader;
	const char *fs = fragmentShader;
    
    glShaderSource(uiVS, 1, &vs, NULL);
    if (PrintShaderError(uiVS, true)
		!= GL_NO_ERROR)
		return false;


    glShaderSource(uiFS, 1, &fs, NULL);
    if (PrintShaderError(uiFS, true)
		!= GL_NO_ERROR)
		return false;

	if (Verbose(VerboseInfo))
		printf("Compiling vertex shader %s... ", vertexShader);

	glCompileShader(uiVS);
	glGetShaderiv(uiVS, GL_COMPILE_STATUS, &vertCompiled);
	if (!vertCompiled)
	{
		printf("\nUnable to compile %s\n", vertexShader);
		return false;
	}
   	if (PrintShaderError(uiVS, true)
		!= GL_NO_ERROR)
		return false;

	if (Verbose(VerboseInfo))
		printf("and fragment shader %s... ", fragmentShader);

    glCompileShader(uiFS);
	glGetShaderiv(uiVS, GL_COMPILE_STATUS, &fragCompiled);
	if (!fragCompiled)
	{
		printf("\nUnable to compile %s\n", fragmentShader);
		return false;
	}
    if (PrintShaderError(uiFS, true)
		!= GL_NO_ERROR)
		return false;

    program = glCreateProgram();

    glAttachObjectARB(program, uiVS);
	PrintShaderError(uiVS, true);
    glAttachObjectARB(program, uiFS);
	PrintShaderError(uiFS, true);

	if (Verbose(VerboseInfo))
		printf("linking... ");

    glLinkProgram(program);
    if (PrintShaderError(program, false)
		!= GL_NO_ERROR)
		return false;
	//glGetShaderiv(program, GL_LINK_STATUS, &linked);
	//if (!linked)
	//{
	//	printf("\nUnable to link %s %s\n", vertexShader, fragmentShader);
	//	return false;
	//}

	// Clear errors
	glGetError();

	if (Verbose(VerboseInfo))
		printf("Done\n");
	
	apShader.push_back(new Shader(vertexShader, fragmentShader, uiVS, uiFS,
		program));

    return true;	
}


bool GLResourceManager::LoadShaderFromFile(const char *vertexShader,
	const char *fragmentShader, GLuint &program)
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

	if (LoadFileIntoMemory(vertexShader, &pszVertShader) <= 0)
    {
		printf("Unable to load shader %s!\n", vertexShader);
		return false;
	}

	if (LoadFileIntoMemory(fragmentShader, &pszFragShader) <= 0)
    {
		printf("Unable to load shader %s!\n", fragmentShader);
		return false;
	}

	GLint vertCompiled, fragCompiled;//, linked;
	GLuint uiVS, uiFS;

    uiVS = glCreateShader(GL_VERTEX_SHADER);
    uiFS = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vs = pszVertShader;
	const char *fs = pszFragShader;
    
    glShaderSource(uiVS, 1, &vs, NULL);
    if (PrintShaderError(uiVS, true)
		!= GL_NO_ERROR)
		return false;


    glShaderSource(uiFS, 1, &fs, NULL);
    if (PrintShaderError(uiFS, true)
		!= GL_NO_ERROR)
		return false;

	FreeFileMemory(&pszVertShader);
	FreeFileMemory(&pszFragShader);

	if (Verbose(VerboseInfo))
		printf("Compiling vertex shader %s... ", vertexShader);

	glCompileShader(uiVS);
	glGetShaderiv(uiVS, GL_COMPILE_STATUS, &vertCompiled);
	if (!vertCompiled)
	{
		printf("\nUnable to compile %s\n", vertexShader);
		return false;
	}
   	if (PrintShaderError(uiVS, true)
		!= GL_NO_ERROR)
		return false;

	if (Verbose(VerboseInfo))
		printf("and fragment shader %s... ", fragmentShader);

    glCompileShader(uiFS);
	glGetShaderiv(uiVS, GL_COMPILE_STATUS, &fragCompiled);
	if (!fragCompiled)
	{
		printf("\nUnable to compile %s\n", fragmentShader);
		return false;
	}
    if (PrintShaderError(uiFS, true)
		!= GL_NO_ERROR)
		return false;

    program = glCreateProgram();

    glAttachObjectARB(program, uiVS);
	PrintShaderError(uiVS, true);
    glAttachObjectARB(program, uiFS);
	PrintShaderError(uiFS, true);

	if (Verbose(VerboseInfo))
		printf("linking... ");

    glLinkProgram(program);
    if (PrintShaderError(program, false)
		!= GL_NO_ERROR)
		return false;
	//glGetShaderiv(program, GL_LINK_STATUS, &linked);
	//if (!linked)
	//{
	//	printf("\nUnable to link %s %s\n", vertexShader, fragmentShader);
	//	return false;
	//}

	// Clear errors
	glGetError();

	if (Verbose(VerboseInfo))
		printf("Done\n");
	
	apShader.push_back(new Shader(vertexShader, fragmentShader, uiVS, uiFS,
		program));

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

bool GLResourceManager::LoadTextureFromFile(const char *textureFile,
	GLuint &texture, GLint minFilter, GLint magFilter)
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

	if (LoadImage(textureFile, surface, texture_format, nOfColors))
	{
 
		// Check that the image's width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 )
		{
			printf("warning: %s's width is not a power of 2\n", textureFile);
		}
	
		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 )
		{
			printf("warning: %s's height is not a power of 2\n", textureFile);
		}
	 
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
	 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
	 
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	 
		// Edit the texture object's image data using the information
		// SDL_Surface gives us
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

void GLResourceManager::ListMeshNames(unsigned int index3DS)
{
	Lib3dsFile *f = ap3DS[index3DS]->GetFile();
	Lib3dsMesh *mesh = f->meshes;
	printf("Listing meshes:\n");
	while (mesh)
	{
		printf(" %s\n", mesh->name);
		mesh = mesh->next;
	}
}

Lib3dsMesh *GLResourceManager::FindMesh(unsigned int index3DS, const char *name)
{
	Lib3dsFile *f = ap3DS[index3DS]->GetFile();
	Lib3dsMesh *mesh = f->meshes;

	while (mesh)
	{
		if (!strcmp(mesh->name, name))
			break;
		mesh = mesh->next;
	}
	return mesh;
}


float GLResourceManager::MeshSize(unsigned int index3DS, const char *name)
{
	Lib3dsMesh *mesh = FindMesh(index3DS, name);
	if (!mesh)
		return 0.0f;

	// Bounding Box calculation
	float
		minX = mesh->pointL[0].pos[permutation[0]],
		maxX = minX,
		minY = mesh->pointL[0].pos[permutation[1]],
		maxY = minY,
		minZ = mesh->pointL[0].pos[permutation[2]],
		maxZ = minZ;

	unsigned int i;

	for (i = 1; i < mesh->points; ++i)
	{
		if (mesh->pointL[i].pos[permutation[0]] < minX)
		   minX = mesh->pointL[i].pos[permutation[0]];
		if (mesh->pointL[i].pos[permutation[0]] > maxX)
		   maxX = mesh->pointL[i].pos[permutation[0]];

		if (mesh->pointL[i].pos[permutation[1]] < minY)
		   minY = mesh->pointL[i].pos[permutation[1]];
		if (mesh->pointL[i].pos[permutation[1]] > maxY)
		   maxY = mesh->pointL[i].pos[permutation[1]];

		if (mesh->pointL[i].pos[permutation[2]] < minZ)
		   minZ = mesh->pointL[i].pos[permutation[2]];
		if (mesh->pointL[i].pos[permutation[2]] > maxZ)
		   maxZ = mesh->pointL[i].pos[permutation[2]];
	}

	// dimensions calculation: max is the maximum dimension
	// dimX = max{dy, dz}
	// dimY = max{dx, dz}
	// dimZ = max{dy, dx}
	float dimY = maxX - minX;
	float dimZ = maxY - minY;
	float dimX = maxZ - minZ;
	float max = dimX;
	if (dimY > max)
	   max = dimY;
	if (dimZ > max)
	   max = dimZ;

	return max;
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


