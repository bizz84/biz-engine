/*****************************************************************************
 * Filename			version.h
 * 
 * License			LGPL
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Version for BigHeadScreamers
 *
 *****************************************************************************/

#ifndef _VERSION_H_
#define _VERSION_H_

/* TODO

- Improve collision detection:
http://www.gamedev.net/reference/programming/features/AbsPolyOpt/
- StraightBullet gun
- OpenAL integration
- 3D Meshes
- Jumping
- Explosions
- Armageddon mode (with particles and stuff exploding everywhere)
- Bloody rain
- Game logic
- Crosshair
- HUD

*/

/* Version History
V0.1.2
- Added laser gun
V0.1.1
- Some code cleanup
- Added FontManager and TextGraph, FontGothic, FontTechno
V0.1.0
- Started porting to Boost
- Boost pointer containers integration
V0.0.9
- Settings on a file
- Added FOV control
V0.0.8
- Proper GravityBullet model
- Big classes redesign
V0.0.7
- Working istancing
- Blood
V0.0.6
- Collision detection between enemies
- Formalized Weapon and AI subsystems
- Some code redesign
V0.0.5
- Added Enemies as sprites
- Rearranged rendering code
V0.0.4
- Working reflection shader on Ground
V0.0.3
- More code cleanup
V0.0.2
- Working infinite plane
- Working SkyBox
V0.0.1
- Working grenades
- Plane rendering + projection functions
- Run&strafe on plane

*/


const char szAppVersion[] = "0.1.2";
const char szAppName[] = "Big Head Screamers";


#endif

