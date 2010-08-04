/*****************************************************************************
 * Filename			version.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Fountain Simulation
 *
 *****************************************************************************/

#ifndef _VERSION_H_
#define _VERSION_H_

/* TODO
- Stability issues on PID controller

*/

/* Version History 
V0.0.4
- PID based on frame time rather than fps
V0.0.3
- Added new font
V0.0.2
- Added CPU reference implementation
V0.0.1
- Different presets for updating particle system

*/


const char szAppVersion[] = "0.0.4";
const char szAppName[] = "Water Fountain OpenCL Demo";


#endif

