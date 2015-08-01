#pragma once
//------------------------------------------------------------------------------
/**
	@class Base::ShaderConstantBufferBase
	
	A buffer which represents a set of shader constant variables 
    (uniforms in OpenGL) which are contained within a buffer object.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "coregraphics/shadervariable.h"

namespace CoreGraphics
{
class ShaderInstance;
}

namespace Base
{
class ConstantBufferBase : public Core::RefCounted
{
	__DeclareClass(ConstantBufferBase);
public:
	/// constructor
	ConstantBufferBase();
	/// destructor
	virtual ~ConstantBufferBase();

    /// setup buffer
    void Setup();
    /// bind variables in a block with a name in a shader to this buffer (only do this on system managed blocks)
    void SetupFromBlockInShader(const Ptr<CoreGraphics::ShaderInstance>& shader, const Util::String& blockName);
    /// discard buffer
    void Discard();

    /// set if this buffer should be updated synchronously, the default behaviour is not
    void SetSync(bool b);

    /// get variable inside constant buffer by name
    const Ptr<CoreGraphics::ShaderVariable>& GetVariableByName(const Util::StringAtom& name) const;

    /// set the size, must be done prior to setting it up
    void SetSize(uint size);

    /// returns buffer handle
    void* GetHandle() const;

    /// begin updating the buffer using a synchronous method
    void BeginUpdateSync();
    /// update buffer, if not within begin/end synced update, it will be an asynchronous update
    template <class T> void Update(const T& data, uint offset, uint size);
    /// update buffer using an array, if not within begin/end synced update, it will be an asynchronous update
    template <class T> void UpdateArray(const T data, uint offset, uint size, uint count);
    /// end updating the buffer using a synchronous method
    void EndUpdateSync();

    /// cycle to next buffer
    void CycleBuffers();

    static const int NumBuffers = 3;

protected:

    /// update buffer asynchronously, depending on implementation, this might overwrite data before used
    virtual void UpdateAsync(void* data, uint offset, uint size);
    /// update segment of buffer as array, depending on implementation, this might overwrite data before used
    virtual void UpdateArrayAsync(void* data, uint offset, uint size, uint count);

    /// update buffer synchronously
    virtual void UpdateSync(void* data, uint offset, uint size);
    /// update buffer synchronously using an array of data
    virtual void UpdateArraySync(void* data, uint offset, uint size, uint count);

    Util::Array<Ptr<CoreGraphics::ShaderVariable>> variables;
    Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderVariable>> variablesByName;
    bool isSetup;
    uint size;
    IndexT bufferIndex;

    bool sync;
    bool inUpdateSync;
    bool isDirty;
    void* buffer;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ConstantBufferBase::SetSync(bool b)
{
    this->sync = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Base::ConstantBufferBase::UpdateAsync(void* data, uint offset, uint size)
{
     // override this in subclass where we might, or might not have the ability to update a buffer asynchronously
}

//------------------------------------------------------------------------------
/**
*/
inline void
Base::ConstantBufferBase::UpdateArrayAsync(void* data, uint offset, uint size, uint count)
{
    // override this in subclass where we might, or might not have the ability to update a buffer asynchronously
}

//------------------------------------------------------------------------------
/**
*/
inline void
Base::ConstantBufferBase::UpdateSync(void* data, uint offset, uint size)
{
    n_assert(this->inUpdateSync);   
    n_assert(this->sync);
    byte* buf = (byte*)this->buffer + offset;
    memcpy(buf, data, size);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Base::ConstantBufferBase::UpdateArraySync(void* data, uint offset, uint size, uint count)
{
    n_assert(this->inUpdateSync);
    n_assert(this->sync);
    byte* buf = (byte*)this->buffer + offset;
    memcpy(buf, data, size * count);
}

//------------------------------------------------------------------------------
/**
*/
template <class T>
inline void
Base::ConstantBufferBase::Update(const T& data, uint offset, uint size)
{
    n_assert(0 != this->buffer);
    switch (this->inUpdateSync)
    {
    case true:  this->UpdateSync((void*)&data, offset, size); break;
    case false: this->UpdateAsync((void*)&data, offset, size); break;
    }
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
template <class T>
inline void
Base::ConstantBufferBase::UpdateArray(const T data, uint offset, uint size, uint count)
{
    n_assert(0 != this->buffer);
    switch (this->inUpdateSync)
    {
    case true:  this->UpdateArraySync((void*)data, offset, size, count); break;
    case false: this->UpdateArrayAsync((void*)data, offset, size, count); break;
    }
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ConstantBufferBase::SetSize(uint size)
{
    this->size = size;
}

//------------------------------------------------------------------------------
/**
*/
inline void*
ConstantBufferBase::GetHandle() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>&
ConstantBufferBase::GetVariableByName(const Util::StringAtom& name) const
{
    return this->variablesByName[name];
}

} // namespace Base