//------------------------------------------------------------------------------
//  variationdatabase.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variationdatabase.h"
#include "io/ioserver.h"
#include "io/uri.h"

namespace Nody
{
__ImplementClass(Nody::VariationDatabase, 'VADB', Core::RefCounted);
__ImplementSingleton(Nody::VariationDatabase);

//------------------------------------------------------------------------------
/**
*/
VariationDatabase::VariationDatabase() :
	isOpen(false)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
VariationDatabase::~VariationDatabase()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
VariationDatabase::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
VariationDatabase::Close()
{
	n_assert(this->IsOpen());

	// discard variations
	IndexT i;
	for (i = 0; i < this->variationsByIndex.Size(); i++)
	{
		this->variationsByIndex[i]->Discard();
	}

	for (i = 0; i < this->variationsByCategory.Size(); i++)
	{
		this->variationsByCategory.ValueAtIndex(i).Clear();
	}
	this->variationsByCategory.Clear();
	this->variationsByIndex.Clear();
    this->variationsByName.Clear();

    for (i = 0; i < this->superVariationsByIndex.Size(); i++)
    {
        this->superVariationsByIndex[i]->Discard();
    }
    this->superVariationsByIndex.Clear();
    this->superVariationsByName.Clear();

	this->variationLoader = 0;
    this->superVariationLoader = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
	Loads variations from chosen root folder
*/
void 
VariationDatabase::LoadVariations( const IO::URI& path )
{
	n_assert(path.IsValid());
	n_assert(this->variationLoader.isvalid());
	n_assert(this->IsOpen());

	Util::String origPath = path.GetHostAndLocalPath();

	// get pointer to IoServer
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();

	// list directories
	Util::Array<Util::String> dirs = ioServer->ListDirectories(path, "*");

	// add directories as categories
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		const Util::String& dir = dirs[i];
		this->variationsByCategory.Add(dir, Util::Array<Ptr<Variation> >());

		// list files
		Util::Array<Util::String> files = ioServer->ListFiles(origPath + "/" + dir, "*.ndv");

		IndexT j;
		for (j = 0; j < files.Size(); j++)
		{
			Util::String file = files[j];

			// open file stream
			Ptr<IO::Stream> fileStream = ioServer->CreateStream(origPath + "/" + dir + "/" + file);

			// only create variation if stream is opened
			if (fileStream->Open())
			{
				// now load!
				Ptr<Variation> variation = this->variationLoader->Load(fileStream);
				n_assert(variation.isvalid());
                
                // remove file extension from variation
                file.StripFileExtension();

				// add variation to dictionaries
				this->variationsByIndex.Append(variation);
                this->variationsByName.Add(file, variation);
				this->variationsByCategory[dir].Append(variation);

				// and close file stream
				fileStream->Close();
			}
			else
			{
				n_error("Variation file '%s' could not be loaded!", file.AsCharPtr());
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
VariationDatabase::LoadSuperVariations( const IO::URI& path )
{
    n_assert(path.IsValid());
    n_assert(this->variationLoader.isvalid());
    n_assert(this->IsOpen());

    Util::String origPath = path.GetHostAndLocalPath();

    // get pointer to IoServer
    Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();

    // list directories
    Util::Array<Util::String> files = ioServer->ListFiles(path, "*.ndv");

    IndexT j;
    for (j = 0; j < files.Size(); j++)
    {
        Util::String file = files[j];

        // open file stream
        Ptr<IO::Stream> fileStream = ioServer->CreateStream(origPath + "/" + file);

        // only create variation if stream is opened
        if (fileStream->Open())
        {
            // now load!
            Ptr<SuperVariation> variation = this->superVariationLoader->Load(fileStream);
            n_assert(variation.isvalid());

            // remove file extension from variation
            file.StripFileExtension();

            // add variation to dictionaries
            this->superVariationsByIndex.Append(variation);
            this->superVariationsByName.Add(file, variation);

            // and close file stream
            fileStream->Close();
        }
        else
        {
            n_error("Super variation file '%s' could not be loaded!", file.AsCharPtr());
        }
    }
}


} // namespace Nody
