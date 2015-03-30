#include "take.h"
namespace Importer
{

	//------------------------------------------------------------------------------
	/**
	*/
	Take::Take()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	Take::~Take()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	Take::AddClip( Clip* clip )
	{
		this->clips.append(clip);
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	Take::RemoveClip( const uint index )
	{
		this->clips.removeAt(index);
	}

	//------------------------------------------------------------------------------
	/**
	*/
	Clip* 
	Take::GetClip( const uint index )
	{
		return this->clips[index];
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const QList<Clip*>& 
	Take::GetClips() const
	{
		return this->clips;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	Take::SetName( const QString& name )
	{
		this->name = name;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const QString& 
	Take::GetName() const
	{
		return this->name;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const uint 
	Take::GetNumClips()
	{
		return this->clips.size();
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const int 
	Take::FindClip( Clip* clip )
	{
		return this->clips.indexOf(clip);
	}
}