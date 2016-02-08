#pragma once
//------------------------------------------------------------------------------
/**
	Handler for physics nodes.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "modelhandler.h"
#include "ui_physicsnodeinfowidget.h"
#include <QObject>
namespace Widgets
{
class PhysicsNodeHandler :
	public QObject,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(PhysicsNodeHandler);
public:
	/// constructor
	PhysicsNodeHandler();
	/// destructor
	virtual ~PhysicsNodeHandler();

	/// sets the ui
	void SetUI(Ui::PhysicsNodeInfoWidget* ui);
	/// gets the ui
	Ui::PhysicsNodeInfoWidget* GetUI() const;

	/// set pointer to original item handler
	void SetModelHandler(const Ptr<ModelHandler>& itemHandler);
	/// get pointer to original item handler
	const Ptr<ModelHandler>& GetModelHandler() const;

	/// add collider
	void AddNode(const Util::String& name);

	/// discards a model node handler
	void Discard();

private slots:
	/// handle changing the material
	void OnMaterialChanged(int index);
private:

	Util::Array<QLabel*> labels;
	Ptr<ModelHandler> modelHandler;
	Ui::PhysicsNodeInfoWidget* ui;
};


//------------------------------------------------------------------------------
/**
*/
inline void
PhysicsNodeHandler::SetUI(Ui::PhysicsNodeInfoWidget* ui)
{
	n_assert(ui);
	this->ui = ui;

	connect(this->ui->physicsMaterialBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaterialChanged(int)));
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::PhysicsNodeInfoWidget*
PhysicsNodeHandler::GetUI() const
{
	return this->ui;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PhysicsNodeHandler::SetModelHandler(const Ptr<ModelHandler>& itemHandler)
{
	this->modelHandler = itemHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ModelHandler>&
PhysicsNodeHandler::GetModelHandler() const
{
	return this->modelHandler;
}

} // namespace Widgets