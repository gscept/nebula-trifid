#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::SimResult
    
    SimResult describes the result outputted by a node. It has two parts, the graphical representation, and the numeric one.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "math/float4.h"
#include "util/fixedtable.h"
#include <QImage>
#include <QPixmap>
#include <QColor>
namespace Nody
{
class SimResult : public Core::RefCounted
{
	__DeclareClass(SimResult);
public:
    
	/// constructor
	SimResult();
	/// destructor
	virtual ~SimResult();

    /// returns width of simulation result
    const SizeT GetWidth() const;
    /// returns height of simulation result
    const SizeT GetHeight() const;

    /// sets the currently numerical value as default
    void SetCurrentDefault();
    /// returns default value
    const Util::FixedTable<Math::float4>& GetCurrentDefault() const;
    /// reset numerical value to default
    void ResetDefault();

    /// copy from another result
    void Copy(const SimResult& rhs);

    /// fills result with float
    void Fill(const float f);
    /// fills result with float vector
    void Fill(const Util::Array<float>& vec);
    /// fills result with int
    void Fill(const int i);
    /// fills result with int vector
    void Fill(const Util::Array<int>& vec);
    /// fills from pixmap
    void Fill(const QPixmap& pixmap);

    /// get value
    const Math::float4& GetValue(const int x, const int y);
    /// set value
    void SetValue(const Math::float4& color, const int x, const int y);
    /// get value from default
    const Math::float4 GetDefaultValue(const int x, const int y);

    /// set pixels in graphics representation
    void UpdateGraphics();
    
    /// get graphics
    QImage* GetGraphics() const;

    static const SizeT Width = 64;
    static const SizeT Height = 64;

private:
    
    Util::FixedTable<Math::float4> defaultValue;
    Util::FixedTable<Math::float4> numericalValue;
    QImage* graphicalValue;
};

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
SimResult::GetWidth() const
{
    return this->Width;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
SimResult::GetHeight() const
{
    return this->Height;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
SimResult::GetValue(const int x, const int y)
{
    return this->numericalValue.At(x, y);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SimResult::SetValue(const Math::float4& color, const int x, const int y)
{
    this->numericalValue.Set(x, y, color);
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4 
SimResult::GetDefaultValue(const int x, const int y)
{
    return this->defaultValue.At(x, y);
}

//------------------------------------------------------------------------------
/**
*/
inline QImage* 
SimResult::GetGraphics() const
{
    return this->graphicalValue;
}

} // namespace Nody
//------------------------------------------------------------------------------