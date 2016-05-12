#pragma once
//------------------------------------------------------------------------------
/**
	Implements the window used to modify/setup the material settings used by this material.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "ui_materialwizard.h"
#include <QDialog>
namespace Shady
{
class MaterialWindow : 
	public QDialog,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(MaterialWindow);
public:
	/// constructor
	MaterialWindow();
	/// destructor
	virtual ~MaterialWindow();

	/// setup material window by parsing frame shaders
	void Setup();
private:
	Ui::MaterialWizard ui;
	QGridLayout layout;
	QFrame contents;
};
} // namespace Shady