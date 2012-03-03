/*==============================================
 * routine to convert almost anything to a string for C++
 *
 * Written by Mark Gossage
 *==============================================*/
#pragma once
#include <sstream>

/** Generic (templated) ToString function, converts just about anything to a string.
Within Java it is possible to have code looking a bit like this
\code
// WARNING: Java code, will not work in C++
SomeClass obj = ...;
String s = obj + " a string";
System.out.println(s);
\endcode
This provides a similar function with a slightly different syntax
\code
// This WILL work in C++
SomeClass obj = ...;
string s = ToString(obj) + " a string";
DrawD3DFont(font,s,0,0,WHITE_COL);
\endcode
There are also other versions of this function which take up to 4 parameters
\code
// This will work in C++
SomeClass obj = ...;
string s = ToString(obj," a string",3.142);	// s=obj+"a string"+3.142
DrawD3DFont(font,s,0,0,WHITE_COL);
\endcode
The functions return an std::string, so if you want to convert into a char*
you will need to ToString(whatever).c_str()

\note This function is a templated function, its not been explained in the module.
Look up 'templates' if you want to know more about them.
*/
template<class T>
inline std::string ToString(const T& t1)
{
	std::stringstream sout;
	sout<<t1;
	return sout.str();
}

template<class T1,class T2>
inline std::string ToString(const T1& t1,const T2& t2)
{
	std::stringstream sout;
	sout<<t1<<t2;
	return sout.str();
}

template<class T1,class T2,class T3>
inline std::string ToString(const T1& t1,const T2& t2,const T3& t3)
{
	std::stringstream sout;
	sout<<t1<<t2<<t3;
	return sout.str();
}

template<class T1,class T2,class T3,class T4>
inline std::string ToString(const T1& t1,const T2& t2,const T3& t3,const T4& t4)
{
	std::stringstream sout;
	sout<<t1<<t2<<t3<<t4;
	return sout.str();
}

#include <stdio.h>

/** Alternative to-string code based upon printf.
For those who know how to use printf, you will like this.
For those who don't know printf, its probably better not to use this.
EG.
\code
DrawD3DFont(mpFont,format("Rot: %.2f %.2f %.2f",D2R(rot.x),D2R(rot.y),D2R(rot.z)),650,60,WHITE_COL);
\endcode

\note because of the way the static's work, you cannot call this function twice in the same line of code
(it will give the same result twice)
*/
static const char* format(const char* fmt, ... )
{
	static char buff[256];
	va_list args;				// declaring a argument list
    va_start(args, fmt);		// initialise the argument list
	vsnprintf(buff,256,fmt,args);	// put data into buffer
    va_end(args);
	return buff;
}
