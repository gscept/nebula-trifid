#ifndef VEGETATION_GRASSLAYERINFO_H
#define VEGETATION_GRASSLAYERINFO_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassLayerInfo
    
    Describes all resources needed for a grass layer.
    
    (C) 2008 Radon Labs GmbH
*/
#include "core/types.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassLayerInfo
{
public:
    /// constructor
    GrassLayerInfo();
    /// set a unique layer name
    void SetLayerName(const Util::String& n);
    /// get unique layer name
    const Util::String& GetLayerName() const;
    /// set channel index in weight texture
    void SetWeightTextureChannelIndex(int i);
    /// get channel index in weight texture
    int GetWeightTextureChannelIndex() const;
    /// set grass library resource name
    void SetGrassLibraryName(const Util::String& n);
    /// get grass library resource name
    const Util::String& GetGrassLibraryName() const;
    /// set ground resource name 
    void SetGroundName(const Util::String& n);
    /// get ground resource name
    const Util::String& GetGroundName() const;
    /// set ground transform matrix
    void SetGroundTransform(const Math::matrix44& m);
    /// get ground transform matrix
    const Math::matrix44& GetGroundTransform() const;

private:
    int weightTextureChannelIndex;
    Util::String layerName;
    Util::String grassLibraryName;
    Util::String groundName;
    Math::matrix44 groundTransform;
};

//------------------------------------------------------------------------------
/**
*/
inline
GrassLayerInfo::GrassLayerInfo() :
    weightTextureChannelIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassLayerInfo::SetLayerName(const Util::String& n)
{
    n_assert(n.IsValid());
    this->layerName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
GrassLayerInfo::GetLayerName() const
{
    return this->layerName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassLayerInfo::SetWeightTextureChannelIndex(int i)
{
    this->weightTextureChannelIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassLayerInfo::GetWeightTextureChannelIndex() const
{
    return this->weightTextureChannelIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassLayerInfo::SetGrassLibraryName(const Util::String& n)
{
    this->grassLibraryName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
GrassLayerInfo::GetGrassLibraryName() const
{
    return this->grassLibraryName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassLayerInfo::SetGroundName(const Util::String& n)
{
    this->groundName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
GrassLayerInfo::GetGroundName() const
{
    return this->groundName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassLayerInfo::SetGroundTransform(const Math::matrix44& m)
{
    this->groundTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
GrassLayerInfo::GetGroundTransform() const
{
    return this->groundTransform;
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
