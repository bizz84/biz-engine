/*****************************************************************************
 * Filename			version.h
 * 
 * License			GPLv3
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

/* TODO


*/

/* Version History 
V1.1.7
- MipMapped texture
- Added new font, removed GL state changes
V1.1.6
- Added First Person Shooter Camera mode

V1.1.5
- Added support for multiple lights
- Code reformatting
- Known Issue: when multiple lights are used, self shadowing is wrong.
  This is due to areas in shadow from a light viewpoint that are visible from
  the eye position.

V1.1.3
- Correct Shadow Volume precomputation in ShadowVolume.cpp
- ShowVolumeShadows renders transparent shadow volumes
- Occluders can be grouped together
- Added PreRenderGeometry and PostRenderGeometry
- Added RenderPlane
- Added FPS BaseGraph

*/


const char szAppVersion[] = "1.1.7";
const char szAppName[] = "Vertex Shader Depth-fail Shadow Volumes";


#endif

