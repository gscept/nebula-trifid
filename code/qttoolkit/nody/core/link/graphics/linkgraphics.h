#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::LinkGraphics
    
    Graphical representation of links
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "antialiasedlinegraphics.h"

#include <QObject>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QPen>
#include <QTimer>

namespace Nody
{
class NodeGraphicsScene;
class Link;
class LinkGraphics : 
	public QObject,
	public Core::RefCounted
{
	__DeclareClass(LinkGraphics);
	Q_OBJECT	
public:

    enum LinkCurveType
    {
        Linear,
        Quadratic,
        Cubic
    };

	/// constructor
	LinkGraphics();
	/// destructor
	virtual ~LinkGraphics();

    /// marks this node as being visited by the generator
    void Visit();
    /// marks this node as being unvisited by the generator
    void Unvisit();

    /// sets the type of curve which should be used for the link
    void SetLinkCurveType(const LinkCurveType& type);

	/// locks link, this should occur when this link is linked between variables
	void Lock();
	/// unlocks link, this occurs when we disconnect this link
	void Unlock();
	/// returns true if the link is locked
	bool IsLocked() const;

	/// toggle visibility
	void SetVisible(bool b);

	/// triggered when a node is moved which in turn updates the connection graphically
	void OnNodeMoved();

	/// adds graphics to scene
	virtual void AddToScene(const Ptr<NodeGraphicsScene>& scene);
	/// removes graphics from scene
	virtual void RemoveFromScene(const Ptr<NodeGraphicsScene>& scene);

	/// sets point at which this link is coming from
	void SetAnchorFrom(const QPointF& from);
	/// gets point at which this link is coming from
	const QPointF& GetAnchorFrom() const;
	/// sets point to which this link is pointing
	void SetAnchorTo(const QPointF& to);
	/// gets point to which this link is pointing 
	const QPointF& GetAnchorTo() const;

	/// creates actual graphics
	virtual void Generate();
	/// destroys graphics
	virtual void Destroy();

	/// update link
	void Update();

private slots:
	/// update loop which renders information flow dots
	void RenderDataFlow();

private:
	friend class Link;
    LinkCurveType curveType;
	Ptr<Link> link;
	QPainterPath linePath;
	AntialiasedLineGraphics* linePathItem;
	QGraphicsEllipseItem* ellipseItems[4];
	QPointF fromAnchor;
	QPointF toAnchor;
	QPen standardPen;
	QPen lockedPen;
	bool isLocked;

	QTimer dataFlowTimer;
	float dataFlowTime;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
LinkGraphics::SetAnchorFrom(const QPointF& from)
{
	this->fromAnchor = from;
}

//------------------------------------------------------------------------------
/**
*/
inline const QPointF& 
LinkGraphics::GetAnchorFrom() const
{
	return this->fromAnchor;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LinkGraphics::SetAnchorTo(const QPointF& to)
{
	this->toAnchor = to;
}

//------------------------------------------------------------------------------
/**
*/
inline const QPointF& 
LinkGraphics::GetAnchorTo() const
{
	return this->toAnchor;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
LinkGraphics::IsLocked() const
{
	return this->isLocked;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LinkGraphics::SetLinkCurveType(const LinkCurveType& type)
{
    this->curveType = type;
}
} // namespace Nody
//------------------------------------------------------------------------------