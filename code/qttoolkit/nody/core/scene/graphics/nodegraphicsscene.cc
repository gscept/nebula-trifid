//------------------------------------------------------------------------------
//  nodegraphicsscene.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nody_config.h"
#include "nodegraphicsscene.h"
#include "scene/nodescene.h"
#include "node/graphics/nodegraphics.h"
#include "node/graphics/nodeitemgroup.h"
#include "link/graphics/linkgraphics.h"	
#include "variable/graphics/variableinstancegraphics.h"
#include <QGraphicsSceneMouseEvent>
#include "framesync/framesynctimer.h"
#include "project/project.h"
#include "nodegraphicssceneview.h"

namespace Nody
{
__ImplementClass(Nody::NodeGraphicsScene, 'NDGS', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsScene::NodeGraphicsScene() :
	fromConnectorItem(0),
	toConnectorItem(0),
	currentConnectorItem(0),
	linkUtilGraphics(0),
    focusedNode(0),
    nodeScene(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsScene::~NodeGraphicsScene()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::Setup()
{
	n_assert(!this->linkUtilGraphics.isvalid());

	// create link util, generate graphics and add it to the scene
	this->linkUtilGraphics = LinkGraphics::Create();
    this->linkUtilGraphics->SetLinkCurveType(LinkGraphics::Linear);
	this->linkUtilGraphics->Generate();
	this->linkUtilGraphics->AddToScene(this);
    this->interactiveTimer.setInterval(10);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::Discard()
{
	n_assert(this->linkUtilGraphics.isvalid());
    this->interactiveTimer.stop();
	this->linkUtilGraphics->RemoveFromScene(this);
	this->linkUtilGraphics->Destroy();
	this->linkUtilGraphics = 0;

	this->nodeScene = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::SetInteractiveMode(bool enabled)
{
    if (enabled)
    {
        connect(&this->interactiveTimer, SIGNAL(timeout()), this, SLOT(OnTick()));
        this->interactiveTimer.start();
    }
    else
    {
        disconnect(&this->interactiveTimer, SIGNAL(timeout()), this, SLOT(OnTick()));
        this->interactiveTimer.stop();
    }
    
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::SetNodeScene(const Ptr<NodeScene>& scene)
{
	n_assert(scene.isvalid());
	this->nodeScene = scene;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<NodeScene>& 
NodeGraphicsScene::GetNodeScene() const
{
	return this->nodeScene;
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::AddNodeGraphics(const Ptr<NodeGraphics>& node)
{
	// add node to scene
	node->AddToScene(this);

    const Ptr<Node>& origNode = node->node;
    if (!origNode->IsSuperNode() && origNode->GetVariation()->GetOriginalVariation()->IsSimulationInteractive())
    {
        this->interactiveNodes.Append(node);
    }
    
    // also move this node to front
    this->MoveToFront(node);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::RemoveNodeGraphics(const Ptr<NodeGraphics>& node)
{
	// remove from scene
	node->RemoveFromScene(this);

    IndexT i = this->interactiveNodes.FindIndex(node);
    if (i != InvalidIndex) this->interactiveNodes.EraseIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::AddLinkGraphics(const Ptr<LinkGraphics>& link)
{
	// add link to scene
	link->AddToScene(this);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::RemoveLinkGraphics(const Ptr<LinkGraphics>& link)
{
	// remove link from scene
	link->RemoveFromScene(this);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::MoveToFront(const Ptr<NodeGraphics>& node)
{
	/*
	const QList<QGraphicsItem*> items = this->items();
	IndexT i;
	for (i = 0; i < items.size(); i++)
	{
		QGraphicsItem* item = items[i];
		if (dynamic_cast<QGraphicsItemGroup*>(item))
		{
			int newZ = item->zValue() + 1;
			node->group->setZValue(newZ);
			break;
		}
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::OnScale(Math::scalar zoom)
{

}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (0 == this->currentConnectorItem)
    {
        // handle base class event first
        QGraphicsScene::mousePressEvent(event);
    }	

	// causes connectors to be highlighted if hovered
	QList<QGraphicsItem*> items = this->items(event->scenePos());
	NodeGraphics* topMostItem = 0;
	int highestZ = -1;
	if (items.size() > 0)
	{
		IndexT i;
		for (i = 0; i < items.size(); i++)
		{
			QGraphicsItem* item = items[i];

			// we store our reference to our original object in the data location 0 interpreted as an integer
			const QVariant& variant = item->data(NEBULAUSERPOINTERLOCATION);
			if (variant.isValid())
			{
				Core::RefCounted* obj = 0;
				qint32 ptr = variant.toInt();
				if (n_convertinttotype<VariableInstanceGraphics>(ptr, &obj))
				{
					VariableInstanceGraphics* varInst = (VariableInstanceGraphics*)obj;
					if (varInst->GetVariableInstance()->GetOriginalVariable()->GetIOFlag() == Variable::Input)
					{
						this->toConnectorItem = varInst;
						if (this->toConnectorItem->IsLocked())
						{
							// get variable instance
							const Ptr<VariableInstance>& varInst = this->toConnectorItem->GetVariableInstance();

							// set anchor to be where the link used to originate from
							this->linkUtilGraphics->SetAnchorFrom(varInst->GetInLink()->GetGraphics()->GetAnchorFrom());
							this->linkUtilGraphics->SetAnchorTo(event->scenePos());

							// remove the old link
							this->fromConnectorItem = varInst->GetInLink()->GetFromVariable()->GetGraphics();
							Ptr<Link> link = this->toConnectorItem->GetVariableInstance()->GetInLink();
							this->GetNodeScene()->RemoveLink(link);

                            // set util to be visible since we are relinking
                            this->linkUtilGraphics->SetVisible(true);

						}
					}
					else
					{
						this->fromConnectorItem = varInst;
						this->fromConnectorItem->Highlight();

						// set link and update
						this->linkUtilGraphics->SetAnchorFrom(this->fromConnectorItem->GetCenter());
						this->linkUtilGraphics->SetAnchorTo(event->scenePos());
                        this->linkUtilGraphics->SetVisible(true);
					}

					// set link to be visible
					this->linkUtilGraphics->Update();
				}
                else if (n_convertinttotype<NodeGraphics>(ptr, &obj))
                {
                    if (highestZ < item->zValue())
                    {
                        highestZ = item->zValue();
                        topMostItem = (NodeGraphics*)obj;
                    }
                }
			}
		}

		// if we clicked a new node, make it focused and move it to the top of the stack
		if (0 != topMostItem)
		{
	        if (this->focusedNode.isvalid()) this->focusedNode->SetFocus(false);
		    this->focusedNode = topMostItem;
			this->focusedNode->SetFocus(true);
			this->MoveToFront(this->focusedNode);
			emit NodeFocused(this->focusedNode->node);
        }
	}
    else
    {
        // drop the focus
        this->DropFocus();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// handle base class event first
	QGraphicsScene::mouseReleaseEvent(event);

	// before we remove the highlight we should test if we created a valid link
	if (this->fromConnectorItem && this->currentConnectorItem)
	{
		// get variable instances
		const Ptr<VariableInstance>& fromVar = this->fromConnectorItem->GetVariableInstance();
		const Ptr<VariableInstance>& toVar = this->currentConnectorItem->GetVariableInstance();

        // remove hightlight from variables
        this->fromConnectorItem->UnHighlight();
        this->currentConnectorItem->UnHighlight();

		// attempt to create link between variables
		this->GetNodeScene()->CreateLink(fromVar, toVar);

		// hide link util
		this->linkUtilGraphics->SetVisible(false);
        this->fromConnectorItem = 0;
        this->currentConnectorItem = 0;
	}

	// if we release the mouse button, we should remove the highlight whatever we are currently highlighting
	if (this->fromConnectorItem)
	{
		// remove highlight for connector, trigger a link-from event
		this->views()[0]->setDragMode(QGraphicsView::RubberBandDrag);
		this->fromConnectorItem->UnHighlight();
		this->fromConnectorItem = 0;

		// hide link util
		this->linkUtilGraphics->SetVisible(false);
	}

	// save center of camera
	NodeGraphicsSceneView* view = static_cast<NodeGraphicsSceneView*>(this->views()[0]);
	const QPoint& viewCenter = view->GetCenter();
	Project::Instance()->globalState.viewCenter = Math::float2(viewCenter.x(), viewCenter.y());
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	// handle base class event first
	QGraphicsScene::mouseMoveEvent(event);

	// causes connectors to be highlighted if hovered
	QList<QGraphicsItem*> items = this->items(event->scenePos());
	if (items.size() > 0)
	{
		IndexT i;
		for (i = 0; i < items.size(); i++)
		{
			QGraphicsItem* item = items[i];

			// we store our reference to our original object in the data location 0 interpreted as an integer
			const QVariant& variant = item->data(NEBULAUSERPOINTERLOCATION);
			if (variant.isValid())
			{
				// reinterpret to RefCounted* and then dynamic cast to wanted class
				qint32 ptr = variant.toInt();
                Core::RefCounted* connectorItem = 0;
				if (n_convertinttotype<VariableInstanceGraphics>(ptr, &connectorItem))
				{
					this->currentConnectorItem = (VariableInstanceGraphics*)connectorItem;
					this->views()[0]->setDragMode(QGraphicsView::NoDrag);
					this->currentConnectorItem->Highlight();
					break;
				}
			}
			else if (this->currentConnectorItem && this->currentConnectorItem != this->fromConnectorItem)
			{
				this->currentConnectorItem->UnHighlight();
				this->currentConnectorItem = 0;
			}
		}
	}
	else if (this->currentConnectorItem && this->currentConnectorItem != this->fromConnectorItem)
	{
		this->currentConnectorItem->UnHighlight();
		this->currentConnectorItem = 0;
	}
	
	if (this->fromConnectorItem)
	{
		// set link and update link
		this->linkUtilGraphics->SetAnchorTo(event->scenePos());
		this->linkUtilGraphics->Update();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::DeleteCurrent()
{
    // only allow this to happen if 
    if (!(this->focusedNode->node->GetFlags() & Node::NoDelete))
    {
		if (this->focusedNode.isvalid()) this->nodeScene->RemoveNode(this->focusedNode->node);
	    this->focusedNode = 0;
        emit NodeFocused(NULL);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::OnTick()
{
    // update time
    FrameSync::FrameSyncTimer::Instance()->UpdateTimePolling();
    IndexT i;
    for (i = 0; i < this->interactiveNodes.Size(); i++)
    {
		this->interactiveNodes[i]->node->Simulate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsScene::DropFocus()
{
    if (this->focusedNode.isvalid()) this->focusedNode->SetFocus(false, false);
    this->focusedNode = 0;    
    emit NodeFocused(NULL);
}

} // namespace Nody
