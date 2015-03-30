//------------------------------------------------------------------------------
//  attributetabletest.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attributetabletest.h"
#include "testattrs.h"

namespace Test
{
__ImplementClass(Test::AttributeTableTest, 'ATST', Test::TestCase);

using namespace Attr;
using namespace Math;

//------------------------------------------------------------------------------
/**
    Verify the contents of row 0.
*/
void
AttributeTableTest::VerifyRow(AttributeTable* table, int rowIndex)
{
    if ((0 == rowIndex) || (2 == rowIndex))
    {
        this->Verify(table->GetString(Attr::Name, 0) == "Bernhard");
        this->Verify(table->GetBool(Attr::Male, 0) == true);
        this->Verify(table->GetFloat(Attr::IQ, 0) == 120.2f);
        this->Verify(table->GetInt(Attr::Age, 0) == 23);
        this->Verify(table->GetFloat4(Attr::Color, 0) == float4(1.0f, 1.0f, 1.0f, 1.0f));
        this->Verify(table->GetMatrix44(Attr::Transform, 0).getrow0() == float4(1.0f, 0.0f, 0.0f, 0.0f));
        this->Verify(table->GetMatrix44(Attr::Transform, 0).getrow1() == float4(0.0f, 1.0f, 0.0f, 0.0f));
        this->Verify(table->GetMatrix44(Attr::Transform, 0).getrow2() == float4(0.0f, 0.0f, 1.0f, 0.0f));
        this->Verify(table->GetMatrix44(Attr::Transform, 0).getrow3() == float4(0.0f, 0.0f, 0.0f, 1.0f));
        this->Verify(table->GetString(Attr::CarModel, 0) == "BMW");
    }
    else if (1 == rowIndex)
    {
        this->Verify(table->GetString(Attr::Name, 1) == "Agnes");
        this->Verify(table->GetBool(Attr::Male, 1) == false);
        this->Verify(table->GetFloat(Attr::IQ, 1) == 130.0f);
        this->Verify(table->GetInt(Attr::Age, 1) == 21);
        this->Verify(table->GetFloat4(Attr::Velocity, 1) == float4(3.0f, 4.0f, 5.0f, 0.0f));
        this->Verify(table->GetFloat4(Attr::Color, 1) == float4(0.5f, 0.5f, 0.5f, 0.5f));
        this->Verify(table->GetString(Attr::CarModel, 1) == "Audi");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AttributeTableTest::Run()
{
    // create an attribute table
    Ptr<AttributeTable> table = AttributeTable::Create();

    // initialize table columns
    table->AddColumn(Attr::Name);
    table->AddColumn(Attr::Male);
    table->AddColumn(Attr::IQ);
    table->AddColumn(Attr::Age);
    table->AddColumn(Attr::Velocity);
    table->AddColumn(Attr::Color);
    table->AddColumn(Attr::Transform);
    table->AddColumn(Attr::CarModel);

    // verify column data
    this->Verify(table->GetNumColumns() == 8);
    this->Verify(table->HasColumn(Attr::Name));
    this->Verify(table->HasColumn(Attr::Male));
    this->Verify(table->HasColumn(Attr::IQ));
    this->Verify(table->HasColumn(Attr::Age));
    this->Verify(table->HasColumn(Attr::Velocity));
    this->Verify(table->HasColumn(Attr::Color));
    this->Verify(table->HasColumn(Attr::Transform));
    this->Verify(0 == table->GetColumnIndex(Attr::Name));
    this->Verify(1 == table->GetColumnIndex(Attr::Male));
    this->Verify(2 == table->GetColumnIndex(Attr::IQ));
    this->Verify(3 == table->GetColumnIndex(Attr::Age));
    this->Verify(4 == table->GetColumnIndex(Attr::Velocity));
    this->Verify(5 == table->GetColumnIndex(Attr::Color));
    this->Verify(6 == table->GetColumnIndex(Attr::Transform));
    this->Verify(table->GetColumnId(0) == Attr::Name);
    this->Verify(table->GetColumnId(1) == Attr::Male);
    this->Verify(table->GetColumnId(2) == Attr::IQ);
    this->Verify(table->GetColumnId(3) == Attr::Age);
    this->Verify(table->GetColumnId(4) == Attr::Velocity);
    this->Verify(table->GetColumnId(5) == Attr::Color);
    this->Verify(table->GetColumnId(6) == Attr::Transform);
    this->Verify(table->GetColumnName(0) == "Name");
    this->Verify(table->GetColumnName(1) == "Male");
    this->Verify(table->GetColumnName(2) == "IQ");
    this->Verify(table->GetColumnName(3) == "Age");
    this->Verify(table->GetColumnName(4) == "Velocity");
    this->Verify(table->GetColumnName(5) == "Color");
    this->Verify(table->GetColumnName(6) == "Transform");
    this->Verify(table->GetColumnFourCC(0) == 'NAME');
    this->Verify(table->GetColumnFourCC(1) == 'MALE');
    this->Verify(table->GetColumnFourCC(2) == 'IQ__');
    this->Verify(table->GetColumnFourCC(3) == 'AGE_');
    this->Verify(table->GetColumnFourCC(4) == 'VELO');
    this->Verify(table->GetColumnFourCC(5) == 'COLR');
    this->Verify(table->GetColumnFourCC(6) == 'TFRM');
    this->Verify(table->GetColumnAccessMode(0) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(1) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(2) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(3) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(4) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(5) == Attr::ReadWrite);
    this->Verify(table->GetColumnAccessMode(6) == Attr::ReadWrite);
    this->Verify(table->GetColumnValueType(0) == Attr::StringType);
    this->Verify(table->GetColumnValueType(1) == Attr::BoolType);
    this->Verify(table->GetColumnValueType(2) == Attr::FloatType);
    this->Verify(table->GetColumnValueType(3) == Attr::IntType);
    this->Verify(table->GetColumnValueType(4) == Attr::Float4Type);
    this->Verify(table->GetColumnValueType(5) == Attr::Float4Type);
    this->Verify(table->GetColumnValueType(6) == Attr::Matrix44Type);
    this->Verify(table->GetNewColumnIndices().Size() == 8);

    // add some data to the table
    table->AddRow();
    this->Verify(table->GetNumRows() == 1);
    // row is initialised with default attribute values, so its marked modified
    this->Verify(table->IsRowModified(0));
    table->AddRow();
    this->Verify(table->GetNumRows() == 2);
    this->Verify(table->IsRowModified(0));
    this->Verify(table->GetNewRowIndices().Size() == 2);
    
    // fill row 0 with data and verify
    const matrix44 identityMatrix = matrix44::identity();
    table->SetString(Attr::Name, 0, "Bernhard");
    table->SetBool(Attr::Male, 0, true);
    table->SetFloat(Attr::IQ, 0, 120.2f);
    table->SetInt(Attr::Age, 0, 23);
    table->SetFloat4(Attr::Velocity, 0, float4(1.0f, 2.0f, 3.0f, 0.0f));
    table->SetFloat4(Attr::Color, 0, float4(1.0f, 1.0f, 1.0f, 1.0f));
    table->SetMatrix44(Attr::Transform, 0, identityMatrix);
    table->SetString(Attr::CarModel, 0, "BMW");
    this->Verify(table->IsRowModified(0));
    this->VerifyRow(table, 0);

    table->SetString(Attr::Name, 1, "Agnes");
    table->SetBool(Attr::Male, 1, false);
    table->SetFloat(Attr::IQ, 1, 130.0f);
    table->SetInt(Attr::Age, 1, 21);
    table->SetFloat4(Attr::Velocity, 1, float4(3.0f, 4.0f, 5.0f, 0.0f));
    table->SetFloat4(Attr::Color, 1, float4(0.5f, 0.5f, 0.5f, 0.5f));
    table->SetString(Attr::CarModel, 1, "Audi");
    this->VerifyRow(table, 1);

    // copy a row
    IndexT newRowIndex = table->CopyRow(0);
    this->Verify(2 == newRowIndex);
    this->VerifyRow(table, newRowIndex);

    // delete a row
    table->DeleteRow(0);
    this->Verify(table->GetDeletedRowIndices().Size() == 1);
    this->Verify(table->GetDeletedRowIndices()[0] == 0);
    this->Verify(table->IsRowDeleted(0));

    // make sure all the other rows are still in place (deleted
    // rows are only MARKED as deleted, but they stay in RAM)
    this->VerifyRow(table, 1);
    this->VerifyRow(table, 2);

    // reset the modified state
    table->ResetModifiedState();
    this->Verify(table->GetNewColumnIndices().Size() == 0);
    this->Verify(table->GetNewRowIndices().Size() == 0);
    this->Verify(!table->IsRowModified(0));
    this->Verify(!table->IsRowModified(1));

    // add a new empty row
    table->AddRow();
    this->VerifyRow(table, 0);
    this->VerifyRow(table, 1);
    this->Verify(table->GetNumRows() == 4);
    this->Verify(table->GetNewRowIndices().Size() == 1);
    table->AddColumn(Attr::Street);
    this->VerifyRow(table, 0);
    this->VerifyRow(table, 1);
    this->Verify(table->GetNumColumns() == 9);
    this->Verify(table->GetNewColumnIndices().Size() == 1);
    table->SetString(Attr::Street, 1, "Schwedter Strasse");
}

} // namespace Test
