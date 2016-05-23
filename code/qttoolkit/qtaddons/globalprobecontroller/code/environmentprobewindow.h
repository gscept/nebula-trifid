#pragma once
//------------------------------------------------------------------------------
/**
	@class Lighting::EnvironmentProbeWindow
	
	Handles the environment probe window used in the content browser.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QDialog>
#include "ui_environmentprobedialog.h"
#include "util/string.h"
namespace Lighting
{
class EnvironmentProbeWindow : public QDialog
{
	Q_OBJECT
public:
	/// constructor
	EnvironmentProbeWindow();
	/// destructor
	virtual ~EnvironmentProbeWindow();

    /// set the reflection map
    void SetReflectionMap(const Util::String & refl);
    /// set the irradiance map
    void SetIrradianceMap(const Util::String & irr);

protected:
    ///
    void showEvent(QShowEvent * event);

private slots:
	/// call when the reflection map is changed
	void OnReflectionChanged();
	/// call when the irradiance map is changed
	void OnIrradianceChanged();

	/// call when asked to browse reflection maps
	void OnBrowseReflection();
	/// call when asked to browse irradiance maps
	void OnBrowseIrradiance();

    ///
    void OnAccepted();
    ///
    void OnRejected();

private:
	Ui::EnvironmentProbeDialog ui;
};
} // namespace Lighting