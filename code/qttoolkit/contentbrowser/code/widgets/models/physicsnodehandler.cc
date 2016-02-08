//------------------------------------------------------------------------------
// physicsnodehandler.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsnodehandler.h"

namespace Widgets
{

__ImplementClass(Widgets::PhysicsNodeHandler, 'PHNR', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
PhysicsNodeHandler::PhysicsNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PhysicsNodeHandler::~PhysicsNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsNodeHandler::AddNode(const Util::String& name)
{
	QLabel* label = new QLabel;
	label->setAlignment(Qt::AlignRight);
	label->setText(name.AsCharPtr());
	this->ui->nodeFrame->layout()->addWidget(label);
	this->labels.Append(label);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsNodeHandler::Discard()
{
	IndexT i;
	for (i = 0; i < this->labels.Size(); i++)
	{
		delete this->labels[i];
	}
	this->labels.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsNodeHandler::OnMaterialChanged(int index)
{
	//this->modelHandler->GetPhysics()->set
}

} // namespace Widgets