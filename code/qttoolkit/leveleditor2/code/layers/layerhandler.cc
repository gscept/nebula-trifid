//------------------------------------------------------------------------------
//  layerhandler.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "layerhandler.h"
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include "widgets/audiodialoghandler.h"
#include "entityutils/selectionutil.h"

namespace Layers
{
__ImplementClass(Layers::LayerHandler, 'LAHA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
LayerHandler::LayerHandler() :
	tableWidget(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
LayerHandler::~LayerHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::Setup()
{
	n_assert(this->tableWidget != NULL);
	this->baseLayer = Layer::Create();
	baseLayer->SetName("Default");
	baseLayer->SetAutoLoad(true);
	baseLayer->SetVisible(true);
	baseLayer->SetLocked(false);

	this->AddRow(baseLayer);
	this->layers.Add("Default", baseLayer);

	this->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
	connect(this->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnCellDoubleClicked(int, int)));
	connect(this->ui->newLayerAndAddEntitiesButton, SIGNAL(pressed()), this, SLOT(OnMoveSelectionToNewLayerClicked()));
	connect(this->ui->newLayerButton, SIGNAL(pressed()), this, SLOT(OnNewLayerClicked()));

	connect(this->ui->actionDelete, SIGNAL(triggered()), this, SLOT(OnDeletePressed()));
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::Discard()
{
	this->tableWidget->setRowCount(0);
	this->entityToLayer.Clear();
	this->uiToLayers.Clear();
	this->layerToRow.Clear();
	this->layers.Clear();
	disconnect(this->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
	disconnect(this->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnCellDoubleClicked(int, int)));
	disconnect(this->ui->newLayerAndAddEntitiesButton, SIGNAL(pressed()), this, SLOT(OnMoveSelectionToNewLayerClicked()));
	disconnect(this->ui->newLayerButton, SIGNAL(pressed()), this, SLOT(OnNewLayerClicked()));
	disconnect(this->ui->actionDelete, SIGNAL(triggered()), this, SLOT(OnDeletePressed()));
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnContextMenu(const QPoint& pos)
{
	QPoint localPos = this->tableWidget->mapToGlobal(pos);
	QModelIndex index = this->tableWidget->indexAt(pos);
	if (index.row() > -1)
	{
		QMenu menu;
		QAction* moveToLayerAction = menu.addAction("Move selected entities to layer");
		QAction* removeLayerAction = menu.addAction("Remove layer");

		// > 0 so we cant remove the Default layer
		if (index.row() == 0) removeLayerAction->setDisabled(true);

		// execute menu
		QAction* selected = menu.exec(localPos);
		if (selected == moveToLayerAction)
		{
			QWidget* label = this->tableWidget->cellWidget(index.row(), 2);
			Ptr<Layer> layer = this->uiToLayers[label];

			// go through layers, remove them from their current layers, and add to the selected layer
			Util::Array<Ptr<Game::Entity>> entities = LevelEditor2::SelectionUtil::Instance()->GetSelectedEntities();

			// moves entities into layer
			this->MoveEntitiesToLayer(entities, layer);
		}
		else if (selected == removeLayerAction && index.row() > 0)
		{
			this->DeleteLayer(index.row());
		}
	}
	else
	{
		QMenu menu;
		QAction* newLayerAction = menu.addAction("New layer");

		// execute menu
		QAction* selected = menu.exec(localPos);
		if (selected == newLayerAction)
		{
			this->NewLayer();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::AddRow(const Ptr<Layer>& layer)
{
	this->tableWidget->insertRow(this->tableWidget->rowCount());

	QPushButton* showButton = new QPushButton;
	showButton->setCheckable(true);
	showButton->setChecked(layer->GetVisible());
	showButton->setText("Show");
	showButton->setFixedSize(QSize(70, 20));
	showButton->setStyleSheet("QPushButton::checked, QPushButton::pressed{background-color: rgb(0, 128, 0);} QPushButton{background-color:rgb(128, 0, 0);}");
	if (showButton->isChecked()) showButton->setText("Showing");
	else						 showButton->setText("Hiding");

	QPushButton* loadButton = new QPushButton;
	loadButton->setCheckable(true);
	loadButton->setChecked(layer->GetAutoLoad());
	loadButton->setText("Load");
	loadButton->setFixedSize(QSize(70, 20));
	loadButton->setStyleSheet("QPushButton::checked, QPushButton::pressed{background-color: rgb(0, 128, 0);} QPushButton{background-color:rgb(128, 0, 0);}");
	if (loadButton->isChecked()) loadButton->setText("Loads");
	else						 loadButton->setText("Ignored");

	QPushButton* lockButton = new QPushButton;
	lockButton->setCheckable(true);
	lockButton->setChecked(layer->GetLocked());
	lockButton->setText("Locked");
	lockButton->setFixedSize(QSize(70, 20));
	lockButton->setStyleSheet("QPushButton::checked, QPushButton::pressed{background-color: rgb(0, 128, 0);} QPushButton{background-color:rgb(128, 0, 0);}");
	if (lockButton->isChecked()) lockButton->setText("Locked");
	else						 lockButton->setText("Unlocked");

	QLabel* nameLabel = new QLabel;
	nameLabel->setAlignment(Qt::AlignCenter);
	nameLabel->setText(layer->GetName().AsCharPtr());

	// add to layers
	this->uiToLayers.Add(nameLabel, layer);
	this->uiToLayers.Add(showButton, layer);
	this->uiToLayers.Add(loadButton, layer);
	this->uiToLayers.Add(lockButton, layer);

	// column 0: visible
	// column 1: autoload
	// column 2: locking
	// column 3: name
	int row = this->tableWidget->rowCount() - 1;
	this->tableWidget->setCellWidget(row, 0, showButton);
	this->tableWidget->setCellWidget(row, 1, loadButton);
	this->tableWidget->setCellWidget(row, 2, lockButton);
	this->tableWidget->setCellWidget(row, 3, nameLabel);
	this->layerToRow.Add(layer, row);

	// connect signals
	connect(showButton, SIGNAL(pressed()), this, SLOT(OnShowClicked()));
	connect(loadButton, SIGNAL(pressed()), this, SLOT(OnLoadClicked()));
	connect(lockButton, SIGNAL(pressed()), this, SLOT(OnLockClicked()));
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Layer>
LayerHandler::NewLayer()
{
	Ptr<Layer> layer = Layer::Create();

	// find name
	Util::String name = "New layer";
	IndexT uniqueIndex = 0;
	while (this->layers.FindIndex(name) != InvalidIndex) name = "New layer " + Util::String::FromInt(uniqueIndex++);

	layer->SetName(name);
	layer->SetVisible(true);
	layer->SetAutoLoad(true);
	layer->SetLocked(false);
	this->layers.Add(name, layer);
	this->AddRow(layer);
	return layer;
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::DeleteLayer(int row)
{
	QWidget* visible = this->tableWidget->cellWidget(row, 0);
	QWidget* autoload = this->tableWidget->cellWidget(row, 1);
	QWidget* locked = this->tableWidget->cellWidget(row, 2);
	QWidget* label = this->tableWidget->cellWidget(row, 3);
	Ptr<Layer> layer = this->uiToLayers[label];
	this->RemoveRow(row);
	this->uiToLayers.Erase(visible);
	this->uiToLayers.Erase(autoload);
	this->uiToLayers.Erase(locked);
	this->uiToLayers.Erase(label);
	this->layers.Erase(layer->GetName());
	this->layerToRow.Erase(layer);

	// also update our entityTolayer dictionary
	IndexT i;
	for (i = 0; i < layer->entities.Size(); i++)
	{
		const Ptr<Game::Entity>& entity = layer->entities[i];
		this->entityToLayer.Erase(entity);
		this->entityToLayer.Add(entity, this->baseLayer);
	}

	// migrate all elements of this layer to the base layer
	layer->MoveToLayer(this->baseLayer);
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::RemoveRow(int row)
{
	this->tableWidget->removeRow(row);
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::MoveEntitiesToLayer(const Util::Array<Ptr<Game::Entity>>& entities, const Ptr<Layer>& layer)
{
	IndexT i;
	for (i = 0; i < entities.Size(); i++)
	{
		const Ptr<Game::Entity>& entity = entities[i];
		if (this->entityToLayer.Contains(entity))
		{
			// remove from layer and erase from table
			this->entityToLayer[entity]->Remove(entity);
			this->entityToLayer.Erase(entity);
		}

		// add to layer
		this->entityToLayer.Add(entity, layer);
		layer->Add(entity);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnShowClicked()
{
	QPushButton* showButton = (QPushButton*)this->sender();
	Ptr<Layer> layer = this->uiToLayers[showButton];

	// set visibility in layer
	layer->SetVisible(!showButton->isChecked());
	if (!showButton->isChecked()) showButton->setText("Showing");
	else						  showButton->setText("Hiding");
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnLoadClicked()
{
	QPushButton* loadButton = (QPushButton*)this->sender();
	Ptr<Layer> layer = this->uiToLayers[loadButton];

	// set visibility in layer
	layer->SetAutoLoad(!loadButton->isChecked());
	if (!loadButton->isChecked()) loadButton->setText("Loads");
	else						  loadButton->setText("Ignored");
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnLockClicked()
{
	QPushButton* lockButton = (QPushButton*)this->sender();
	Ptr<Layer> layer = this->uiToLayers[lockButton];

	// set locked status in layer
	layer->SetLocked(!lockButton->isChecked());
	if (!lockButton->isChecked()) lockButton->setText("Locked");
	else						  lockButton->setText("Unlocked");
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnNewLayerClicked()
{
	// simple...
	this->NewLayer();
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnMoveSelectionToNewLayerClicked()
{
	const Util::Array<Ptr<Game::Entity>> entities = LevelEditor2::SelectionUtil::Instance()->GetSelectedEntities();
	if (entities.Size() > 0)
	{
		const Ptr<Layer>& layer = this->NewLayer();
		this->MoveEntitiesToLayer(entities, layer);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnCellDoubleClicked(int row, int column)
{
	// only handle labels
	if (column == 3 && row > 0)
	{
		QLabel* label = (QLabel*)this->tableWidget->cellWidget(row, column);
		QString text = QInputDialog::getText(this->tableWidget, tr("Change layer name"), tr("Layer name:"), QLineEdit::Normal, label->text());

		// if we press cancel, do nothing
		if (!text.isEmpty())
		{
			// change layer
			Ptr<Layer> layer = this->uiToLayers[label];
			this->layers.Erase(layer->GetName());

			Util::String name(text.toUtf8().constData());
			if (name.ContainsCharFromSet(";"))
			{
				name.SubstituteChar(';', '_');
			}

			// make sure name is unique
			Util::String origName = name;
			IndexT num = 0;
			while (this->layers.Contains(name)) name.Format("%s_%d", origName.AsCharPtr(), num);
			label->setText(name.AsCharPtr());

			// set name of layer
			layer->SetName(name);
			this->layers.Add(name, layer);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::OnDeletePressed()
{
	QModelIndexList selectedRows = this->tableWidget->selectionModel()->selectedRows();
	if (selectedRows.size() > 0 && this->tableWidget->hasFocus())
	{
		int row = selectedRows[0].row();
		if (row > 0)
		{
			this->DeleteLayer(row);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::HandleEntityCreated(const Ptr<Game::Entity> & entity)
{
	Util::String activeLayer = entity->GetString(Attr::_Layers);
	
	Ptr<Layer> layer;
	if (this->layers.Contains(activeLayer))
	{
		layer = this->layers[activeLayer];
	}
	else
	{
		layer = this->baseLayer;
	}

	// add entity and update tables
	layer->Add(entity);
	this->entityToLayer.Add(entity, layer);	
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::HandleEntitySelected(const Ptr<Game::Entity>& entity)
{
	if (entity.isvalid())
	{
		if (this->entityToLayer.Contains(entity))
		{
			int row = this->layerToRow[this->entityToLayer[entity]];
			this->tableWidget->selectRow(row);
		}
	}
	else
	{
		this->tableWidget->clearSelection();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::HandleEntityDeleted(const Ptr<Game::Entity>& entity)
{
	if (this->entityToLayer.Contains(entity))
	{
		this->entityToLayer[entity]->Remove(entity);
		this->entityToLayer.Erase(entity);
	}	
}

//------------------------------------------------------------------------------
/**
*/
bool
LayerHandler::eventFilter(QObject* obj, QEvent* event)
{
	// basically handle events for focusing
	if (obj == this->tableWidget)
	{
		// if we lose focus to this widget we clear the selection so that we don't risk deleting the layer
		if (event->type() == QEvent::FocusOut)
		{
			this->tableWidget->clearSelection();
		}
	}
	else if (obj == this->tableWidget->viewport())
	{
		// if we press somewhere which isn't an item, we should also clear the selection
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouseEvent = (QMouseEvent*)event;
			if (this->tableWidget->rowAt(mouseEvent->pos().y()) == -1)
			{
				this->tableWidget->clearSelection();
			}
		}
	}

	return QObject::eventFilter(obj, event);
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Layer>>
LayerHandler::GetLayers()
{
	return this->layers.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
*/
void
LayerHandler::AddLayer(const Ptr<Layer>&layer)
{
	this->AddRow(layer);
	this->layers.Add(layer->GetName(), layer);
}

//------------------------------------------------------------------------------
/**
*/
const bool
LayerHandler::IsEntityInLockedLayer(const Ptr<Game::Entity>& entity) const
{
	if (this->entityToLayer.Contains(entity))
	{
		return this->entityToLayer[entity]->GetLocked();
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Layer>&
LayerHandler::GetEntityLayer(const Ptr<Game::Entity>& entity) const
{
	return this->entityToLayer[entity];
}

} // namespace Layers