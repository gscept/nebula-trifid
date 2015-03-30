//------------------------------------------------------------------------------
//  CSM.fxh
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#define PCF 0
#define BLURSAMPLES 1
#define NO_COMPARISON 1

#include "shadowbase.fxh"
mat4          	CSMShadowMatrix;

vec4          	CascadeOffset[CASCADE_COUNT_FLAG];
vec4          	CascadeScale[CASCADE_COUNT_FLAG];
float			CascadeBlendArea = 0.3f;

float           MinBorderPadding;     
float           MaxBorderPadding;
float           ShadowPartitionSize; 
float 			GlobalLightShadowBias = 0.0f;
	
const int SplitsPerRow = 2;
const int SplitsPerColumn = 2;

sampler2D ShadowProjMap;            

samplerstate ShadowProjMapSampler
{
	Samplers = { ShadowProjMap };
	//Filter = MinMagLinearMipPoint;
	AddressU = Clamp;
	AddressV = Clamp;
	//BorderColor = { 1,1,1,1 };
};

const float FLT_MIN = 1E-37;

const vec4 colors[] = {
	vec4(1, 0, 0, 0),
	vec4(0, 1, 0, 0),
	vec4(0, 0, 1, 0),
	vec4(1, 1, 1, 0)
};

const vec2 kernel[] = {
	vec2(0.5142, 0.6165),
	vec2(-0.03512, -0.02231),
	vec2(0.04147, 0.01242),
	vec2(-0.02526, 0.01662),
	vec2(0.03134, -0.02526),
	vec2(-0.03725, 0.01724),
	vec2(0.01252, 0.038314),
	vec2(0.02127, -0.02667),
	vec2(-0.01231, 0.04142),
	vec2(0.03124, 0.02253),
	vec2(-0.05661, -0.07112),
	vec2(0.09937, 0.02521),
	vec2(0.04132, 0.04566),
	vec2(-0.07264, -0.01778),
	vec2(0.03831, 0.08636),
	vec2(-0.03573, -0.07284)
};

//------------------------------------------------------------------------------
/**
*/
vec4 ConvertViewRayToWorldPos(vec3 viewRay, float length, vec3 cameraPosition)
{
	return vec4(cameraPosition + viewRay * length, 1);
}

//------------------------------------------------------------------------------
/**
	Converts World Position into shadow texture lookup vector, modelviewprojection position and interpolated position, as well as depth
*/
void CSMConvert(in vec4 worldPosition,	
				out vec4 texShadow)
{       
    // Transform the shadow texture coordinates for all the cascades.
    texShadow = CSMShadowMatrix * worldPosition;
}

const vec3 sampleOffsetWeights[] = {
    vec3( -1.5,  0.5, 0.024882 ),
    vec3( -0.5, -0.5, 0.067638 ),
    vec3( -0.5,  0.5, 0.111515 ),
    vec3( -0.5,  1.5, 0.067638 ),
    vec3(  0.5, -1.5, 0.024882 ),
    vec3(  0.5, -0.5, 0.111515 ),
    vec3(  0.5,  0.5, 0.183858 ),
    vec3(  0.5,  1.5, 0.111515 ),
    vec3(  0.5,  2.5, 0.024882 ),
    vec3(  1.5, -0.5, 0.067638 ),
    vec3(  1.5,  0.5, 0.111515 ),
    vec3(  1.5,  1.5, 0.067638 ),
    vec3(  2.5,  0.5, 0.024882 )
};

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
float 
GetInvertedOcclusionCSM(vec2 lightSpaceUv,
					 float lightSpaceDepth)
{
	/*
	vec2 shadowSample = vec2(0);
	vec2 pixelSize = GetPixelSize(ShadowProjMap);
	int i;
    for (i = 0; i < 13; i++)
	{
		shadowSample += textureLod(ShadowProjMap, lightSpaceUv + sampleOffsetWeights[i].xy * pixelSize.xy, 0).rg;
	}
	shadowSample /= 13.0f;
	
	
	for (float y = -1.5f; y < 1.5f; y += 1.0f)
	{
		for (float x = -1.5f; x < 1.5f; x += 1.0f)
		{
			shadowSample += textureLod(ShadowProjMap, lightSpaceUv, 0).rg;
		}
	}
	*/
	//shadowSample /= 4.0f;
	// get pixel size of shadow projection texture
	vec2 shadowSample = textureLod(ShadowProjMap, lightSpaceUv, 0).rg;
	return ChebyshevUpperBound(shadowSample, lightSpaceDepth, FLT_MIN);
}

