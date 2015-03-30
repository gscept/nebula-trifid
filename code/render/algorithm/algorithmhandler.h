#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::AlgorithmHandler
    
    Message handler for all algorithm-related messages.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "interface/interfacehandlerbase.h"
namespace Algorithm
{
class AlgorithmBase;
class AlgorithmHandler : public Interface::InterfaceHandlerBase
{
	__DeclareClass(AlgorithmHandler);
public:
	/// constructor
	AlgorithmHandler();
	/// destructor
	virtual ~AlgorithmHandler();

	/// sets algorithm
	void SetAlgorithm(const Ptr<AlgorithmBase>& algorithm);
	/// get algorithm
	const Ptr<AlgorithmBase>& GetAlgorithm() const;

	/// handle a message, return true if handled
	virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

private:
	Ptr<AlgorithmBase> algorithm;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
AlgorithmHandler::SetAlgorithm( const Ptr<AlgorithmBase>& algorithm )
{
	this->algorithm = algorithm;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<AlgorithmBase>& 
AlgorithmHandler::GetAlgorithm() const
{
	return this->algorithm;
}

} // namespace Algorithm
//------------------------------------------------------------------------------