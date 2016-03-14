#pragma once
//------------------------------------------------------------------------------
/**
	@class Layers::LayerHandler
	
	The LayerHandler manages the layer widget in the level editor, and is also responsible for moving entities between layers.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "ui_leveleditor2window.h"
#include "layer.h"
#include <QTableWidget>
#include <QObject>

namespace LevelEditor2
{
class Level;
}

namespace Layers
{
class LayerHandler : 
	public QObject,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(LayerHandler);	
public:
	/// constructor
	LayerHandler();
	/// destructor
	virtual ~LayerHandler();

	/// set ui
	void SetUI(Ui::LevelEditor2Window* ui);

	/// setup from current level
	void Setup();
	/// discard, clearing list of layers so that we can load it again, but retains the pointer to the widget
	void Discard();

	/// react to when an entity is loaded and should be added to a layer, or create a layer and add itself to it
	void HandleEntityCreated(const Ptr<Game::Entity>& entity);
	/// react to an entity being clicked
	void HandleEntitySelected(const Ptr<Game::Entity>& entity);
	/// react to an entity being deleted
	void HandleEntityDeleted(const Ptr<Game::Entity>& entity);

	/// returns true if entity is in a layer which is locked
	const bool IsEntityInLockedLayer(const Ptr<Game::Entity>& entity) const;
	/// get layer an entity is in
	const Ptr<Layer>& GetEntityLayer(const Ptr<Game::Entity>& entity) const;

private slots:
	/// handle right clicking in the widget
	void OnContextMenu(const QPoint& pos);

	/// handle clicking on show
	void OnShowClicked();
	/// handle clicking on load
	void OnLoadClicked();
	/// handle clicking on lock
	void OnLockClicked();
	/// handle clicking on new layer
	void OnNewLayerClicked();
	/// handle clicking on move selection to new layer
	void OnMoveSelectionToNewLayerClicked();
	/// handle changing the name of a label
	void OnCellDoubleClicked(int row, int column);
	/// handle presseing delete
	void OnDeletePressed();
private:
	friend class LevelEditor2::Level;

	/// setup table row from layer
	void AddRow(const Ptr<Layer>& layer);
	/// remove row
	void RemoveRow(int row);

	/// make new layer, returns layer
	Ptr<Layer> NewLayer();
	/// remove layer at index
	void DeleteLayer(int row);
	/// add layer
	void AddLayer(const Ptr<Layer>&layer);	
	/// retrieve a layer
	const Ptr<Layer>& GetLayer(const Util::String & layer);
	/// has layer
	bool HasLayer(const Util::String & layer);
	/// get all layers
	Util::Array<Ptr<Layer>> GetLayers();

	/// handle special events from the table widget
	bool eventFilter(QObject* obj, QEvent* event);

	/// assign entities to layer
	void MoveEntitiesToLayer(const Util::Array<Ptr<Game::Entity>>& entities, const Ptr<Layer>& layer);

	Ptr<Layer> baseLayer;
	Ui::LevelEditor2Window* ui;
	QTableWidget* tableWidget;
	Util::Dictionary<QWidget*, Ptr<Layer>> uiToLayers;
	//il::Dictionary<QPushButton*, int> uiToRows;
	Util::Dictionary<Util::String, Ptr<Layer>> layers;
	Util::Dictionary<Ptr<Game::Entity>, Ptr<Layer>> entityToLayer;
	Util::Dictionary<Ptr<Layer>, int> layerToRow;
};

//------------------------------------------------------------------------------
/**
*/
inline void
LayerHandler::SetUI(Ui::LevelEditor2Window* ui)
{
	this->ui = ui;
	this->tableWidget = ui->layerWidget;
	this->tableWidget->installEventFilter(this);
	this->tableWidget->viewport()->installEventFilter(this);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Layer>&
LayerHandler::GetLayer(const Util::String & layer)
{
	n_assert(this->layers.Contains(layer));
	return this->layers[layer];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
LayerHandler::HasLayer(const Util::String & layer)
{	
	return this->layers.Contains(layer);
}

} // namespace Layers