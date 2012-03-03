/*==============================================
 * Console output utility
 *
 * Written by Mark Gossage
 *
 *==============================================*/
#pragma once

#include <iostream>

/** \file ConsoleOutput.h Console output for Windows application
This is a clever linker setting to make the console window
appear in your windows application.
This means that you can use cout (or printf) for your displays.

You could use the Windows OutputDebugString() function, but this
seems to be slow. Console seems to work better.

If you want this only to be in debug mode (good idea),
try this:

\code
#ifdef _DEBUG
#include "ConsoleOutput.h"
#endif
\endcode

\note you will need to comment out your cout's accordingly.
They will not work if used, and may slow your machine down slightly.
*/

#pragma comment(linker, "/subsystem:console /ENTRY:WinMainCRTStartup")	// console, but starting at Winmain
