#pragma once
//------------------------------------------------------------------------------
/**
    @class Tools::BDApplication

    (C) 2006 Radon Labs GmbH
*/

#include "LinearMath/btIDebugDraw.h"

class DebugDrawer : public btIDebugDraw
{
public:

	DebugDrawer();

	virtual ~DebugDrawer();

	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

	virtual void	reportErrorWarning(const char* warningString);

	virtual void	draw3dText(const btVector3& location,const char* textString);	

	virtual void	setDebugMode(int debugMode);	
	virtual int		getDebugMode() const;

private:
    int m_debugMode;
};

//------------------------------------------------------------------------------
/**
*/
inline void	
DebugDrawer::setDebugMode(int debugMode)
{
    this->m_debugMode = debugMode;
}

//------------------------------------------------------------------------------
/**
*/
inline int		
DebugDrawer::getDebugMode() const
{
    return this->m_debugMode;
}
