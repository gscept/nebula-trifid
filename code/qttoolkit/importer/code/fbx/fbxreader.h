#pragma once
#include <fbxsdk.h>
#include <QString>
#include <QObject>

namespace Importer
{
	class FbxReader : public QObject
	{
		Q_OBJECT

	public:

		enum LoadStatus
		{
			Success,
			Failed
		};

		/// constructor
		FbxReader();
		/// destructor
		~FbxReader();

		/// opens a scene
		void Open(const QString& file, const QString& category);
		/// closes reader (and scene)
		void Close();
		/// returns true if reader is open
		bool IsOpen() const;
		/// sets the parent widget
		void SetParent(QWidget* parent);

		/// gets list of takes from loaded FBX
		const QList<QString> GetTakes();
		/// gets file information from loaded FBX
		const QString GetInfo();

		/// gets the scene pointer
		KFbxScene* GetScene();
	signals:
		/// emitted when scene is loaded
		void Loaded(int status, const QString& file, const QString& category);
	private:
		/// gets a list of joints from root node
		void GetJointsRecursive(QList<QString>& joints, KFbxNode* parent);

		bool isOpen;
		QString file;
		QString category;
		QWidget* parentWidget;
		KFbxSdkManager* sdkManager;
		KFbxIOSettings* ioSettings;
		KFbxScene* scene;
	};
}