#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::ModelModifyAction
    
    Modifies a model by basically saving a version of it.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/array.h"
#include "baseaction.h"

namespace Widgets
{
	class ModelHandler;
}

namespace Actions
{
class ModelModifyAction : public BaseAction
{
	__DeclareClass(ModelModifyAction);
public:
	/// constructor
	ModelModifyAction();
	/// destructor
	virtual ~ModelModifyAction();

    /// cleanup the action
    void Cleanup();

	/// undoes
	void Undo();
	/// redoes
	void Redo();
	/// does
	void Do();
	/// discards action
	void Discard();

	/// adds a version of attributes, constants and physics
	void AddVersion(const Util::String& attributes, const Util::String& constants, const Util::String& physics);
	/// gets the last version of the attributes
	const Util::String& GetLastAttrVersion() const;
	/// gets the last version of the constants
	const Util::String& GetLastConstVersion() const;
	/// gets the last version of the physics
	const Util::String& GetLastPhysVersion() const;

	/// sets the category for the modifier action
	void SetCategory(const Util::String& category);
	/// sets the model for the modifier action
	void SetModel(const Util::String& model);

	/// sets the model handler
	void SetModelHandler(const Ptr<Widgets::ModelHandler>& handler);
	/// gets the model handler
	const Ptr<Widgets::ModelHandler>& GetModelHandler() const;
	
private:
	Ptr<Widgets::ModelHandler> handler;
	Util::Array<Util::String> constVersions;
	Util::Array<Util::String> physVerions;
	Util::Array<Util::String> attrVersions;
	Util::String category;
	Util::String model;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelModifyAction::SetCategory( const Util::String& category )
{
	n_assert(category.IsValid());
	this->category = category;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelModifyAction::SetModel( const Util::String& model )
{
	n_assert(model.IsValid());
	this->model = model;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelModifyAction::SetModelHandler( const Ptr<Widgets::ModelHandler>& handler )
{
	n_assert(handler.isvalid());
	this->handler = handler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Widgets::ModelHandler>& 
ModelModifyAction::GetModelHandler() const
{
	return this->handler;
}

} // namespace Actions
//------------------------------------------------------------------------------