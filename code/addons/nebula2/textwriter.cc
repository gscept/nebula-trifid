//------------------------------------------------------------------------------
//  textwriter.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "io/textwriter.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
/**
*/
template<>
void
IO::TextWriter::Write(const nString& t)
{
    this->stream->Write(t.AsCharPtr(), t.Length());
}

