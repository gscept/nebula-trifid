#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeGraphicsScene
    
    Implements a QGraphicsScene which handles Nody graphical items.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

namespace Nody
{
class VariableInstanceGraphics;
class NodeGraphics;
class NodeScene;
class LinkGraphics;
class VarListGraphics;
class Node;
class NodeGraphicsScene : 
	public QGraphicsScene,
	public Core::RefCounted
{
	__DeclareClass(NodeGraphicsScene);
	Q_OBJECT
public:
	/// constructor
	NodeGraphicsScene();
	/// destructor
	virtual ~NodeGraphicsScene();

	/// sets up graphics scene, must be done after it's been added to a view
	void Setup();
	/// discards graphics scene
	void Discard();

    /// starts/stops interactive mode
    void SetInteractiveMode(bool enabled);

	/// sets node scene
	void SetNodeScene(const Ptr<NodeScene>& scene);
	/// gets node scene
	const Ptr<NodeScene>& GetNodeScene() const;

	/// adds a graphics node to the graphics scene
	void AddNodeGraphics(const Ptr<NodeGraphics>& node);
	/// removes a graphics node from the graphics scene
	void RemoveNodeGraphics(const Ptr<NodeGraphics>& node);

	/// adds link graphics to the graphics scene
	void AddLinkGraphics(const Ptr<LinkGraphics>& link);
	/// removes link graphics from the graphics scene
	void RemoveLinkGraphics(const Ptr<LinkGraphics>& link);

	/// moves node to the front of the list
	void MoveToFront(const Ptr<NodeGraphics>& node);

	/// called whenever zooming occurs
	void OnScale(Math::scalar zoom);

    /// removes node focus programmatically
    void DropFocus();

private slots:
	/// delete current node
	void DeleteCurrent();
    /// called on-tick to update stuff per-frame
    void OnTick();

signals:
    /// called when a node gets clicked
    void NodeFocused(const Ptr<Nody::Node>& node);

private:
	friend class NodeGraphicsSceneView;
	
	/// handle mouse clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// handle mouse releasing
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	/// handle mouse movement for custom hover events
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	VariableInstanceGraphics* fromConnectorItem;
	VariableInstanceGraphics* toConnectorItem;
	VariableInstanceGraphics* currentConnectorItem;
	Util::Array<Ptr<NodeGraphics>> nodes;
	Util::Array<Ptr<LinkGraphics>> links;
    Util::Array<Ptr<NodeGraphics>> interactiveNodes;

	Ptr<LinkGraphics> linkUtilGraphics;
    Ptr<NodeGraphics> focusedNode;
	Ptr<NodeScene> nodeScene;
    QTimer interactiveTimer;

}; 
} // namespace Nody
//------------------------------------------------------------------------------