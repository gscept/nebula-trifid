#pragma once
//------------------------------------------------------------------------------
/**
    @class QtPhysics::MaterialsEditor       
    
    (C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QDialog>
#include "util/array.h"
#include "util/dictionary.h"
#include "physics/materialtable.h"
#include "util/guid.h"
#include "QTableWidget"

namespace Ui
{
    class PhysicsMaterialEditor;
}
namespace QtPhysics
{
class MaterialsEditor : public QDialog
{
	Q_OBJECT
public:
	/// constructor
    MaterialsEditor();
	/// destructor
    virtual ~MaterialsEditor();



protected:
	/// qts close handler
	virtual void closeEvent(QCloseEvent * event);
    /// show handler
    virtual void showEvent(QShowEvent * event);
private slots:
		    
    /// called when cancel is pressed
	void OnCancel();
    ///
    void OnAdd();
    ///
    void OnRemove();

	/// save preset button is pressed
	void OnSave();
	
    ///
    void OnFrictionChanged(double);
    ///
    void OnRestitutionChanged(double);

    ///
    void OnInteractionChanged(int, int);
    /// 
    void OnMaterialChanged(QTableWidgetItem*);

	/// parameters modified
	bool IsModified();
	/// set modified
	void SetModified();


private:	
	/// sets up UI from post effect entity
	void SetupUiFromTable();
	/// reset modified
	void ResetModified();

    Util::Array<Physics::MaterialTable::Material> materials;
    Util::Dictionary<Util::String, Util::String> names;    
    Util::Array<Util::String> realNames;
    Util::Dictionary<Util::String, Util::Dictionary<Util::String, Util::String>> interactions;
    Ui::PhysicsMaterialEditor* ui;
	bool modified;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
MaterialsEditor::IsModified()
{
	return this->modified;	
}
} // namespace QtPhysics
//------------------------------------------------------------------------------