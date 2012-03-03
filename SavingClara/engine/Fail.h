/*==============================================
 * Fail routine
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once

/** generic failure routine.
Pops up a message box & then exits the application with an exit code.

The GDEV engine uses FAIL extensively. If its unable to load a file, or set a screen mode or any other
serious problem occurs. The engine is designed to FAIL() (hopefully with a useful message) and exit neatly.

\param error,title the infomation to display on the message box
\param [optional]file,line the file & line where the error occurs
*/
void FAIL(const char* error="I give up!",const char* title="FAIL",const char* file=NULL,int line=0);

/// TODO macro.
/// its to highlight to students the work they must do
/// it pops up the message giving the item todo, the file & line, then exits.
/// If you ever see this message, then it means you have missed some coding.
#define TODO(X) FAIL(#X,"TODO",__FILE__,__LINE__)