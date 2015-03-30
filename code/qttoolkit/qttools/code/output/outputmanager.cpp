#include "stdneb.h"
#include "outputmanager.h"


namespace QtTools
{

OutputManager* OutputManager::instance = 0;

//------------------------------------------------------------------------------
/**
*/
OutputManager::OutputManager() : 
	isBlocking(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OutputManager::~OutputManager()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OutputManager* 
OutputManager::Instance()
{
	if (instance == 0)
	{
		instance = new OutputManager();
	}
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::AddOutputStream( OutputStream* stream )
{
	Q_ASSERT(stream);
	this->streams.append(stream);
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::Write( const QString& string )
{
	if (!this->isBlocking)
	{
		for (int i = 0; i < this->streams.size(); i++)
		{
			this->streams[i]->Write(string);
		}
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::WriteWarning( const QString& string )
{
	if (!this->isBlocking)
	{
		for (int i = 0; i < this->streams.size(); i++)
		{
			this->streams[i]->WriteWarning(string);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::WriteError( const QString& string )
{
	if (!this->isBlocking)
	{
		for (int i = 0; i < this->streams.size(); i++)
		{
			this->streams[i]->WriteError(string);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::WriteSpecific( const QString& string, OutputStream* stream )
{
	if (!this->isBlocking)
	{
		if (this->streams.contains(stream))
		{
			stream->Write(string);
		}
		else
		{
			qErrnoWarning("Output stream is not registered!");
		}		
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::WriteSpecificWarning( const QString& string, OutputStream* stream )
{
	if (!this->isBlocking)
	{
		if (this->streams.contains(stream))
		{
			stream->WriteWarning(string);
		}
		else
		{
			qErrnoWarning("Output stream is not registered!");
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::WriteSpecificError( const QString& string, OutputStream* stream )
{
	if (!this->isBlocking)
	{
		if (this->streams.contains(stream))
		{
			stream->WriteError(string);
		}
		else
		{
			qErrnoWarning("Output stream is not registered!");
		}	
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::Clear()
{
	for (int i = 0; i < this->streams.size(); i++)
	{
		this->streams[i]->Clear();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::ClearSpecific( OutputStream* stream )
{
	if (this->streams.contains(stream))
	{
		stream->Clear();
	}
	else
	{
		qErrnoWarning("Output stream is not registered!");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::StartBlock()
{
	Q_ASSERT(!this->isBlocking);
	this->isBlocking = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputManager::StopBlock()
{
	Q_ASSERT(this->isBlocking);
	this->isBlocking = false;
}
}