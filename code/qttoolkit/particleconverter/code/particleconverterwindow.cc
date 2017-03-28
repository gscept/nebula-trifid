//------------------------------------------------------------------------------
// surfacegeneratorwindow.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "particleconverterwindow.h"
#include "qttools/code/style/nebulastyletool.h"
#include "core/coreserver.h"
#include "io/fswrapper.h"
#include "io/ioserver.h"
#include "modelutil/modeldatabase.h"
#include "projectinfo.h"
#include "io/xmlwriter.h"

using namespace Util;
namespace ToolkitUtil
{

//------------------------------------------------------------------------------
/**
*/
ParticleConverterWindow::ParticleConverterWindow()
{
	this->ui.setupUi(this);
	this->ui.searchbar->setVisible(false);

	QtToolkitUtil::NebulaStyleTool palette;
	QApplication::setPalette(palette);
	connect(this->ui.scanButton, SIGNAL(pressed()), this, SLOT(OnScanClicked()));
}

//------------------------------------------------------------------------------
/**
*/
ParticleConverterWindow::~ParticleConverterWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleConverterWindow::OnScanClicked()
{
	Ptr<Core::CoreServer> coreServer = Core::CoreServer::Create();
	Util::String root = IO::FSWrapper::GetHomeDirectory();
	coreServer->SetRootDirectory(root);
	coreServer->Open();

	// setup IO
	Ptr<IO::IoServer> ioServer = IO::IoServer::Create();
	ioServer->MountStandardArchives();

	// open model database
	Ptr<ModelDatabase> database = ModelDatabase::Create();
	database->Open();

	// setup project info
	ToolkitUtil::ProjectInfo projInfo;

#ifdef __WIN32__
	projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Win32);
#elif __LINUX__
	projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Linux);
#endif

	projInfo.Setup();
	IO::AssignRegistry::Instance()->SetAssign(IO::Assign("src", projInfo.GetAttr("SrcDir")));

	// create string for root folder
	String assetFolder = "proj:work/assets";

	// create dictionary which will contain identical materials (pattern -> nodes)
	Dictionary<String, Array<KeyValuePair<String, String>>> dict;

	// make the progress bar visible and reset max
	this->ui.searchbar->setVisible(true);
	this->ui.searchbar->setMaximum(0);
	this->ui.searchbar->setValue(0);
	QApplication::processEvents();

	// go through all asset folders
	Array<String> dirs = ioServer->ListDirectories(assetFolder, "*");
	IndexT i;
	for (i = 0; i < dirs.Size(); i++)
	{
		Array<String> files = ioServer->ListFiles(assetFolder + "/" + dirs[i], "*.attributes");
		this->ui.searchbar->setMaximum(this->ui.searchbar->maximum() + files.Size());

		IndexT j;
		for (j = 0; j < files.Size(); j++)
		{
			String file = dirs[i] + "/" + files[j];
			file.StripFileExtension();

			// get attributes!
			Ptr<ModelAttributes> attrs = database->LookupAttributes(file);
			Ptr<ModelConstants> consts = database->LookupConstants(file);

			bool changesDone = false;
			Util::Array<ModelConstants::ParticleNode> particles = consts->GetParticleNodes();
			IndexT k;
			for (k = 0; k < particles.Size(); k++)
			{
				const ModelConstants::ParticleNode& par = particles[k];
				ModelAttributes::AppendixNode newnode;
				newnode.name = par.name;
				newnode.path = par.path;
				newnode.type = ModelAttributes::ParticleNode;
				newnode.transform = par.transform;
				newnode.data.particle.primGroup = par.primitiveGroupIndex;
				attrs->AddAppendixNode(newnode.name, newnode);
				consts->DeleteParticleNode(newnode.name);
				changesDone = true;
			}

			if (changesDone)
			{
				// save files
				Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(assetFolder + "/" + file + ".attributes");
				attrs->Save(stream);
				stream = IO::IoServer::Instance()->CreateStream(assetFolder + "/" + file + ".constants");
				consts->Save(stream);
			}			

			this->ui.searchbar->setValue(this->ui.searchbar->value() + 1);
		}
	}
	this->ui.searchbar->setVisible(false);
	database->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ParticleConverterWindow::OnCreateClicked()
{
	// 1 means success
	this->makeDialog.done(1);
}

//------------------------------------------------------------------------------
/**
*/
bool
ParticleConverterWindow::CreateSurface(const ToolkitUtil::State& state, const Util::String& name)
{
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(name);
	Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	writer->SetStream(stream);
	if (writer->Open())
	{
		writer->BeginNode("NebulaT");
			writer->BeginNode("Surface");
			writer->SetString("template", state.material);
			IndexT i;
			for (i = 0; i < state.textures.Size(); i++)
			{
				const ToolkitUtil::Texture& tex = state.textures[i];

				writer->BeginNode("Param");
				writer->SetString("name", tex.textureName);
				writer->SetString("value", tex.textureResource + NEBULA3_TEXTURE_EXTENSION);
				writer->EndNode();
			}

			for (i = 0; i < state.variables.Size(); i++)
			{
				const ToolkitUtil::Variable& var = state.variables[i];

				writer->BeginNode("Param");
				writer->SetString("name", var.variableName);
				writer->SetString("value", var.variableValue.ToString());
				writer->EndNode();
			}
			writer->EndNode();
		writer->EndNode();
	}
	else
	{
		return false;
	}
	return true;
}

} // namespace ToolkitUtil