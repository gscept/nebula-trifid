#pragma once
//------------------------------------------------------------------------------
/**
	@class LevelEditor2::LightProbeManager
	
	Keeps track of all lightprobes in the scene, configures how they are supposed to be output, and to where the output is to be generated.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "ui_lightprobedialog.h"
#include "frame/frameshader.h"
#include "graphics/view.h"
#include "environmentprobecapturer.h"
#include <QDialog>
namespace LevelEditor2
{
class LightProbeManager : 
	public QObject,
	public Core::RefCounted
						  
{
	Q_OBJECT
	__DeclareSingleton(LightProbeManager);
	__DeclareClass(LightProbeManager);
	
public:
	/// constructor
	LightProbeManager();
	/// destructor
	virtual ~LightProbeManager();

	/// open manager, must be done before it can be used
	void Open();
	/// close manager, should be done to deallocate any remaining resources
	void Close();

	/// begin rendering probes, will attach camera to view
	void BeginProbeUpdate();
	/// end rendering probes, will detach camera from view
	void EndProbeUpdate();

	/// set building of mipmaps
	void SetBuildMipmaps(bool b);
	/// set building of irradiance maps
	void SetBuildIrradianceMaps(bool b);

	/// register light probe
	void RegisterProbe(const Ptr<EnvironmentProbeCapturer>& probe);
	/// unregister light probe
	void UnregisterProbe(const Ptr<EnvironmentProbeCapturer>& probe);

	/// get frame shader which should be used when calculating the reflection
	const Ptr<Frame::FrameShader>& GetReflectionFrameShader() const;
	/// get view which should be used when rendering from the probes point of view
	const Ptr<Graphics::View>& GetReflectionView() const;


public slots:
	/// show the window
	void Show();
	/// hide the window
	void Hide();

	/// create new light probe
	void CreateLightProbe();

	/// build lights
	void Build();
private:
	Util::Array<Ptr<EnvironmentProbeCapturer>> lightProbes;
	IO::URI outputFolder;
	Util::String naming;
	bool isOpen;
	Ui::LightProbeWindow ui;
	QDialog dialog;

	Ptr<Frame::FrameShader> lightProbeFrameShader;
	Ptr<Graphics::View> reflectionView;
	Ptr<Graphics::CameraEntity> reflectionCamera;
	bool inBeginUpdate;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Frame::FrameShader>&
LightProbeManager::GetReflectionFrameShader() const
{
	return this->lightProbeFrameShader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::View>&
LightProbeManager::GetReflectionView() const
{
	return this->reflectionView;
}


} // namespace LevelEditor2