//------------------------------------------------------------------------------
/**
*/
void 
CalculateBlendAmountForMap ( in vec4 vShadowMapTextureCoord, 
                             out float fCurrentPixelsBlendBandLocation,
                             out float fBlendBetweenCascadesAmount ) 
{
    // Calcaulte the blend band for the map based selection.
    vec2 distanceToOne = vec2 ( 1.0f - vShadowMapTextureCoord.x, 1.0f - vShadowMapTextureCoord.y );
    fCurrentPixelsBlendBandLocation = min( vShadowMapTextureCoord.x, vShadowMapTextureCoord.y );
    float fCurrentPixelsBlendBandLocation2 = min( distanceToOne.x, distanceToOne.y );
    fCurrentPixelsBlendBandLocation = 
        min( fCurrentPixelsBlendBandLocation, fCurrentPixelsBlendBandLocation2 );
    fBlendBetweenCascadesAmount = fCurrentPixelsBlendBandLocation / CascadeBlendArea;
}

//------------------------------------------------------------------------------
/**
	CSM shadow sampling entry point
*/
float CSMPS(in vec4 TexShadow,
			in vec2 FSUV)
{
	vec4 vShadowMapTextureCoord = vec4(0.0f);
    
    float fPercentLit = 1.0f;
    int iCurrentCascadeIndex = 0;
	bool iCascadeFound = false;
	float bias = GlobalLightShadowBias;
    
    // This for loop is not necessary when the frustum is uniformly divided and interval based selection is used.
    // In this case fCurrentPixelDepth could be used as an array lookup into the correct frustum. 
    vec4 vShadowMapTextureCoordViewSpace = TexShadow;
	
	
	int iCascadeIndex;
	int iNextCascadeIndex;
	for( iCascadeIndex = 0; iCascadeIndex < CASCADE_COUNT_FLAG; ++iCascadeIndex) 
	{
		vShadowMapTextureCoord = vShadowMapTextureCoordViewSpace * CascadeScale[iCascadeIndex];
		vShadowMapTextureCoord += CascadeOffset[iCascadeIndex];

		if ( min( vShadowMapTextureCoord.x, vShadowMapTextureCoord.y ) > MinBorderPadding
		  && max( vShadowMapTextureCoord.x, vShadowMapTextureCoord.y ) < MaxBorderPadding )
		{ 
			iCurrentCascadeIndex = iCascadeIndex;   
			iCascadeFound = true;
			break;
		}
	}
	
	float fBlendBetweenCascadesAmount = 0;
	float fCurrentPixelsBlendBandLocation = 0;
	CalculateBlendAmountForMap ( vShadowMapTextureCoord, fCurrentPixelsBlendBandLocation, fBlendBetweenCascadesAmount );
				
	// if we have no matching cascade, return with a fully lit pixel
	if (!iCascadeFound)
	{
		return 1.0f;
	}		
	
	// calculate texture coordinate in shadow space
	vec2 texCoord = vShadowMapTextureCoord.xy / vShadowMapTextureCoord.ww;
	float depth = vShadowMapTextureCoord.z / vShadowMapTextureCoord.w;

	vec2 sampleCoord = texCoord;
	sampleCoord.xy *= ShadowPartitionSize;
	sampleCoord.xy += vec2(mod(iCurrentCascadeIndex, SplitsPerRow) * ShadowPartitionSize, (iCurrentCascadeIndex / SplitsPerColumn) * ShadowPartitionSize);
	
	float occlusion = GetInvertedOcclusionCSM(sampleCoord, depth);
	
	// smoothstep current sample in order to counteract light bleeding
	occlusion = smoothstep(0.5f, 1, occlusion);
	
	iNextCascadeIndex = iCurrentCascadeIndex + 1; 
	float occlusionBlend = 1.0f;
	if (fCurrentPixelsBlendBandLocation < CascadeBlendArea)
	{
		if (iNextCascadeIndex < CASCADE_COUNT_FLAG)
		{
			vShadowMapTextureCoord = vShadowMapTextureCoordViewSpace * CascadeScale[iNextCascadeIndex];
			vShadowMapTextureCoord += CascadeOffset[iNextCascadeIndex];
			
			sampleCoord = vShadowMapTextureCoord.xy / vShadowMapTextureCoord.ww;
			depth = vShadowMapTextureCoord.z / vShadowMapTextureCoord.w;
			
			sampleCoord.xy *= ShadowPartitionSize;
			sampleCoord.xy += vec2(mod(iNextCascadeIndex, SplitsPerRow) * ShadowPartitionSize, (iNextCascadeIndex / SplitsPerColumn) * ShadowPartitionSize);
			
			occlusionBlend = GetInvertedOcclusionCSM(sampleCoord, depth);		
			//occlusionBlend = smoothstep(0.98888f, 1, occlusionBlend);
		}
		
		// blend next cascade onto previous
		occlusion = lerp(occlusionBlend, occlusion, fBlendBetweenCascadesAmount);
	}
	
	// finally clamp all shadow values 0.5, this avoids any weird bleeding from the cascade blending
	return smoothstep(0.5f, 1, occlusion);
}
