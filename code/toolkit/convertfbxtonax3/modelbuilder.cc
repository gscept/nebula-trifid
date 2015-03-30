//------------------------------------------------------------------------------
//  modelbuilder.cc
//  LTU Luleå University of Technology
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelbuilder.h"

namespace ToolkitUtil
{

bool ModelBuilder::SaveModels( const Ptr<ModelWriter>& writer )
{
	/*writer->BeginModel("Models::Model", FourCC('MODL'), curModelName);
	if (!this->ParseNodes(reader, writer))
	{
		n_printf("Conversion error in RecurseReadNodes() ('%s')!\n", this->srcPath.AsCharPtr());
		return false;
	}
	writer->EndModel();
	n_assert(this->nodeTypeStack.IsEmpty()); */
	return true;
}

void ModelBuilder::AddModel( String modelName, const ModelBuilder::Model& newModel )
{
	this->models.Add(modelName, newModel);
}

ModelBuilder::Model* ModelBuilder::GetModelByName( String modelName )
{
	ModelBuilder::Model* result = &(this->models[modelName]);
	return result;
}
}// namespace Toolkit