//------------------------------------------------------------------------------
//  loader/LoaderServer.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loader/loaderserver.h"
#include "core/factory.h"
#include "loader/levelloader.h"
#include "io/ioserver.h"

namespace BaseGameFeature
{
__ImplementClass(LoaderServer, 'LOSV', Core::RefCounted);
__ImplementSingleton(LoaderServer);

//------------------------------------------------------------------------------
/**
*/
LoaderServer::LoaderServer() :
    isOpen(false),
    debugTextEnabled(true)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
LoaderServer::~LoaderServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the loader subsystem.

    @param  applicationName     the app name
    @param  fileFormatVersion   string in the form "x.y" describing the file format version
    @return                     true if loader subsystem successfully opened
*/
bool
LoaderServer::Open()
{
    n_assert(!this->isOpen);

    // setup default user profile
    Ptr<UserProfile> userProfile = this->CreateUserProfile();
    userProfile->SetName("default");
    if (IO::AssignRegistry::Instance()->HasAssign("user"))
    {
        userProfile->Load();
    }
    this->SetUserProfile(userProfile);
  
    // create progress indicator window
    //this->progressIndicator = UI::ProgressBarWindow::Create();
    //this->progressIndicator->SetResource("gui/progressbar");

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the loader subsystem.
*/
void
LoaderServer::Close()
{
    n_assert(this->isOpen);

    // close progress indicator window
    //if (this->progressIndicator->IsOpen())
    //{
    //    this->progressIndicator->Close();
    //}
    //this->progressIndicator = 0;

    // remove all loader from list
    this->RemoveAllLoaders();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Creates a new user profile object. Override in subclass to create
    your own user profile subclass instances.
*/
Ptr<UserProfile>
LoaderServer::CreateUserProfile() const
{
    return UserProfile::Create();
}


//------------------------------------------------------------------------------
/**
    Load a new game level from the world database.

    @param  filename    a level name (this is not a filename!)
    @return             success
*/
bool
LoaderServer::LoadLevel(const Util::String& levelName)
{
    n_assert(levelName.IsValid());
    bool success = LevelLoader::Load(levelName);
    return success;
}

//------------------------------------------------------------------------------
/**
    Attach new entity loader to LoaderServer.

    @param  loader    loader class   
*/
void
LoaderServer::AttachEntityLoader(const Ptr<EntityLoaderBase>& loader)
{
    n_assert(0 != loader);
    this->entityLoaders.Append(loader);
}

//------------------------------------------------------------------------------
/**
    Remove given loader from LoaderServer.

    @param  loader    loader class   
*/
void
LoaderServer::RemoveEntityLoader(const Ptr<EntityLoaderBase>& loader)
{
    n_assert(0 != loader);
    IndexT i = this->entityLoaders.FindIndex(loader);
    if (InvalidIndex != i)
    {
        this->entityLoaders.EraseIndex(i);
    }
}

//------------------------------------------------------------------------------
/**
    Remove all loaders.
*/
void
LoaderServer::RemoveAllLoaders()
{
    this->entityLoaders.Clear();
}

//------------------------------------------------------------------------------
/**
    Go thru all entity loader and call its Load function.
*/
void
LoaderServer::LoadEntities(const Util::Array<Util::String>& activeLayers)
{
    IndexT i;
    for (i = 0; i < this->entityLoaders.Size(); i++)
    {
        this->entityLoaders[i]->Load(activeLayers);
    }
}

//------------------------------------------------------------------------------
/**
*/
IO::URI 
LoaderServer::GetScreenshotFilepath(const Util::String& extension)
{
    n_assert(this->userProfile.isvalid());
    const Util::String& profilePath = userProfile->GetProfileRootDirectory();
    IO::URI uri(profilePath + "/capture");
    // if directory doesn't exists, create it
    if (!IO::IoServer::Instance()->DirectoryExists(uri))
    {
        IO::IoServer::Instance()->CreateDirectory(uri);
    }

    // TODO : move to a captureserver with sessions, like in n2
    Util::String fileName("screenshot");
    Util::Array<Util::String> existingFiles = IO::IoServer::Instance()->ListFiles(uri, fileName + "*");
    IndexT screenshotNumber = 0; 
    while (InvalidIndex != existingFiles.FindIndex(fileName + Util::String::FromInt(screenshotNumber))) 
    {
        screenshotNumber++;
    }
    uri.AppendLocalPath(fileName + Util::String::FromInt(screenshotNumber) + "." + extension);
    return uri;
}
} // namespace BaseGameFeature