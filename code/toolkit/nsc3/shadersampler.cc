//------------------------------------------------------------------------------
//  shadersampler.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadersampler.h"

namespace Tools
{
__ImplementClass(Tools::ShaderSampler, 'SSMP', Core::RefCounted);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderSampler::ShaderSampler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderSampler::~ShaderSampler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderSampler::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Sampler");

    // parse attributes
    n_assert(xmlReader->HasAttr("name"));
    n_assert(xmlReader->HasAttr("texture"));
    this->name = xmlReader->GetString("name");
    this->textureParamName = xmlReader->GetString("texture");
    if (xmlReader->HasAttr("addrU"))
    {
        this->addrU = xmlReader->GetString("addrU");
    }
    if (xmlReader->HasAttr("addrV"))
    {
        this->addrV = xmlReader->GetString("addrV");
    }
    if (xmlReader->HasAttr("addrW"))
    {
        this->addrW = xmlReader->GetString("addrW");
    }
    if (xmlReader->HasAttr("borderColor"))
    {
        this->borderColor = xmlReader->GetString("borderColor");
    }
    if (xmlReader->HasAttr("magnifyFilter"))
    {
        this->magFilter = xmlReader->GetString("magnifyFilter");
    }
    if (xmlReader->HasAttr("minifyFilter"))
    {
        this->minFilter = xmlReader->GetString("minifyFilter");
    }
    if (xmlReader->HasAttr("mipMapFilter"))
    {
        this->mipFilter = xmlReader->GetString("mipMapFilter");
    }
    if (xmlReader->HasAttr("maxAnisotropy"))
    {
        this->maxAnisotropy = xmlReader->GetString("maxAnisotropy");
    }
    if (xmlReader->HasAttr("maxMipLevel"))
    {
        this->maxMipLevel = xmlReader->GetString("maxMipLevel");
    }
    if (xmlReader->HasAttr("mipMapLodBias"))
    {
        this->mipLodBias = xmlReader->GetString("mipLodBias");
    }
    if (xmlReader->HasAttr("srgbTexture"))
    {
        this->srgbTexture = xmlReader->GetString("srgbTexture");
    }
    return true;
}

} // namespace Tools
