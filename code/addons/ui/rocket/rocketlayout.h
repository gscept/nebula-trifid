#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketLayout
    
    Implements a UI-layout for LibRocket
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "ui/base/uilayoutbase.h"

namespace Rocket
{
namespace Core
{
class ElementDocument;
}
}

namespace LibRocket
{
class RocketLayout : public Base::UiLayoutBase
{
	__DeclareClass(RocketLayout);
public:
	/// constructor
	RocketLayout();
	/// destructor
	virtual ~RocketLayout();

	/// load layout
	void Load();
	/// reloads layout
	void Reload();
	/// shows layout
	void Show();
	/// hide layout
	void Hide();
	/// set size of layout
	void SetSize(const Math::float2& size);
	/// updates
	void Update();
	/// renders
	void Render(const Ptr<Frame::FrameBatch>& frameBatch);
	/// discards layout
	void Discard();
	/// returns true if layout is valid
	bool IsValid() const;
	
	/// returns handle to layout
	Rocket::Core::ElementDocument* GetLayout();
	
private:

	Rocket::Core::ElementDocument* layout;
}; 


//------------------------------------------------------------------------------
/**
*/
inline bool 
RocketLayout::IsValid() const
{
	return this->layout != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline Rocket::Core::ElementDocument* 
RocketLayout::GetLayout()
{
	return this->layout;
}

} // namespace Rocket
//------------------------------------------------------------------------------