#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::AnyFXGenerator
    
    Generates and validates AnyFX compliant code.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "generation/generator.h"

namespace Nody
{
    class Node;
    class Link;
    class NodeScene;
    class VariableInstance;
}

namespace Shady
{
class AnyFXGenerator : public Nody::Generator
{
	__DeclareClass(AnyFXGenerator);
public:
	/// constructor
	AnyFXGenerator();
	/// destructor
	virtual ~AnyFXGenerator();

    /// generate code to path
    void GenerateToFile(const Ptr<Nody::NodeScene>& scene, const IO::URI& path);
    /// generate code to buffer
    void GenerateToBuffer(const Ptr<Nody::NodeScene>& scene, Util::Blob& output);

    /// validate generated buffer
	void Validate(const Ptr<Nody::NodeScene>& scene, const Util::String& language);

protected:
    /// handle single node
    void VisitNode(const Ptr<Nody::Node>& node);
    /// handles a single link in the network
    void VisitLink(const Ptr<Nody::Link>& link);
    /// handle node when backwards traversing
    void RevisitNode(const Ptr<Nody::Node>& node);
    /// handle link when backwards traversing
    void RevisitLink(const Ptr<Nody::Link>& link);

private:
    /// converts Util::Variant and VarType to a compilable string
    Util::String ConvertFromVariant(const Util::Variant& value, const Nody::VarType& type);
    /// tests if types can be converted
    bool CanExplicitlyConvert(const Nody::VarType& lhs, const Nody::VarType& rhs);

    /// runs the generation
    Util::String DoWork(const Ptr<Nody::NodeScene>& scene);

    Util::String paramBlock;
    Util::String generatedCode;
    uint localVarCounter;

    Util::Array<Ptr<Nody::Node>> revisitedNodes;
    Util::Array<Ptr<Nody::Node>> visitedNodes;
    Util::Array<Ptr<Nody::VariableInstance>> usedVariables;
    Util::Array<Ptr<Nody::Link>> usedLinks;
    Util::Array<Ptr<Nody::VariableInstance>> parameters;
    Util::Array<Util::String> defines;
    Util::Dictionary<Ptr<Nody::VariableInstance>, Nody::VarType> vartypeInferenceMapping;
    Util::Dictionary<Ptr<Nody::VariableInstance>, Util::String> variableInstanceNameMapping;
    Util::String lastGeneratedName;
    
}; 
} // namespace Shady
//------------------------------------------------------------------------------