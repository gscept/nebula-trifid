#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeGraphics
    
    Implements a graphical representation of a Nody node.
    This is a QObject even though it doesn't use it. 
    This is because we might want to use signals/slots for a node graphical representation later.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/variant.h"
#include "variable/vartype.h"
#include "simulation/simresult.h"

#include <QGraphicsTextItem>
#include <QMenu>

namespace Nody
{
class Node;
class NodePixmapItem;
class NodeItemGroup;
class NodeGraphicsScene;
class NodeGraphics : 
    public QObject,
    public Core::RefCounted
{
    __DeclareClass(NodeGraphics);
    Q_OBJECT
public:
	/// constructor
	NodeGraphics();
	/// destructor
	virtual ~NodeGraphics();

    /// sets this node to be in focus or to lose focus
    virtual void SetFocus(bool b, bool resort = true);

    /// marks this node as being visited by the generator
    virtual void Visit();
    /// marks this node as being unvisited by the generator
    virtual void Unvisit();
    /// marks this node as having a problem
    virtual void Error();
    /// unmarks this node as having a problem
    virtual void Unerror();

	/// called from UI whenever the move has been dragged
	void OnMoved(const Math::float2& pos);

    /// sets position programmatically
    void SetPosition(const Math::float2& pos);
	/// get position of node graphics
	const Math::float2& GetPosition() const;

	/// adds graphics to scene
	virtual void AddToScene(const Ptr<NodeGraphicsScene>& scene);
	/// removes graphics from scene
	virtual void RemoveFromScene(const Ptr<NodeGraphicsScene>& scene);

	/// creates actual graphics, override in subclass
	virtual void Generate();
	/// destroys graphics
	virtual void Destroy();

    /// draw simulation, this will cause the illustration box to draw a preview of the result of the node
    virtual void OnSimulate(const Util::Variant& value, const Nody::VarType& type, SimResult& result);
    /// draw simulation with image as input instead of singular value
    virtual void OnSimulate(const SimResult& value);
    /// sets up context menu
    virtual QMenu* OnRightClick() const;

	/// converts a Nebula float2 to a QPointF
	static QPointF FromFloat2(const Math::float2& pos);
protected:
	friend class NodeGraphicsScene;
	friend class Node;

    Ptr<Nody::NodeItemGroup> group;
	Ptr<Node> node;
	Math::float2 position;
    bool focus;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Math::float2& 
NodeGraphics::GetPosition() const
{
    return this->position;
}

} // namespace Nody
//------------------------------------------------------------------------------