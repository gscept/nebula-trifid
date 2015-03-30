//------------------------------------------------------------------------------
//  xmlnodemodifier.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "xmlnodemodifier.h"
#include "io/xmlreader.h"
#include "io/xmlwriter.h"

using namespace Util;
using namespace IO;

__ImplementClass(ToolkitUtil::XMLNodeModifier, 'XNMF', Core::RefCounted);
namespace ToolkitUtil
{

//------------------------------------------------------------------------------
/**
*/
XMLNodeModifier::XMLNodeModifier()
{

}

//------------------------------------------------------------------------------
/**
*/
XMLNodeModifier::~XMLNodeModifier()
{
    // empty
}

//------------------------------------------------------------------------------
/**
This initialises all data of the modifier recursively, using a xml-structure
for specification. You have to make sure that the current node name of reader
is "ModifyNode".
*/
bool
XMLNodeModifier::Initialise(const Ptr<IO::XmlReader> & spec)
{
    if ((spec->GetCurrentNodeName() == "ModifyNode")
        && spec->HasAttr("node")
        && spec->HasAttr("action"))
    {
        // set up standard attributes
        this->nodeName = spec->GetString("node");
        this->action = spec->GetString("action");
        // if spec doesn't contain these attributes, values will be an empty string
        this->lookupKey = GetStringFromAttribute(spec, "lookupkey");
        this->lookupValue = GetStringFromAttribute(spec, "lookupvalue");
        this->referenceKey = GetStringFromAttribute(spec, "referencekey");
        this->referenceValue = GetStringFromAttribute(spec, "referencevalue");
        if (("" == referenceKey) || ("" == referenceValue))
        {
            // in case that no reference was specified, or the specified reference was invalid, use this path
            this->referenceKey = this->lookupKey;
            this->referenceValue = this->lookupValue;
        }
        // parse attributes and fill attribute lists
        if(true == spec->SetToFirstChild("ModifyAttribute"))
        {             
            do // as long as there are Attribute-Tags
            {
                if(spec->HasAttr("name") && spec->HasAttr("action"))
                {
                    // append attribute to createlist
                    if("create" == spec->GetString("action"))
                    {
                        AttrInfo info;
                        info.key = spec->GetString("name");
                        info.value = this->GetStringFromAttribute(spec, "value");
                        info.reference = this->GetStringFromAttribute(spec, "reference");
                        info.action = "create";
                        // only append attributes with either a value or a reference
                        if (("" != info.value) || ("" != info.reference))
                        {
                            this->attributeCreationList.Append(info);                        
                        }
                    }
                    // append attribute to modificationlist
                    else if(("change" == spec->GetString("action"))
                        || ("delete" == spec->GetString("action")))
                    {
                        AttrInfo info;
                        info.key = spec->GetString("name");
                        info.value = this->GetStringFromAttribute(spec, "value");
                        info.reference = this->GetStringFromAttribute(spec, "reference");
                        info.action = spec->GetString("action");
                        // only append attributes with either a value or a reference
                        // if action is delete, value or reference is not needed
                        if (("" != info.value) || ("" != info.reference)
                            || ("delete" == info.action))
                        {
                            this->attributeModificationList.Append(info);                        
                        }
                    }
                    else
                    {
                        n_printf("invalid action \"%s\" for attribute \"%s\" in line no. %d\n", spec->GetString("action").AsCharPtr(),
                            spec->GetString("name").AsCharPtr(), spec->GetCurrentNodeLineNumber());
                    }
                }
                else
                {
                    n_printf("missing name or action attribute in line no. %d\n", spec->GetCurrentNodeLineNumber());
                }
            }
            while(spec->SetToNextChild("ModifyAttribute")); // go to next Attribute-Tag if existing
        }

        // parse subnodes and fill subnodelist
        if(true == spec->SetToFirstChild("ModifyNode"))
        {             
            do
            {
                Ptr<XMLNodeModifier> subNode = XMLNodeModifier::Create();
                if (subNode->Initialise(spec))
                {
                    if("create" == subNode->GetAction())
                    {
                        this->subNodeCreateList.Append(subNode);
                    }
                    else if("delete" == subNode->GetAction())
                    {
                        this->subNodeDeleteList.Append(subNode);
                    }
                    else if("change" == subNode->GetAction())
                    {
                        this->subNodeChangeList.Append(subNode);
                    }
                    else
                    {
                        n_printf("invalid action \"%s\" for node \"%s\" in line no. %d\n", spec->GetString("action").AsCharPtr(),
                            spec->GetString("node").AsCharPtr(), spec->GetCurrentNodeLineNumber());
                    }
                }
            }
            while(spec->SetToNextChild("ModifyNode")); // go to next Attribute-Tag if existing
        }
        return true;
    }
    else
    {
        // some error messages (the method won't fail hard, but skips invalid entries) 
        if(false == spec->HasAttr("node"))
        {
            n_printf("no \"node\"-attribute found in line no. %d\n", spec->GetCurrentNodeLineNumber());
        }
        else if(false == spec->HasAttr("action"))
        {
            n_printf("no \"action\"-attribute found in line no. %d\n", spec->GetCurrentNodeLineNumber());
        }
        else if(spec->GetCurrentNodeName() != "ModifyNode")
        {
            n_printf("unknown node <%s> in line no. %d\n", spec->GetCurrentNodeName().AsCharPtr(), spec->GetCurrentNodeLineNumber());
        }
    }
    // if the nodename wasn't "ModifyNode" or
    // if "node" or "action" attributes were missing
    return false;
}

//------------------------------------------------------------------------------
/**
    This method simply returns a string-attribute. In case of failure the
    string will be empty but program execution will continue (the regular
    GetString-method fails hard if attribute is not existing).
*/
Util::String
XMLNodeModifier::GetStringFromAttribute(const Ptr<IO::XmlReader> & reader, Util::String attrName)
{
    String result = "";
    if (reader->HasAttr(attrName.AsCharPtr()))
    {
        result = reader->GetString(attrName.AsCharPtr());
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Sets reader to a specific sibling node using key and value to identify it.
    If no sibling is found with these attributes the method will return false
    and reader is set to its original position.
*/
bool
XMLNodeModifier::SetReaderToSiblingWithAttr(const Ptr<IO::XmlReader> & reader, Util::String node, Util::String key, Util::String value)
{
    bool found = false;
    bool done = false;
    if (("" != key) && ("" != value))
    {
        do
        {
            if (reader->HasAttr(key.AsCharPtr()))
            {
                // stop when value is matching
                found = (reader->GetString(key.AsCharPtr()) == value);
            }
            if (false == found) {
                // if there are no more childs set done to true
                done = (false == reader->SetToNextChild(node));
            }
        }
        while((false == found) && (false == done));
    }
    // note: if no node is found, reader gets set back to parent automaticly (SetToNextChild does that)
    return found;
}

//------------------------------------------------------------------------------
/**
    Sets reader to a specific child node using key and value to identify it.
    If no child is found with these attributes the method will return false
    and reader is set to its original position.
*/
bool
XMLNodeModifier::SetReaderToChildWithAttr(const Ptr<IO::XmlReader> & reader, Util::String node, Util::String key, Util::String value)
{
    bool found = false;
    bool done = false;
    //n_assert("" != key);
    //n_assert("" != value);
    if (("" != key) && ("" != value))
    {
        if (reader->SetToFirstChild(node))
        {
            do
            {
                if (reader->HasAttr(key.AsCharPtr()))
                {
                    // stop when value is matching
                    found = (reader->GetString(key.AsCharPtr()) == value);
                }
                if (false == found) {
                    // if there are no more childs set done to true
                    done = (false == reader->SetToNextChild(node));
                }
            }
            while((false == found) && (false == done));
        }
    }
    // note: if no node is found, reader gets set back to parent automaticly (SetToNextChild does that)
    return found;
}

//------------------------------------------------------------------------------
/**
This recursively applies modifications to src and writes it into dst.
You have to make sure that the current node name is equal to the
modifiers nodeName. If you call this function outside of an XMLNodeModifier
object, which you'll usually do, the topnode of destination must already be open.

    xmlwriter->BeginNode(modifier->GetNodeName());
    modifier->Modify(xmlwriter, xmlreader);
    xmlwriter->EndNode();

*/
void
XMLNodeModifier::Modify(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src)
{
    // debug
    //n_printf("line %d\n", src->GetCurrentNodeLineNumber());
    n_assert(src->GetCurrentNodeName() == this->nodeName);
    // handle attributes
    Util::Array<Util::String> sourceAttrs = src->GetAttrs();
    // first iterate through attributeCreationList
    for (int i = 0; i < this->attributeCreationList.Size(); i++)
    {
        bool isNew = true;
        // before we check if attribute already exists, we have to determine if we
        // are in the original node or in a reference node. cause from reference
        // there's no way of overwriting, so isNew will stay true
        if (((this->referenceKey == this->lookupKey) && (this->referenceValue == this->lookupValue))
            && (("" != this->referenceKey) && ("" != this->lookupKey)))
        {
            // if we are in the original source, check if attribute already exists
            isNew = (InvalidIndex == sourceAttrs.FindIndex(attributeCreationList[i].key));
        }
        // if attribute does not exist in source create it, else do nothing
        if (isNew)
        {
            if ("" != attributeCreationList[i].value)
            {
                dst->SetString(attributeCreationList[i].key, attributeCreationList[i].value);
            }
            else
            {
                // set value from reference
                dst->SetString(attributeCreationList[i].key, src->GetString(attributeCreationList[i].reference.AsCharPtr()));
            }            
        }
    }
    // then iterate through all existing attributes to see if there's a rule to apply
    for (int i = 0; i < sourceAttrs.Size(); i++)
    {
        ModifyAttribute(dst, src, sourceAttrs[i]);
    }

    // handle subnodes
    // first iterate through subNodeCreateList
    for (int i = 0; i < this->subNodeCreateList.Size(); i++)
    {
        String subNodeName = this->subNodeCreateList[i]->GetNodeName();
        String subNodeLookupKey = this->subNodeCreateList[i]->GetLookupKey();
        String subNodeLookupValue = this->subNodeCreateList[i]->GetLookupValue();
        String subNodeReferenceKey = this->subNodeCreateList[i]->GetReferenceKey();
        String subNodeReferenceValue = this->subNodeCreateList[i]->GetReferenceValue();
        // keep reference up to date
        bool referenceFound = SetReaderToChildWithAttr(src, subNodeName, subNodeReferenceKey, subNodeReferenceValue);
        // only proceed if the reference is set correctly, or if there was no
        // intension to use the reference in the first place
        if (referenceFound || (("" == subNodeReferenceKey) && ("" == subNodeReferenceValue)))
        {
            // create subnode (don't check for existing ones,
            // cause you might want to have nodes of the same type
            dst->BeginNode(subNodeName);
            // allow modification even if no reference was found (thats only for NEW subnodes!!)
            subNodeCreateList[i]->Modify(dst, src);
            // if we were using a reference, set reader back to parent
            if (referenceFound)
            {
                src->SetToParent();
            }
            // now look if there's a modification for this subnode...
            // you need to do this here, because if you iterate through the
            // source subnodes you won't find this newly created node and
            // therefor ignore any other rule for it
            for (int j = 0; j < this->subNodeChangeList.Size(); j++)
            {
                // if the current rule refers to this new node...
                if (subNodeChangeList[j]->HasSameTargetNode(subNodeCreateList[i]))
                {
                    // ... check for a valid reference ...
                    if (SetReaderToChildWithAttr(src, subNodeName,
                        this->subNodeChangeList[j]->GetReferenceKey(),
                        this->subNodeChangeList[j]->GetReferenceValue()))
                    {
                        // ... and apply modification to it                    
                        subNodeChangeList[j]->Modify(dst, src);
                        // keep reference up to date
                        src->SetToParent();
                    }
                }
            }
            // now we're all done with the new node
            dst->EndNode();
        }
    }

    // Now, before we continue, we have check if the action for this current node is "create"
    // because creation can not contain any subnodes, unless it has a valid reference.
    // Otherwise there would be nothing to delte or copy from. 
    if (("create" != this->action)
        || (("create" == this->action) && ("" != this->referenceKey) && ("" != this->referenceValue)))
    {
        // iterate through all existing subnodes to see if there's a rule to apply
        if (src->SetToFirstChild())
        {   
            do
            {
                // if there is a rule to delete this subnode, then dont write it
                // into dst. that way it'll be lost. otherwise proceed with modification
                if (false == NodeHasRuleInList(this->subNodeDeleteList, src))
                {
                    // create subnode
                    dst->BeginNode(src->GetCurrentNodeName());
                    // find and apply modification to it
                    ModifyNode(dst, src);
                    // close and proceed to next subnode
                    dst->EndNode();
                }
            }
            while(src->SetToNextChild());
        }    
    }
}

//------------------------------------------------------------------------------
/**
*/
void
XMLNodeModifier::ModifyAttribute(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src, Util::String attrName)
{
    bool hasBeenModified = false;
    for (int i = 0; i < this->attributeModificationList.Size(); i++)
    {
        AttrInfo rule = this->attributeModificationList[i];
        if (attrName == rule.key)
        {
            if ("change" == rule.action)
            {
                // take value from modification rule in stead of source value
                if ("" != rule.value)
                {
                    dst->SetString(attrName, rule.value);
                }
                else
                {
                    // set value from reference
                    dst->SetString(attrName, src->GetString(rule.reference.AsCharPtr()));
                }
                
            }
            else if ("delete" == rule.action)
            {
                // simply don't copy. that way it won't appear in dst
            }
            else
            {
                // invalid action, don't cange anything and copy from src
                n_printf("invalid action\n");
                dst->SetString(attrName, src->GetString(attrName.AsCharPtr()));
            }
            hasBeenModified = true;
        }
    }
    // also check the newly create attributes cause we dont want to overwrite them
    for(int i = 0; i < this->attributeCreationList.Size(); i++)
    {
        hasBeenModified |= (attrName == attributeCreationList[i].key);      
    }
    // no rule specified for this attribute, so simply copy it from src to dst
    if (false == hasBeenModified)
    {
        dst->SetString(attrName, src->GetString(attrName.AsCharPtr()));     
    }
}

//------------------------------------------------------------------------------
/**
*/
void
XMLNodeModifier::ModifyNode(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src)
{
    bool hasBeenModified = false;
    // iterate through all node rules
    for(int i = 0; i < this->subNodeChangeList.Size(); i++)
    {
        String subNodeName = this->subNodeChangeList[i]->GetNodeName();
        String subNodeLookupKey = this->subNodeChangeList[i]->GetLookupKey();
        String subNodeLookupValue = this->subNodeChangeList[i]->GetLookupValue();
        String subNodeReferenceKey = this->subNodeChangeList[i]->GetReferenceKey();
        String subNodeReferenceValue = this->subNodeChangeList[i]->GetReferenceValue();
        // see if nodename matches
        if (src->GetCurrentNodeName() == subNodeName) {
            
            // try further criteria
            if ((subNodeLookupKey == "") && (subNodeLookupValue == ""))
            {
                // no further specification, modify any node of this type
                // recursive function call to "main"-function
                subNodeChangeList[i]->Modify(dst, src);
                hasBeenModified = true;
            }
            else
            {
                if(src->HasAttr(subNodeLookupKey.AsCharPtr()))
                {
                    if(src->GetString(subNodeLookupKey.AsCharPtr()) == subNodeLookupValue)
                    {
                        // node matches exactly the pattern, modify it
                        // set src to reference (in this case reference is a sibling of subnode)
                        if (SetReaderToSiblingWithAttr(src, subNodeName, subNodeReferenceKey, subNodeReferenceValue))
                        {
                            // recursive function call to "main"-function
                            subNodeChangeList[i]->Modify(dst, src);
                        }
                        hasBeenModified = true;
                    }
                } 
            }
        }
    }
    // no rule specified for this node, so simply copy it from src to dst
    if (false == hasBeenModified)
    {
        CopyWholeNode(dst, src);
    }
}

//------------------------------------------------------------------------------
/**
compares two XMLNodeModifier objects, if their nodeName, lookupKey
and lookupValue are equal.
*/
bool
XMLNodeModifier::HasSameTargetNode(const Ptr<XMLNodeModifier> & modifier)
{
    return (this->nodeName == modifier->GetNodeName()
        && this->lookupKey == modifier->GetLookupKey()
        && this->lookupValue == modifier->GetLookupValue());
}

//------------------------------------------------------------------------------
/**
This looks through the given list of XMLNodeModifier Objects, to determine if one
of them refer to the readers current node. If thats the case, it returns true.
*/
bool
XMLNodeModifier::NodeHasRuleInList(const Util::Array<Ptr<XMLNodeModifier>> & list, const Ptr<IO::XmlReader> & reader)
{
    bool found = false;
    for (int i = 0; i < list.Size(); i++)
    {
        String nodeName = list[i]->GetNodeName();
        String nodeLookupKey = list[i]->GetLookupKey();
        String nodeLookupValue = list[i]->GetLookupValue();
        if (nodeName == reader->GetCurrentNodeName())
        {
            if ((nodeLookupKey != "") && (nodeLookupValue != ""))
            {
                if (reader->HasAttr(nodeLookupKey.AsCharPtr()))
                {
                    found |= (reader->GetString(nodeLookupKey.AsCharPtr()) == nodeLookupValue);
                }
            }
            else
            {
                found |= true;
            }
        }
    }
    return found;
}

//------------------------------------------------------------------------------
/**
This simply takes everthing from source and writes it into destionation, no matter what.
*/
void
XMLNodeModifier::CopyWholeNode(const Ptr<IO::XmlWriter> & dst, const Ptr<IO::XmlReader> & src)
{
    Util::Array<Util::String> attributes = src->GetAttrs();
    for(int i = 0; i < attributes.Size(); i++)
    {
        dst->SetString(attributes[i], src->GetString(attributes[i].AsCharPtr()));
    }
    if (src->SetToFirstChild())
    {   
        do
        {
            dst->BeginNode(src->GetCurrentNodeName());
            CopyWholeNode(dst, src);
            dst->EndNode();
        }
        while(src->SetToNextChild());
    }
}

} // namespace Toolkit