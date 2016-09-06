//------------------------------------------------------------------------------
//  shadynodegraphics.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadynodegraphics.h"
#include "scene/graphics/nodegraphicsscene.h"
#include "node/node.h"
#include "variation/variationinstance.h"
#include "io/uri.h"
#include "variable/graphics/variableinstancegraphics.h"
#include "nody_config.h"
#include <QApplication>
#include <QWidgetAction>
#include <QLabel>
#include <QPainter>

#define PARAMETEROFFSET 18
#define CONNECTOROFFSET 5
#define CONNECTORSIZE 10
#define VALUEPIXMAPSIZE 64
#define ITEMTEXTFONT QFont("Segoe UI", 8)
#define NODELABELTEXT QFont("Segoe UI", 10)
using namespace Util;
using namespace Nody;
namespace Shady
{
__ImplementClass(Shady::ShadyNodeGraphics, 'SHNG', Nody::NodeGraphics);

//------------------------------------------------------------------------------
/**
*/
ShadyNodeGraphics::ShadyNodeGraphics() :
	basePlateItem(0),
	labelPlateItem(0),
	labelItem(0),
    valuePixmapItem(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyNodeGraphics::~ShadyNodeGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNodeGraphics::AddToScene(const Ptr<Nody::NodeGraphicsScene>& scene)
{
    NodeGraphics::AddToScene(scene);
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNodeGraphics::RemoveFromScene(const Ptr<Nody::NodeGraphicsScene>& scene)
{
    NodeGraphics::RemoveFromScene(scene);
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Generate()
{
    // generate graphics for base class
    NodeGraphics::Generate();

    if (this->node->GetVariation().isvalid())
    {
        this->GenerateFromVariation();
    }
	else if (this->node->GetSuperVariation().isvalid())
    {
        this->GenerateFromSuperVariation();
    }
    else
    {
        n_error("Graphics created without either a variation or supervariation!");
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::GenerateFromVariation()
{
    // get original variation
	const Ptr<VariationInstance>& variation = this->node->GetVariation();
    const Array<Ptr<VariableInstance>>& inputs = variation->GetInputs();
    const Array<Ptr<VariableInstance>>& outputs = variation->GetOutputs();

    // create a total bounding box
    QRectF boundingBox;

    // create label
    this->labelItem = new QGraphicsTextItem(variation->GetOriginalVariation()->GetName().AsCharPtr());
	this->labelItem->setFont(NODELABELTEXT);
    qreal textHeight = this->labelItem->boundingRect().height();

    // we define an offset at which all the content should appear
    float contentOffset = this->labelItem->boundingRect().height();

    // create height which will increase for each text item
    float y = contentOffset;
    float x = 0;
    QSizeF leftOffset;

    // create inputs
    IndexT i;
    for (i = 0; i < inputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& input = inputs[i];

        // set left offset
        leftOffset = QSizeF(PARAMETEROFFSET, 0);

        // create label
        QGraphicsTextItem* textItem = new QGraphicsTextItem;
        textItem->setHtml("<p>" +
            QString(VarType::ToString(input->GetOriginalVariable()->GetType()).AsCharPtr()) + 
            "<b> " + 
            QString(input->GetOriginalVariable()->GetName().AsCharPtr()) + 
            "</b></p>");
        textItem->setPos(PARAMETEROFFSET, y);
        textItem->setDefaultTextColor(Qt::white);
		textItem->setZValue(NODELAYERFOREGROUND);
		textItem->setFont(ITEMTEXTFONT);
        this->inputLabelItems.Append(textItem);
        boundingBox = boundingBox.united(QRectF(textItem->pos(), textItem->boundingRect().size() + leftOffset));
        this->group->addToGroup(textItem);

        y += textHeight;
    }

    // create pixmap inbetween inputs and outputs
    this->valuePixmap = QPixmap(VALUEPIXMAPSIZE, VALUEPIXMAPSIZE);
    this->valuePixmap.fill(Qt::black);
    this->valuePixmapItem = new QGraphicsPixmapItem(this->valuePixmap);
    this->valuePixmapItem->setPos(boundingBox.width()+1, contentOffset+1);
	this->valuePixmapItem->setZValue(NODELAYERFOREGROUND);
    this->group->addToGroup(this->valuePixmapItem);	

    // reset height
    boundingBox.setWidth(boundingBox.width() + this->valuePixmap.width() + leftOffset.width());
    boundingBox.setHeight(Math::n_max(this->valuePixmap.height(), (int)boundingBox.height()));
    y = contentOffset + 2;
    x = boundingBox.width();

    // create outputs
    for (i = 0; i < outputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& output = outputs[i];

        // create label
        QGraphicsTextItem* textItem = new QGraphicsTextItem;
        textItem->setHtml("<p><b>" + 
            QString(output->GetOriginalVariable()->GetName().AsCharPtr()) + 
            "</b> " + 
            QString(VarType::ToString(output->GetOriginalVariable()->GetType()).AsCharPtr()) + 
            "</p>");
        textItem->setPos(x - leftOffset.width(), y);
        textItem->setDefaultTextColor(Qt::white);
		textItem->setZValue(NODELAYERFOREGROUND);
		textItem->setFont(ITEMTEXTFONT);
        boundingBox = boundingBox.united(QRectF(QPointF(x, y), textItem->boundingRect().size() + QSizeF(PARAMETEROFFSET, 0)));
        this->group->addToGroup(textItem);
        this->inputLabelItems.Append(textItem);

        y += textHeight;
    }

    QRect bb = boundingBox.toRect();

    // create background
    this->basePlate = QPolygon(QRect(0, 0, bb.width(), bb.height() + contentOffset + 2));
    this->basePlateItem = new QGraphicsPolygonItem(this->basePlate);
    this->basePlateItem->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    this->basePlateItem->setBrush(QBrush(qRgb(51, 153, 255)));
    this->basePlateItem->setZValue(NODELAYERBACKGROUND);

    // reset y
    y = contentOffset;

    // create connectors for inputs
    for (i = 0; i < inputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& input = inputs[i];
        input->GenerateGraphics();

        // create connector
        const Ptr<VariableInstanceGraphics>& connectorItem = input->GetGraphics();
        connectorItem->SetSize(Math::float2(CONNECTORSIZE, CONNECTORSIZE));
        connectorItem->SetPosition(Math::float2(CONNECTOROFFSET, y+textHeight*0.25f));
        connectorItem->AddToGroup(this->group);
        this->inputConnectorItems.Append(connectorItem);

        y += textHeight;
    }

    // reset y one last time
    y = contentOffset;

    // create connectors for outputs
    for (i = 0; i < outputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& output = outputs[i];
        output->GenerateGraphics();

        // create connector
        const Ptr<VariableInstanceGraphics>& connectorItem = output->GetGraphics();
        connectorItem->SetSize(Math::float2(CONNECTORSIZE, CONNECTORSIZE));
        connectorItem->SetPosition(Math::float2(boundingBox.size().width() - CONNECTOROFFSET - CONNECTORSIZE, y + textHeight * 0.25f));
        connectorItem->AddToGroup(this->group);
        this->outputConnectorItems.Append(connectorItem);

        y += textHeight;
    }

    // finally add item to group
    this->group->addToGroup(this->basePlateItem);	

    // adjust label and create background, -2 is for the border
    this->labelPlate = QPolygon(QRect(1, 1, bb.width()-2, this->labelItem->boundingRect().height()));
    this->labelPlateItem = new QGraphicsPolygonItem(this->labelPlate);
    this->labelPlateItem->setPen(QPen(QBrush(Qt::transparent), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    this->labelPlateItem->setBrush(QBrush(qRgb(96, 96, 96)));
	this->labelPlateItem->setZValue(NODELAYERBACKGROUND);
    this->labelItem->setTextWidth(this->group->boundingRect().width());
    this->labelItem->setDefaultTextColor(Qt::white);
    this->labelItem->setHtml("<p align=\"center\">" + QString(variation->GetOriginalVariation()->GetName().AsCharPtr()) + "</p>");
	this->labelItem->setZValue(NODELAYERFOREGROUND);

    // add label to group
    this->group->addToGroup(this->labelPlateItem);
    this->group->addToGroup(this->labelItem);
	this->group->setZValue(NODELAYERFOREGROUND);
    this->group->setPos(NodeGraphics::FromFloat2(this->GetPosition()));
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::GenerateFromSuperVariation()
{
	const Ptr<SuperVariationInstance>& superVariation = this->node->GetSuperVariation();
    const Array<Ptr<VariableInstance>>& inputs = superVariation->GetInputs();

    // create a total bounding box
    QRectF boundingBox;

    // create label
    this->labelItem = new QGraphicsTextItem(superVariation->GetOriginalSuperVariation()->GetName().AsCharPtr());
	this->labelItem->setFont(NODELABELTEXT);
    qreal textHeight = this->labelItem->boundingRect().height();

    // we define an offset at which all the content should appear
    float contentOffset = this->labelItem->boundingRect().height();

    // create height which will increase for each text item
    float y = contentOffset;
    float x = 0;
    QSizeF leftOffset;

    // create inputs
    IndexT i;
    for (i = 0; i < inputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& input = inputs[i];

        // set left offset
        leftOffset = QSizeF(PARAMETEROFFSET, 0);

        // create label
        QGraphicsTextItem* textItem = new QGraphicsTextItem;
        textItem->setHtml("<p>" +
            QString(VarType::ToString(input->GetOriginalVariable()->GetType()).AsCharPtr()) + 
            "<b> " + 
            QString(input->GetOriginalVariable()->GetName().AsCharPtr()) + 
            "</b></p>");
        textItem->setPos(PARAMETEROFFSET, y);
        textItem->setDefaultTextColor(Qt::white);
		textItem->setZValue(NODELAYERFOREGROUND);
		textItem->setFont(ITEMTEXTFONT);
        this->inputLabelItems.Append(textItem);
        boundingBox = boundingBox.united(QRectF(textItem->pos(), textItem->boundingRect().size() + leftOffset));
        this->group->addToGroup(textItem);

        y += textHeight;
    }

    QRect bb = boundingBox.toRect();

    // create background
    this->basePlate = QPolygon(QRect(0, 0, bb.width(), bb.height() + contentOffset));
    this->basePlateItem = new QGraphicsPolygonItem(this->basePlate);
    this->basePlateItem->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    this->basePlateItem->setBrush(QBrush(qRgb(0, 204, 102)));
	this->basePlateItem->setZValue(NODELAYERBACKGROUND);

    // reset y
    y = contentOffset;

    // create connectors for inputs
    for (i = 0; i < inputs.Size(); i++)
    {
        // get variable
        const Ptr<VariableInstance>& input = inputs[i];
        input->GenerateGraphics();

        // create connector
        const Ptr<VariableInstanceGraphics>& connectorItem = input->GetGraphics();
        connectorItem->SetSize(Math::float2(CONNECTORSIZE, CONNECTORSIZE));
        connectorItem->SetPosition(Math::float2(CONNECTOROFFSET, y+textHeight*0.25f));
        connectorItem->AddToGroup(this->group);
        this->inputConnectorItems.Append(connectorItem);

        y += textHeight;
    }

    // finally add item to group
    this->group->addToGroup(this->basePlateItem);	

    // adjust label and create background
    this->labelPlate = QPolygon(QRect(1, 1, bb.width()-2, this->labelItem->boundingRect().height()));
    this->labelPlateItem = new QGraphicsPolygonItem(this->labelPlate);
    this->labelPlateItem->setPen(QPen(QBrush(Qt::transparent), 0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    this->labelPlateItem->setBrush(QBrush(qRgb(96, 96, 96)));
	this->labelPlateItem->setZValue(NODELAYERBACKGROUND);
    this->labelItem->setTextWidth(this->group->boundingRect().width());
    this->labelItem->setDefaultTextColor(Qt::white);
    this->labelItem->setHtml("<p align=\"center\">" + QString(superVariation->GetOriginalSuperVariation()->GetName().AsCharPtr()) + "</p>");
	this->labelItem->setZValue(NODELAYERFOREGROUND);

    // add label to group
    this->group->addToGroup(this->labelPlateItem);
    this->group->addToGroup(this->labelItem);
	this->group->setZValue(NODELAYERFOREGROUND);
	this->group->setPos(NodeGraphics::FromFloat2(this->GetPosition()));
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Destroy()
{
	n_assert(this->group.isvalid());
	delete this->basePlateItem;
	delete this->labelPlateItem;
	delete this->labelItem;
	if (0 != this->valuePixmapItem) delete this->valuePixmapItem;

	// we don't need to delete the objects if we just delete the group ( Qt magic! )
	this->group = 0;

	// clear lists
	this->inputConnectorItems.Clear();
	this->inputLabelItems.Clear();

	this->outputConnectorItems.Clear();
	this->outputLabelItems.Clear();

	this->constantConnectorItems.Clear();
	this->constantLabelItems.Clear();

	this->dynamicConnectorItems.Clear();
	this->dynamicLabelItems.Clear();

	NodeGraphics::Destroy();
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNodeGraphics::SetFocus(bool b, bool resort)
{
    NodeGraphics::SetFocus(b, resort);
    if (b)
    {
        QPen pen(QBrush(qRgb(210, 105, 30)), 2.0f, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin);
        this->basePlateItem->setPen(pen);
    }
    else
    {
        QPen pen(QBrush(Qt::black), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        this->basePlateItem->setPen(pen);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Visit()
{
    QPen pen(QBrush(Qt::green), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    this->basePlateItem->setPen(pen);
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Unvisit()
{
    QPen pen(QBrush(Qt::black), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    this->basePlateItem->setPen(pen);
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Error()
{
    QPen pen(QBrush(Qt::red), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    this->basePlateItem->setPen(pen);
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::Unerror()
{
    QPen pen(QBrush(Qt::black), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    this->basePlateItem->setPen(pen);
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNodeGraphics::OnSimulate(const Util::Variant& value, const Nody::VarType& type, Nody::SimResult& result)
{
    // don't simulate if we don't have a value pixmap
    if (this->valuePixmapItem == 0) return;

    uint vectorSize = VarType::VectorSize(type);
    VarType::ValueType primitiveType = VarType::ComponentType(type);
    if (type.GetType() >= VarType::Sampler2D && type.GetType() <= VarType::SamplerCubeArray)
    {
        // load texture
        IO::URI tex = IO::URI(value.GetString());

        QPixmap pixmap;
        if (pixmap.load(tex.LocalPath().AsCharPtr()))
        {
            this->valuePixmap = pixmap.scaled(QSize(VALUEPIXMAPSIZE, VALUEPIXMAPSIZE), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            result.Fill(this->valuePixmap);
        }
    }
    else
    {
        switch (vectorSize)
        {
        case 1:
            {
                switch (primitiveType)
                {
                case VarType::Float:
                    {
                        float val = value.GetFloat();
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val * 255, val * 255, val * 255, 255));
                    }
                    break;
                case VarType::Int:
                    {
                        int val = value.GetInt();
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val * 255, val * 255, val * 255, 255));
                    }
                    break;
                case VarType::Bool:
                    {
                        bool val = value.GetBool();
                        val == true ? this->valuePixmap.fill(Qt::white) : this->valuePixmap.fill(Qt::black);
                    }
                    break;
                }
                break;
            }
        case 2:
            {
                switch (primitiveType)
                {
                case VarType::Float:
                    {
                        Util::Array<float> val = value.GetFloatArray();
                        n_assert(val.Size() == 2);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, 0, 255));
                    }
                    break;
                case VarType::Int:
                    {
                        Util::Array<int> val = value.GetIntArray();
                        n_assert(val.Size() == 2);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, 0, 255));
                    }
                    break;
                case VarType::Bool:
                    {
                        Util::Array<bool> val = value.GetBoolArray();
                        n_assert(val.Size() == 2);
                        QColor q1 = val[0] == true ? Qt::white : Qt::black;
                        QColor q2 = val[1] == true ? Qt::white : Qt::black;
                        QPainter painter(&this->valuePixmap);
                        painter.setPen(q1);
                        painter.drawRect(0, 0, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE);
                        painter.setPen(q2);
                        painter.drawRect(VALUEPIXMAPSIZE/2, 0, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE);
                    }
                    break;
                }
                break;
            }
        case 3:
            {
                switch (primitiveType)
                {
                case VarType::Float:
                    {
                        Util::Array<float> val = value.GetFloatArray();
                        n_assert(val.Size() == 3);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, val[2] * 255, 255));
                    }
                    break;
                case VarType::Int:
                    {
                        Util::Array<int> val = value.GetIntArray();
                        n_assert(val.Size() == 3);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, val[2] * 255, 255));
                    }
                    break;
                case VarType::Bool:
                    {
                        Util::Array<bool> val = value.GetBoolArray();
                        n_assert(val.Size() == 3);
                        QColor q1 = val[0] == true ? Qt::white : Qt::black;
                        QColor q2 = val[1] == true ? Qt::white : Qt::black;
                        QColor q3 = val[2] == true ? Qt::white : Qt::black;
                        QPainter painter(&this->valuePixmap);
                        painter.setPen(q1);
                        painter.drawRect(0, 0, VALUEPIXMAPSIZE/3, VALUEPIXMAPSIZE);
                        painter.setPen(q2);
                        painter.drawRect(VALUEPIXMAPSIZE/3, 0, VALUEPIXMAPSIZE/3, VALUEPIXMAPSIZE);
                        painter.setPen(q3);
                        painter.drawRect(2 *  VALUEPIXMAPSIZE / 3, 0, VALUEPIXMAPSIZE/3, VALUEPIXMAPSIZE);
                    }
                    break;
                }
                break;
            }
        case 4:
            {
                switch (primitiveType)
                {
                case VarType::Float:
                    {
                        Util::Array<float> val = value.GetFloatArray();
                        n_assert(val.Size() == 4);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, val[2] * 255, val[3] * 255));
                    }
                    break;
                case VarType::Int:
                    {
                        Util::Array<int> val = value.GetIntArray();
                        n_assert(val.Size() == 4);
                        result.Fill(val);
                        this->valuePixmap.fill(qRgba(val[0] * 255, val[1] * 255, val[2] * 255, val[3] * 255));
                    }
                    break;
                case VarType::Bool:
                    {
                        Util::Array<bool> val = value.GetBoolArray();
                        n_assert(val.Size() == 4);
                        QColor q1 = val[0] == true ? Qt::white : Qt::black;
                        QColor q2 = val[1] == true ? Qt::white : Qt::black;
                        QColor q3 = val[2] == true ? Qt::white : Qt::black;
                        QColor q4 = val[3] == true ? Qt::white : Qt::black;
                        QPainter painter(&this->valuePixmap);
                        painter.setPen(q1);
                        painter.drawRect(0, 0, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2);
                        painter.setPen(q2);
                        painter.drawRect(VALUEPIXMAPSIZE/2, 0, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2);
                        painter.setPen(q3);
                        painter.drawRect(0, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2);
                        painter.setPen(q4);
                        painter.drawRect(VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2, VALUEPIXMAPSIZE/2);
                    }
                    break;
                }
                break;
            }
        }
    }

    // set pixmap and return result as image
    this->valuePixmapItem->setPixmap(this->valuePixmap);
    result.SetCurrentDefault();
    result.UpdateGraphics();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::OnSimulate(const Nody::SimResult& res)
{
    if (0 != this->valuePixmapItem)
    {
        this->valuePixmap = QPixmap::fromImage(*res.GetGraphics());
        this->valuePixmapItem->setPixmap(this->valuePixmap);
    }    
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNodeGraphics::TriggerSimulate()
{
    this->node->Simulate();
}

//------------------------------------------------------------------------------
/**
*/
QMenu* 
ShadyNodeGraphics::OnRightClick() const
{
    QMenu* menu = new QMenu;
    if (this->node->IsSuperNode())
    {
        const Ptr<SuperVariation>& var = this->node->GetSuperVariation()->GetOriginalSuperVariation();

        QWidgetAction* labelAction = new QWidgetAction(NULL);
        QLabel* label = new QLabel(NULL);
        label->setFixedWidth(125);
        labelAction->setDefaultWidget(label);
        QFont labelFont;
        labelFont.setItalic(true);
        labelFont.setPointSize(12);
        label->setAlignment(Qt::AlignHCenter);
        label->setFont(labelFont);
        label->setText(var->GetName().AsCharPtr());
        menu->addAction(labelAction);
        menu->addSeparator();

        QAction* switchAction = new QAction(NULL);
        switchAction->setText("Switch...");
        menu->addAction(switchAction);

        QAction* renderAction = new QAction(NULL);
        renderAction->setText("Render settings...");
        menu->addAction(renderAction);
    }
    else
    {
        const Ptr<Variation>& var = this->node->GetVariation()->GetOriginalVariation();

        QWidgetAction* labelAction = new QWidgetAction(NULL);
        QLabel* label = new QLabel(NULL);
        label->setFixedWidth(125);
        labelAction->setDefaultWidget(label);
        QFont labelFont;
        labelFont.setItalic(true);
        labelFont.setPointSize(12);
        label->setAlignment(Qt::AlignHCenter);
        label->setFont(labelFont);
        label->setText(var->GetName().AsCharPtr());
        menu->addAction(labelAction);
        menu->addSeparator();

        QAction* simulateAction = new QAction(NULL);
        simulateAction->setText("Simulate");
        connect(simulateAction, SIGNAL(triggered()), this, SLOT(TriggerSimulate()));
        menu->addAction(simulateAction);

        QAction* helpAction = new QAction(NULL);
        helpAction->setText("Help");
        menu->addAction(helpAction);
    }
    return menu;
}
} // namespace Shady
