#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyProject
    
    Stores information for a Shady project.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "project/project.h"
namespace Shady
{
class ShadyProject : public Nody::Project
{
	__DeclareClass(ShadyProject);
    __DeclareSingleton(ShadyProject);
public:
	/// constructor
	ShadyProject();
	/// destructor
	virtual ~ShadyProject();

    /// override apply
    void Apply(const Ptr<Nody::NodeScene>& scene);
    /// override store
    void Store(const Ptr<Nody::NodeScene>& scene);

private:

    friend class ShadyWindow;
	friend class ShadyProjectLoader;
	friend class ShadyProjectSaver;
    struct Settings
    {

    } settings;

	struct Material
	{
		Util::Array<Util::String> batches;
	} material;
}; 
} // namespace Shady
//------------------------------------------------------------------------------