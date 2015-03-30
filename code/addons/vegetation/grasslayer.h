#ifndef VEGETATIONFEATURE_GRASSLAYER_H
#define VEGETATIONFEATURE_GRASSLAYER_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassLayer
  
    A grass layer associates a painted weight map with a grass library. It
    defines what type of grass must be rendered where. Note that one
    grass library may be associated with several different weight maps.
    
    (C) 2008 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "vegetation/grasslayerinfo.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassWeightMap;
class GrassLibrary;

class GrassLayer : public Core::RefCounted
{
    __DeclareClass(GrassLayer);
public:
    /// constructor
    GrassLayer();
    
    /// setup the grass layer from a layer info object
    void Setup(const GrassLayerInfo& layerInfo);
    /// get layer info
    const GrassLayerInfo& GetLayerInfo() const;
    /// get the grass library associated with this layer
    Ptr<GrassLibrary> GetGrassLibrary() const;

private:
    GrassLayerInfo layerInfo;
    Ptr<GrassLibrary> library;
};

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
