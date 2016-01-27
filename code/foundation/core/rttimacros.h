#pragma once
//------------------------------------------------------------------------------
/**
    @file core/rttimacros.h
    
    This defines the macros for Nebula3's RTTI mechanism 
    (__DeclareClass, __ImplementClass, etc...).
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file	
*/

//------------------------------------------------------------------------------
/**
    Declaration macro. Put this into the class declaration.
*/
#define __DeclareClass(type) \
public: \
    void* operator new(size_t size) \
    { \
        return RTTI.AllocInstanceMemory(); \
    }; \
    void operator delete(void* p) \
    { \
        RTTI.FreeInstanceMemory(p); \
    }; \
    static Core::Rtti RTTI; \
    static Core::RefCounted* FactoryCreator(); \
    static type* Create(); \
    static bool RegisterWithFactory(); \
    virtual Core::Rtti* GetRtti() const; \
private:

#define __DeclareAbstractClass(class_name) \
public: \
    static Core::Rtti RTTI; \
    virtual Core::Rtti* GetRtti() const; \
private:

//------------------------------------------------------------------------------
/**
    Register a class with the factory. This is only necessary for classes
    which can create objects by name or fourcc.
*/
#define __RegisterClass(type) \
    static const bool type##_registered = type::RegisterWithFactory(); \

//------------------------------------------------------------------------------
/**
    Implementation macro for default memory pool sizes. Put this into the source file.
*/
#if NEBULA3_DEBUG
#define __ImplementClass(type, fourcc, baseType) \
    Core::Rtti type::RTTI(#type, fourcc, type::FactoryCreator, &baseType::RTTI, sizeof(type)); \
    Core::Rtti* type::GetRtti() const { return &this->RTTI; } \
    Core::RefCounted* type::FactoryCreator() { return type::Create(); } \
    type* type::Create() \
    { \
        RefCounted::criticalSection.Enter(); \
        RefCounted::isInCreate = true; \
        type* newObject = n_new(type); \
        RefCounted::isInCreate = false; \
        RefCounted::criticalSection.Leave(); \
        return newObject; \
    }\
    bool type::RegisterWithFactory() \
    { \
        Core::SysFunc::Setup(); \
        if (!Core::Factory::Instance()->ClassExists(#type)) \
        { \
            Core::Factory::Instance()->Register(&type::RTTI, #type, fourcc); \
        } \
        return true; \
    }
#else
#define __ImplementClass(type, fourcc, baseType) \
    Core::Rtti type::RTTI(#type, fourcc, type::FactoryCreator, &baseType::RTTI, sizeof(type)); \
    Core::Rtti* type::GetRtti() const { return &this->RTTI; } \
    Core::RefCounted* type::FactoryCreator() { return type::Create(); } \
    type* type::Create() \
    { \
        return n_new(type); \
    }\
    bool type::RegisterWithFactory() \
    { \
        Core::SysFunc::Setup(); \
        if (!Core::Factory::Instance()->ClassExists(#type)) \
        { \
            Core::Factory::Instance()->Register(&type::RTTI, #type, fourcc); \
        } \
        return true; \
    }
#endif

//------------------------------------------------------------------------------
/**
*/
#define __ImplementAbstractClass(type, fourcc, baseType) \
    Core::Rtti type::RTTI(#type, fourcc, 0, &baseType::RTTI, 0); \
    Core::Rtti* type::GetRtti() const { return &this->RTTI; }

//------------------------------------------------------------------------------
/**
    Type implementation of topmost type in inheritance hierarchy (source file).
*/
#if NEBULA3_DEBUG
#define __ImplementRootClass(type, fourcc) \
    Core::Rtti type::RTTI(#type, fourcc, type::FactoryCreator, 0, sizeof(type)); \
    Core::Rtti* type::GetRtti() const { return &this->RTTI; } \
    Core::RefCounted* type::FactoryCreator() { return type::Create(); } \
    type* type::Create() \
    { \
        RefCounted::criticalSection.Enter(); \
        RefCounted::isInCreate = true; \
        type* newObject = n_new(type); \
        RefCounted::isInCreate = false; \
        RefCounted::criticalSection.Leave(); \
        return newObject; \
    }\
    bool type::RegisterWithFactory() \
    { \
        if (!Core::Factory::Instance()->ClassExists(#type)) \
        { \
            Core::Factory::Instance()->Register(&type::RTTI, #type, fourcc); \
        } \
        return true; \
    }
#else
#define __ImplementRootClass(type, fourcc) \
    Core::Rtti type::RTTI(#type, fourcc, type::FactoryCreator, 0, sizeof(type)); \
    Core::Rtti* type::GetRtti() const { return &this->RTTI; } \
    Core::RefCounted* type::FactoryCreator() { return type::Create(); } \
    type* type::Create() \
    { \
        return n_new(type); \
    }\
    bool type::RegisterWithFactory() \
    { \
        if (!Core::Factory::Instance()->ClassExists(#type)) \
        { \
            Core::Factory::Instance()->Register(&type::RTTI, #type, fourcc); \
        } \
        return true; \
    }
#endif
    
#define __SetupExternalAttributes() \
	public: \
	virtual void SetupExternalAttributes(); \
	private: 
