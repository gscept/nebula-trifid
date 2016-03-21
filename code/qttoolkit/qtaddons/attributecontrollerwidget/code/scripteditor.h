#pragma once
//------------------------------------------------------------------------------
/**    
    @class LevelEditor2::ScriptEditor
    
    Script editor which can edit scripts using syntax highlighting and multiple tabs.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include <QObject>
#include <QMainWindow>
#include <QLabel>
#include "io/uri.h"
#include "qluatextedit.h"

namespace Ui
{
    class ScriptEditorWindow;
}

namespace QtAttributeControllerAddon
{	

class ScriptEditor : public QMainWindow
{
	Q_OBJECT

public:
    /// constructor
	ScriptEditor(QWidget* parent = 0);
    /// destructor
	~ScriptEditor();

    /// returns singleton instance
    static ScriptEditor* Instance();

	/// setup dialog using uri as file and window title
	void SetupDialog(IO::URI file, const Util::String & shorttitle);
    /// add a file to be opened
    void EditFile(const IO::URI& file, const Util::String& title);	

public slots:		
    /// open file
    void Open();
    /// new file
    void New();
	/// save file
	void Save();
    /// close file
    void Close(int index);
	/// contents have been modified
	void Modified(bool);

private:
    static ScriptEditor* singleton;
    QLabel* backgroundLabel;
	Ui::ScriptEditorWindow* ui;
    Util::Array<IO::URI> files;
    Util::Array<Util::String> titles;
    Util::Array<QLuaTextEdit*> textFields;
};


//------------------------------------------------------------------------------
/**
*/
inline ScriptEditor* 
ScriptEditor::Instance()
{
    return singleton;
}


} // namespace QtAttributeControllerAddon