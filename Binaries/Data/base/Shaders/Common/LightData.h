#pragma once

// clang-format off

#if SHADING_QUALITY != SHADING_QUALITY_NORMAL
#error "Functions in LightData.h are only for NORMAL shading quality. Todo: Split up file"
#endif

#include "Platforms.h"
#include "ConstantBufferMacros.h"

#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_SPOT 1
#define LIGHT_TYPE_DIR 2

struct XII_SHADER_STRUCT xiiPerLightData
{
  UINT1(colorAndType);
  FLOAT1(intensity);
  UINT1(direction); // 10 bits fixed point per axis
  UINT1(shadowDataOffset);

  FLOAT3(position);
  FLOAT1(invSqrAttRadius);

  UINT1(spotParams); // scale and offset as 16 bit floats
  UINT1(projectorAtlasOffset); // xy as 16 bit floats
  UINT1(projectorAtlasScale); // xy as 16 bit floats

  UINT1(reserved);
};

#if XII_ENABLED(PLATFORM_SHADER)
  StructuredBuffer<xiiPerLightData> perLightDataBuffer;
#else
  XII_CHECK_AT_COMPILETIME(sizeof(xiiPerLightData) == 48);
#endif

struct XII_SHADER_STRUCT xiiPointShadowData
{
  FLOAT4(shadowParams); // x = slope bias, y = constant bias, z = penumbra size in texel, w = fadeout
  MAT4(worldToLightMatrix)[6];
};

struct XII_SHADER_STRUCT xiiSpotShadowData
{
  FLOAT4(shadowParams); // x = slope bias, y = constant bias, z = penumbra size in texel, w = fadeout
  MAT4(worldToLightMatrix);
};

struct XII_SHADER_STRUCT xiiDirShadowData
{
  FLOAT4(shadowParams); // x = slope bias, y = constant bias, z = penumbra size in texel, w = num cascades
  MAT4(worldToLightMatrix);
  FLOAT4(shadowParams2); // x = cascade border threshold, y = xy dither multiplier, z = z dither multiplier, w = penumbra size increment
  FLOAT4(fadeOutParams); // x = xy fadeout scale, y = xy fadeout offset, z = z fadeout scale, w = z fadeout offset
  FLOAT4(cascadeScaleOffset)[6]; // interleaved, maxNumCascades - 1 since first cascade has identity scale and offset
  FLOAT4(atlasScaleOffset)[4];

};

#define GET_SHADOW_PARAMS_INDEX(baseOffset) (baseOffset + 0)
#define GET_WORLD_TO_LIGHT_MATRIX_INDEX(baseOffset, index) (baseOffset + 1 + 4 * (index))
#define GET_SHADOW_PARAMS2_INDEX(baseOffset) (baseOffset + 5)
#define GET_FADE_OUT_PARAMS_INDEX(baseOffset) (baseOffset + 6)
#define GET_CASCADE_SCALE_INDEX(baseOffset, index) (baseOffset + 7 + 2 * (index))
#define GET_CASCADE_OFFSET_INDEX(baseOffset, index) (baseOffset + 8 + 2 * (index))
#define GET_ATLAS_SCALE_OFFSET_INDEX(baseOffset, index) (baseOffset + 13 + (index))

#if XII_ENABLED(PLATFORM_SHADER)
  StructuredBuffer<float4> shadowDataBuffer;
#endif

#define DECAL_USE_NORMAL (1 << 0)
#define DECAL_USE_ORM (1 << 1)
#define DECAL_USE_EMISSIVE (1 << 2)
#define DECAL_BLEND_MODE_COLORIZE (1 << 7)
#define DECAL_WRAP_AROUND (1 << 8)
#define DECAL_MAP_NORMAL_TO_GEOMETRY (1 << 9)

struct XII_SHADER_STRUCT xiiPerDecalData
{
  TRANSFORM(worldToDecalMatrix);

  UINT1(applyOnlyToId);
  UINT1(decalFlags);
  UINT1(angleFadeParams); // scale and offset as 16 bit floats
  UINT1(baseColor);

