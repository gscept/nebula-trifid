#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::ModelBuilder
    
    LTU Luleå University of Technology
*/
//------------------------------------------------------------------------------

#include "..\..\foundation\core\refcounted.h"
#include "..\toolkitutil\modelwriter.h"
#include "math\xnamath\xna_float4.h"

using namespace Util;
using namespace Math;

namespace ToolkitUtil
{
class ModelBuilder
{
public:
	struct Joint 
	{
		int jointIndex;
		int parentJointIndex;
		float4 poseTranslation;
		float4 poseRotation;
		float4 poseScale;
		String jointName;
	};
	struct Skin 
	{
		HashTable<String, int> shaderInts;
		HashTable<String, float> shaderFloats;
		HashTable<String, String> shaderTextures;
		HashTable<String, bool> shaderBools;
		String shaderFileName;
		String shaderTechnique;
		String meshFileName;
		int primitiveGroupIndex;
		int numFragments;
		Array<int> skinFragmentPrimGroupIndices;
		Array<Array<int> > skinFragmentJointPalettes;
	};
	struct Model
	{
		Array<Joint> joints;
		Array<Skin> skins;
		float4 collisionBoxMax;
		float4 collisionBoxMin;
		String skinListName;
	};

	bool SaveModels(const Ptr<ModelWriter>& writer);
	void AddModel( String modelName, const ModelBuilder::Model& newModel );
	int GetNumModels(){return models.Size();}
	ModelBuilder::Model* GetModelByName(String modelName);
private:
	HashTable<String, ModelBuilder::Model> models;

};

}// namespace Toolkit