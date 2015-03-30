#pragma once
//------------------------------------------------------------------------------
/**
    @class DrawState
    
    A draw state encapsulates an entire GPU state to be applied before rendering.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "renderstaterow.h"
#include "blendstaterow.h"
#include "symbol.h"
#include "annotation.h"
namespace AnyFX
{

class RenderState : public Symbol
{
public:

	/// constructor
	RenderState();
	/// destructor
	virtual ~RenderState();

	/// set annotation
	void SetAnnotation(const Annotation& annotation);

	/// consumes a draw state row
	void ConsumeRenderRow(const RenderStateRow& row);
	/// consumes a blend state row
	void ConsumeBlendRow(const BlendStateRow& row);

	/// type checks render state
	void TypeCheck(TypeChecker& typechecker);
	/// compiles render state
	void Compile(BinWriter& writer);

	/// currently the upper limit of render targets is 8
	static const int MaxNumRenderTargets = 8;

private:

	struct InvalidValueContainer
	{
		unsigned entry;
		std::string flag;
		std::string value;
	};

	std::vector<std::string> invalidExpressionFlags;
	std::vector<std::string> invalidStringFlags;
	std::vector<InvalidValueContainer> invalidValues;
	unsigned numEntries;

	std::vector<Expression*> blendEnumIndexExpressions;
	std::vector<std::pair<int, BlendStateRow::EnumFlag> > blendEnumPairs;
	int blendEnumFlags[MaxNumRenderTargets][BlendStateRow::NumEnumFlags];

	std::vector<Expression*> blendBoolIndexExpressions;
	std::vector<std::pair<Expression*, BlendStateRow::BoolFlag> > blendBoolPairs;
	bool blendBoolFlags[MaxNumRenderTargets][BlendStateRow::NumBoolFlags];

	Expression* drawIntExpressions[RenderStateRow::NumIntFlags];
	int drawIntFlags[RenderStateRow::NumIntFlags];
	Expression* drawUintExpressions[RenderStateRow::NumIntFlags];
	unsigned drawUintFlags[RenderStateRow::NumUintFlags];
	Expression* drawBoolExpressions[RenderStateRow::NumBoolFlags];
	bool drawBoolFlags[RenderStateRow::NumBoolFlags];
	Expression* drawFloatExpressions[RenderStateRow::NumFloatFlags];
	float drawFloatFlags[RenderStateRow::NumFloatFlags];

	int drawEnumFlags[RenderStateRow::NumEnumFlags];
	

	bool hasAnnotation;
	Annotation annotation;
}; 

} // namespace AnyFX
//------------------------------------------------------------------------------