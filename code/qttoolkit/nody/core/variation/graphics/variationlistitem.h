#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariationListItem
    
    Overloads QTreeWidgetItem to supply an item-variation pair.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QTreeWidgetItem>
#include "variation/variation.h"
namespace Nody
{
class VariationListItem : public QTreeWidgetItem
{
public:
	/// constructor
	VariationListItem();
	/// destructor
	virtual ~VariationListItem();

	/// set variation
	void SetVariation(const Ptr<Variation>& variation);
	/// get variation
	const Ptr<Variation>& GetVariation() const;

private:

	Ptr<Variation> variation;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
VariationListItem::SetVariation( const Ptr<Variation>& variation )
{
	n_assert(variation.isvalid());
	this->variation = variation;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variation>& 
VariationListItem::GetVariation() const
{
	return this->variation;
}

} // namespace Nody
//------------------------------------------------------------------------------