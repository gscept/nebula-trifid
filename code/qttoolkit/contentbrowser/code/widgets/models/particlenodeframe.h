#pragma once
//------------------------------------------------------------------------------
/**
    @class Particles::ParticleNodeFrame
    
    Implements a node frame which is to be used for particle nodes explicitly.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QFrame>
#include "particlenodehandler.h"
#include "ui_particlenodeinfowidget.h"
namespace Widgets
{
class ParticleNodeFrame : public QFrame
{
public:
	/// constructor
	ParticleNodeFrame();
	/// destructor
	virtual ~ParticleNodeFrame();

	/// returns pointer to handler
	const Ptr<ParticleNodeHandler>& GetHandler() const;

	/// discards particle node frame
	void Discard();
	/// refresh model node frames
	void Refresh();

private:
	Ptr<ParticleNodeHandler> itemHandler;
	Ui::ParticleNodeInfoWidget ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ParticleNodeHandler>& 
ParticleNodeFrame::GetHandler() const
{
	return this->itemHandler;
}
} // namespace Particles
//------------------------------------------------------------------------------