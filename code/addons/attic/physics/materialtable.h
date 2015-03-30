#ifndef PHYSICS_MATERIALTABLE_H
#define PHYSICS_MATERIALTABLE_H
//------------------------------------------------------------------------------
/**
    @class Physics::MaterialTable

    Contains material properties and friction coefficients.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/types.h"
#include "util/string.h"
#include "util/fixedarray.h"

//------------------------------------------------------------------------------
namespace Physics
{
typedef int MaterialType;
const MaterialType InvalidMaterial = -1;

class MaterialTable
{
public:
    /// initialize material table
    static void Setup();

    /// translate material type to string
    static const Util::String&  MaterialTypeToString(MaterialType t);
    /// translate string to material type
    static MaterialType StringToMaterialType(const Util::String& str);

    /// get density for a material type
    static float GetDensity(MaterialType t);
    /// get friction coefficient for 2 materials
    static float GetFriction(MaterialType t0, MaterialType t1);
    /// get bounce for 2 materials
    static float GetBounce(MaterialType t0, MaterialType t1);
    /// get collision sound for two material
    static const Util::String& GetCollisionSound(MaterialType t0, MaterialType t1);

    /// set density of material type
    static void SetDensity(MaterialType type, float density);
    /// set friction of two material types
    static void SetFriction(MaterialType t0, MaterialType t1, float friction);
    /// set bouncyness of two material types
    static void SetBouncyness(MaterialType t0, MaterialType t1, float bouncyness);

private:
    /// constructor
    MaterialTable();
    /// destructor
    ~MaterialTable();

    struct Material 
    {
        Material() : name(), density(1.0f) {}
        Util::String name; 
        float density; 
    };
    struct Interaction 
    { 
        Interaction() : friction(0.0f), bouncyness(0.0f) {}
        float friction; 
        float bouncyness; 
        Util::String collSound;
    };

    static Util::String invalidTypeString;
    static Util::FixedArray<Material> materials;
    static Util::FixedArray<Util::FixedArray<Interaction> > interactions;
};

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    
