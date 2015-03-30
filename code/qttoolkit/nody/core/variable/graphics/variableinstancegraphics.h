#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::ConnectorGraphicsItem
    
	Describes the graphical anchor-point to a variable inside the UI.
	Uses a QGraphicsPolygonItem as base, but has its own hover events.

    The QObject inheritance here is just to be able to cast to it from a QGraphicsItem
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "math//float2.h"
#include <QBrush>
#include <QGraphicsPolygonItem>
namespace Nody
{
class NodeGraphicsScene;
class VariableInstance;
class VariableInstanceGraphics : 
    public QObject,
    public Core::RefCounted
{
	__DeclareClass(VariableInstanceGraphics);
public:
	/// constructor
	VariableInstanceGraphics();
	/// destructor
	virtual ~VariableInstanceGraphics();

	/// generate graphics
	void Generate();
	/// destroy graphics
	void Destroy();

	/// add to group
	void AddToGroup(QGraphicsItemGroup* group);
	/// remove from group
	void RemoveFromGroup(QGraphicsItemGroup* group);
	/// add to scene
	void AddToScene(const Ptr<NodeGraphicsScene>& scene);
	/// remove from scene
	void RemoveFromScene(const Ptr<NodeGraphicsScene>& scene);

	/// sets upper left position of graphics
	void SetPosition(const Math::float2& pos);
	/// sets size of connector
	void SetSize(const Math::float2& size);
	/// get scene center of graphics
	QPointF GetCenter() const;

	/// locks variable (occurs when we link the variable)
	void Lock();
	/// unlocks variable (occurs when we break the link to the variable)
	void Unlock();
	/// return lock
	bool IsLocked() const;

	/// call when we are hovering
	void Highlight();
	/// called when we are leaving
	void UnHighlight();

    /// marks this node as being visited by the generator
    void Visit();
    /// marks this node as being unvisited by the generator
    void Unvisit();

	/// set hover brush
	void SetHoverBrush(const QBrush& brush);

	/// sets variable associated with connector
	void SetVariableInstance(const Ptr<VariableInstance>& var);
	/// gets variable associated with connector
	const Ptr<VariableInstance>& GetVariableInstance() const;

private:
	Math::float2 pos;
	Math::float2 size;
	QGraphicsPolygonItem* connectorItem;
	Ptr<VariableInstance> varInst;
	QBrush highlightBrush;
	QBrush standardBrush;
	QBrush lockedBrush;
	bool isLocked;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
VariableInstanceGraphics::SetHoverBrush( const QBrush& brush )
{
	this->highlightBrush = brush;
}

//------------------------------------------------------------------------------
/**
*/
inline QPointF 
VariableInstanceGraphics::GetCenter() const
{
	return this->connectorItem->sceneBoundingRect().center();
}



} // namespace Nody
//------------------------------------------------------------------------------