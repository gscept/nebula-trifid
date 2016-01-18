//------------------------------------------------------------------------------
//  shadow.fx
//
//	Defines shadows for standard geometry
//
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/shared.fxh"
#include "lib/skinning.fxh"
#include "lib/shadowbase.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"

//------------------------------------------------------------------------------
/**
*/
// Spotlight methods
SimpleTechnique(Spotlight, "Spot|Static", 								vsStatic(), psShadow(), ShadowState);
SimpleTechnique(SpotlightAlpha, "Spot|Alpha", 							vsStatic(), psShadowAlpha(), ShadowState);
SimpleTechnique(SpotlightSkinned, "Spot|Skinned", 						vsSkinned(), psShadow(), ShadowState);
SimpleTechnique(SpotlightSkinnedAlpha, "Spot|Skinned|Alpha", 			vsSkinned(), psShadowAlpha(), ShadowState);
SimpleTechnique(SpotlightInstanced, "Spot|Static|Instanced", 			vsStaticInst(), psShadow(), ShadowState);
TessellationTechnique(SpotlightTessellated, "Spot|Static|Tessellated", 	vsTess(), psShadow(), hsShadow(), dsShadow(), ShadowState);

// Pointlight methods
GeometryTechnique(PointlightDefault, "Point|Static", 				vsStaticCSM(), psVSM(), gsPoint(), ShadowState);
GeometryTechnique(PointlightAlpha, "Point|Alpha", 					vsStaticCSM(), psVSMAlpha(), gsPoint(), ShadowState);
GeometryTechnique(PointlightSkinned, "Point|Skinned", 				vsSkinnedCSM(), psVSM(), gsPoint(), ShadowState);
GeometryTechnique(PointlightSkinnedAlpha, "Point|Skinned|Alpha", 	vsSkinnedCSM(), psVSMAlpha(), gsPoint(), ShadowState);
GeometryTechnique(PointlightInstanced, "Point|Static|Instanced", 	vsStaticInstCSM(), psVSM(), gsPoint(), ShadowState);
FullTechnique(PointlightTessellated, "Point|Static|Tessellated", 	vsTessCSM(), psVSM(), hsShadow(), dsCSM(), gsPoint(), ShadowState);

// CSM methods
GeometryTechnique(CSM, "Global|Static", 							vsStaticCSM(), psVSM(), gsCSM(), ShadowStateCSM);
GeometryTechnique(CSMAlpha, "Global|Alpha", 						vsStaticCSM(), psVSMAlpha(), gsCSM(), ShadowStateCSM);
GeometryTechnique(CSMInstanced, "Global|Static|Instanced", 			vsStaticInstCSM(), psVSM(), gsCSM(), ShadowStateCSM);
GeometryTechnique(CSMInstancedAlpha, "Global|Alpha|Instanced", 		vsStaticInstCSM(), psVSMAlpha(), gsCSM(), ShadowStateCSM);
GeometryTechnique(CSMSkinned, "Global|Skinned", 					vsSkinnedCSM(), psVSM(), gsCSM(), ShadowStateCSM);
GeometryTechnique(CSMSkinnedAlpha, "Global|Skinned|Alpha", 			vsSkinnedCSM(), psVSMAlpha(), gsCSM(), ShadowStateCSM);
FullTechnique(CSMTessellated, "Global|Static|Tessellated", 			vsTessCSM(), psVSM(), hsShadow(), dsCSM(), gsCSM(), ShadowStateCSM);