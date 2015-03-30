#pragma once
#include <QList>
#include "clip.h"
namespace Importer
{
	class Take
	{
	public:
		/// constructor
		Take();
		/// destructor
		~Take();

		/// adds a clip to the take
		void AddClip(Clip* clip);
		/// removes clip from the take
		void RemoveClip(const uint index);
		/// gets clip from the take
		Clip* GetClip(const uint index);
		/// gets all clips from the take
		const QList<Clip*>& GetClips() const;
		/// gets the number of clips
		const uint GetNumClips();
		/// gets the index of a clip
		const int FindClip(Clip* clip);

		/// sets the take name
		void SetName(const QString& name);
		/// gets the take name
		const QString& GetName() const;
	private:
		QString name;
		QList<Clip*> clips;
	};
}