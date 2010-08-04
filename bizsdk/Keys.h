/*****************************************************************************
 * Filename			TTFont.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		True Type font rendering
 *
 *****************************************************************************/

#ifndef _KEYS_H_
#define _KEYS_H_

#ifdef __linux__

#define KEY_SPACE 65

#define KEY_1 10
#define KEY_2 11
#define KEY_3 12
#define KEY_4 13
#define KEY_5 14
#define KEY_6 15
#define KEY_7 16
#define KEY_8 17
#define KEY_9 18
#define KEY_0 19

#define KEY_Q 24
#define KEY_W 25
#define KEY_E 26
#define KEY_R 27

#define KEY_A 38
#define KEY_S 39
#define KEY_D 40
#define KEY_F 41


#define KEY_ENTER 36

#define KEY_UP 111
#define KEY_DOWN 116
#define KEY_LEFT 113
#define KEY_RIGHT 114

#else
#define KEY_SPACE 57
#define KEY_1 2
#define KEY_2 3
#define KEY_3 4
#define KEY_4 5
#define KEY_5 6
#define KEY_6 7
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_0 11

#define KEY_Q 16
#define KEY_W 17
#define KEY_E 18
#define KEY_R 19

#define KEY_A 30
#define KEY_S 31
#define KEY_D 32
#define KEY_F 33

#define KEY_ENTER 28

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#endif

#endif

