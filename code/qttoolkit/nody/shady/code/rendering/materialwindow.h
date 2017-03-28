#pragma once
//------------------------------------------------------------------------------
/**
	Implements the window used to modify/setup the material settings used by this material.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "ui_materialwizard.h"
#include "core/refcounted.h"
#include "io/uri.h"
#include "io/xmlreader.h"
#include "materials/materialfeature.h"
#include "materials/material.h"
#include "passrowdelegate.h"
#include "variablerowdelegate.h"

#include <QDialog>

namespace Shady
{
class MaterialWindow : 
	public QMainWindow,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(MaterialWindow);
public:

	struct MaterialPassState
	{
		Util::String batch;
		Util::String shader;
		Util::String variation;
	};

	struct MaterialVariableState
	{
		Util::String name;
		Util::String desc;
		Util::Variant::Type type;
		Util::Variant defaultVal;
		Util::Variant minVal;
		Util::Variant maxVal;
		Util::String editType;

		bool system;
	};

	struct MaterialState
	{
		Util::StringAtom name;
		Util::String desc;
		Util::String type;
		Util::String inherit;
		bool isVirtual;
		Util::Array<MaterialPassState> passes;
		Util::Array<MaterialVariableState> variables;
	};

	/// constructor
	MaterialWindow();
	/// destructor
	virtual ~MaterialWindow();

	/// setup material window by parsing frame shaders
	void Setup();
	/// create material from current project
	void SetupFromShader(const IO::URI& path);

	/// get current material
	MaterialState& GetCurrentState() const;
	/// called by variable delegate to disable min-max values for variables
	void SetVariableItemEnabled(const QModelIndex& index, bool enable);

private slots:
	/// handle saving material list
	void OnSaveMaterialList();
	/// handle saving material list with another name
	void OnSaveMaterialListAs();
	/// handle opening a material list
	void OnOpenMaterialList();
	/// handle creating a new material list
	void OnNewMaterialList();

	/// handle selecting material
	void OnMaterialSelected(int index);
	/// handle creating a new material into list
	void OnNewMaterial();
	/// handle deleting the current material
	void OnDeleteMaterial();

	/// handle changing material name
	void OnMaterialNameChanged();
	/// handle setting the material to be virtual
	void OnMaterialVirtualSet();
	/// handle editing the description of the material
	void OnMaterialEditDescription();
	/// handle changing the inherited material
	void OnMaterialInheritChanged();

	/// handle adding a pass
	void OnAddPass();
	/// handle removing selected passes
	void OnRemovePass();
	/// handle adding a variable
	void OnAddVariable();
	/// handle removing selected variables
	void OnRemoveVariable();

	/// handle right clicking in the variable widget
	void OnVariableRightClicked(const QPoint& point);

private:


	Util::Array<MaterialState> materials;

	/// load material from file
	void Load(const IO::URI& path);
	/// parse material
	void ParseMaterial(const Ptr<IO::XmlReader>& reader);
	/// parse pass
	void ParsePass(const Ptr<IO::XmlReader>& reader, MaterialState& state);
	/// parse parameter
	void ParseParameter(const Ptr<IO::XmlReader>& reader, MaterialState& state);

	/// save material to file
	void Save(const IO::URI& path);

	/// setup variable frame
	void SetupVariable(const MaterialVariableState* state);
	/// setup pass frame
	void SetupPass(const MaterialPassState* state);

	/// setup table widget using a frameless dialog
	static QDialog* CreateItemFrame(QWidget* widget);

	Ui::MaterialEditorWindow ui;
	Util::String currentListfilePath;
	PassRowDelegate passDelegate;
	VariableRowDelegate variableDelegate;
};
} // namespace Shady