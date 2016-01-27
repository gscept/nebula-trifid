#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::SamplerTextureList
    
    A sampler texture list is a list of textures to which a sampler should be assigned.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <vector>
namespace AnyFX
{
class SamplerTextureList
{
public:
	/// constructor
	SamplerTextureList();
	/// destructor
	virtual ~SamplerTextureList();

	/// adds a texture
	void AddTexture(const std::string& texture);

	/// get number of textures
	const unsigned GetNumTextures() const;
	/// get texture at index
	const std::string& GetTexture(unsigned index) const;

private:

	std::vector<std::string> textures;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void
SamplerTextureList::AddTexture(const std::string& texture)
{
	this->textures.push_back(texture);
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
SamplerTextureList::GetNumTextures() const
{
	return this->textures.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
SamplerTextureList::GetTexture(unsigned index) const
{
	return this->textures[index];
}

} // namespace AnyFX
//------------------------------------------------------------------------------