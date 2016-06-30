//------------------------------------------------------------------------------
//  qdevilplugin.cc
//  (C) 2012 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "qdevilplugin.h"
#include "IL/il.h"
#include "IL/ilu.h"


//------------------------------------------------------------------------------
/**
*/
QDevilPlugin::QDevilPlugin(QObject* parent) :
	QImageIOPlugin(parent)
{
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);	
}

//------------------------------------------------------------------------------
/**
*/
QDevilPlugin::~QDevilPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QStringList 
QDevilPlugin::keys() const
{
	return QStringList() << "psd" << "tga" << "dds";
}

//------------------------------------------------------------------------------
/**
*/
QImageIOPlugin::Capabilities 
QDevilPlugin::capabilities( QIODevice* device, const QByteArray& format ) const
{
	if (format == "psd")
	{
		return Capabilities(CanRead);
	}
	else if (format == "tga")
	{
		return Capabilities(CanRead);
	}
	else if (format == "dds")
	{
		return Capabilities(CanRead);
	}
	else
	{
		return 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
QImageIOHandler* 
QDevilPlugin::create( QIODevice* device, const QByteArray& format /*= QByteArray*/ ) const
{
	QImageIOHandler* handler = new QDevilHandler;
	handler->setDevice(device);
	handler->setFormat(format);
	return handler;
}

#ifdef __USE_QT4
Q_EXPORT_PLUGIN2(qdevil, QDevilPlugin);
#endif
//------------------------------------------------------------------------------
/**
*/

QDevilHandler::QDevilHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/

QDevilHandler::~QDevilHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
QDevilHandler::canRead() const
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
QDevilHandler::read( QImage* image )
{
	int index;
	index = ilGenImage();
	ilBindImage(index);
	QByteArray data = device()->readAll();

	ILboolean success;

	// load using il
	if (this->format() == "tga")
	{
		success = ilLoadL(IL_TGA, data.constData(), data.size());
	}
	else if (this->format() == "psd")
	{
		success = ilLoadL(IL_PSD, data.constData(), data.size());
	}
	else if (this->format() == "dds")
	{
		success = ilLoadL(IL_DDS, data.constData(), data.size());
	}
	else if (this->format() == "thumb") // this is a special solution for our implementation, thumbnails are always PNGs
	{
		success = ilLoadL(IL_PNG, data.constData(), data.size());
	}

	// convert image
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	// flip image
	iluFlipImage();

	// get image measurements
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	int width = ilGetInteger(IL_IMAGE_WIDTH);

	// get image data
	void* dataPtr = ilGetData();
	int size = height * width * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	// create new image locally, then create a copy
	// this is important, because we want to free the DevIL memory to prevent a memory leak, but Qt only allows us to create an image by copy the pointer, and not the actual value...
	QImage temp((const uchar*)dataPtr, width, height, QImage::Format_RGB888);
	*image = temp.copy();

	// delete il image
	ilDeleteImage(index);

	return success;
}
