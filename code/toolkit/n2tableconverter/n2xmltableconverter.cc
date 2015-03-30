//------------------------------------------------------------------------------
//  n2xmltableconverter.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n2xmltableconverter.h"
#include "io/excelxmlreader.h"
#include "io/ioserver.h"
#include "util/round.h"

namespace Toolkit
{
using namespace ToolkitUtil;
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
N2XmlTableConverter::N2XmlTableConverter() :
    uniqueStringTableSize(0)
{
    // empty
}    

//------------------------------------------------------------------------------
/**
*/
bool
N2XmlTableConverter::ConvertAnimsXmlTable(Logger& logger)
{
    // NOTE: cannot re-use object!
    n_assert(0 == this->uniqueStringTableSize);
    if (this->Load(logger, "proj:data/tables/anims.xml", true))
    {
        return this->Save(logger, "dst:anims/anims.bin", 'ATBL');
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
N2XmlTableConverter::ConvertHotspotsXmlTable(Logger& logger)
{
    // NOTE: cannot re-use object!
    n_assert(0 == this->uniqueStringTableSize);
    if (this->Load(logger, "proj:data/tables/hotspots.xml", false))
    {
        return this->Save(logger, "dst:data/hotspots.bin", 'HSPT');
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Load the anims.xml in Excel XML format.
*/
bool
N2XmlTableConverter::Load(Logger& logger, const Util::String& srcPath, bool tokenizeEntries)
{
    n_assert(srcPath.IsValid());
        
    logger.Print("-> loading '%s'...\n", srcPath.AsCharPtr());

    Ptr<Stream> stream = IoServer::Instance()->CreateStream(srcPath);
    Ptr<ExcelXmlReader> reader = ExcelXmlReader::Create();
    reader->SetStream(stream);
    if (reader->Open())
    {
        SizeT numRows = reader->GetNumRows() - 1;
        SizeT numColumns = reader->GetNumColumns() - 1;
        n_assert(numRows > 0);
        n_assert(numColumns > 0);

        logger.Print("-> building data structures...\n");

        // reserve array sizes
        this->columns.Reserve(numColumns);
        this->rows.Reserve(numRows);
        this->columnIndexMap.Reserve(numColumns);
        this->entryTable.SetSize(numColumns, numRows);
        this->entryNames.Reserve(4096);

        // setup the column-index map
        IndexT colIndex;
        for (colIndex = 0; colIndex < numColumns; colIndex++)
        {
            StringAtom str = reader->GetElement(0, colIndex + 1);
            this->columns.Append(this->AddUniqueString(str));
            this->columnIndexMap.Add(str, this->columns.Size() - 1);
        }

        // setup row-index map
        IndexT rowIndex;
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            StringAtom str = reader->GetElement(rowIndex + 1, 0);
            this->rows.Append(this->AddUniqueString(str));
        }

        // setup the animation entries
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            for (colIndex = 0; colIndex < numColumns; colIndex++)
            {
                const String& xmlEntry = reader->GetElement(rowIndex + 1, colIndex + 1);
                if (xmlEntry.IsEmpty())
                {
                    /*
                    logger.Warning("*** N2AnimXmlConverter: empty cell in anims.xml (row=%s, col=%s)!\n", 
                        reader->GetElement(rowIndex + 1, 0).AsCharPtr(),
                        reader->GetElement(0, colIndex + 1).AsCharPtr());
                    */
                }
                else if (xmlEntry[0] == '*')
                {
                    // a reference to another column...
                    StringAtom refColName = xmlEntry.ExtractToEnd(1);
                    if (!this->columnIndexMap.Contains(refColName))
                    {
                        n_error("N2XmlTableConverter: reference column name '%s' doesn't exist!", refColName.Value());
                    }
                    IndexT refColIndex = this->columnIndexMap[refColName];
                    if (refColIndex >= colIndex)
                    {
                        n_error("N2XmlTableConverter: reference column name '%s': no forward declaration allowed in row '%s'!",
                            refColName.Value(), reader->GetElement(rowIndex + 1, 0).AsCharPtr());
                    }

                    // copy values of referenced entry
                    const TableEntry& refEntry = this->entryTable.At(refColIndex, rowIndex);
                    this->entryTable.Set(colIndex, rowIndex, refEntry);
                }
                else
                {
                    if (tokenizeEntries)
                    {
                        Array<String> tokens = xmlEntry.Tokenize("; ");
                        TableEntry tableEntry;
                        tableEntry.startIndex = this->entryNames.Size();
                        tableEntry.numEntries = tokens.Size();
                        this->entryTable.Set(colIndex, rowIndex, tableEntry);
                        IndexT i;
                        for (i = 0; i < tokens.Size(); i++)
                        {
                            this->entryNames.Append(this->AddUniqueString(tokens[i]));
                        }
                    }
                    else
                    {
                        TableEntry tableEntry;
                        tableEntry.startIndex = this->entryNames.Size();
                        tableEntry.numEntries = 1;
                        this->entryTable.Set(colIndex, rowIndex, tableEntry);
                        this->entryNames.Append(this->AddUniqueString(xmlEntry));
                    }
                }
            }
        }
        reader->Close();
        return true;
    }
    else
    {
        logger.Error("Failed to open 'anims.xml' file!\n");
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Save the destination file.
*/
bool
N2XmlTableConverter::Save(Logger& logger, const Util::String& dstPath, uint magic)
{
    n_assert(dstPath.IsValid());

    logger.Print("-> saving binary destination file '%s'...\n", dstPath.AsCharPtr());

    Ptr<Stream> stream = IoServer::Instance()->CreateStream(dstPath);
    Ptr<BinaryWriter> writer = BinaryWriter::Create();
    writer->SetStream(stream);
    if (writer->Open())
    {
        // write the magic number
        writer->WriteUInt(magic);
        this->WriteUniqueStringTable(writer);
        this->WriteColumnNameIndices(writer);
        this->WriteRowNameIndices(writer);
        this->WriteEntryNameIndices(writer);
        this->WriteEntryTable(writer);
        
        writer->Close();
        return true;
    }
    else
    {
        logger.Error("Failed to open '%s' for writing!\n", dstPath.AsCharPtr());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Write the unique string table to the file. Note that the size
    of the string table must be rounded up to 4 bytes.
*/
void
N2XmlTableConverter::WriteUniqueStringTable(const Ptr<BinaryWriter>& writer)
{
    UniqueStringTableHeader header = { NULL };
    header.magic = 'USTT';
    header.size = sizeof(UniqueStringTableHeader) + Round::RoundUp4(this->uniqueStringTableSize);
    header.numStrings = this->uniqueStringTable.Size();
    writer->WriteRawData(&header, sizeof(header));

    SizeT bytesWritten = 0;
    IndexT i;
    for (i = 0; i < this->uniqueStringTable.Size(); i++)
    {
        String curString = this->uniqueStringTable[i].Value();
        SizeT numBytes = curString.Length() + 1;
        writer->WriteRawData(curString.AsCharPtr(), numBytes);
        bytesWritten += numBytes;
    }
    n_assert(bytesWritten == this->uniqueStringTableSize);

    SizeT numPadBytes = Round::RoundUp4(this->uniqueStringTableSize) - this->uniqueStringTableSize;
    n_assert(numPadBytes < 4);
    if (numPadBytes > 0)
    {
        uint null = 0;
        writer->WriteRawData(&null, numPadBytes);
    }
}

//------------------------------------------------------------------------------
/**
    Writes the column names as indices into the binary writer.
*/
void
N2XmlTableConverter::WriteColumnNameIndices(const Ptr<BinaryWriter>& writer)
{
    StringArrayHeader header = { NULL };
    header.magic = 'COLS';
    header.size = sizeof(StringArrayHeader) + this->columns.Size() * sizeof(ushort);
    header.numEntries = this->columns.Size();
    writer->WriteRawData(&header, sizeof(header));
    writer->WriteRawData(&this->columns.Front(), this->columns.Size() * sizeof(ushort));
}

//------------------------------------------------------------------------------
/**
    Writes the row names as indices into the binary writer.
*/
void
N2XmlTableConverter::WriteRowNameIndices(const Ptr<BinaryWriter>& writer)
{
    StringArrayHeader header = { NULL };
    header.magic = 'ROWS';
    header.size = sizeof(StringArrayHeader) + this->rows.Size() * sizeof(ushort);
    header.numEntries = this->rows.Size();
    writer->WriteRawData(&header, sizeof(header));
    writer->WriteRawData(&this->rows.Front(), this->rows.Size() * sizeof(ushort));
}

//------------------------------------------------------------------------------
/**
    Writes the entry name indices into the binary writer.
*/
void
N2XmlTableConverter::WriteEntryNameIndices(const Ptr<BinaryWriter>& writer)
{
    StringArrayHeader header = { NULL };
    header.magic = 'ENMS';
    header.size  = sizeof(StringArrayHeader) + this->entryNames.Size() * sizeof(ushort);
    header.numEntries = this->entryNames.Size();
    writer->WriteRawData(&header, sizeof(header));
    writer->WriteRawData(&this->entryNames.Front(), this->entryNames.Size() * sizeof(ushort));
}

//------------------------------------------------------------------------------
/**
    Write the actual entry table to the binary writer.
*/
void
N2XmlTableConverter::WriteEntryTable(const Ptr<BinaryWriter>& writer)
{
    SizeT tableByteSize = this->entryTable.Width() * this->entryTable.Height() * sizeof(TableEntry);

    EntryTableHeader header = { NULL };
    header.magic = 'ETBL';
    header.size = sizeof(EntryTableHeader) + tableByteSize;
    header.numColumns = this->entryTable.Width();
    header.numRows = this->entryTable.Height();
    header.elmSize = sizeof(TableEntry);
    writer->WriteRawData(&header, sizeof(header));
    writer->WriteRawData(&this->entryTable.At(0, 0), tableByteSize);
}

} // namespace Toolkit

    