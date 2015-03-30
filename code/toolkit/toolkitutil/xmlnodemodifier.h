#pragma once
//------------------------------------------------------------------------------
/**
    @class ToolkitUtil::XMLNodeModifier
    
    A utility class to modify xml nodes.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "util/array.h"
#include "core/refcounted.h"

namespace IO
{
    class XmlReader;
    class XmlWriter;
}

//------------------------------------------------------------------------------
namespace ToolkitUtil
{
    class XMLNodeModifier : public Core::RefCounted
    {
        __DeclareClass(XMLNodeModifier);
    public:
        /// constructor
        XMLNodeModifier();
        /// destructor
        ~XMLNodeModifier();
        /// initialises all internal data. builds subnodes, using the specifications from the config-file
        bool Initialise(const Ptr<IO::XmlReader> & spec);
        /// determines if these two modifers refer to the same node
        bool HasSameTargetNode(const Ptr<XMLNodeModifier> & modifier);
        /// applies modifications to src and writes it into dst
        void Modify(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src);
        /// gets the name of the node which is handeled by this object
        Util::String GetNodeName();
        /// gets the key which is used to match the search pattern
        Util::String GetLookupKey();
        /// gets the value which is used to match the search pattern
        Util::String GetLookupValue();
        /// gets the key which is used for reference
        Util::String GetReferenceKey();
        /// gets the value which is used for reference
        Util::String GetReferenceValue();
        /// gets the action which will be performed upon the node
        Util::String GetAction();

    private:
        /// Information on key, value and action for this attribute
        struct AttrInfo
        {
            Util::String key;
            Util::String value;
            Util::String reference;
            Util::String action;
        };

        /// used for identification
        Util::String nodeName;
        /// an attribute which is used to match only nodes with a specific value
        Util::String lookupKey;
        /// value of the attribute used to match only nodes with a specific value
        Util::String lookupValue;
        /// an attribute which is used to identify a node for reference
        Util::String referenceKey;
        /// value of the attribute used to identify a node for reference
        Util::String referenceValue;

        /// type of modification: possible values are create, change, delete
        Util::String action;

        /// attributes to be created
        Util::Array<AttrInfo> attributeCreationList;
        /// attributes to be modified or deleted
        Util::Array<AttrInfo> attributeModificationList;

        /// subnodes to be created
        Util::Array<Ptr<XMLNodeModifier>> subNodeCreateList;
        /// subnodes to be deleted
        Util::Array<Ptr<XMLNodeModifier>> subNodeDeleteList;
        /// subnodes to be modified
        Util::Array<Ptr<XMLNodeModifier>> subNodeChangeList;

        /// sets reader to a specific child node which value equals the parameter
        bool SetReaderToChildWithAttr(const Ptr<IO::XmlReader> & reader, Util::String node, Util::String key, Util::String value);
        /// sets reader to a specific sibling node which value equals the parameter
        bool SetReaderToSiblingWithAttr(const Ptr<IO::XmlReader> & reader, Util::String node, Util::String key, Util::String value);
        /// gets a string value. checks for existance and returns an empty string if attr is not found
        Util::String GetStringFromAttribute(const Ptr<IO::XmlReader> & reader, Util::String attrName);

        /// takes an existing attribute, look if there's a modification rule and in case there is modifies it
        void ModifyAttribute(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src, Util::String attrName);

        void ModifyNode(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src);

        void CopyWholeNode(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src);

        bool XMLNodeModifier::NodeHasRuleInList(const Util::Array<Ptr<XMLNodeModifier>> & list, const Ptr<IO::XmlReader> & reader);
    };

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetNodeName()
{
    return this->nodeName;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetLookupKey()
{
    return this->lookupKey;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetLookupValue()
{
    return this->lookupValue;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetReferenceKey()
{
    return this->referenceKey;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetReferenceValue()
{
    return this->referenceValue;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
XMLNodeModifier::GetAction()
{
    return this->action;
}

} // namespace ToolkitUtil
//------------------------------------------------------------------------------