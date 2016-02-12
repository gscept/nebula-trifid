#pragma once
//------------------------------------------------------------------------------
/**
    @class Particles::ParticleEnvelopeWidget
    
    A particle envelope widget implements all handles attached to each attribute.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QFrame>
#include "particles/envelopecurve.h"
#include "ui_particlenodesettingtemplate.h"
#include "particles/emitterattrs.h"
namespace Particles
{
class ParticleEnvelopeWidget : public QFrame
{
	Q_OBJECT
public:
	/// constructor
	ParticleEnvelopeWidget(QWidget* parent);
	/// destructor
	virtual ~ParticleEnvelopeWidget();

	/// sets up ui from curve
	void Setup(const EnvelopeCurve& curve, EmitterAttrs::EnvelopeAttr attr);

	/// get internal envelope curve
	const EnvelopeCurve& GetEnvelope() const;

signals:
	/// emit this when a value has changed
	void ValueChanged(Particles::EmitterAttrs::EnvelopeAttr);

private slots:
	/// called when a point value is changed
	void PointValueChanged();
	/// called whenever a point value has been updated
	void PointValueUpdated(QPointF point);
	/// called whenever a point is selected
	void PointSelected(int point);
	/// called whenever the max value is changed
	void MaxValueChanged(double max);
	/// called whenever the min value is changed
	void MinValueChanged(double min);

	/// called whenever the frequency has changed
	void FrequencyChanged(double freq);
	/// called whenever the amplitude has changed
	void AmplitudeChanged(double amp);
	/// called whenever the function is changed
	void FunctionChanged(int func);

	/// updates point positions from data
	void UpdatePoints();

private:
	float xVal, yVal;
	Ui::ParticleNodeSettingsFrame ui;
	EnvelopeCurve curve;
	EmitterAttrs::EnvelopeAttr attr;

	int activePoint;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const EnvelopeCurve& 
ParticleEnvelopeWidget::GetEnvelope() const
{
	return this->curve;
}

} // namespace Particles
//------------------------------------------------------------------------------