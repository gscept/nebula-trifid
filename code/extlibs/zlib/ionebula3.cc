/* ionebula3.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

    ZLib File IO functions for Nebula3

   (C) 2007 Radon Labs GmbH   
*/
#include "stdneb.h"
#include "io/filestream.h"

#include "zlib.h"
#include "ioapi.h"
#include "ionebula3.h"

voidpf ZCALLBACK nebula3_open_file_func OF((
   voidpf opaque,
   const char* filename,
   int mode));

uLong ZCALLBACK nebula3_read_file_func OF((
   voidpf opaque,
   voidpf stream,
   void* buf,
   uLong size));

uLong ZCALLBACK nebula3_write_file_func OF((
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size));

long ZCALLBACK nebula3_tell_file_func OF((
   voidpf opaque,
   voidpf stream));

long ZCALLBACK nebula3_seek_file_func OF((
   voidpf opaque,
   voidpf stream,
   uLong offset,
   int origin));

int ZCALLBACK nebula3_close_file_func OF((
   voidpf opaque,
   voidpf stream));

int ZCALLBACK nebula3_error_file_func OF((
   voidpf opaque,
   voidpf stream));

//------------------------------------------------------------------------------
/**
*/
voidpf ZCALLBACK nebula3_open_file_func (voidpf opaque, const char* filename, int mode)
{
    n_assert(mode == (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING));
    Ptr<IO::FileStream> fileStream = IO::FileStream::Create();
    fileStream->SetAccessMode(IO::Stream::ReadAccess);
    fileStream->SetURI(filename);
    if (fileStream->Open())
    {
        fileStream->AddRef();
        return fileStream.get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
uLong ZCALLBACK nebula3_read_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret = 0;
    if (NULL != stream)
    {
        IO::FileStream* fileStream = (IO::FileStream*) stream;
        ret = fileStream->Read(buf, size);
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
uLong ZCALLBACK nebula3_write_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    n_error("nebula3_write_file_func(): Writing to ZIP archives not supported!");
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
long ZCALLBACK nebula3_tell_file_func (voidpf opaque, voidpf stream)
{
    long ret = -1;
    if (NULL != stream)
    {
        IO::FileStream* fileStream = (IO::FileStream*) stream;
        ret = fileStream->GetPosition();
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
long ZCALLBACK nebula3_seek_file_func (voidpf opaque, voidpf stream, uLong offset, int origin)
{
    long ret = -1;
    if (NULL != stream)
    {
        IO::FileStream* fileStream = (IO::FileStream*) stream;
        IO::Stream::SeekOrigin neb3Origin;
        switch (origin)
        {
            case ZLIB_FILEFUNC_SEEK_CUR:
                neb3Origin = IO::Stream::Current;
                break;
            case ZLIB_FILEFUNC_SEEK_END:
                neb3Origin = IO::Stream::End;
                break;
            case ZLIB_FILEFUNC_SEEK_SET:
                neb3Origin = IO::Stream::Begin;
                break;
            default:
                return -1;
        }
        // FIXME: hmm... should we return -1 if going past the valid file area??
        fileStream->Seek(offset, neb3Origin);
        ret = 0;
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
int ZCALLBACK nebula3_close_file_func (voidpf opaque, voidpf stream)
{
    int ret = -1;
    if (NULL != stream)
    {
        IO::FileStream* fileStream = (IO::FileStream*) stream;
        fileStream->Close();
        fileStream->Release();
        fileStream = 0;
        ret = 0;
    }
    return ret;
}

//------------------------------------------------------------------------------
/**
*/
int ZCALLBACK nebula3_error_file_func (voidpf opaque, voidpf stream)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void fill_nebula3_filefunc (zlib_filefunc_def* pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen_file = nebula3_open_file_func;
    pzlib_filefunc_def->zread_file = nebula3_read_file_func;
    pzlib_filefunc_def->zwrite_file = nebula3_write_file_func;
    pzlib_filefunc_def->ztell_file = nebula3_tell_file_func;
    pzlib_filefunc_def->zseek_file = nebula3_seek_file_func;
    pzlib_filefunc_def->zclose_file = nebula3_close_file_func;
    pzlib_filefunc_def->zerror_file = nebula3_error_file_func;
    pzlib_filefunc_def->opaque=NULL;
}
