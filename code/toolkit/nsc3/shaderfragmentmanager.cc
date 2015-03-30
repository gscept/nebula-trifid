//------------------------------------------------------------------------------
//  shaderfragmentmanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderfragmentmanager.h"
#include "io/ioserver.h"

namespace Tools
{
__ImplementClass(Tools::ShaderFragmentManager, 'SFMG', Core::RefCounted);
__ImplementSingleton(Tools::ShaderFragmentManager);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderFragmentManager::ShaderFragmentManager() :
    innerIndex(0),
    outerIndex(0),
    numGroups(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShaderFragmentManager::~ShaderFragmentManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    This method loads all shader fragments and shader fragment groups 
    from the provided directory.
*/
bool
ShaderFragmentManager::LoadFragmentsAndGroups(const URI& uri)
{
    IoServer* ioServer = IoServer::Instance();
    Array<String> files = ioServer->ListFiles(uri, "*.xml");
    IndexT i;
    for (i = 0; i < files.Size(); i++)
    {
        n_printf("Parsing fragment '%s'.\n", files[i].AsCharPtr());

        // build fragment file uri
        URI fileUri = uri;
        fileUri.AppendLocalPath(files[i]);

        // create a stream with an XML reader and load fragments from it
        // note that there may be several fragment definitions per file
        Ptr<Stream> stream = ioServer->CreateStream(fileUri);
        Ptr<XmlReader> xmlReader = XmlReader::Create();
        xmlReader->SetStream(stream);
        if (xmlReader->Open())
        {
            // make sure it's an Nebula3 material system file...
            if (xmlReader->HasNode("/Nebula3MaterialSystem"))
            {
                xmlReader->SetToNode("/Nebula3MaterialSystem");

                // for each fragment...
                if (xmlReader->SetToFirstChild("Fragment")) do
                {
                    // create a new ShaderFragment object
                    Ptr<ShaderFragment> shaderFragment = ShaderFragment::Create();
                    shaderFragment->SetFileURI(fileUri);
                    if (shaderFragment->Parse(xmlReader))
                    {
                        if (!this->fragments.Contains(shaderFragment->GetName()))
                        {
                            this->fragments.Add(shaderFragment->GetName(), shaderFragment);
                        }
                        else
                        {
                            n_printf("WARNING: duplicate fragment '%s' in files '%s' and '%s'!\n",
                                shaderFragment->GetName().AsCharPtr(),
                                fileUri.AsString().AsCharPtr(),
                                this->fragments[shaderFragment->GetName()]->GetFileURI().AsString().AsCharPtr());
                        }
                    }
                    else
                    {
                        n_printf("ERROR parsing fragment in file '%s' (skipping)", fileUri.AsString().AsCharPtr());
                    }
                }
                while (xmlReader->SetToNextChild("Fragment"));

                // for each fragment group...
                if (xmlReader->SetToFirstChild("FragmentGroup")) do
                {
                    // create a new fragment group object
                    Ptr<ShaderFragmentGroup> fragmentGroup = ShaderFragmentGroup::Create();
                    fragmentGroup->SetFileURI(fileUri);
                    if (fragmentGroup->Parse(xmlReader))
                    {
                        // need to make sure that no fragment with the same name exists
                        if (this->fragments.Contains(fragmentGroup->GetName()))
                        {
                            n_printf("WARNING: fragment group/fragment name collision, name is '%s' in files '%s' and '%s'!\n",
                                fragmentGroup->GetName().AsCharPtr(),
                                fileUri.AsString().AsCharPtr(),
                                this->fragments[fragmentGroup->GetName()]->GetFileURI().AsString().AsCharPtr());
                        }
                        if (!this->fragmentGroups.Contains(fragmentGroup->GetName()))
                        {
                            this->fragmentGroups.Add(fragmentGroup->GetName(), fragmentGroup);
                        }
                        else
                        {
                            n_printf("WARNING: duplicate fragment group '%s' in files '%s' and '%s'!\n",
                                fragmentGroup->GetName().AsCharPtr(),
                                fileUri.AsString().AsCharPtr(),
                                this->fragmentGroups[fragmentGroup->GetName()]->GetFileURI().AsString().AsCharPtr());
                        }
                    }
                    else
                    {
                        n_printf("ERROR parsing fragment group in file '%s' (skipping)", fileUri.AsString().AsCharPtr());
                    }
                }
                while (xmlReader->SetToNextChild("FragmentGroup"));
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This takes a fragment group name and a fragment name in that group,
    and adds a copy of the group fragment to the list of fragments. The new
    fragment will be named like the fragment group (i.e. if the
    fragment group is called "PhongLighting", and has the fragments 
    "PointLight", "DirectionalLight" and "SpotLight", and you call this
    method with the arguments ("PhongLighting", "PointLight"), the PointLight
    fragment will be duplicated and added under the name "PhongLighting"
    to the list of fragments).
*/
void
ShaderFragmentManager::ActivateGroupFragment(const String& fragmentGroup, const String& groupFragment)
{
    n_assert(this->fragmentGroups.Contains(fragmentGroup));
    n_assert(this->fragmentGroups[fragmentGroup]->GetFragments().Contains(groupFragment));

    // duplicate a new fragment
    const Ptr<ShaderFragment>& origFragment = this->fragmentGroups[fragmentGroup]->GetFragments()[groupFragment];
    Ptr<ShaderFragment> dupFrag = origFragment->Duplicate(fragmentGroup);
    n_assert(!this->fragments.Contains(dupFrag->GetName()));
    this->fragments.Add(dupFrag->GetName(), dupFrag);
    this->activeGroupFragments.InsertSorted(groupFragment);
}

//------------------------------------------------------------------------------
/**
    This removes a fragment added with ActivateGroupFragment().
*/
void
ShaderFragmentManager::DeactivateGroupFragment(const String& fragmentGroup)
{
    if (this->fragments.Contains(fragmentGroup))
    {
        this->fragments.Erase(fragmentGroup);
    }
}

//------------------------------------------------------------------------------
/**
    This activates the fragments of the current group fragment iteration
    state.
*/
void
ShaderFragmentManager::ActivateCurrentGroupFragments()
{
    this->activeGroupFragments.Clear();
    IndexT groupIndex;
    for (groupIndex = 0; groupIndex < this->numGroups; groupIndex++)
    {
        const Ptr<ShaderFragmentGroup> group = this->fragmentGroups.ValueAtIndex(groupIndex);
        this->DeactivateGroupFragment(group->GetName());
        this->ActivateGroupFragment(group->GetName(), group->GetFragments().ValueAtIndex(this->fragIndices[groupIndex])->GetName());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderFragmentManager::BeginIterateGroupFragments()
{
    this->outerIndex = 0;
    this->innerIndex = 0;
    this->numGroups = this->fragmentGroups.Size();    
    this->fragIndices.SetSize(this->numGroups);
    this->fragIndices.Fill(0);
    this->ActivateCurrentGroupFragments();
    return true;
}

//------------------------------------------------------------------------------
/**
    Hmmm... there must be a simpler way for walking through all combinations...
*/
bool
ShaderFragmentManager::ContinueIterateGroupFragments()
{
    // advance the fragment index of the current inner group
    this->fragIndices[this->innerIndex]++;
    if (this->fragIndices[this->innerIndex] == this->fragmentGroups.ValueAtIndex(this->innerIndex)->GetFragments().Size())
    {
        // advance inner group index
        this->fragIndices[this->innerIndex] = 0;
        this->innerIndex++;

        // if inner index has reached the end, advance outer frag index, and restart inner iteration
        if (this->innerIndex == this->numGroups)
        {
            this->innerIndex = 0;
            this->fragIndices[this->outerIndex]++;
            if (this->fragIndices[this->outerIndex] == this->fragmentGroups.ValueAtIndex(this->outerIndex)->GetFragments().Size())
            {
                // need to advance outer index by one
                this->fragIndices[this->outerIndex] = 0;
                this->outerIndex++;
                if (this->outerIndex == this->numGroups)
                {
                    // we're done!
                    return false;
                }
            }
        }
        // cannot combine fragments within the same fragment group
        if (this->innerIndex == this->outerIndex)
        {
            this->innerIndex++;
            if (this->innerIndex == this->numGroups)
            {
                return false;
            }
        }
    }
    // more combinations exist...
    this->ActivateCurrentGroupFragments();
    return true;
}

} // namespace Tools