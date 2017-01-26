#version 330 core

#define FXAA_PRESET 3
#define FXAA_HLSL_4 1
#include “FxaaShader.h”
cbuffer cbFxaa : register(b1) {
    float4 rcpFrame : packoffset(c0); };
struct FxaaVS_Output {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0; };
FxaaVS_Output FxaaVS(uint id : SV_VertexID) { FxaaVS_Output Output;
