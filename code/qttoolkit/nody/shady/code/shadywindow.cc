//------------------------------------------------------------------------------
//  shadywindow.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "config.h"
#include "shadywindow.h"
#include "variation/variationdatabase.h"
#include "variation/graphics/variationlistitem.h"
#include "variation/shadyvariationloader.h"
#include "variation/shadysupervariationloader.h"
#include "node/shadynode.h"
#include "project/shadyprojectsaver.h"
#include "project/shadyprojectloader.h"
#include "project/shadyproject.h"
#include "generation/anyfxgenerator.h"
#include "vecvalidator.h"
#include "pathvalidator.h"
#include "timing/timer.h"
#include "ui_shadywindow.h"
#include "ui_buildsettings.h"
#include "ui_validation.h"
#include <QFileDialog>
#include <QScrollBar>
#include <QDoubleValidator>
#include <QIntValidator>

using namespace Util;
using namespace Nody;
namespace Shady
{
__ImplementClass(Shady::ShadyWindow, 'SHWN', Core::RefCounted);
__ImplementSingleton(Shady::ShadyWindow);

Ui::ShadyMainWindow* ShadyWindow::ui;
//------------------------------------------------------------------------------
/**
*/
ShadyWindow::ShadyWindow() :
    nodeScene(0)
{
    this->ui = new Ui::ShadyMainWindow;
	this->ui->setupUi(this);
    this->highlighter = new Highlighter(this->ui->codeOutput->document());
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShadyWindow::~ShadyWindow()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::Setup()
{
	// create variation database
	this->variationDatabase = VariationDatabase::Create();

    // setup project
    this->project = Shady::ShadyProject::Create();

	// create variation loader
	Ptr<ShadyVariationLoader> variationLoader = ShadyVariationLoader::Create();
	this->variationDatabase->SetVariationLoader(variationLoader.upcast<Nody::VariationLoader>());

    // create super variation loader
    Ptr<ShadySuperVariationLoader> superVariationLoader = ShadySuperVariationLoader::Create();
    this->variationDatabase->SetSuperVariationLoader(superVariationLoader.upcast<Nody::SuperVariationLoader>());

	// open database
	this->variationDatabase->Open();

	// now load variations
	this->variationDatabase->LoadVariations("toolkit:work/shady/variations");
    this->variationDatabase->LoadSuperVariations("toolkit:work/shady/supervariations");    

    // create scene
    this->nodeScene = NodeScene::Create();

	// set scenes for shade
    this->nodeScene->LinkWithGraphics(ShadyWindow::ui->NodeView->GetScene());

	// now setup palette
	this->SetupVariationTree();

    // get default super variation
    this->superVariation = this->variationDatabase->GetSuperVariationByName("Default");

    // setup dialogs
    this->buildSettingsUi = new Ui::BuildSettings;
    this->validationUi = new Ui::Validation;
    this->buildSettingsUi->setupUi(&this->settingsDialog);
    this->validationUi->setupUi(&this->validationDialog);

	// connect UI signals
	QObject::connect(ShadyWindow::ui->actionReal_time_visualize, SIGNAL(triggered()), this, SLOT(OnRealtimeVisualizeClicked()));

	// connect signal for creating nodes
	QObject::connect(ShadyWindow::ui->variationTree,     SIGNAL(CreateNode(const Ptr<Nody::Variation>&)), this, SLOT(OnCreateNode(const Ptr<Nody::Variation>&)));
    QObject::connect(ShadyWindow::ui->actionSettings,    SIGNAL(triggered()), &this->settingsDialog, SLOT(show()));
    QObject::connect(ShadyWindow::ui->actionValidate,    SIGNAL(triggered()), &this->validationDialog, SLOT(show()));

    // connect signals for managing the project
    QObject::connect(ShadyWindow::ui->actionNew,         SIGNAL(triggered()), this, SLOT(OnNew()));
    QObject::connect(ShadyWindow::ui->actionOpen,        SIGNAL(triggered()), this, SLOT(OnOpen()));
    QObject::connect(ShadyWindow::ui->actionSave,        SIGNAL(triggered()), this, SLOT(OnSave()));
    QObject::connect(ShadyWindow::ui->actionSave_as,     SIGNAL(triggered()), this, SLOT(OnSaveAs()));    

    // connect signals related to building
    QObject::connect(ShadyWindow::ui->actionBuild,       SIGNAL(triggered()), this, SLOT(OnBuild()));

    // connect signals related to the scene
    QObject::connect(this->nodeScene->GetNodeSceneGraphics(), SIGNAL(NodeFocused(const Ptr<Nody::Node>&)), this, SLOT(OnNodeClicked(const Ptr<Nody::Node>&)));
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::Close()
{
    this->superVariation = 0;
    this->mainNode = 0;
    this->project = 0;

    // delete dialogs
    delete this->buildSettingsUi;
    delete this->validationUi;

    // clear scene
    this->nodeScene->Clear();
    this->nodeScene = 0;

    // close variaton database
    this->variationDatabase->Close();
    this->variationDatabase = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::SetupVariationTree()
{
    VariationDatabase* varDb = VariationDatabase::Instance();
    n_assert(varDb->IsOpen());

    // get categories
    const Array<String>& categories = varDb->GetVariationCategories();

    // create root elements
    IndexT i, j;
    for (i = 0; i < categories.Size(); i++)
    {
        // get category
        const String& category = categories[i];

        // add root element to list
        QTreeWidgetItem* topItem = new QTreeWidgetItem();
        topItem->setText(0, category.AsCharPtr());
        this->ui->variationTree->addTopLevelItem(topItem);

        // get variations by category
        const Array<Ptr<Variation> >& variations = varDb->GetVariationsByCategory(category);
        for (j = 0; j < variations.Size(); j++)
        {
            // get variation
            const Ptr<Variation>& variation = variations[j];

            // create item
            VariationListItem* subItem = new VariationListItem;
            subItem->SetVariation(variation);
            subItem->setText(0, variation->GetName().AsCharPtr());
            subItem->setToolTip(0, variation->GetDescription().AsCharPtr());
            topItem->addChild(subItem);
        }
    }

    // sort elements
    this->ui->variationTree->sortByColumn(0, Qt::AscendingOrder);
}


//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::showEvent(QShowEvent* e)
{
    this->nodeScene->GetNodeSceneGraphics()->SetInteractiveMode(true);

	// setup new scene first time we show the window
	this->SetupNewScene();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::closeEvent(QCloseEvent* e)
{
    this->nodeScene->GetNodeSceneGraphics()->SetInteractiveMode(false);

	// discard main node
	this->nodeScene->Clear();
	this->mainNode = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnCreateNode( const Ptr<Nody::Variation>& variation )
{
	// create and setup node
	Ptr<ShadyNode> node = ShadyNode::Create();
	node->Setup(variation);

	// now add node to scene
	this->nodeScene->AddNode(node.upcast<Nody::Node>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::SetupNewScene()
{
    n_assert(!this->mainNode.isvalid());

    // reset center
    this->ui->NodeView->ResetCenter();

    // add a main node
    this->mainNode = ShadyNode::Create();
    this->mainNode->SetFlags(Node::NoDelete);
    this->mainNode->Setup(this->superVariation);
    this->nodeScene->AddNode(this->mainNode.downcast<Nody::Node>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnSave()
{
    const Ptr<ShadyProject>& proj = ShadyProject::Instance();

    // if project name is not set, use save as instead
    if (!proj->GetPath().IsValid())
    {
        this->OnSaveAs();
    }
    else
    {
        // if this project already exists, write over the old file
        Ptr<ShadyProjectSaver> saver = ShadyProjectSaver::Create();
        proj->Store(this->nodeScene);
        saver->Save(proj.upcast<Nody::Project>(), proj->GetPath());

        SHADY_MESSAGE_FORMAT("Saved project '%s'", proj->GetPath().LocalPath().AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnSaveAs()
{
    const Ptr<ShadyProject>& proj = ShadyProject::Instance();
    IO::URI path;
    if (this->currentPath.IsEmpty())    path = IO::URI("root:work/shady/projects");
    else                                path = this->currentPath;
    this->currentPath = path;
    QString file = QFileDialog::getSaveFileName(NULL, "Save Shady project as", path.LocalPath().AsCharPtr(), "*.ndp");
    if (!file.isEmpty())
    {
        Ptr<ShadyProjectSaver> saver = ShadyProjectSaver::Create();
        String fullPath = String(file.toUtf8().constData());
        proj->SetPath(fullPath);
        proj->Store(this->nodeScene);
        saver->Save(proj.upcast<Nody::Project>(), fullPath);

        SHADY_MESSAGE_FORMAT("Saved project '%s'", fullPath.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnOpen()
{
    const Ptr<ShadyProject>& proj = ShadyProject::Instance();
    IO::URI path;
    if (this->currentPath.IsEmpty())    path = IO::URI("root:work/shady/projects");
    else                                path = this->currentPath;
    this->currentPath = path;
    QString file = QFileDialog::getOpenFileName(NULL, "Open Shady project", path.LocalPath().AsCharPtr(), "*.ndp");
    if (!file.isEmpty())
    {
        Ptr<ShadyProjectLoader> loader = ShadyProjectLoader::Create();
        String fullPath = String(file.toUtf8().constData());

        // clear project prior to opening
        proj->Clear(true);

        // run loader
        loader->Load(proj.upcast<Nody::Project>(), fullPath);

        // set path in project
        proj->SetPath(fullPath);

        // when project is loaded, apply it to the scene
        proj->Apply(this->nodeScene);

        // set main node from scene
        this->mainNode = this->nodeScene->GetSuperNode().downcast<ShadyNode>();
        this->ui->NodeView->ResetCenter();

        SHADY_MESSAGE_FORMAT("Opened project '%s'", fullPath.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnNew()
{
    // get project and clear it
    const Ptr<ShadyProject>& proj = ShadyProject::Instance();
    proj->Clear(true);

    // clear scene, should also add the main node back
    this->nodeScene->Clear();

    // set main node to 0
    this->mainNode = 0;

    // setup a new scene
    this->SetupNewScene();

    SHADY_MESSAGE("New project");
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnBuild()
{
    n_assert(this->mainNode.isvalid());
    OutputLanguage effectType = (OutputLanguage)this->buildSettingsUi->outputLanguage->currentIndex();

    // create output buffer
    Util::Blob result;
    bool success = false;

    switch (effectType)
    {
    case AnyFX_GLSL:
        {
            SHADY_MESSAGE("Building...");
            Timing::Timer timer;
            timer.Start();

            // create generator
            Ptr<AnyFXGenerator> generator = AnyFXGenerator::Create();
			generator->SetVisualize(this->ui->actionDebug_code_generation->isChecked(), 500);
            generator->GenerateToBuffer(this->nodeScene, result);

            // run generation, if successful, also stop timer
            success = generator->GetStatus();
            if (success)
            {
                timer.Stop();
                SHADY_MESSAGE_FORMAT("Build done! Build took %f s", timer.GetTime());
                emit this->OnBuildDone(result);
            }
            
            break;
        }
    case AnyFX_HLSL:
        {
            SHADY_ERROR("AnyFX (HLSL) generation not yet implemented");
            break;
        }
    case FX_HLSL:
        {
            SHADY_ERROR("Microsoft FX generation not yet implemented");
            break;
        }
    case LUA_SCRIPT:
        {
            SHADY_ERROR("LUA generation not yet implemented");
            break;
        }
    case JS_SCRIPT:
        {
            SHADY_ERROR("JavaScript generation not yet implemented");
            break;
        }
    }

    if (success)
    {
        // when done, put output buffer in text field
        int value = this->ui->codeOutput->verticalScrollBar()->value();
        this->ui->codeOutput->setText(QByteArray((const char*)result.GetPtr(), result.Size()));
        this->ui->codeOutput->verticalScrollBar()->setValue(value);
    }    
}

//------------------------------------------------------------------------------
/**
*/
struct NodeAttributeGroup
{
    Util::String key;
    Util::Variant value;
};
Q_DECLARE_METATYPE(Shady::NodeAttributeGroup)

//------------------------------------------------------------------------------
/**
*/
void
ShadyWindow::OnNodeClicked(const Ptr<Nody::Node>& node)
{
    // clear properties
    this->ui->propertiesTable->clearContents();
    this->ui->propertiesTable->setRowCount(0);
    this->currentNode = node;

    if (node.isvalid())
    {
        IndexT i;
        const Util::Dictionary<Util::String, Util::Variant>& values = node->GetValueDictionary();
        for (i = 0; i < values.Size(); i++)
        {
            NodeAttributeGroup group;
            group.key = values.KeyAtIndex(i);
            group.value = values.ValueAtIndex(i);

            // get type
            const VarType& type = node->GetValueType(group.key);
            
            QLabel* propertyLabel = new QLabel(NULL);
            propertyLabel->setContentsMargins(QMargins(15, 0, 0, 0));
            propertyLabel->setText(group.key.AsCharPtr());

            // create new row
            this->ui->propertiesTable->insertRow(i);

            QLineEdit* valueLine = new QLineEdit(NULL);
            valueLine->setText(group.value.ToString().AsCharPtr());
            if      (type.GetType() == Nody::VarType::Float)              valueLine->setValidator(new QDoubleValidator);
            else if (type.GetType() == Nody::VarType::Float2)             valueLine->setValidator(new QtToolkit::VecValidator(2));
            else if (type.GetType() == Nody::VarType::Float3)             valueLine->setValidator(new QtToolkit::VecValidator(3));
            else if (type.GetType() == Nody::VarType::Float4)             valueLine->setValidator(new QtToolkit::VecValidator(4));
            else if (type.GetType() == Nody::VarType::Int)                valueLine->setValidator(new QIntValidator);
            else if (type.GetType() == Nody::VarType::Int2)               valueLine->setValidator(new QtToolkit::VecValidator(2, 6, 6, Util::Variant::Int));
            else if (type.GetType() == Nody::VarType::Int3)               valueLine->setValidator(new QtToolkit::VecValidator(3, 6, 6, Util::Variant::Int));
            else if (type.GetType() == Nody::VarType::Int4)               valueLine->setValidator(new QtToolkit::VecValidator(4, 6, 6, Util::Variant::Int));
            else if (type.GetType() == Nody::VarType::Bool2)              valueLine->setValidator(new QtToolkit::VecValidator(1, 0, 0, Util::Variant::Bool));
            else if (type.GetType() == Nody::VarType::Bool2)              valueLine->setValidator(new QtToolkit::VecValidator(2, 0, 0, Util::Variant::Bool));
            else if (type.GetType() == Nody::VarType::Bool3)              valueLine->setValidator(new QtToolkit::VecValidator(3, 0, 0, Util::Variant::Bool));
            else if (type.GetType() == Nody::VarType::Bool4)              valueLine->setValidator(new QtToolkit::VecValidator(4, 0, 0, Util::Variant::Bool));
            else if (type.GetType() == Nody::VarType::Matrix2x2)          valueLine->setValidator(new QtToolkit::VecValidator(4));
            else if (type.GetType() == Nody::VarType::Matrix2x3)          valueLine->setValidator(new QtToolkit::VecValidator(6));
            else if (type.GetType() == Nody::VarType::Matrix2x4)          valueLine->setValidator(new QtToolkit::VecValidator(8));
            else if (type.GetType() == Nody::VarType::Matrix3x2)          valueLine->setValidator(new QtToolkit::VecValidator(6));
            else if (type.GetType() == Nody::VarType::Matrix3x3)          valueLine->setValidator(new QtToolkit::VecValidator(9));
            else if (type.GetType() == Nody::VarType::Matrix3x4)          valueLine->setValidator(new QtToolkit::VecValidator(12));
            else if (type.GetType() == Nody::VarType::Matrix4x2)          valueLine->setValidator(new QtToolkit::VecValidator(8));
            else if (type.GetType() == Nody::VarType::Matrix4x3)          valueLine->setValidator(new QtToolkit::VecValidator(12));
            else if (type.GetType() == Nody::VarType::Matrix4x4)          valueLine->setValidator(new QtToolkit::VecValidator(16));
			/*
            else if (type.GetType() == Nody::VarType::Sampler2D)          valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
            else if (type.GetType() == Nody::VarType::Sampler2DArray)     valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
            else if (type.GetType() == Nody::VarType::Sampler3D)          valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
            else if (type.GetType() == Nody::VarType::Sampler3DArray)     valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
            else if (type.GetType() == Nody::VarType::SamplerCube)        valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
            else if (type.GetType() == Nody::VarType::SamplerCubeArray)   valueLine->setValidator(new QtToolkit::PathValidator("dds", "tex"));
			*/

            // set label and value
            this->ui->propertiesTable->setCellWidget(i, 0, propertyLabel);
            this->ui->propertiesTable->setCellWidget(i, 1, valueLine);

            // connect value and set custom data in value to indicate which field index it belongs to
            valueLine->setProperty(NEBULAUSERPROPERTYNAME, QVariant::fromValue(group));
            connect(valueLine, SIGNAL(editingFinished()), this, SLOT(OnNodeValueChanged()));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyWindow::OnNodeValueChanged()
{
    QLineEdit* line = (QLineEdit*)this->sender();
    QVariant var = line->property(NEBULAUSERPROPERTYNAME);
    NodeAttributeGroup group = var.value<NodeAttributeGroup>();

    if (group.value.SetParseString(line->text().toUtf8().constData()))
    {
        // set value
        this->currentNode->SetValue(group.key, group.value);

        // perform simulation
        this->currentNode->Simulate();
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyWindow::ConsoleMessage(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    Util::String msg;
    msg.FormatArgList(format, arg);
    QPalette palette = ShadyWindow::ui->output->palette();
    palette.setColor(QPalette::Text, Qt::green);
    ShadyWindow::ui->output->setPalette(palette);
    ShadyWindow::ui->output->setText(msg.AsCharPtr());
    va_end(arg);
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyWindow::ConsoleError(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    Util::String msg;
    msg.FormatArgList(format, arg);
    QPalette palette = ShadyWindow::ui->output->palette();
    palette.setColor(QPalette::Text, Qt::red);
    ShadyWindow::ui->output->setPalette(palette);
    ShadyWindow::ui->output->setText(msg.AsCharPtr());
    va_end(arg);
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyWindow::ConsoleWarning(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    Util::String msg;
    msg.FormatArgList(format, arg);
    QPalette palette = ShadyWindow::ui->output->palette();
    palette.setColor(QPalette::Text, Qt::darkYellow);
    ShadyWindow::ui->output->setPalette(palette);
    ShadyWindow::ui->output->setText(msg.AsCharPtr());
    va_end(arg);
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyWindow::OnRealtimeVisualizeClicked()
{
	this->nodeScene->GetNodeSceneGraphics()->SetInteractiveMode(this->ui->actionReal_time_visualize->isChecked());
}

} // namespace Shady