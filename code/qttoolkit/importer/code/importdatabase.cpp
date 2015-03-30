#include "importdatabase.h"
#include "tools\uri.h"
#include <QFile>

namespace Importer
{

using namespace QtTools;
ImportDatabase* ImportDatabase::instance = 0;
//------------------------------------------------------------------------------
/**
*/
ImportDatabase* 
ImportDatabase::Instance()
{
	if (instance == 0)
	{
		instance = new ImportDatabase();
	}

	return instance;
}

//------------------------------------------------------------------------------
/**
*/
ImportDatabase::ImportDatabase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ImportDatabase::~ImportDatabase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ImportDatabase::Load( const QString& file )
{
	QFile databaseFile(URI("workgfx:"+file).AsString());

	if (databaseFile.open(QIODevice::ReadOnly))
	{
		QDomDocument document;
		document.setContent(&databaseFile);

		QDomElement rootElement = document.documentElement();

		QDomNodeList importerList = rootElement.childNodes();
		for (int optionsIndex = 0; optionsIndex < importerList.size(); optionsIndex++)
		{
			ImportOptions* options = new ImportOptions();
			QDomElement optionsNode = importerList.at(optionsIndex).toElement();

			options->name = optionsNode.attribute("name", "");
			Q_ASSERT_X(!options->name.isEmpty(), "ImportDatabase.cpp", "Corrupt batch info file!");

			ImportOptions::MeshFlag meshFlags = (ImportOptions::MeshFlag)optionsNode.attribute("flags", "0").toInt();
			ImportOptions::ImportMode importMode = (ImportOptions::ImportMode)optionsNode.attribute("mode", "0").toInt();

			options->SetExportFlags(meshFlags);
			options->SetExportMode(importMode);

			QDomNodeList takeNodes = optionsNode.childNodes();
			for (int takeIndex = 0; takeIndex < takeNodes.size(); takeIndex++)
			{
				QDomElement takeElement = takeNodes.at(takeIndex).toElement();
				if (takeElement.tagName() == "Take")
				{
					Take* take = new Take;
					take->SetName(takeElement.attribute("name", ""));

					QDomNodeList clipElements = takeElement.childNodes();
					for (int clipIndex = 0; clipIndex < clipElements.size(); clipIndex++)
					{
						Clip* clip = new Clip;
						QDomElement clipElement = clipElements.at(clipIndex).toElement();

						clip->SetName(clipElement.attribute("name", ""));
						clip->SetStart(clipElement.attribute("start", "0").toInt());
						clip->SetEnd(clipElement.attribute("end", "0").toInt());
						clip->SetPreInfinity((Clip::InfinityType)clipElement.attribute("pre", "0").toInt());
						clip->SetPostInfinity((Clip::InfinityType)clipElement.attribute("post", "0").toInt());

						Q_ASSERT_X(!clip->GetName().isEmpty(), "BatchAttributes.cpp", "Corrupt batch info file!");

						take->AddClip(clip);
					}
					options->AddTake(take);
				}

			}
			this->importOptions[options->name] = options;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ImportDatabase::Save( const QString& file )
{
	QFile databaseFile(URI("workgfx:"+file).AsString());

	if (databaseFile.open(QIODevice::WriteOnly))
	{
		QDomDocument document;
		QDomElement rootElement = document.createElement("Nebula3");

		document.appendChild(rootElement);

		QMapIterator<QString, ImportOptions*> optionsIterator(this->importOptions);
		while (optionsIterator.hasNext())
		{
			optionsIterator.next();
			QString optionsName = optionsIterator.key();
			ImportOptions* options = optionsIterator.value();

			QDomElement optionsElement = document.createElement("Options");
			optionsElement.setAttribute("name", optionsName);

			optionsElement.setAttribute("flags", options->GetExportFlags());
			optionsElement.setAttribute("mode", options->GetExportMode());

			for (int takeIndex = 0; takeIndex < options->takes.size(); takeIndex++)
			{
				QDomElement takeElement = document.createElement("Take");
				Take* take = options->takes[takeIndex];
				takeElement.setAttribute("name", take->GetName());
				for (uint clipIndex = 0; clipIndex < take->GetNumClips(); clipIndex++)
				{
					Clip* clip = take->GetClip(clipIndex);
					QDomElement clipElement = document.createElement("Clip");

					clipElement.setAttribute("name", clip->GetName());
					clipElement.setAttribute("start", clip->GetStart());
					clipElement.setAttribute("end", clip->GetEnd());
					clipElement.setAttribute("pre", clip->GetPreInfinity());
					clipElement.setAttribute("post", clip->GetPostInfinity());

					takeElement.appendChild(clipElement);
				}
				optionsElement.appendChild(takeElement);
			}

			rootElement.appendChild(optionsElement);

		}
		QByteArray bytes(document.toString().toStdString().c_str());
		databaseFile.write(bytes);
		databaseFile.close();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ImportDatabase::SaveToDefault()
{
	this->Save("batchattributes.xml");
}

//------------------------------------------------------------------------------
/**
*/
void 
ImportDatabase::LoadFromDefault()
{
	this->Load("batchattributes.xml");
}
}