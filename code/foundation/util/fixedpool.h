#pragma once
//------------------------------------------------------------------------------
/**
    @class Util::FixedPool
    
    Implements a fixed size pool, from which objects of a specific type can be
	allocated and freed for reuse.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/array.h"
#include <functional>

//------------------------------------------------------------------------------
namespace Util
{
template<class TYPE> class FixedPool
{
public:

	/// default constructor
	FixedPool();
	/// constructor with fixed size
	FixedPool(SizeT s, std::function<void(TYPE& val, IndexT idx)> setupFunc);

	/// assignment operator
	void operator=(const FixedArray<TYPE>& rhs);

	/// allocate number of elements from pool
	TYPE Alloc();
	/// free element allocated from pool
	void Free(const TYPE& elem);

	/// get number of elements
	SizeT Size() const;
	/// reset pool and resize pool
	void Resize(SizeT newSize);
	/// returns true if no more free values are available
	bool IsFull() const;
	/// clear all pool values
	void Clear();

	/// set optional setup value
	void SetSetupFunc(const std::function<void(TYPE& val, IndexT idx)>& func);

private:
	std::function<void(TYPE& val, IndexT idx)> setupFunc;
	SizeT size;
	Util::Array<TYPE> freeValues;
	Util::Array<TYPE> usedValues;
};

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
Util::FixedPool<TYPE>::FixedPool() :
	size(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline
Util::FixedPool<TYPE>::FixedPool(SizeT s, std::function<void(TYPE& val, IndexT idx)> setupFunc) :
	size(s)
{
	this->freeValues.Reserve(s);
	this->setupFunc = setupFunc;
	if (this->setupFunc != nullptr)
	{
		IndexT i;
		for (i = 0; i < this->size; i++)
		{
			this->freeValues.Append(TYPE());
			setupFunc(this->freeValues[i], i);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
Util::FixedPool<TYPE>::operator=(const FixedArray<TYPE>& rhs)
{
	this->freeValues = rhs.freeValues;
	this->usedValues = rhs.usedValues;
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline TYPE
Util::FixedPool<TYPE>::Alloc()
{
	n_assert(!this->freeValues.IsEmpty());
	TYPE elem = this->freeValues.Front();
	this->freeValues.EraseIndex(0);
	this->usedValues.Append(elem);
	return this->usedValues.Back();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
Util::FixedPool<TYPE>::Free(const TYPE& elem)
{
	IndexT idx = this->usedValues.FindIndex(elem);
	n_assert(idx != InvalidIndex);
	this->usedValues.EraseIndex(idx);
	this->freeValues.Append(elem);
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
Util::FixedPool<TYPE>::Resize(SizeT newSize)
{
	this->size = newSize;
	this->usedValues.Clear();
	this->freeValues.Clear();
	this->freeValues.Reserve(newSize);

	if (this->setupFunc != nullptr)
	{
		IndexT i;
		for (i = 0; i < this->size; i++)
		{
			this->freeValues.Append(TYPE());
			setupFunc(this->freeValues[i], i);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline bool
Util::FixedPool<TYPE>::IsFull() const
{
	return this->freeValues.IsEmpty();
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
Util::FixedPool<TYPE>::Clear()
{
	this->usedValues.Clear();
	this->freeValues.Clear();
	this->freeValues.Reserve(this->size);

	if (this->setupFunc != nullptr)
	{
		IndexT i;
		for (i = 0; i < this->size; i++)
		{
			this->freeValues.Append(TYPE());
			setupFunc(this->freeValues[i], i);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
inline void
Util::FixedPool<TYPE>::SetSetupFunc(const std::function<void(TYPE& val, IndexT idx)>& func)
{
	this->setupFunc = func;
}

}