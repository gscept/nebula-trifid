#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::N2XmlTableConverter
    
    Converter class for Nebula2 anims.xml and hotspots.xml table.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "toolkitutil/logger.h"
#include "util/dictionary.h"
#include "util/stringatom.h"
#include "util/fixedtable.h"
#include "io/binarywriter.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class N2XmlTableConverter
{
public:
    /// constructor
    N2XmlTableConverter();

    /// perform anim.xml conversion
    bool ConvertAnimsXmlTable(ToolkitUtil::Logger& logger);
    /// perform hotspots.xml conversion
    bool ConvertHotspotsXmlTable(ToolkitUtil::Logger& logger);
    
private:
    /// load anims table / hotspot table xml file
    bool Load(ToolkitUtil::Logger& logger, const Util::String& srcPath, bool tokenizeEntries);
    /// save destination binary file
    bool Save(ToolkitUtil::Logger& logger, const Util::String& dstPath, uint magic);
    /// add a string to the unique string table
    ushort AddUniqueString(const Util::StringAtom& str);
    /// write unique string table data block
    void WriteUniqueStringTable(const Ptr<IO::BinaryWriter>& writer);
    /// write column name indices
    void WriteColumnNameIndices(const Ptr<IO::BinaryWriter>& writer);
    /// write row name indices
    void WriteRowNameIndices(const Ptr<IO::BinaryWriter>& writer);
    /// write entry name indices
    void WriteEntryNameIndices(const Ptr<IO::BinaryWriter>& writer);
    /// write entry table 
    void WriteEntryTable(const Ptr<IO::BinaryWriter>& writer);

    #pragma pack(push,1)
    struct TableEntry
    {
        /// constructor
        TableEntry() : startIndex(0xffff), numEntries(0) {};
                
        ushort startIndex;
        ushort numEntries;
    };
    
    struct UniqueStringTableHeader
    {
        uint magic;
        uint size;          // number of bytes to skip to next block
        uint numStrings;

        // data block is strings separated by 0's
    };
    struct StringArrayHeader    // used for rows, columns and entry names
    {
        uint magic;
        uint size;
        uint numEntries;

        // data block is ushort indices into string table
    };
    struct EntryTableHeader
    {
        uint magic;
        uint size;
        uint numColumns;
        uint numRows;
        uint elmSize;

        // data block is array of TableEntry structs
    };
    #pragma pack(pop)

    Util::Array<ushort> columns;        // indices into uniqueStringTable
    Util::Array<ushort> rows;           // indices into uniqueStringTable
    Util::Dictionary<Util::StringAtom, IndexT> columnIndexMap;
    Util::Array<ushort> entryNames;     // indices into uniqueStringTable
    Util::FixedTable<TableEntry> entryTable;

    Util::Array<Util::StringAtom> uniqueStringTable;
    Util::Dictionary<Util::StringAtom, IndexT> uniqueStringIndexMap;
    SizeT uniqueStringTableSize;        // accumulated size of all strings
};

//------------------------------------------------------------------------------
/**
*/
inline ushort
N2XmlTableConverter::AddUniqueString(const Util::StringAtom& str)
{
    n_assert(this->uniqueStringTable.Size() < 65535);
    IndexT mapIndex = this->uniqueStringIndexMap.FindIndex(str);
    if (InvalidIndex != mapIndex)
    {
        return this->uniqueStringIndexMap.ValueAtIndex(mapIndex);
    }
    else
    {
        this->uniqueStringTable.Append(str);
        IndexT i = this->uniqueStringTable.Size() - 1;
        this->uniqueStringIndexMap.Add(str, i);
        this->uniqueStringTableSize += strlen(str.Value()) + 1;
        return i;
    }
}

} // namespace Toolkit
//------------------------------------------------------------------------------
    