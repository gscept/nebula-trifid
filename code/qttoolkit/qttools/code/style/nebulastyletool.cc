//------------------------------------------------------------------------------
//  graypalette.cc
//  (C) 2012-2016 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nebulastyletool.h"

namespace QtToolkitUtil
{

//------------------------------------------------------------------------------
/**
*/
NebulaStyleTool::NebulaStyleTool()
{
	this->setBrush(QPalette::Window, QBrush(qRgb(110, 110, 110)));
	this->setBrush(QPalette::Base, QBrush(qRgb(80, 80, 80)));
	this->setBrush(QPalette::Button, QBrush(qRgb(100, 100, 100)));
	this->setBrush(QPalette::ButtonText, QBrush(Qt::white));
	this->setBrush(QPalette::WindowText, QBrush(Qt::white));
	this->setBrush(QPalette::Text, QBrush(Qt::white));
	this->setBrush(QPalette::BrightText, QBrush(Qt::lightGray));
	this->setBrush(QPalette::ToolTipBase, QBrush(Qt::darkGray));
	this->setBrush(QPalette::ToolTipText, QBrush(Qt::white));
    this->setBrush(QPalette::AlternateBase, QBrush(Qt::darkGray));

	this->setBrush(QPalette::Disabled, QPalette::Button, QBrush(qRgb(70, 70, 70)));
	this->setBrush(QPalette::Disabled, QPalette::ButtonText, QBrush(Qt::darkGray));
	this->setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(qRgb(20, 20, 20)));
	this->setBrush(QPalette::Disabled, QPalette::Text, QBrush(qRgb(20, 20, 20)));
	this->setBrush(QPalette::Disabled, QPalette::BrightText, QBrush(Qt::lightGray));
	this->setBrush(QPalette::Disabled, QPalette::Shadow, QBrush(Qt::black));

    this->setBrush(QPalette::Highlight, QBrush(qRgb(240, 160, 0))); // orange	
	this->setBrush(QPalette::Mid, QBrush(qRgb(80, 80, 80)));
	this->setBrush(QPalette::Light, QBrush(qRgb(90, 90, 90)));
	this->setBrush(QPalette::Midlight, QBrush(qRgb(60, 60, 60)));
	this->setBrush(QPalette::Dark, QBrush(qRgb(20, 20, 20)));
    this->setBrush(QPalette::Shadow, QBrush(Qt::black));

