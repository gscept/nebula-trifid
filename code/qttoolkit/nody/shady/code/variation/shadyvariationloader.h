#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyVariationLoader
    
    Loads variations using the Shady feature set.

    Follows a very specific XML notation system. A is declared using the <Variation></Variation> tags.
    Within these tags, we can define a set of inputs, outputs and in the case of Shady, also a list of sources.
    Inputs are treated as incoming data to a node.
    Outputs are treated as outcoming data from a node.
    Hidden are variables which can be set from outside the source code, but is neither an input nor an output.

    Input
    -----------------------

    Requires attributes:
    type            - string matching a type, see vartype.h for reference
    name            - variable name, must match name in source
    (opt)inference  - true/false if this variable should use type inference resolving, default is true

    Output
    -----------------------
    type            -               -||-
    name            -               -||-
    (opt)constant   - true/false if output is assuming a constant value, default is false
    (opt)param      - true/false if output is a parameter, meaning it can be set from outside the shader as a uniform (same as Hidden but mapped directly as output)
    (opt)static     - true/false if output name is solid, meaning it should not receive a custom name, also requires param to be true

    Hidden
    -----------------------
    type            -               -||-
    name            -               -||-
    (opt)static     - true/false if name of parameter is solid, meaning it should not receive a custom name


    Source
    -----------------------
    type - the language in which this source is being defined, see shadydefs.h for reference.

        Code
        -----------------------
        output - the name of a defined output variable to which this code should emit.

        (contents) - must contain a variable called [output] which is the output target, this will be replaced with the real output name and stored within the variable object

    For each output variable we define a new set of tags <Code> </Code>.
    The Code tag accepts one parameter, called output, which is the supposed output of this code, it must match a name in the output section.
    The code must also contain a variable called 'output' in order for it to properly work.

    The content of this tag is what is used as the source for the node itself. The source MUST NOT CONTAIN variables with the name
    LocalX where X is an integer number.

    Note:
    Variable types are somewhat limited to be float, float2, float3, float4, int, int2, int3, int4 and bool.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variation/variationloader.h"
#include "shadyvariation.h"
#include "shadydefs.h"
#include "io/xmlreader.h"

namespace Shady
{
class ShadyVariationLoader : public Nody::VariationLoader
{
	__DeclareClass(ShadyVariationLoader);
public:
	/// constructor
	ShadyVariationLoader();
	/// destructor
	virtual ~ShadyVariationLoader();

	/// load shady variation
	Ptr<Nody::Variation> Load(const Ptr<IO::Stream>& stream);

private:

    /// parse variation
    void ParseVariation(const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation);
    /// parse input
    void ParseInput(const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation);
    /// parse output
    void ParseOutput(const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation);
    /// parse hidden
    void ParseHidden(const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation);
    /// parse source
    void ParseSource(const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation);
}; 
} // namespace Shady
//------------------------------------------------------------------------------