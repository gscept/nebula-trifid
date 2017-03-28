#pragma once
//------------------------------------------------------------------------------
/**
	Implements widgets used to more dynamically select style rules.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
namespace QtToolkitUtil
{

class ToolPanel : public QFrame
{
	Q_OBJECT
public:
	ToolPanel(QWidget* parent) : QFrame(parent) {};
	ToolPanel() : QFrame() {};
};

class ToolScroll : public QScrollArea
{
	Q_OBJECT
public:
	ToolScroll(QWidget* parent) : QScrollArea(parent) {};
	ToolScroll() : QScrollArea() {};
};

class ToolButtonMenu : public QToolButton
{
	Q_OBJECT
public:
	ToolButtonMenu(QWidget* parent) : QToolButton(parent) {};
	ToolButtonMenu() : QToolButton() {};
};

class ColorToolButton : public QToolButton
{
	Q_OBJECT
public:
	ColorToolButton(QWidget* parent) : QToolButton(parent) {};
	ColorToolButton() : QToolButton() {};
};

class ColorPushButton : public QPushButton
{
	Q_OBJECT
public:
	ColorPushButton(QWidget* parent) : QPushButton(parent) {};
	ColorPushButton() : QPushButton() {};
};

class TextureToolButton : public QToolButton
{
	Q_OBJECT
public:
	TextureToolButton(QWidget* parent) : QToolButton(parent) {};
	TextureToolButton() : QToolButton() {};
};

class TexturePushButton : public QPushButton
{
	Q_OBJECT
public:
	TexturePushButton(QWidget* parent) : QPushButton(parent) {};
	TexturePushButton() : QPushButton() {};
};

class TextureLabel : public QLabel
{
	Q_OBJECT
public:
	TextureLabel(QWidget* parent) : QLabel(parent) {};
	TextureLabel() : QLabel() {};
};
} // namespace QtToolkitUtil