	QString mainWindowSetup = "QMainWindow::separator { background: palette(window); width: 4px; } QToolBar { border-bottom: 0px; background-color: palette(window); } QToolBar::separator { border-left: 1px solid palette(midlight); } QMainWindow::separator:hover, QMainWindow::separator:pressed { background-color: palette(highlight); } QDockWidget QWidget, QDockWidget > QWidget { background-color: palette(base); } QDockWidget::title { height: 1px; text-align: center; background-color: palette(highlight); border-top: 1px solid palette(dark); border-left: 1px solid palette(dark); border-right: 1px solid palette(dark); border-top-left-radius: 3px; border-top-right-radius: 3px; } QDockWidget::close-button, QDockWidget::float-button { background-color: palette(highlight); } QDockWidget::close-button { right: 10px; top: 3px; } QDockWidget::float-button { right: 25px; top: 3px; } QDockWidget > * > *, QDockWidget > QWidget { color: black; border-top: 0; border-left: 1px solid palette(dark); border-right: 1px solid palette(dark); border-bottom: 1px solid palette(dark); } QDialog, QDialog *{ background-color: palette(base); }";
	QString frameSetup = ".QFrame { border 1px solid palette(midlight); }";
	QString tabWidgetSetup = "QTabWidget QFrame, QTabWidget QFrame * { background: palette(light); } QTabWidget::pane { border-top: 1px solid palette(midlight); border-bottom: 1px solid palette(midlight); } QTabWidget::tab-bar { left: 10px; } QTabBar::tab { color: palette(bright-text); border: 1px solid palette(midlight); border-bottom: 1px solid transparent; padding: 2px 5px; border-top-left-radius: 2px; border-top-right-radius: 2px; } QTabBar::tab:selected, QTabBar::tab:hover { color: palette(highlight); } QTabBar::tab:selected { border-bottom: 0; } QTabBar::tab:!selected { margin-top: 2px; } QTabBar::tab:!first { margin-left: -1px; } QTabBar::tab:only-one { margin-left: 0; } QTabBar QToolButton { border: 1px solid palette(dark); background: palette(button); } QTabBar::scroller { width: 10px; } QTabBar QToolButton:hover { border: 1px solid palette(highlight); }";
	QString scrollSetup = ".QScrollBar:vertical { background: transparent; width: 8px; border: none; } .QScrollBar::handle:vertical { border: 3px solid palette(midlight); border-radius:4px; background: palette(midlight); width: 4px; } .QScrollBar:horizontal { background: transparent; height: 8px; border: none; } .QScrollBar::handle:horizontal { border: 3px solid palette(midlight); border-radius:4px; background: palette(midlight); height: 4px; } QScrollBar::add-line:horizontal { background: transparent; border: none; } QScrollBar::sub-line:horizontal {  background: transparent; border: none; } QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {  background: transparent; border: none; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {  background: transparent; border: none; } QScrollBar::add-line:vertical { background: transparent; border: none; } QScrollBar::sub-line:vertical { background: transparent; border: none; } QAbstractScrollArea::corner { border: none; }";
	QString sliderSetup = ".QSlider { background: transparent; } .QSlider::handle { background: palette(button); border: 1px solid palette(dark); width: 10px; height: 10px; border-radius: 2px; } .QSlider::handle:hover { background: palette(highlight); } .QSlider::groove:horizontal { background: transparent; height: 10px; } .QSlider::groove:vertical { background: transparent; width: 10px; } .QSlider::sub-page:horizontal { background: palette(highlight); margin: 2px 0px; border: 1px solid palette(midlight); border-radius: 2px; } .QSlider::add-page:horizontal { background: palette(light); margin: 2px 0px; border: 1px solid palette(midlight); border-radius: 2px; } .QSlider::sub-page:vertical { background: palette(highlight); margin: 0px 2px; border: 1px solid palette(midlight); border-radius: 2px; } .QSlider::add-page:vertical { background: palette(light); margin: 0px 2px; border: 1px solid palette(midlight); 	border-radius: 2px; }";
	QString spinboxSetup = ".QSpinBox, .QDoubleSpinBox { border-radius: 2px; border: 1px solid palette(midlight); background-color: palette(button); } .QSpinBox::up-button, .QDoubleSpinBox::up-button { background: transparent; border-top-right-radius: 2px; border-image: url(:/icons/icons/uparrow.png) 1; width: 12px; height: 8px; } .QSpinBox::down-button, .QDoubleSpinBox::down-button { background: transparent; border-bottom-right-radius: 2px; border-image: url(:/icons/icons/downarrow.png) 1; width: 12px; height: 8px; } .QSpinBox:focus, .QDoubleSpinBox:focus { background: palette(highlight); color: black; } ";
	QString lineEditSetup = ".QLineEdit { background: palette(button); border: 1px solid palette(midlight); border-radius: 2px; } QLineEdit:focus { background: palette(highlight); color: black; }";
	QString pushButtonSetup = ".QPushButton { background: palette(button); padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-top: 2px; border: 1px solid palette(dark); border-radius: 2px; } .QPushButton:hover { border-color: palette(highlight); }";
	QString labelSetup = ".QLabel { background: transparent; }";
	QString menuSetup = ".QMenu { border: 1px solid palette(dark); border-bottom-left-radius: 2px; border-bottom-right-radius: 2px; } .QMenu::separator { border-top: 1px solid palette(midlight); }";
	QString comboBoxSetup = ".QComboBox { background: palette(button); border: 1px solid palette(dark); border-radius: 2px; padding-left: 2px; } .QComboBox:on { background: palette(highlight); border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; } .QComboBox:editable { border: 0; border-left: 1px solid palette(midlight); border-right: 1px solid palette(midlight); border-bottom: 1px solid palette(midlight); padding-left: 5px; padding-right: 5px } .QComboBox:!editable, QComboBox::drop-down:editable { background: palette(button); } .QComboBox:!editable:on, QComboBox::drop-down:editable:on { background: palette(highlight); } .QComboBox::down-arrow { background: transparent; border: 0; border-image: url(:/icons/icons/downarrow.png) 1; width: 12px; height: 8px; } .QComboBox::drop-down { background: transparent; width: 12px; border: 0; } .QComboBox::down-arrow:on { top: 1px; } .QComboBox QAbstractItemView { background: palette(button); border: 1px solid palette(dark); border-top: 0; selection-background-color: palette(highlight); outline: 0px; } ";
	QString listViewSetup = ".QListView, .QTreeView, .QTableView, .QListWidget, .QTreeWidget, .QTableWidget { border: 1px solid palette(midlight); border-radius: 2px; background: palette(light); } .QHeaderView { border: 0; border-bottom: 1px solid palette(midlight); } .QHeaderView::section { border: 0; background: palette(midlight); padding-left: 2px; } .QHeaderView::section:!first { border-left: 1px solid palette(midlight); } .QHeaderView::section:checked { background: palette(highlight); } ";
	QString toolPanelSetup = ".QtToolkitUtil--ToolPanel { background: palette(light); border: 0; border-top: 1px solid palette(midlight); }";
	QString toolButtonMenuSetup = ".QtToolkitUtil--ToolButtonMenu { color: palette(text); background: palette(button); border: 1px solid palette(dark); border-radius: 2px; padding-right: 25px; } QtToolkitUtil--ToolButtonMenu:hover { border-color: palette(highlight); } QtToolkitUtil--ToolButtonMenu:disabled { border: 1px solid palette(light); }";
	QString textureButtonsSetup = ".QtToolkitUtil--TextureToolButton, .QtToolkitUtil--TexturePushButton { background: transparent; border: 1px solid palette(dark); border-radius: 2px; margin: 0; padding: 0; } .QtToolkitUtil--TextureToolButton:hover, .QtToolkitUtil--TexturePushButton:hover { border: 1px solid palette(highlight); } ";
	QString colorButtonsSetup = ".QtToolkitUtil--ColorToolButton, .QtToolkitUtil--ColorPushButton { background: 0; border: 1px solid palette(dark); border-radius: 2px; padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-top: 2px; } .QtToolkitUtil--ColorToolButton:hover, .QtToolkitUtil--ColorPushButton:hover { border: 1px solid palette(highlight); }";
	QString textureLabelSetup = ".QtToolkitUtil--TextureLabel { background: 0; border: 1px solid palette(midlight); border-radius: 5px; }";
	QString embedWindowSetup = ".QtNebulaProxyWidget { background: 0; }";
	this->globalStyleSheet = 
		mainWindowSetup + 
		frameSetup +
		scrollSetup + 
		sliderSetup + 
		spinboxSetup + 
		tabWidgetSetup + 
		lineEditSetup + 
		pushButtonSetup + 
		labelSetup + 
		menuSetup + 
		comboBoxSetup +
		listViewSetup +
		toolPanelSetup + 
		toolButtonMenuSetup + 
		textureButtonsSetup +
		colorButtonsSetup +
		textureLabelSetup +
		embedWindowSetup;
	this->font = QFont("Tahoma", 8);
	this->font.setStyleStrategy(QFont::PreferAntialias);
	this->font.setStyleHint(QFont::Monospace);
	this->font.setKerning(false);
}

//------------------------------------------------------------------------------
/**
*/
NebulaStyleTool::~NebulaStyleTool()
{
	// empty
}


} // namespace QtToolkitUtil