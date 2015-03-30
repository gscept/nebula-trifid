#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIServer

    Implmentation of crazy eddies GUI system as a nebula3 addon.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "sui/base/suiserverbase.h"
#include "sui/ceui/ceuirenderer.h"
#include "sui/ceui/ceuiresourceprovider.h"
#include "cegui/include/CEGUI.h"

namespace CEUI
{

class CEUIServer : public Base::SUIServerBase
{
    __DeclareClass(CEUIServer);
    __DeclareSingleton(CEUIServer);
public:
    /// constructor
    CEUIServer();
    /// destructor
    virtual ~CEUIServer();

    /// setup the server
    void Setup();
    /// discard the server
    void Discard();
    /// update the SUI system, called exactly once per frame
    void Update();
    /// render SUI system, may be called several times per frame
    void Render(const Ptr<Frame::FrameBatch>& frameBatch);
	/// render SUI system, may be called several times per frame
	void Render(const Ptr<CoreGraphics::ShaderInstance>& shader);

	Util::Array<SUI::SUIEvent> ProcessInputEvents(const Util::Array<Input::InputEvent>& inputEvents);

    friend class CEUILayout;

protected:

	bool HandleWindowEvent(const CEGUI::EventArgs&);
	bool HandleKeyEvent(const CEGUI::EventArgs&);
	bool HandleMouseEvent(const CEGUI::EventArgs&);
	bool HandleMouseEnterEvent(const CEGUI::EventArgs&);
	bool HandleMouseLeaveEvent(const CEGUI::EventArgs&);

    Ptr<CEUI::CEUIRenderer> renderer;
    CEUI::CEUIResourceProvider* resourceProvider;
    CEGUI::System* cegui;
    CEGUI::WindowManager* wndManager;
    CEGUI::Window* rootSheet;
    Ptr<CoreGraphics::ShaderInstance> shdInst;
	Ptr<CEUILayout> currentInputTarget;

	
};

}