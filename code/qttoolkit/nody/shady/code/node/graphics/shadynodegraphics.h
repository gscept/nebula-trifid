#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyNodeGraphics
    
    Implements a Shady-specific graphical representation of a node.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variable/vartype.h"
#include "util/variant.h"
#include "node/graphics/nodegraphics.h"
#include "node/graphics/nodeitemgroup.h"
#include "variable/graphics/variableinstancegraphics.h"
#include <QPolygonF>
#include <QPixmap>
#include <QMenu>

namespace Shady
{
class ShadyNodeGraphics : public Nody::NodeGraphics
{
    __DeclareClass(ShadyNodeGraphics);
    Q_OBJECT
public:
	/// constructor
	ShadyNodeGraphics();
	/// destructor
	virtual ~ShadyNodeGraphics();

    /// mark node as visited
    void Visit();
    /// mark node as unvisited
    void Unvisit();
    /// marks this node as having a problem
    void Error();
    /// unmarks this node as having a problem
    void Unerror();

    /// toggle focus on or off
    void SetFocus(bool b, bool resort = true);

	/// add graphics to scene
	void AddToScene(const Ptr<Nody::NodeGraphicsScene>& scene);
	/// remove from scene
	void RemoveFromScene(const Ptr<Nody::NodeGraphicsScene>& scene);

	/// generate graphics
	void Generate();
	/// destroy graphics
	void Destroy();

    /// draw simulation, this will cause the illustration box to draw a preview of the result of the node
    void OnSimulate(const Util::Variant& value, const Nody::VarType& type, Nody::SimResult& result);
    /// draw simulation with image as source
    void OnSimulate(const Nody::SimResult& res);
    /// setup right click
    QMenu* OnRightClick() const;

private slots:
    /// invoked when the simulate button gets pressed
    void TriggerSimulate();

private:
	friend class ShadyNode;
    /// generates graphics based on an ordinary variation
    void GenerateFromVariation();
    /// generates graphics based on a super variation
    void GenerateFromSuperVariation();

	QPolygonF basePlate;
	QGraphicsPolygonItem* basePlateItem;

	QPolygonF labelPlate;
	QGraphicsPolygonItem* labelPlateItem;
	QGraphicsTextItem* labelItem;

	Util::Array<QGraphicsTextItem*> inputLabelItems;
	Util::Array<Ptr<Nody::VariableInstanceGraphics> > inputConnectorItems;
	Util::Array<QGraphicsTextItem*> outputLabelItems;
	Util::Array<Ptr<Nody::VariableInstanceGraphics> > outputConnectorItems;
	Util::Array<QGraphicsTextItem*> constantLabelItems;
	Util::Array<Ptr<Nody::VariableInstanceGraphics> > constantConnectorItems;
	Util::Array<QGraphicsTextItem*> dynamicLabelItems;
	Util::Array<Ptr<Nody::VariableInstanceGraphics> > dynamicConnectorItems;
	QPixmap valuePixmap;
	QGraphicsPixmapItem* valuePixmapItem;
}; 

} // namespace Shady
//------------------------------------------------------------------------------