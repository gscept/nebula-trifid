//------------------------------------------------------------------------------
// surfacegeneratorwindow.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfacegeneratorwindow.h"
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
SurfaceGeneratorWindow::SurfaceGeneratorWindow()
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
SurfaceGeneratorWindow::~SurfaceGeneratorWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceGeneratorWindow::OnScanClicked()
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

			// create serialization string which we will use to match materials
			String matPattern;

			// get shapes
			const Array<ModelConstants::ShapeNode>& shapeNodes = consts->GetShapeNodes();
			IndexT k;
			for (k = 0; k < shapeNodes.Size(); k++)
			{
				const ModelConstants::ShapeNode& shapeNode = shapeNodes[k];
				const State& state = attrs->GetState(shapeNode.path);
				if (state.textures.Size() == 0 && state.variables.Size() == 0) continue;
				matPattern += state.material + ">";

				IndexT l;
				for (l = 0; l < state.textures.Size(); l++)
				{
					const Texture& tex = state.textures[l];
					matPattern += tex.textureName + "=" + tex.textureResource + ";";
				}

				for (l = 0; l < state.variables.Size(); l++)
				{
					const Variable& var = state.variables[l];
					matPattern += var.variableName + "=" + var.variableValue.ToString() + ";";
				}

				if (!dict.Contains(matPattern))
				{
					dict.Add(matPattern, Array<KeyValuePair<String, String>>());
				}
				dict[matPattern].Append(KeyValuePair<String, String>(file, shapeNode.path));
			}

			const Array<ModelConstants::Skin>& skins = consts->GetSkins();
			for (k = 0; k < skins.Size(); k++)
			{
				const ModelConstants::Skin& skin = skins[k];
				const State& state = attrs->GetState(skin.path);
				if (state.textures.Size() == 0 && state.variables.Size() == 0) continue;
				matPattern += state.material + ">";

				IndexT l;
				for (l = 0; l < state.textures.Size(); l++)
				{
					const Texture& tex = state.textures[l];
					matPattern += tex.textureName + "=" + tex.textureResource + ";";
				}

				for (l = 0; l < state.variables.Size(); l++)
				{
					const Variable& var = state.variables[l];
					matPattern += var.variableName + "=" + var.variableValue.ToString() + ";";
				}

				if (!dict.Contains(matPattern))
				{
					dict.Add(matPattern, Array<KeyValuePair<String, String>>());
				}
				dict[matPattern].Append(KeyValuePair<String, String>(file, skin.path));
			}

			const Array<ModelConstants::ParticleNode>& particles = consts->GetParticleNodes();
			for (k = 0; k < particles.Size(); k++)
			{
				const ModelConstants::ParticleNode& particle = particles[k];
				const State& state = attrs->GetState(particle.path);
				if (state.textures.Size() == 0 && state.variables.Size() == 0) continue;
				matPattern += state.material + ">";

				IndexT l;
				for (l = 0; l < state.textures.Size(); l++)
				{
					const Texture& tex = state.textures[l];
					matPattern += tex.textureName + "=" + tex.textureResource + ";";
				}

				for (l = 0; l < state.variables.Size(); l++)
				{
					const Variable& var = state.variables[l];
					matPattern += var.variableName + "=" + var.variableValue.ToString() + ";";
				}

				if (!dict.Contains(matPattern))
				{
					dict.Add(matPattern, Array<KeyValuePair<String, String>>());
				}
				dict[matPattern].Append(KeyValuePair<String, String>(file, particle.path));
			}
		}
	}

	// setup dialog
	this->makeUi.setupUi(&this->makeDialog);
	connect(this->makeUi.create, SIGNAL(pressed()), this, SLOT(OnCreateClicked()));

	// now go through all setups and query a rename
	for (i = 0; i < dict.Size(); i++)
	{
		this->makeUi.duplicateList->clear();
		const Array<KeyValuePair<String, String>>& files = dict.ValueAtIndex(i);
		IndexT j;
		for (j = 0; j < files.Size(); j++)
		{
			const String& file = files[j].Key();
			const String& node = files[j].Value();

			String format = String::Sprintf("%s (%s)", file.AsCharPtr(), node.AsCharPtr());

			// simply add to duplicate list
			this->makeUi.duplicateList->addItem(format.AsCharPtr());
		}
		const String& firstFile = files[0].Key();
		String category = firstFile.ExtractDirName();
		category.Strip("/");
		String file = firstFile.ExtractFileName();

		// make name appear to be the first name
		this->makeUi.folder->setText(category.AsCharPtr());
		this->makeUi.name->setText(file.AsCharPtr());

		// this means we will create and assign a material
		if (this->makeDialog.exec() == 1)
		{
			for (j = 0; j < files.Size(); j++)
			{
				const String& file = files[j].Key();
				const String& node = files[j].Value();

				// get attributes
				Ptr<ModelAttributes> attrs = database->LookupAttributes(file, true);

				// make sure that the folder exists
				IO::IoServer::Instance()->CreateDirectory(this->makeUi.folder->text().toUtf8().constData());

				// modify state to remove textures, variables
				State state = attrs->GetState(node);
				String surface = String::Sprintf("proj:work/assets/%s/%s.sur", this->makeUi.folder->text().toUtf8().constData(), this->makeUi.name->text().toUtf8().constData());
				if (this->CreateSurface(state, surface))
				{
					// modify model only if we successfully created the surface
					state.material = String::Sprintf("sur:%s/%s", this->makeUi.folder->text().toUtf8().constData(), this->makeUi.name->text().toUtf8().constData());
					state.textures.Clear();
					state.variables.Clear();
					attrs->SetState(node, state);

					// save attributes
					String path = String::Sprintf("src:assets/%s.attributes", file.AsCharPtr());
					Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
					attrs->Save(stream);
				}
			}
		}
		this->ui.searchbar->setValue(this->ui.searchbar->value() + files.Size());
	}

	// hide bar again
	this->ui.searchbar->setVisible(false);
	database->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceGeneratorWindow::OnCreateClicked()
{
	// 1 means success
	this->makeDialog.done(1);
}

//------------------------------------------------------------------------------
/**
*/
bool
SurfaceGeneratorWindow::CreateSurface(const ToolkitUtil::State& state, const Util::String& name)
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