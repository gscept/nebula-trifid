#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::BillboardNode
    
     representation of a billboard shape. 
	It's almost exactly like a StateNode with the exception of being instanced as a BillboardNodeInstance.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "models/nodes/statenode.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/vertexlayout.h"
#include "coregraphics/shaderinstance.h"
namespace Models
{
class BillboardNode : public StateNode
{
	__DeclareClass(BillboardNode);
public:
	/// constructor
	BillboardNode();
	/// destructor
	virtual ~BillboardNode();

	/// create a model node instance
	virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;

}; 

} // namespace Models
//------------------------------------------------------------------------------