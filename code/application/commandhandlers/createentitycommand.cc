//------------------------------------------------------------------------------
//  createentitycommand.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "commandhandlers/createentitycommand.h"
#include "basegamefeature/managers/entitymanager.h"
#include "basegamefeature/managers/factorymanager.h"
#include "basegamefeature/basegameprotocol.h"

namespace Commands
{
__ImplementClass(Commands::CreateEntityCommand, 'CREH', Remote::RemoteCommandHandler);

using namespace Util;
using namespace Math;
using namespace Attr;
using namespace BaseGameFeature;
using namespace Game;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
CreateEntityCommand::CreateEntityCommand():
    cmdName("createentity")
{
    
}

//------------------------------------------------------------------------------
/**
*/
CreateEntityCommand::~CreateEntityCommand()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
CreateEntityCommand::HandleCommand(const Util::CommandLineArgs& args)
{
    if (args.GetCmdName() != this->cmdName)
    {
        return; // no create entity command
    }

    const String& category = args.GetString("category");
    const String& tmplt = args.GetString("template");
    const String& id = args.GetString("id");
    matrix44 entityTransform;
    if (args.HasArg("transform"))
    {
        entityTransform = args.GetMatrix44("transform");
    }

    // convert attributes into an attribute container
    Attr::AttributeContainer attrs;
    int numArgs = args.GetNumArgs();
    int argIndex;
    for (argIndex = 0; argIndex < numArgs; argIndex++)
    {
        if (args.GetStringAtIndex(argIndex) == "attr")
        {
            n_assert(args.GetNumArgs() > argIndex+1);
            const String& arg = args.GetStringAtIndex(argIndex+1);
            Array<String> tokens = arg.Tokenize("#");
            n_assert(tokens.Size() >= 1);
            const String& attrName = tokens[0];
            String attrValue;
            if (tokens.Size() > 1)
            {
                attrValue = tokens[1];
            }
            if (AttrId::IsValidName(attrName))
            {
                AttrId attrId(attrName);
                switch (attrId.GetValueType())
                {
                case VoidType:
                case BlobType:
                case GuidType:
                    // these types don't make sense here, ignore them
                    break;
                case IntType:
                    attrs.AddAttr(Attribute(IntAttrId(attrName), attrValue.AsInt()));
                    break;
                case FloatType:
                    attrs.AddAttr(Attribute(FloatAttrId(attrName), attrValue.AsFloat()));
                    break;
                case BoolType:
                    if (attrValue == "0")
                    {
                        attrs.AddAttr(Attribute(BoolAttrId(attrName), false));
                    }
                    else
                    {
                        attrs.AddAttr(Attribute(BoolAttrId(attrName), true));
                    }
                    break;
                case Float4Type:
                    attrs.AddAttr(Attribute(Float4AttrId(attrName), attrValue.AsFloat4()));
                    break;
                case StringType:
                    attrs.AddAttr(Attribute(StringAttrId(attrName), attrValue));
                    break;
                case Matrix44Type:
                    attrs.AddAttr(Attribute(Matrix44AttrId(attrName), attrValue.AsMatrix44()));
                    break;
                }
            }
        }
    }

    // handle special cases...
    if (category == "_Environment")
    {
        // TODO
    }
    else if (category == "Tree")
    {
        // TODO
    }
    else
    {
        // handle default game entity
        this->HandleDefaultEntity(category, tmplt, id, entityTransform, attrs);
    }
}


//------------------------------------------------------------------------------
/**
*/
void
CreateEntityCommand::HandleDefaultEntity(const String& category,
                                  const String& tmplt,
                                  const String& id,
                                  const matrix44& entityTransform,
                                  const AttributeContainer& attrs)
{
    EntityManager* entityManager = EntityManager::Instance();

    // check if the entity exists...
    bool wasCreated = false;
    Ptr<Entity> entity;
    if (entityManager->ExistsEntityByAttr(Attr::Attribute(Attr::_ID, id)))
    {
        // entity already exists
        entity = entityManager->GetEntityByAttr(Attr::Attribute(Attr::_ID, id));
    }
    else
    {
        if (tmplt.IsValid())
        {
            // default case: create new entity
            entity = FactoryManager::Instance()->CreateEntityByTemplate(category, tmplt);
        }
        else
        {
            // could be a light
            entity = FactoryManager::Instance()->CreateEntityByAttrs(category, attrs.GetAttrs().ValuesAs<Array<Attribute> >());
        }
        wasCreated = true;
    }

    // transfer attributes
    int attrIndex;
    for (attrIndex = 0; attrIndex < attrs.GetAttrs().Size(); attrIndex++)
    {
        entity->SetAttr(attrs.GetAttrs().ValueAtIndex(attrIndex));
    }

    // set entity id
    entity->SetString(Attr::_ID, id);

    // set transform
    Ptr<BaseGameFeature::SetTransform> msg = BaseGameFeature::SetTransform::Create();
    msg->SetMatrix(entityTransform);
    entity->SendSync(msg.cast<Message>());

    // attach new entities to world
    if (wasCreated)
    {
        EntityManager::Instance()->AttachEntity(entity);
    }
    else
    {
        // notify entity that its attributes have been updated
        Ptr<BaseGameFeature::AttributesUpdated> attrsUpdatedMsg = BaseGameFeature::AttributesUpdated::Create();
        entity->SendSync(attrsUpdatedMsg.cast<Message>());
    }
}
} // namespace Remote
