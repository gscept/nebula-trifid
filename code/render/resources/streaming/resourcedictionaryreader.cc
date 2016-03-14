//------------------------------------------------------------------------------
//  resourcedictionaryreader.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcedictionaryreader.h"
#include "util/string.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "io/binaryreader.h"

namespace Resources
{
using namespace IO;
using namespace Util;

__ImplementClass(Resources::ResourceDictionaryReader, 'RDCR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceDictionaryReader::ResourceDictionaryReader(void)
{
}

//------------------------------------------------------------------------------
/**
*/
ResourceDictionaryReader::~ResourceDictionaryReader(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceDictionaryReader::CreateResourceDictionary(const URI& dstPath, Dictionary<ResourceId, ResourceInfo*>& dict)
{
    n_assert2(IoServer::Instance()->FileExists(dstPath), ("ResourceDictionary not found at " + dstPath.AsString() + "\nRun ResourceDictionaryBuilder first.").AsCharPtr());
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(dstPath);
    Ptr<BinaryReader> reader = BinaryReader::Create();
    reader->SetStream(stream);

    // @todo: get current platform from projectinfo.xml
    /*
    if (ToolkitUtil::Platform::Win32 == this->platform)
    {
        reader->SetStreamByteOrder(ByteOrder::LittleEndian);
    }
    else
    {
        reader->SetStreamByteOrder(ByteOrder::BigEndian);
    }
    */
    reader->SetStreamByteOrder(System::ByteOrder::LittleEndian);

    if (reader->Open())
    {
        // order:
        // FourCC-code          : uint
        // version              : uint
        // dictionary-entries   : uint

        // texType              : int
        // texWidth             : uint
        // texHeight            : uint
        // texDepth             : uint
        // mipLevels            : uint
        // pixelFormat          : int
        // size                 : uint
        // Id                   : uchar[]
        // texType and pixelFormat are read are enums and so read as int
        // - if this may produce problems on platforms change to other data-type
        // here and in ToolkitUtil::ResourceDictionaryBuilder
        FourCC classCode(reader->ReadUInt());
        uint version = reader->ReadUInt();
        uint dictSize = reader->ReadUInt();

        uint i;
        dict.BeginBulkAdd();
        for (i = 0; i < dictSize; i++)
        {
            TextureInfo* newInfo = n_new(TextureInfo);
            //ResourceId newResourceId;
            uchar buffer[MaxResIdSize] = {0};
            newInfo->SetType(CoreGraphics::Texture::Type(reader->ReadInt()));
            newInfo->SetWidth(reader->ReadUInt());
            newInfo->SetHeight(reader->ReadUInt());
            newInfo->SetDepth(reader->ReadUInt());
            newInfo->SetMipLevels(reader->ReadUInt());
            newInfo->SetPixelFormat(CoreGraphics::PixelFormat::Code(reader->ReadInt()));
            // HACK: change format from R8G8B8 to A8R8G8B8
            if (newInfo->GetPixelFormat() == CoreGraphics::PixelFormat::R8G8B8)
            {
                n_error("R8G8B8 is 24bit and should not be used. Convert to A8R8G8B8 or other valid format");
                newInfo->SetPixelFormat(CoreGraphics::PixelFormat::A8R8G8B8);
            }
            newInfo->SetSize(reader->ReadUInt());
            reader->ReadRawData(buffer, sizeof(buffer));
            ResourceId newResourceId(buffer);
            
            dict.Add(newResourceId, newInfo);
        }
        dict.EndBulkAdd();
    }
    else
    {
	n_error("Failed to open %s for reading", stream->GetURI().AsString().AsCharPtr());
    }
}
} // namespace Resources
//------------------------------------------------------------------------------