  UINT1(emissiveColorRG); // as 16 bit floats
  UINT1(emissiveColorBA); // as 16 bit floats

  UINT1(baseColorAtlasScale); // xy as 16 bit floats
  UINT1(baseColorAtlasOffset); // xy as 16 bit floats

  UINT1(normalAtlasScale); // xy as 16 bit floats
  UINT1(normalAtlasOffset); // xy as 16 bit floats

  UINT1(ormAtlasScale); // xy as 16 bit floats
  UINT1(ormAtlasOffset); // xy as 16 bit floats
};

#if XII_ENABLED(PLATFORM_SHADER)
  StructuredBuffer<xiiPerDecalData> perDecalDataBuffer;
#else // C++
  XII_CHECK_AT_COMPILETIME(sizeof(xiiPerDecalData) == 96);
#endif

#define REFLECTION_PROBE_IS_SPHERE (1 << 31)
#define REFLECTION_PROBE_IS_PROJECTED (1 << 30)
#define REFLECTION_PROBE_INDEX_BITMASK 0x3FFFFFFF
#define GET_REFLECTION_PROBE_INDEX(index) (index & REFLECTION_PROBE_INDEX_BITMASK)

  struct XII_SHADER_STRUCT xiiPerReflectionProbeData
  {
    TRANSFORM(WorldToProbeProjectionMatrix);
    FLOAT4(Scale);
    FLOAT4(ProbePosition);
    FLOAT4(PositiveFalloff);
    FLOAT4(NegativeFalloff);
    FLOAT4(InfluenceScale);
    FLOAT4(InfluenceShift);
    UINT1(Index);
    UINT1(Padding1);
    UINT1(Padding2);
    UINT1(Padding3);
  };

#if XII_ENABLED(PLATFORM_SHADER)
  StructuredBuffer<xiiPerReflectionProbeData> perPerReflectionProbeDataBuffer;
#else // C++
  XII_CHECK_AT_COMPILETIME(sizeof(xiiPerReflectionProbeData) == 160);
#endif

  CONSTANT_BUFFER(xiiClusteredDataConstants, 3)
  {
    FLOAT1(DepthSliceScale);
    FLOAT1(DepthSliceBias);
    FLOAT2(InvTileSize);

    UINT1(NumLights);
    UINT1(NumDecals);
    UINT1(Padding);

    UINT1(SkyIrradianceIndex);

    FLOAT1(FogHeight);
    FLOAT1(FogHeightFalloff);
    FLOAT1(FogDensityAtCameraPos);
    FLOAT1(FogDensity);
    COLOR4F(FogColor);
    FLOAT1(FogInvSkyDistance);
};

#define NUM_CLUSTERS_X 16
#define NUM_CLUSTERS_Y 8
#define NUM_CLUSTERS_Z 24
#define NUM_CLUSTERS_XY (NUM_CLUSTERS_X * NUM_CLUSTERS_Y)
#define NUM_CLUSTERS (NUM_CLUSTERS_X * NUM_CLUSTERS_Y * NUM_CLUSTERS_Z)

#define LIGHT_BITMASK 0x3FF
#define DECAL_SHIFT 10
#define DECAL_BITMASK 0x3FF
#define PROBE_SHIFT 20
#define PROBE_BITMASK 0x3FF

#define GET_LIGHT_INDEX(index) (index & LIGHT_BITMASK)
#define GET_DECAL_INDEX(index) ((index >> DECAL_SHIFT) & DECAL_BITMASK)
#define GET_PROBE_INDEX(index) ((index >> PROBE_SHIFT) & PROBE_BITMASK)

struct xiiPerClusterData
{
  UINT1(offset);
  UINT1(counts);
};

#if XII_ENABLED(PLATFORM_SHADER)
  StructuredBuffer<xiiPerClusterData> perClusterDataBuffer;
  StructuredBuffer<uint> clusterItemBuffer;
#endif

  // clang-format on
