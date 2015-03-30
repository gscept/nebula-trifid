#include <QProgressBar>
#include <QLabel>
#include <QProcess>
#include <QDataStream>
#include <QtNetwork/QUdpSocket>

namespace QtTools
{
class ProgressNotifier : public QObject
{
	Q_OBJECT
public:


	/// returns singleton instance
	static ProgressNotifier* Instance();

	/// sets the progress bar to be notified
	void SetProgressBar(QProgressBar* bar);
	/// sets the status label
	void SetStatusLabel(QLabel* label);

	/// sets the UI visible and sets progress to 0
	void Start(const QString& status = "");
	/// updates the progress bar
	void Update(float value, const QString& status);
	/// increments the progress bar
	void Increment(float increment, const QString& status = "");
	/// hides the ui
	void End();

private:
	/// constructor
	ProgressNotifier();
	/// destructor
	~ProgressNotifier();

	QUdpSocket remoteSocket;
	QProgressBar* bar;
	QLabel* status;
	static ProgressNotifier* instance;

};
}