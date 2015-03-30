//------------------------------------------------------------------------------
//  physics/materialtable.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/materialtable.h"
#include "io/excelxmlreader.h"
#include "io/filestream.h"
#include "io/ioserver.h"

namespace Physics
{
using namespace Util;

Util::FixedArray<struct MaterialTable::Material> MaterialTable::materials;
Util::FixedArray<Util::FixedArray<struct MaterialTable::Interaction> > MaterialTable::interactions;
Util::String MaterialTable::invalidTypeString = "InvalidMaterial";

//------------------------------------------------------------------------------
/**
    Materials used in ODE
    
    Unit defintions are:
    Density:                t/m^3
    Force (MegaNewton):     t*m/s^2
    Momentum:               t*m/s
    Moment of intertia:     t*m^2
    Moment of momentum:     t*m^2/s
    Moment of force:        N*m
    Pressure (MegaPascal):  MN/m^2
    Weight (MegaNewton):    t*m/s^2
    Work (MegaJoule):       MN*m 
    Energy (MegaJoule):     MN*m

    Note: I'm not really sure that this is right... ;-)
*/      
void 
MaterialTable::Setup()
{
	// open xml file
	Ptr<IO::ExcelXmlReader> excelReader = IO::ExcelXmlReader::Create();
    excelReader->SetStream(IO::IoServer::Instance()->CreateStream("data:tables/materials.xml"));
    bool materialTableExists = excelReader->Open();
    n_assert(materialTableExists);

	// now find the tables
	int matTabIdx = -1; 
	int fricTabIdx = -1;
	int bouncTabIdx = -1;
	int soundTabIdx = -1;
	IndexT index;
	for(index = 0; index < excelReader->GetNumTables(); index++)
	{
		if (excelReader->GetTableName(index) == "materials")		{ matTabIdx = index;   continue; }
		if (excelReader->GetTableName(index) == "friction")			{ fricTabIdx = index;  continue; }
		if (excelReader->GetTableName(index) == "bouncyness")		{ bouncTabIdx = index; continue; }
		if (excelReader->GetTableName(index) == "sound")			{ soundTabIdx = index; continue; }
	}
	n_assert(matTabIdx != -1);
	n_assert(fricTabIdx != -1);
	n_assert(bouncTabIdx != -1);
	n_assert(soundTabIdx != -1);
	
	// set sizes
	SizeT materialsCount = excelReader->GetNumRows(matTabIdx) - 2;
	materials.SetSize(materialsCount);
	interactions.SetSize(materialsCount);
	for(index = 0; index < interactions.Size(); index++)
	{
		interactions[index].SetSize(materialsCount);
	}

	// read materials table
	IndexT row;
	for(row = 2; row < excelReader->GetNumRows(matTabIdx); ++row)
	{
		struct Material& material = materials[row - 2];
		material.name = excelReader->GetElement(row, "Name", matTabIdx);
		material.density = excelReader->GetElement(row, "Density", matTabIdx).AsFloat();
	}

	// load friction data
	n_assert(excelReader->GetNumRows(fricTabIdx) == materials.Size() + 2);
	n_assert(excelReader->GetNumColumns(fricTabIdx) == materials.Size() + 1);
	for(row = 2; row < excelReader->GetNumRows(fricTabIdx); ++row)
	{
		MaterialType mat1 = StringToMaterialType(excelReader->GetElement(row, 0, fricTabIdx));
		
		IndexT col;
		for(col = row - 1; col < excelReader->GetNumColumns(fricTabIdx); ++col)
		{
			MaterialType mat2 = StringToMaterialType(excelReader->GetElement(0, col, fricTabIdx));
			
            interactions[mat1][mat2].friction = excelReader->GetElement(row, col, fricTabIdx).AsFloat();            
			interactions[mat2][mat1].friction = excelReader->GetElement(row, col, fricTabIdx).AsFloat();            
		}
	}

	// load bounce data
	n_assert(excelReader->GetNumRows(bouncTabIdx) == materialsCount + 2);
	n_assert(excelReader->GetNumColumns(bouncTabIdx) == materialsCount + 1);
	for(row = 2; row < excelReader->GetNumRows(bouncTabIdx); ++row)
	{
		MaterialType mat1 = StringToMaterialType(excelReader->GetElement(row, 0, bouncTabIdx));

		IndexT col;
		for(col = row - 1; col < excelReader->GetNumColumns(bouncTabIdx); ++col)
		{
			MaterialType mat2 = StringToMaterialType(excelReader->GetElement(0, col, bouncTabIdx));

			interactions[mat1][mat2].bouncyness = excelReader->GetElement(row, col, bouncTabIdx).AsFloat();            
			interactions[mat2][mat1].bouncyness = excelReader->GetElement(row, col, bouncTabIdx).AsFloat();            
		}
	}

	// load sound data
	n_assert(excelReader->GetNumRows(soundTabIdx) == materialsCount + 2);
	n_assert(excelReader->GetNumColumns(soundTabIdx) == materialsCount + 1);
	for(row = 2; row < excelReader->GetNumRows(soundTabIdx); ++row)
	{
		MaterialType mat1 = StringToMaterialType(excelReader->GetElement(row, 0, soundTabIdx));

		IndexT col;
		for(col = row + 1; col < excelReader->GetNumColumns(soundTabIdx); ++col)
		{
			MaterialType mat2 = StringToMaterialType(excelReader->GetElement(0, col, soundTabIdx));

			if (excelReader->GetElement(row, col, soundTabIdx) != "")
			{
				interactions[mat1][mat2].collSound = excelReader->GetElement(row, col, soundTabIdx);
				interactions[mat2][mat1].collSound = excelReader->GetElement(row, col, soundTabIdx);
			}
			else
            {
                interactions[mat1][mat2].collSound.Clear();
                interactions[mat2][mat1].collSound.Clear();
            }
		}
	}
};

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
MaterialTable::MaterialTypeToString(MaterialType t)
{
    if (-1 == t)
    {
        return invalidTypeString;
    }
    else
    {
        n_assert(t >= 0 && (IndexT)t < materials.Size());
        return materials[t].name;
    }
}

//------------------------------------------------------------------------------
/**
*/
MaterialType 
MaterialTable::StringToMaterialType(const Util::String& str)
{
    IndexT i;
    for (i = 0; i < materials.Size(); ++i)
    {
        if (materials[i].name == str)
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
*/
float
MaterialTable::GetDensity(MaterialType t)
{
    n_assert(t >= 0 && (IndexT)t < materials.Size());
    return materials[t].density;
}

//------------------------------------------------------------------------------
/**
*/
float
MaterialTable::GetFriction(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materials.Size());
    n_assert(t1 >= 0 && t1 < materials.Size());
    return interactions[t0][t1].friction;
}

//------------------------------------------------------------------------------
/**
*/
float 
MaterialTable::GetBounce(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materials.Size());
    n_assert(t1 >= 0 && t1 < materials.Size());
    return interactions[t0][t1].bouncyness;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
MaterialTable::GetCollisionSound(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materials.Size());
    n_assert(t1 >= 0 && t1 < materials.Size());
    return interactions[t0][t1].collSound;
}

//------------------------------------------------------------------------------
/**
*/
void 
MaterialTable::SetDensity(MaterialType type, float density)
{
    n_assert(type != -1 && type < materials.Size());
    materials[type].density = density;
}

//------------------------------------------------------------------------------
/**
*/
void 
MaterialTable::SetFriction(MaterialType t0, MaterialType t1, float friction)
{
    n_assert(t0 != -1 && t0 < materials.Size());
    n_assert(t1 != -1 && t1 < materials.Size());
    interactions[t0][t1].friction = friction;
}

//------------------------------------------------------------------------------
/**
*/
void 
MaterialTable::SetBouncyness(MaterialType t0, MaterialType t1, float bouncyness)
{
    n_assert(t0 != -1 && t0 < materials.Size());
    n_assert(t1 != -1 && t1 < materials.Size());
    interactions[t0][t1].bouncyness = bouncyness;
}

} // namespace Physics