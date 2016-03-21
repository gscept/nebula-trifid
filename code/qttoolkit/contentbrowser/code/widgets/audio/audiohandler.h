#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::AudioItemHandler
    
    Handles audio items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
#include <QObject>
#include "ui_audioinfowidget.h"
#include "faudio/eventinstance.h"
namespace FMOD
{
	namespace Studio
	{
		class Bus;
	}
}
namespace Widgets
{
class AudioHandler : public BaseHandler
{
	Q_OBJECT
	__DeclareClass(AudioHandler);
public:
	/// constructor
	AudioHandler();
	/// destructor
	virtual ~AudioHandler();

	/// sets up handler
	virtual void Setup();

	/// discards handler
	virtual bool Discard();

	/// cleans up the handler
	virtual void Cleanup();

	/// sets the node name of the handler
	void SetName(const Util::String& name);
	/// gets the node name of the handler
	const Util::String& GetName() const;

	/// sets the ui on which this handler shall perform its actions
	void SetUI(Ui::AudioInfoWidget* ui);
	/// gets pointer to ui
	Ui::AudioInfoWidget* GetUI() const;

	/// loads info and puts it in the ui
	void LoadAudioInfo();

	private slots:
	/// Stops playing 
	void Stop();
	/// Pause
	void Pause();
	/// Play
	void Play();

private:
	Util::String path;
	Ptr<FAudio::EventInstance> eventInstance;
	Ui::AudioInfoWidget* ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
AudioHandler::SetName(const Util::String& resource)
{
	n_assert(resource.IsValid());
	this->path = resource;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
AudioHandler::GetName() const
{
	return this->path;
}


} // namespace ContentBrowser
//------------------------------------------------------------------------------