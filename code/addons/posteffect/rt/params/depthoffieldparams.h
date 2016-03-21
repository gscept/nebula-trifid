#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::DepthOfFieldParams
    
    Defines parameters for the Depth-Of-Field posteffect.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class DepthOfFieldParams : public ParamBase
{
    __DeclareClass(DepthOfFieldParams);
public:
    /// constructor
    DepthOfFieldParams();
    /// constructor
    virtual ~DepthOfFieldParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set focus distance
    void SetFocusDistance(float f);
    /// get focus distance
    float GetFocusDistance() const;
    /// set focus length
    void SetFocusLength(float f);
    /// get focus length
    float GetFocusLength() const;
    /// set blur filter size
    void SetFilterSize(float f);
    /// get filter size
    float GetFilterSize() const;
    
private:    
    float focusDistance;
    float focusLength;
    float filterSize;
};
//------------------------------------------------------------------------------
/**
*/
inline
void
DepthOfFieldParams::SetFocusDistance(float f)
{
    this->focusDistance = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
DepthOfFieldParams::GetFocusDistance() const
{
    return this->focusDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
DepthOfFieldParams::SetFocusLength(float f)
{
    this->focusLength = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
DepthOfFieldParams::GetFocusLength() const
{
    return this->focusLength;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
DepthOfFieldParams::SetFilterSize(float f)
{
    this->filterSize = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
DepthOfFieldParams::GetFilterSize() const
{
    return this->filterSize;
}
} // namespace PostEffects
//------------------------------------------------------------------------------
