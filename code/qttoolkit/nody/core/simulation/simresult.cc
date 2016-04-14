//------------------------------------------------------------------------------
//  simresult.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "simresult.h"

namespace Nody
{
__ImplementClass(Nody::SimResult, 'SIMR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SimResult::SimResult()
{
	// create image
    this->graphicalValue = new QImage(Width, Height, QImage::Format_ARGB32);
    this->graphicalValue->fill(Qt::black);
    this->numericalValue.SetSize(Width, Height);
    this->numericalValue.Clear(Math::float4(0, 0, 0, 1));
    this->defaultValue.SetSize(Width, Height);
    this->defaultValue.Clear(Math::float4(0, 0, 0, 1));
}

//------------------------------------------------------------------------------
/**
*/
SimResult::~SimResult()
{
	// delete image
    delete this->graphicalValue;
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Copy(const SimResult& rhs)
{
    this->numericalValue = rhs.numericalValue;
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Fill(const float f)
{
    Math::float4 val;
    val.set(f, f, f, 1);
    this->numericalValue.Clear(val);
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Fill(const Util::Array<float>& vec)
{
    Math::float4 val;
    IndexT i;
    val.set(0, 0, 0, 1);
    for (i = 0; i < vec.Size(); i++)
    {
        val[i] = vec[i];
    }
    this->numericalValue.Clear(val);
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Fill(const int i)
{
    Math::float4 val;
    val.set(i, i, i, 1);
    this->numericalValue.Clear(val);
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Fill(const Util::Array<int>& vec)
{
    Math::float4 val;
    IndexT i;
    val.set(0, 0, 0, 1);
    for (i = 0; i < vec.Size(); i++)
    {
        val[i] = vec[i];
    }
    this->numericalValue.Clear(val);
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::Fill(const QPixmap& pixmap)
{
    QImage img = pixmap.toImage();
    IndexT x, y;
    for (x = 0; x < img.width(); x++)
    {
        for (y = 0; y < img.height(); y++)
        {
            QColor color = img.pixel(x, y);
            float red = color.red() / 255.0f;
            float green = color.green() / 255.0f;
            float blue = color.blue() / 255.0f;
            float alpha = color.alpha() / 255.0f;
            this->SetValue(Math::float4(red, green, blue, alpha), x, y);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::UpdateGraphics()
{
    SizeT width = this->numericalValue.Width();
    SizeT height = this->numericalValue.Height();
    IndexT x, y;
    for (x = 0; x < width; x++)
    {
        for (y = 0; y < height; y++)
        {
            const Math::float4& val = this->numericalValue.At(x, y);
            unsigned red = qBound(0.0f, val.x() * 255, 255.0f);
            unsigned green = qBound(0.0f, val.y() * 255, 255.0f);
            unsigned blue = qBound(0.0f, val.z() * 255, 255.0f);
            unsigned alpha = qBound(0.0f, val.w() * 255, 255.0f);
            unsigned pixel = qRgba(red, green, blue, alpha);
            this->graphicalValue->setPixel(x, y, pixel);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::SetCurrentDefault()
{
    this->defaultValue = this->numericalValue;
}

//------------------------------------------------------------------------------
/**
*/
const Util::FixedTable<Math::float4>& 
SimResult::GetCurrentDefault() const
{
    return this->defaultValue;
}

//------------------------------------------------------------------------------
/**
*/
void 
SimResult::ResetDefault()
{
    this->numericalValue = this->defaultValue;
}
} // namespace Nody