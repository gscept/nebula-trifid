//------------------------------------------------------------------------------
// frameshaderviz.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frameshaderviz.h"
#include "io/ioserver.h"
#include "io/bxmlreader.h"
#include "shadywindow.h"
#include "coregraphics/rendertarget.h"

namespace Shady
{

//------------------------------------------------------------------------------
/**
*/
FrameshaderViz::FrameshaderViz()
{
	this->ui.setupUi(this);

	QObject::connect(this->ui.frameShaderSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSelectFrameshader()));
	QObject::connect(this->ui.showAllBatches, SIGNAL(stateChanged(int)), this, SLOT(OnSelectFrameshader()));
	QObject::connect(this->ui.refreshButton, SIGNAL(pressed()), this, SLOT(OnSelectFrameshader()));
}

//------------------------------------------------------------------------------
/**
*/
FrameshaderViz::~FrameshaderViz()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameshaderViz::showEvent(QShowEvent* event)
{
	Util::Array<Util::String> frameshaders = IO::IoServer::Instance()->ListFiles("frame:", "*.xml");
	IndexT i;
	for (i = 0; i < frameshaders.Size(); i++)
	{
		Util::String frameshader = frameshaders[i];
		frameshader = frameshader.ExtractFileName();
		frameshader.StripFileExtension();
		this->ui.frameShaderSelect->addItem(frameshader.AsCharPtr());
	}

	// make sure it selected the default frame shader
	this->ui.frameShaderSelect->setCurrentIndex(this->ui.frameShaderSelect->findText(NEBULA3_DEFAULT_FRAMESHADER_NAME));
}

//------------------------------------------------------------------------------
/**
*/
void
FrameshaderViz::OnSelectFrameshader()
{
	Util::String path = Util::String::Sprintf("frame:%s.xml", this->ui.frameShaderSelect->currentText().toUtf8().constData());
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
	Ptr<IO::BXmlReader> reader = IO::BXmlReader::Create();
	reader->SetStream(stream);
	if (reader->Open())
	{
		if (!reader->HasNode("/Nebula3/FrameShader"))
		{
			n_error("FrameShaderLoader: '%s' is not a valid frame shader!", path.AsCharPtr());
		}
		reader->SetToNode("/Nebula3/FrameShader");

		// valid frameshader, so clear tree
		this->ui.frameShaderTree->clear();

		// go through all passes
		if (reader->SetToFirstChild("Pass")) do 
		{
			Util::String name = reader->GetString("name");
			Util::String shader = reader->GetOptString("shader", "");
			Util::String rt;
			if (reader->HasAttr("renderTarget")) rt = reader->GetString("renderTarget");
			if (reader->HasAttr("multipleRenderTarget")) rt = reader->GetString("multipleRenderTarget");
			if (reader->HasAttr("renderTargetCube")) rt = reader->GetString("renderTargetCube");
			uint clearFlags = 0;
			clearFlags |= reader->GetOptBool("clearColor", false) ? CoreGraphics::RenderTarget::ClearColor : 0;
			clearFlags |= reader->GetOptBool("clearDepth", false) ? CoreGraphics::DepthStencilTarget::ClearDepth : 0;
			clearFlags |= reader->GetOptBool("clearStencil", false) ? CoreGraphics::DepthStencilTarget::ClearStencil : 0;
			Util::String clearFlag;
			if (clearFlags & CoreGraphics::RenderTarget::ClearColor) clearFlag += "Color";
			if (clearFlags & CoreGraphics::DepthStencilTarget::ClearDepth) { if (!clearFlag.IsEmpty()) clearFlag += " & "; clearFlag += "Depth"; }
			if (clearFlags & CoreGraphics::DepthStencilTarget::ClearStencil) { if (!clearFlag.IsEmpty()) clearFlag += " & "; clearFlag += "Stencil"; }

			QTreeWidgetItem* pass = new QTreeWidgetItem;
			pass->setText(0, name.AsCharPtr());
			pass->setText(1, shader.AsCharPtr());
			pass->setText(2, rt.IsEmpty() ? "Screen" : rt.AsCharPtr());
			pass->setText(3, clearFlag.IsEmpty() ? "None" : clearFlag.AsCharPtr());
			this->ui.frameShaderTree->addTopLevelItem(pass);

			// go through all batches...
			if (reader->SetToFirstChild("Batch")) do
			{
				Util::String type = reader->GetString("type");
				Util::String batch = reader->GetOptString("batchGroup", "");

				// only show geometry passes
				if (type == "Geometry" || this->ui.showAllBatches->isChecked())
				{
					QTreeWidgetItem* batchItem = new QTreeWidgetItem;
					batchItem->setText(0, batch.IsEmpty() ? "None" : batch.AsCharPtr());
					batchItem->setText(1, type.AsCharPtr());
					pass->addChild(batchItem);
				}				
			}
			while (reader->SetToNextChild("Batch"));
		} 
		while (reader->SetToNextChild("Pass"));
	}
	else
	{
		SHADY_ERROR_FORMAT("Could not open frameshader %s", path.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameshaderViz::OnSelectBatch()
{

}

} // namespace Shady