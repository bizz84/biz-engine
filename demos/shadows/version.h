/*****************************************************************************
 * Filename			version.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Lighting & Shadows demo using stencil buffers
 *
 *****************************************************************************/

#ifndef _VERSION_H_
#define _VERSION_H_

/* Log History 

V1.1.3
- Correct Shadow Volume precomputation in ShadowVolume.cpp
- ShowVolumeShadows renders transparent shadow volumes
- Occluders can be grouped together
- Added PreRenderGeometry and PostRenderGeometry
- Added RenderPlane
- Added FPS BaseGraph

*/


const char szAppVersion[] = "1.1.4";
const char szAppName[] = "Vertex Shader Depth-fail Shadow Volumes";


#endif

