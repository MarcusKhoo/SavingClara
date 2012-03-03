/*==============================================
 * Fail routine
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once

#include <string>
#include <sstream>
#include <windows.h>
#include "Fail.h"

void FAIL(const char* error,const char* title,const char* file,int line)
{
	std::stringstream sout;
	sout<<error;
	if (file!=NULL)
	{
		sout<<" @ "<<file<<":"<<line;
	}
	MessageBox(NULL,sout.str().c_str(),title,MB_ICONERROR);
	//PostQuitMessage(3);
	exit(3);	// exit fast
}
