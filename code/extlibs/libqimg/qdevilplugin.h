#pragma once
//------------------------------------------------------------------------------
/**
    @class ::QDevilPlugin
    
    Implements a DevIL plugin for Qt.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QtGui/QImageIOPlugin>
#include <QtGui/QImageIOHandler>
#include <QByteArray>
#include <QImage>
class QDevilPlugin : public QImageIOPlugin
{
	Q_OBJECT
public:
	/// constructor
	QDevilPlugin(QObject* parent = 0);
	/// destructor
	virtual ~QDevilPlugin();

	/// returs list of keys for plugin
	QStringList keys() const;
	/// returns capabilities
	Capabilities capabilities(QIODevice* device, const QByteArray& format) const;
	/// creatse handler
	QImageIOHandler* create(QIODevice* device, const QByteArray& format = QByteArray()) const;
}; 
//------------------------------------------------------------------------------


class QDevilHandler : public QImageIOHandler
{
public:
	/// constructor
	QDevilHandler();
	/// destructor
	virtual ~QDevilHandler();

	/// returns true if image can be read
	bool canRead() const;
	/// reads image
	bool read(QImage* image);

};