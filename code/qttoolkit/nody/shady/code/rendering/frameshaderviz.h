#pragma once
//------------------------------------------------------------------------------
/**
	Tool used to visualize a frame shader file.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "ui_frameshaderbrowser.h"

#include <QDialog>
namespace Shady
{
class FrameshaderViz : public QDialog
{
	Q_OBJECT
public:
	/// constructor
	FrameshaderViz();
	/// destructor
	virtual ~FrameshaderViz();

	/// handle opening
	void showEvent(QShowEvent* event);

private slots:
	/// handle selecting a frame shader
	void OnSelectFrameshader();
	/// handle selecting a batch
	void OnSelectBatch();
private:
	Ui::FrameshaderBrowser ui;
};
} // namespace Shady