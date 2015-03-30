#include "clipcontroller.h"
#include <QHeaderView>
#include <QTableView>
#include "importdatabase.h"
namespace Importer
{

//------------------------------------------------------------------------------
/**
*/ 
ClipController::ClipController( Ui::ImporterQtClass* ui ) : 
	currentBatch(0),
	currentTake(0)
{
	this->model = new ClipModel();
	this->delegate = new ClipDelegate(this);
	this->ui = ui;

	this->SetControllersEnabled(false);

	this->ui->clipView->setItemDelegate(this->delegate);
	this->ui->clipView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	this->ui->clipView->verticalHeader()->setDefaultSectionSize(32);
	this->ui->clipView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
	this->ui->clipView->setModel(this->model);

	connect(this->ui->addClip, SIGNAL(clicked()), this, SLOT(AddClip()));
	connect(this->ui->removeClip, SIGNAL(clicked()), this, SLOT(RemoveClip()));
	connect(this->ui->takes, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(TakeSelected(QListWidgetItem*)));

	connect(this->ui->clipView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(SetCurrentClip(const QModelIndex&)));
}

//------------------------------------------------------------------------------
/**
*/ 
ClipController::~ClipController()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::AddClip()
{
	Clip* newClip = new Clip;
	newClip->SetName("New clip");
	newClip->SetStart(0);
	newClip->SetEnd(0);
	newClip->SetPreInfinity(Clip::Constant);
	newClip->SetPostInfinity(Clip::Constant);

	this->currentTake->AddClip(newClip);

	if (this->currentTake->GetNumClips() == 1)
	{
		this->currentClip = newClip;
	}

	this->Refresh();

	emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::RemoveClip()
{
	int clipIndex = this->currentTake->FindClip(this->currentClip);
	if (clipIndex != -1)
	{
		delete this->currentTake->GetClip(clipIndex);
		this->currentTake->RemoveClip(clipIndex);

		int newCurrentIndex = clipIndex > (int)this->currentTake->GetNumClips()-1 ? clipIndex-1 : clipIndex;
		if (newCurrentIndex == -1)
		{
			this->currentClip = 0;
		}
		else
		{
			this->currentClip = this->currentTake->GetClip(newCurrentIndex);
		}

		this->Refresh();	

		emit Changed(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::Refresh()
{
	if (this->currentTake)
	{
		this->model->setRowCount(this->currentTake->GetNumClips());

		for (uint clipIndex = 0; clipIndex < this->currentTake->GetNumClips(); clipIndex++)
		{
			Clip* clip = this->currentTake->GetClip(clipIndex);
			this->model->setData(this->model->index(clipIndex, 0), clip->GetName());
			this->model->setData(this->model->index(clipIndex, 1), clip->GetStart());
			this->model->setData(this->model->index(clipIndex, 2), clip->GetEnd());
			this->model->setData(this->model->index(clipIndex, 3), clip->GetPreInfinity() == Clip::Constant ? "Constant" : "Cycle");
			this->model->setData(this->model->index(clipIndex, 4), clip->GetPostInfinity() == Clip::Constant ? "Constant" : "Cycle");
		}

		int clipIndex = this->currentTake->FindClip(this->currentClip);
		this->model->setData(this->model->index(clipIndex, 0), QPixmap(":/WhiteIcons/icons/white/png/rnd_br_next_icon_16.png"), Qt::DecorationRole);
	}
	else
	{
		this->model->setRowCount(0);
	}

}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetCurrentClip( const QModelIndex& index )
{
	int clipIndex = this->currentTake->FindClip(this->currentClip);
	this->model->setData(this->model->index(clipIndex, 0), 0, Qt::DecorationRole);

	currentClip = this->currentTake->GetClip(index.row());
	this->delegate->SetCurrentClip(currentClip);

	this->Refresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetReader( FbxReader* reader )
{
	this->reader = reader;
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::ReaderLoaded( int status, const QString& file, const QString& category )
{
	if (status == FbxReader::Success)
	{

		bool enableControllers = true;
		this->ui->takes->clear();
		const QList<QString> takes = this->reader->GetTakes();
		for (int takeIndex = 0; takeIndex < takes.size(); takeIndex++)
		{
			this->ui->takes->addItem(takes[takeIndex]);
		}

		if (ImportDatabase::Instance()->HasImportOptions(category + "/" + file))
		{
			ImportOptions* batch = ImportDatabase::Instance()->GetImportOptions(category + "/" + file);
			if (batch->GetTakes().isEmpty())
			{
				this->currentTake = 0;
				enableControllers = false;
			}
			else
			{
				this->currentTake = batch->GetTakes().first();
				this->ui->takes->setItemSelected(this->ui->takes->item(0), true);
			}
			this->currentBatch = batch;
		}
		else
		{
			ImportOptions* newBatch = new ImportOptions();
			newBatch->SetName(category + "/" + file);
			this->currentBatch = newBatch;
			for (int takeIndex = 0; takeIndex < takes.size(); takeIndex++)
			{
				Take* take = new Take();
				take->SetName(takes[takeIndex]);
				newBatch->AddTake(take);
			}
			if (takes.size() > 0)
			{
				this->currentTake = newBatch->GetTakes()[0];
				this->ui->takes->setItemSelected(this->ui->takes->item(0), true);
			}
			else
			{
				this->currentTake = 0;
				enableControllers = false;
			}
			ImportDatabase::Instance()->AddImportOptions(category + "/" + file, newBatch);
		}

		this->SetControllersEnabled(enableControllers);

		this->ui->takes->clear();
		for (int takeIndex = 0; takeIndex < this->currentBatch->GetTakes().size(); takeIndex++)
		{
			Take* take = this->currentBatch->GetTakes()[takeIndex];
			this->ui->takes->addItem(take->GetName());
		}
		

		this->Refresh();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::TakeSelected( QListWidgetItem* item )
{
	int clipIndex = this->currentTake->FindClip(this->currentClip);
	this->model->setData(this->model->index(clipIndex, 0), 0, Qt::DecorationRole);

	int selectedIndex = this->ui->takes->currentRow();
	this->currentTake = this->currentBatch->GetTakes()[selectedIndex];



	this->Refresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::EmitChanged()
{
	emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetControllersEnabled( bool state )
{
	this->ui->addClip->setEnabled(state);
	this->ui->removeClip->setEnabled(state);
}
}