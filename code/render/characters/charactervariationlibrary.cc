//------------------------------------------------------------------------------
//  CharacterVariationLibrary.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/charactervariationlibrary.h"
#include "coreanimation/jobs/animjobutil.h"

namespace Characters
{
using namespace CoreAnimation;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
CharacterVariationLibrary::CharacterVariationLibrary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterVariationLibrary::~CharacterVariationLibrary()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterVariationLibrary::Setup(const Ptr<AnimResource>& animRes, const CharacterSkeleton& skeleton)
{
    n_assert(!this->IsValid());
    n_assert(animRes.isvalid());
    this->animResource = animRes;

    // sample character variation
    SizeT numVariations = animRes->GetNumClips();
    const Ptr<AnimKeyBuffer>& keyBuffer = animRes->GetKeyBuffer();
    Math::float4* keyPtr = keyBuffer->GetKeyBufferPointer();
    IndexT i;
    for (i = 0; i < numVariations; ++i)
    {
        const CoreAnimation::AnimClip& clip = animRes->GetClipByIndex(i);
        Util::StringAtom variationName = clip.GetName();                   
        // just get the static keys from the variation curves
        SizeT numCurves = clip.GetNumCurves(); 
        SizeT keyStride = clip.GetKeyStride();
        // save as CharJointComponents                      
        Util::FixedArray<CharJointComponents> jointCompArray(skeleton.GetNumJoints());    
        SizeT curveJointRatio = 3; // translationm, scaling, rotation
        IndexT curveIdx;
        for (curveIdx = 0; curveIdx < numCurves; ++curveIdx)
        {      
            IndexT jointIdx = curveIdx / curveJointRatio;
            CharJointComponents& jointComp = jointCompArray[jointIdx];
            const AnimCurve& curve = clip.CurveByIndex(curveIdx);  
            IndexT startKeyIndex = clip.GetStartKeyIndex();
            float4 key;
            if (curve.IsStatic())
            {
                key = curve.GetStaticKey();
            }
            else
            {
                // use last key
                IndexT keyIndex = curve.GetFirstKeyIndex();
                key = keyPtr[keyIndex + (clip.GetNumKeys()-1) * clip.GetKeyStride()];                               
            }
            
            switch (curve.GetCurveType())
            {
            case CurveType::Translation:
                {
                    const vector& poseTranslation = skeleton.GetJoint(jointIdx).GetPoseTranslation();
                    jointComp.varTranslationX = key.x() - poseTranslation.x();
                    jointComp.varTranslationY = key.y() - poseTranslation.y();
                    jointComp.varTranslationZ = key.z() - poseTranslation.z();
                }
                break;
            case CurveType::Scale:  
                jointComp.varScaleX = key.x();
                jointComp.varScaleY = key.y();
                jointComp.varScaleZ = key.z();
                break;
            default:
                break;
            }; 
            // patch parent joint index, to allow direct copy of variation set
            // per joint there a 3 curves (translation, rotation, scale)
            const CharacterJoint& joint = skeleton.GetJoint(jointIdx);
            jointComp.parentJointIndex = joint.GetParentJointIndex();
        }
        this->variationSets.Add(variationName, jointCompArray);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterVariationLibrary::Discard()
{
    n_assert(this->IsValid());
    this->animResource = 0;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<AnimResource>&
CharacterVariationLibrary::GetAnimResource() const
{
    return this->animResource;
}                                    

//------------------------------------------------------------------------------
/**
*/
bool 
CharacterVariationLibrary::HasVariation(const Util::StringAtom& name) const
{
    return this->variationSets.Contains(name);
}
} // namespace Characters
