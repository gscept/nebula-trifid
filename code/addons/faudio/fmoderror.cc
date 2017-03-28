//------------------------------------------------------------------------------
//  fmoderror.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/fmoderror.h"

#include <fmod_errors.h>

//------------------------------------------------------------------------------
/**
*/
void FAudio::fmodCheckError(int result, const char *file, int line)
{
    if(result == FMOD_OK) return;
    n_error("FMOD error %d: %s\nFILE: %s\nLine:%d\n", result, FMOD_ErrorString((FMOD_RESULT)result), file, line);
}

//------------------------------------------------------------------------------
/**
*/
void FAudio::fmodCheckErrorExt(int result, const char *file, int line, const char *msg, ...)
{
    if(result == FMOD_OK) return;

    char buffer[1024];
    va_list ap; 
    va_start(ap, msg);
#if __WIN32__
    // need to use non-CRT thread safe function under Win32
    StringCchVPrintf(buffer, 1024, msg, ap);
#else
    vsnprintf(buffer, 1024, msg, ap);
#endif
    // ya never know..
    buffer[1023] = '\n';
    va_end(ap);
    n_error("%s\nFMOD error %d: %s\nFILE: %s\nLine:%d\n", buffer, result, FMOD_ErrorString((FMOD_RESULT)result), file, line);
}
