/*****************************************************************************
 * Filename			GlResourceManager.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Resource manager for loading shaders and textures
 *
 *****************************************************************************/

#ifndef _GL_RESOURCE_MANAGER_H_
#define _GL_RESOURCE_MANAGER_H_

#include "Extensions.h"
#include "Misc.h"
#include "VBO.h"

#include <lib3ds/file.h>

#include "boost/ptr_container/ptr_vector.hpp"
using namespace boost;

//#include <vector>
//using namespace std;

class GLResourceManager
{

	/*************************************************************************
	 * Shader definition
	 *************************************************************************/
	class Shader
	{
	private:
		string sVertShader;
		string sFragShader;

		GLuint uiVS;
		GLuint uiFS;
		GLuint uiProgram;
	public:
		Shader(const char *vertexShader, const char *fragmentShader, GLuint vs,
			GLuint fs, GLuint program);
		~Shader();

		bool SameAs(const char *vertexShader, const char *fragmentShader) const;
		GLuint GetProgram() const { return uiProgram; }
	};

	ptr_vector<Shader> apShader;
	
	/*************************************************************************
	 * Texture definition
	 *************************************************************************/

	class Texture
	{
	private:
		string sTextureFile;

		GLuint uiTexture;

	public:
		Texture(const char *textureFile, GLuint texture);
		~Texture();

		bool SameAs(const char *textureFile) const;

		GLuint GetTexture() const { return uiTexture; }
	};

	ptr_vector<Texture> apTexture;

	/*************************************************************************
	 * 3DS files definition
	 *************************************************************************/
	class File3DS
	{
		string s3DSFile;

		Lib3dsFile *f;
	public:
		File3DS(const char *file, Lib3dsFile *obj);
		~File3DS();

		bool SameAs(const char *file) const;

		Lib3dsFile *GetFile() const { return f; }
	};

	ptr_vector<File3DS> ap3DS;

protected:
	GLResourceManager() { }

	/* Shader related members */
	static GLenum PrintShaderError(GLuint obj, bool bCompile);

	bool ReleaseShaders();

	/* Texture related members */
	bool ReleaseTextures();

	/* 3DS files related members */
	bool Release3DSFiles();



public:
	static GLResourceManager &Instance();
	~GLResourceManager();

	bool Release();

	// TODO: Add defines to shaders
	/* Shader related members */
	bool LoadShaderFromMemory(const char *vertexShader,
		const char *fragmentShader, GLuint &program);

	bool LoadShaderFromFile(const char *vertexShader,
		const char *fragmentShader, GLuint &program);

	/* Texture related members */
	bool LoadTextureFromFile(const char *textureFile, GLuint &program,
		GLint minFilter, GLint magFilter);

	/* Geometry related members */
	bool Load3DSFile(const char *file, unsigned int &index);

	void ListMeshNames(unsigned int index3DS);

	Lib3dsMesh *FindMesh(unsigned int index3DS, const char *name);
	float MeshSize(unsigned int index3DS, const char *name);
};

#endif

