#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaExctractorBase
    
    A MayaExtractor object is responsible to extract a specific type
    of data from a Maya scene. Specialized subclasses extract transform
    nodes, static geometry, skinned geometry, particle systems and
    special data from the scene. Scene extractor will populate
    the MayaScene singleton with a simplified, abstract version of
    the actual Maya scene.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "mayacore/mayascene.h"
#include "mayacore/mayastatus.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaExtractorBase : public Core::RefCounted
{
    __DeclareClass(MayaExtractorBase);
public:
    /// constructor
    MayaExtractorBase();
    /// destructor
    virtual ~MayaExtractorBase();
    
    /// setup the object
    virtual void Setup();
    /// discard the object
    virtual void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// called to extract scene data into MayaScene object
    virtual MayaStatus::Code Extract(const Ptr<MayaScene>& mayaScene);
    
    /// get error string
    const Util::String& GetError() const;
    
protected:    
    /// set error string
    void SetError(const Util::String& err);

    bool isValid;
    Util::String error;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaExtractorBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MayaExtractorBase::SetError(const Util::String& err)
{
    this->error = err;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaExtractorBase::GetError() const
{
    return this->error;
}

} // namespace Maya
//------------------------------------------------------------------------------
