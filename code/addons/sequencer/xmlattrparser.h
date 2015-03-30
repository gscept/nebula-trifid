#ifndef SEQUENCER_XMLATTRPARSER_H
#define SEQUENCER_XMLATTRPARSER_H
//------------------------------------------------------------------------------
/**
    @class Sequencer::XmlAttrParser
    
    Helper class which offers static function to parse xml attributes
*/
#include "math/vector.h"
#include "util/string.h"
#include "core/ptr.h"
#include "sequencer/animatedparameter.h"
#include "io/xmlreader.h"

class nStream;

namespace Sequencer
{
class BezierGraph;

// helper struct for attribute setting
struct AttrTransform {
    float4 translation;
    float4 scale;
    float4 rotation;
};

class XmlAttrParser
{
public:
    /// constructor
    XmlAttrParser();
    /// destructor
    virtual ~XmlAttrParser();
    /// parse a non animated transformation from nStream
    static void ParseTransformation(const Ptr<IO::XmlReader>& xml, 
        float4& translation,
        float4& rotation,
        float4& scale);
    /// parse an animated transformation from nStream
    static void ParseAnimatedTransform(const Ptr<IO::XmlReader>& xml, 
        AnimatedParameter<float4>& translation,
        AnimatedParameter<float4>& rotation,
        AnimatedParameter<float4>& scale);
    /// parse a non animated vector3 from nStream
    static void ParseVector4(Ptr<IO::XmlReader>& xml, float4& value );
    /// parse an animated float parameter from nStream
    static void ParseAnimatedFloat(const Ptr<IO::XmlReader>& xml, AnimatedParameter<float> &animFloat, bool isDegrees = false);
    /// parse a non animated vector4 from nStream
    static void ParseAnimatedVector4(Ptr<IO::XmlReader>& xml, AnimatedParameter<float4> &animVec);
    /// parse a bezier graph from nStream
    static void ParseBezierGraph(const Ptr<IO::XmlReader>& xml, Ptr<BezierGraph> graph, bool isDegrees = false);
    /// parse a vector2 from string
    static float2 ParseVector2(const String& value);

private:
};


} // namespace Sequencer
//------------------------------------------------------------------------------
#endif

