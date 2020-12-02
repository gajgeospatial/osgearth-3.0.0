// CMake will compile this file into AutoGenShaders.c""

#include <osgEarthSplat/SplatShaders>

//#define MULTILINE(...) #__VA_ARGS__

using namespace osgEarth::Splat;

SplattingShaders::SplattingShaders()
{
    Types = "Splat.types.glsl";
    _sources[Types] = "// begin: Splat.types.glsl%EOL%%EOL%// Environment structure passed around locally.%EOL%struct oe_SplatEnv {%EOL%    float lod;%EOL%    float range;%EOL%    float elevation;%EOL%    float slope;%EOL%    vec4 noise;%EOL%};%EOL%%EOL%// Rendering parameters for splat texture and noise-based detail texture.%EOL%struct oe_SplatRenderInfo {%EOL%    float primaryIndex;%EOL%    float detailIndex;%EOL%    float materialIndex;%EOL%    float brightness;%EOL%    float contrast;%EOL%    float threshold;%EOL%    float minSlope;%EOL%};%EOL%%EOL%// end: Splat.types.glsl%EOL%";

    Noise = "Splat.Noise.glsl";
    _sources[Noise] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%//%EOL%// Description : Array and textureless GLSL 2D/3D/4D simplex %EOL%//               noise functions.%EOL%//      Author : Ian McEwan, Ashima Arts.%EOL%//  Maintainer : ijm%EOL%//     Lastmod : 20110822 (ijm)%EOL%//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.%EOL%//               Distributed under the MIT License. See LICENSE file.%EOL%//               https://github.com/ashima/webgl-noise%EOL%// %EOL%%EOL%vec4 oe_noise_mod289(vec4 x) {%EOL%  return x - floor(x * (1.0 / 289.0)) * 289.0; }%EOL%%EOL%float oe_noise_mod289(float x) {%EOL%  return x - floor(x * (1.0 / 289.0)) * 289.0; }%EOL%%EOL%vec4 oe_noise_permute(vec4 x) {%EOL%     return oe_noise_mod289(((x*34.0)+1.0)*x);%EOL%}%EOL%%EOL%float oe_noise_permute(float x) {%EOL%     return oe_noise_mod289(((x*34.0)+1.0)*x);%EOL%}%EOL%%EOL%vec4 oe_noise_taylorInvSqrt(vec4 r)%EOL%{%EOL%  return 1.79284291400159 - 0.85373472095314 * r;%EOL%}%EOL%%EOL%float oe_noise_taylorInvSqrt(float r)%EOL%{%EOL%  return 1.79284291400159 - 0.85373472095314 * r;%EOL%}%EOL%%EOL%vec4 oe_noise_grad4(float j, vec4 ip)%EOL%  {%EOL%  const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);%EOL%  vec4 p,s;%EOL%%EOL%  p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;%EOL%  p.w = 1.5 - dot(abs(p.xyz), ones.xyz);%EOL%  s = vec4(lessThan(p, vec4(0.0)));%EOL%  p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www; %EOL%%EOL%  return p;%EOL%  }%EOL%						%EOL%// (sqrt(5) - 1)/4 = F4, used once below%EOL%#define oe_noise_F4 0.309016994374947451%EOL%%EOL%float oe_noise_snoise(vec4 v)%EOL%  {%EOL%  const vec4  C = vec4( 0.138196601125011,  // (5 - sqrt(5))/20  G4%EOL%                        0.276393202250021,  // 2 * G4%EOL%                        0.414589803375032,  // 3 * G4%EOL%                       -0.447213595499958); // -1 + 4 * G4%EOL%%EOL%// First corner%EOL%  vec4 i  = floor(v + dot(v, vec4(oe_noise_F4)) );%EOL%  vec4 x0 = v -   i + dot(i, C.xxxx);%EOL%%EOL%// Other corners%EOL%%EOL%// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)%EOL%  vec4 i0;%EOL%  vec3 isX = step( x0.yzw, x0.xxx );%EOL%  vec3 isYZ = step( x0.zww, x0.yyz );%EOL%//  i0.x = dot( isX, vec3( 1.0 ) );%EOL%  i0.x = isX.x + isX.y + isX.z;%EOL%  i0.yzw = 1.0 - isX;%EOL%//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );%EOL%  i0.y += isYZ.x + isYZ.y;%EOL%  i0.zw += 1.0 - isYZ.xy;%EOL%  i0.z += isYZ.z;%EOL%  i0.w += 1.0 - isYZ.z;%EOL%%EOL%  // i0 now contains the unique values 0,1,2,3 in each channel%EOL%  vec4 i3 = clamp( i0, 0.0, 1.0 );%EOL%  vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );%EOL%  vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );%EOL%%EOL%  //  x0 = x0 - 0.0 + 0.0 * C.xxxx%EOL%  //  x1 = x0 - i1  + 1.0 * C.xxxx%EOL%  //  x2 = x0 - i2  + 2.0 * C.xxxx%EOL%  //  x3 = x0 - i3  + 3.0 * C.xxxx%EOL%  //  x4 = x0 - 1.0 + 4.0 * C.xxxx%EOL%  vec4 x1 = x0 - i1 + C.xxxx;%EOL%  vec4 x2 = x0 - i2 + C.yyyy;%EOL%  vec4 x3 = x0 - i3 + C.zzzz;%EOL%  vec4 x4 = x0 + C.wwww;%EOL%%EOL%// Permutations%EOL%  i = oe_noise_mod289(i); %EOL%  float j0 = oe_noise_permute( oe_noise_permute( oe_noise_permute( oe_noise_permute(i.w) + i.z) + i.y) + i.x);%EOL%  vec4 j1 = oe_noise_permute( oe_noise_permute( oe_noise_permute( oe_noise_permute (%EOL%             i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))%EOL%           + i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))%EOL%           + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))%EOL%           + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));%EOL%%EOL%// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope%EOL%// 7*7*6 = 294, which is close to the ring size 17*17 = 289.%EOL%  vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;%EOL%%EOL%  vec4 p0 = oe_noise_grad4(j0,   ip);%EOL%  vec4 p1 = oe_noise_grad4(j1.x, ip);%EOL%  vec4 p2 = oe_noise_grad4(j1.y, ip);%EOL%  vec4 p3 = oe_noise_grad4(j1.z, ip);%EOL%  vec4 p4 = oe_noise_grad4(j1.w, ip);%EOL%%EOL%// Normalise gradients%EOL%  vec4 norm = oe_noise_taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));%EOL%  p0 *= norm.x;%EOL%  p1 *= norm.y;%EOL%  p2 *= norm.z;%EOL%  p3 *= norm.w;%EOL%  p4 *= oe_noise_taylorInvSqrt(dot(p4,p4));%EOL%%EOL%// Mix contributions from the five corners%EOL%  vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);%EOL%  vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);%EOL%  m0 = m0 * m0;%EOL%  m1 = m1 * m1;%EOL%  return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))%EOL%               + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;%EOL%}%EOL%%EOL%// Generates a tiled fractal simplex noise value and clamps the values to [0..1].%EOL%float oe_noise_fractal4D(in vec2 seed, in float frequency, in float persistence, in float lacunarity, in int octaves)%EOL%{%EOL%    const float TwoPI = 6.283185;%EOL%	float f = frequency;%EOL%	float amp = 1.0;%EOL%	float maxAmp = 0.0;%EOL%	float n = 0.0;%EOL%    %EOL%    vec4 seed4D;%EOL%    seed4D.xy = cos(seed*TwoPI)/TwoPI;%EOL%    seed4D.zw = sin(seed*TwoPI)/TwoPI;%EOL%%EOL%	for(int i=0; i<octaves; ++i)%EOL%	{%EOL%		n += oe_noise_snoise(seed4D*f) * amp;%EOL%		maxAmp += amp;%EOL%		amp *= persistence;%EOL%		f *= lacunarity;%EOL%	}%EOL%	//return n / maxAmp;%EOL%    const float low = 0.0;%EOL%    const float high = 1.0;%EOL%%EOL%    n /= maxAmp;%EOL%    n = n * (high-low)/2.0 + (high+low)/2.0;%EOL%    return clamp(n, 0.0, 1.0);%EOL%}%EOL%%EOL%";

    VertModel = "Splat.vert.model.glsl";
    _sources[VertModel] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#pragma vp_entryPoint oe_splat_vertex_model%EOL%#pragma vp_location   vertex_model%EOL%#pragma vp_order      0.5%EOL%#pragma import_defines(OE_TERRAIN_RENDER_NORMAL_MAP)%EOL%%EOL%// Transmit the approximate terrain slope if we're not rendering normal maps%EOL%// in the terrain engine.%EOL%#ifndef OE_TERRAIN_RENDER_NORMAL_MAP%EOL%vec3 vp_Normal; // stage global%EOL%out float oe_splat_slope;%EOL%%EOL%void oe_splat_vertex_model(inout vec4 VertexMODEL)%EOL%{%EOL%    // calculate slope from the Z component of the current normal%EOL%    // since the terrain is in LTP space. This is only used when normal maps%EOL%    // are not available, which is hopefully never :/%EOL%    oe_splat_slope = 1.0-vp_Normal.z;%EOL%}%EOL%%EOL%#else%EOL%%EOL%void oe_splat_vertex_model(inout vec4 VertexMODEL)%EOL%{%EOL%    //nop%EOL%}%EOL%%EOL%#endif%EOL%%EOL%";

    VertView = "Splat.vert.view.glsl";
    _sources[VertView] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#pragma vp_entryPoint oe_splat_vertex_view%EOL%#pragma vp_location   vertex_view%EOL%%EOL%#pragma include Splat.types.glsl%EOL%%EOL%#pragma import_defines(OE_LANDCOVER_TEX)%EOL%#pragma import_defines(OE_LANDCOVER_TEX_MATRIX)%EOL%%EOL%out vec4 oe_layer_tilec;%EOL%out float oe_splat_range;%EOL%out vec2 oe_splat_covtc;%EOL%%EOL%uniform sampler2D OE_LANDCOVER_TEX;%EOL%uniform mat4 OE_LANDCOVER_TEX_MATRIX;%EOL%%EOL%flat out float oe_splat_coverageTexSize;%EOL%%EOL%//uniform mat4 OE_SPLAT_COVERAGE_TEXMAT;   // assigned at runtime%EOL%%EOL%uniform vec3 oe_Camera; // (vp width, vp height, LOD scale)%EOL%%EOL%%EOL%void oe_splat_vertex_view(inout vec4 VertexVIEW)%EOL%{%EOL%    // range from camera to vertex%EOL%    oe_splat_range = -VertexVIEW.z * oe_Camera.z; // apply LOD scale%EOL%%EOL%    // calculate the coverage sampling coordinates. The texture matrix accounts%EOL%    // for any super-sampling that might be in effect for the current LOD.%EOL%    oe_splat_covtc = (OE_LANDCOVER_TEX_MATRIX * oe_layer_tilec).st;%EOL%%EOL%    // Precalculate the size of the coverage texture. This is faster than%EOL%    // calling textureSize per pixel in the fragment shader.%EOL%    oe_splat_coverageTexSize = textureSize(OE_LANDCOVER_TEX, 0).x;%EOL%}%EOL%%EOL%";

    Frag = "Splat.frag.glsl";
    _sources[Frag] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#if(__VERSION__ < 400)%EOL%    #extension GL_ARB_gpu_shader5 : enable  // textureGather%EOL%#endif%EOL%%EOL%#pragma vp_entryPoint oe_splat_sampleCoverage%EOL%#pragma vp_location   fragment_coloring%EOL%#pragma vp_order      0.2%EOL%%EOL%#pragma import_defines(OE_LANDCOVER_TEX);%EOL%%EOL%uniform sampler2D OE_LANDCOVER_TEX;%EOL%%EOL%//uniform sampler2D oe_splat_coverageTex;%EOL%in vec2 oe_splat_covtc;%EOL%flat in float oe_splat_coverageTexSize;%EOL%%EOL%// stage global: set this and read it in the splatter.%EOL%vec4 oe_LandCover_coverage;%EOL%%EOL%// read the comment below regarding textureGather%EOL%//#define USE_TEXTURE_GATHER%EOL%%EOL%void oe_splat_sampleCoverage(inout vec4 unused)%EOL%{%EOL%#ifdef USE_TEXTURE_GATHER%EOL%    // A wee bit faster, but causes a rendering anomaly%EOL%    oe_splat_coverage = textureGather(oe_tile_landCoverTex, oe_splat_covtc, 0).wzxy;%EOL%#else%EOL%%EOL%    float pixelWidth = 1.0/oe_splat_coverageTexSize;%EOL%    float halfPixelWidth = pixelWidth * 0.5;%EOL%%EOL%    // Find the four quantized coverage coordinates that form a box around the actual%EOL%    // coverage coordinates, where each quantized coord is at the center of a coverage texel.%EOL%    vec2 rem = mod(oe_splat_covtc, pixelWidth);%EOL%    vec2 sw;%EOL%    sw.x = oe_splat_covtc.x - rem.x + (rem.x >= halfPixelWidth ? halfPixelWidth : -halfPixelWidth);%EOL%    sw.y = oe_splat_covtc.y - rem.y + (rem.y >= halfPixelWidth ? halfPixelWidth : -halfPixelWidth); %EOL%    vec2 ne = sw + pixelWidth;%EOL%    vec2 nw = vec2(sw.x, ne.y);%EOL%    vec2 se = vec2(ne.x, sw.y);%EOL%%EOL%    oe_LandCover_coverage = vec4(%EOL%        texture(OE_LANDCOVER_TEX, sw).r,%EOL%        texture(OE_LANDCOVER_TEX, se).r,%EOL%        texture(OE_LANDCOVER_TEX, nw).r,%EOL%        texture(OE_LANDCOVER_TEX, ne).r );%EOL%%EOL%    //return vec4(%EOL%    //    texture(oe_splat_coverageTex, clamp(sw, 0.0, 1.0)).r,%EOL%    //    texture(oe_splat_coverageTex, clamp(se, 0.0, 1.0)).r,%EOL%    //    texture(oe_splat_coverageTex, clamp(nw, 0.0, 1.0)).r,%EOL%    //    texture(oe_splat_coverageTex, clamp(ne, 0.0, 1.0)).r );%EOL%#endif%EOL%}%EOL%%EOL%%EOL%%EOL%[break]%EOL%%EOL%%EOL%    %EOL%#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%// order of 1.1 allows user the opportunity to override oe_splat_coverage stage global%EOL%#pragma vp_entryPoint oe_splat_complex%EOL%#pragma vp_location   fragment_coloring%EOL%#pragma vp_order      1.1%EOL%%EOL%// include files%EOL%#pragma include Splat.types.glsl%EOL%%EOL%// statset defines%EOL%#pragma import_defines(OE_SPLAT_NOISE_SAMPLER)%EOL%#pragma import_defines(OE_SPLAT_EDIT_MODE)%EOL%#pragma import_defines(OE_SPLAT_GPU_NOISE)%EOL%#pragma import_defines(OE_TERRAIN_RENDER_NORMAL_MAP)%EOL%#pragma import_defines(OE_TERRAIN_BLEND_IMAGERY)%EOL%#pragma import_defines(OE_SPLAT_USE_MATERIALS)%EOL%%EOL%// from: Splat.util.glsl%EOL%void oe_splat_getLodBlend(in float range, out float lod0, out float rangeOuter, out float rangeInner, out float clampedRange);%EOL%%EOL%// from terrain SDK:%EOL%vec2 oe_terrain_scaleCoordsToRefLOD(in vec2 tc, in float refLOD);%EOL%%EOL%// from the terrain engine:%EOL%in vec4 oe_layer_tilec;                     // unit tile coords%EOL%%EOL%// from the vertex shader:%EOL%in vec2 oe_splat_covtc;                     // coverage texture coords%EOL%in float oe_splat_range;                    // distance from camera to vertex%EOL%flat in float oe_splat_coverageTexSize;     // size of coverage texture%EOL%%EOL%in float oe_layer_opacity;%EOL%%EOL%// stage global: coverage quad-value set in oe_splat_sampleCoverage%EOL%vec4 oe_LandCover_coverage;%EOL%%EOL%// from SplatLayerFactory:%EOL%uniform sampler2DArray oe_splatTex;%EOL%uniform int oe_splat_scaleOffsetInt;%EOL%%EOL%uniform float oe_splat_detailRange;%EOL%uniform float oe_splat_noiseScale;%EOL%%EOL%#ifdef OE_SPLAT_EDIT_MODE%EOL%uniform float oe_splat_brightness;%EOL%uniform float oe_splat_contrast;%EOL%uniform float oe_splat_threshold;%EOL%uniform float oe_splat_minSlope;%EOL%#endif%EOL%%EOL%// lookup table containing the coverage value => texture index mappings%EOL%uniform samplerBuffer oe_splat_coverageLUT;%EOL%%EOL%uniform int oe_layer_order;%EOL%%EOL%//............................................................................%EOL%// Get the slope of the terrain%EOL%%EOL%#ifdef OE_TERRAIN_RENDER_NORMAL_MAP%EOL%// import SDK%EOL%vec4 oe_terrain_getNormalAndCurvature(in vec2);%EOL%%EOL%// normal map version:%EOL%in vec2 oe_normalMapCoords;%EOL%%EOL%float oe_splat_getSlope()%EOL%{%EOL%    vec4 encodedNormal = oe_terrain_getNormalAndCurvature( oe_normalMapCoords );%EOL%    vec3 normalTangent = normalize(encodedNormal.xyz*2.0-1.0);%EOL%    return clamp((1.0-normalTangent.z)/0.8, 0.0, 1.0);%EOL%}%EOL%%EOL%#else // !OE_TERRAIN_RENDER_NORMAL_MAP%EOL%%EOL%// non- normal map version:%EOL%in float oe_splat_slope;%EOL%%EOL%float oe_splat_getSlope()%EOL%{%EOL%    return oe_splat_slope;%EOL%}%EOL%%EOL%#endif // OE_TERRAIN_RENDER_NORMAL_MAP%EOL%%EOL%%EOL%//............................................................................%EOL%// reads the encoded splatting render information for a coverage value.%EOL%// this data was encoded in Surface::createLUTBUffer().%EOL%%EOL%void oe_splat_getRenderInfo(in float value, in oe_SplatEnv env, out oe_SplatRenderInfo ri)%EOL%{%EOL%    const int num_lods = 26;%EOL%%EOL%    int lutIndex = int(value)*num_lods + int(env.lod);%EOL%%EOL%    // fetch the splatting parameters:%EOL%    vec4 t = texelFetch(oe_splat_coverageLUT, lutIndex);%EOL%%EOL%    ri.primaryIndex = float((int(t[0])>>8)-1);%EOL%    ri.detailIndex  = float((int(t[0])&0xFF)-1);%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%    ri.materialIndex = float(int(t[1])-1);%EOL%#endif%EOL%%EOL%    // brightness and contrast are packed into one float:%EOL%    ri.brightness   = trunc(t[2])/100.0;%EOL%    ri.contrast     = fract(t[2])*10.0;%EOL%%EOL%    // threshold and slope are packed into one float:%EOL%    ri.threshold    = trunc(t[3])/100.0;%EOL%    ri.minSlope     = fract(t[3])*10.0;%EOL%}%EOL%%EOL%%EOL%//............................................................................%EOL%// Sample a texel from the splatting texture catalog%EOL%%EOL%void oe_splat_getTexel(in float index, in vec2 tc, out vec4 texel)%EOL%{%EOL%    texel = index >= 0.0 ? texture(oe_splatTex, vec3(tc, index)) : vec4(1,0,0,0);%EOL%}%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%void oe_splat_getMaterial(in float index, in vec2 tc, out vec4 material)%EOL%{%EOL%    material = index >= 0.0 ? texture(oe_splatTex, vec3(tc, index)) : vec4(.5,.5,1,0);%EOL%}%EOL%#endif%EOL%%EOL%//............................................................................%EOL%// Samples a detail texel using its render info parameters.%EOL%// Returns the weighting factor in the alpha channel.%EOL%%EOL%vec4 oe_splat_getDetailTexel(in oe_SplatRenderInfo ri, in vec2 tc, in oe_SplatEnv env)%EOL%{%EOL%    float hasDetail = clamp(ri.detailIndex+1.0, 0.0, 1.0);%EOL%%EOL%#ifdef OE_SPLAT_EDIT_MODE%EOL%    float brightness = oe_splat_brightness;%EOL%    float contrast = oe_splat_contrast;%EOL%    float threshold = oe_splat_threshold;%EOL%    float minSlope = oe_splat_minSlope;%EOL%#else%EOL%    float brightness = ri.brightness;%EOL%    float contrast = ri.contrast;%EOL%    float threshold = ri.threshold;%EOL%    float minSlope = ri.minSlope;%EOL%#endif%EOL%%EOL%    // start with the noise value%EOL%    float n = env.noise.x;%EOL%	%EOL%    // apply slope limiter, then reclamp and threshold:%EOL%    float s;%EOL%    if ( env.slope >= minSlope )%EOL%        s = 1.0;%EOL%    else if ( env.slope < 0.1*minSlope )%EOL%        s = 0.0;%EOL%    else%EOL%        s = (env.slope-0.1*minSlope)/(minSlope-0.1*minSlope);%EOL%%EOL%    brightness *= s;%EOL%%EOL%    // apply brightness and contrast, then reclamp%EOL%    n = clamp(((n-0.5)*contrast + 0.5) * brightness, 0.0, 1.0);%EOL%    %EOL%    // apply final threshold:%EOL%	n = n < threshold ? 0.0 : n;%EOL%%EOL%    // sample the texel and return it.%EOL%    vec4 result;%EOL%    oe_splat_getTexel(ri.detailIndex, tc, result);%EOL%    return vec4(result.rgb, hasDetail*n);%EOL%}%EOL%%EOL%//............................................................................%EOL%// Generates a texel using nearest-neighbor coverage sampling.%EOL%%EOL%vec4 oe_splat_nearest(in vec2 splat_tc, in oe_SplatEnv env)%EOL%{%EOL%    float coverageValue = oe_LandCover_coverage[0];%EOL%    oe_SplatRenderInfo ri;%EOL%    oe_splat_getRenderInfo(coverageValue, env, ri);%EOL%    vec4 primary;%EOL%    oe_splat_getTexel(ri.primaryIndex, splat_tc, primary);%EOL%    float detailToggle = ri.detailIndex >= 0 ? 1.0 : 0.0;%EOL%    vec4 detail  = oe_splat_getDetailTexel(ri, splat_tc, env) * detailToggle;    %EOL%    return vec4( mix(primary.rgb, detail.rgb, detail.a), primary.a );%EOL%}%EOL%%EOL%//............................................................................%EOL%// Generates a texel using bilinear filtering on the coverage data.%EOL%%EOL%void oe_splat_bilinear(in vec2 splat_tc, in oe_SplatEnv env, out vec4 color, out vec4 material)%EOL%{%EOL%    // Build the render info data for each corner:%EOL%    oe_SplatRenderInfo ri[4]; //sw,se,nw,ne%EOL%    oe_splat_getRenderInfo(oe_LandCover_coverage[0], env, ri[0]);%EOL%    oe_splat_getRenderInfo(oe_LandCover_coverage[1], env, ri[1]);%EOL%    oe_splat_getRenderInfo(oe_LandCover_coverage[2], env, ri[2]);%EOL%    oe_splat_getRenderInfo(oe_LandCover_coverage[3], env, ri[3]);%EOL%%EOL%    // Primary splat:%EOL%    vec4 p[4];%EOL%    oe_splat_getTexel(ri[0].primaryIndex, splat_tc, p[0]);%EOL%    oe_splat_getTexel(ri[1].primaryIndex, splat_tc, p[1]);%EOL%    oe_splat_getTexel(ri[2].primaryIndex, splat_tc, p[2]);%EOL%    oe_splat_getTexel(ri[3].primaryIndex, splat_tc, p[3]);%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%    // Material splat:%EOL%    vec4 m[4];%EOL%    oe_splat_getTexel(ri[0].materialIndex, splat_tc, m[0]);%EOL%    oe_splat_getTexel(ri[1].materialIndex, splat_tc, m[1]);%EOL%    oe_splat_getTexel(ri[2].materialIndex, splat_tc, m[2]);%EOL%    oe_splat_getTexel(ri[3].materialIndex, splat_tc, m[3]);%EOL%#endif%EOL%%EOL%    // Detail splat - weighting is in the alpha channel%EOL%    // TODO: Pointless to have a detail range? -gw%EOL%    // TODO: If noise is a texture, just try to single-sample it instead%EOL%    float dTog = env.range < oe_splat_detailRange ? 1.0 : 0.0;%EOL%    vec4 d[4];%EOL%    d[0] = dTog * oe_splat_getDetailTexel(ri[0], splat_tc, env);%EOL%    d[1] = dTog * oe_splat_getDetailTexel(ri[1], splat_tc, env);%EOL%    d[2] = dTog * oe_splat_getDetailTexel(ri[2], splat_tc, env);%EOL%    d[3] = dTog * oe_splat_getDetailTexel(ri[3], splat_tc, env); %EOL%%EOL%    vec4 b[4];%EOL%    b[0] = vec4(mix(p[0].rgb, d[0].rgb, d[0].a), p[0].a);%EOL%    b[1] = vec4(mix(p[1].rgb, d[1].rgb, d[1].a), p[1].a);%EOL%    b[2] = vec4(mix(p[2].rgb, d[2].rgb, d[2].a), p[2].a);%EOL%    b[3] = vec4(mix(p[3].rgb, d[3].rgb, d[3].a), p[3].a);%EOL%%EOL%    vec2 weight = fract(oe_splat_covtc*oe_splat_coverageTexSize - 0.5);%EOL%%EOL%    vec4 south = mix(b[0], b[1], weight.x);%EOL%    vec4 north = mix(b[2], b[3], weight.x);%EOL%%EOL%    color = mix(south, north, weight.y);%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%    south = mix(m[0], m[1], weight.x);%EOL%    north = mix(m[2], m[3], weight.x);%EOL%    material = mix(south, north, weight.y);%EOL%#endif%EOL%}%EOL%%EOL%//............................................................................%EOL%%EOL%#ifdef OE_SPLAT_GPU_NOISE%EOL%%EOL%uniform float oe_splat_freq;%EOL%uniform float oe_splat_pers;%EOL%uniform float oe_splat_lac;%EOL%uniform float oe_splat_octaves;%EOL%%EOL%// see: Splat.Noise.glsl%EOL%float oe_noise_fractal4D(in vec2 seed, in float frequency, in float persistence, in float lacunarity, in int octaves);%EOL%%EOL%vec4 oe_splat_getNoise(in vec2 tc)%EOL%{%EOL%    return vec4(oe_noise_fractal4D(tc, oe_splat_freq, oe_splat_pers, oe_splat_lac, int(oe_splat_octaves)));%EOL%}%EOL%%EOL%#else // !SPLAT_GPU_NOISE%EOL%%EOL%#ifdef OE_SPLAT_NOISE_SAMPLER%EOL%uniform sampler2D OE_SPLAT_NOISE_SAMPLER;%EOL%vec4 oe_splat_getNoise(in vec2 tc)%EOL%{%EOL%    return texture(OE_SPLAT_NOISE_SAMPLER, tc.st);%EOL%}%EOL%#else%EOL%vec4 oe_splat_getNoise(in vec2 tc)%EOL%{%EOL%    return vec4(0.0);%EOL%}%EOL%#endif%EOL%%EOL%#endif // SPLAT_GPU_NOISE%EOL%%EOL%%EOL%%EOL%//............................................................................%EOL%// Simplified entry point with does no filtering or range blending. (much faster.)%EOL%%EOL%void oe_splat_simple(inout vec4 color)%EOL%{%EOL%    float noiseLOD = floor(oe_splat_noiseScale);%EOL%    vec2 noiseCoords = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, noiseLOD);%EOL%%EOL%    oe_SplatEnv env;%EOL%    env.range = oe_splat_range;%EOL%    env.slope = oe_splat_getSlope();%EOL%    env.noise = oe_splat_getNoise(noiseCoords);%EOL%    env.elevation = 0.0;%EOL%    %EOL%    float lod0;%EOL%    float rangeOuter, rangeInner;%EOL%    oe_splat_getLodBlend(oe_splat_range, lod0, rangeOuter, rangeInner, env.range);%EOL%    vec2 tc = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, lod0 + float(oe_splat_scaleOffsetInt));%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%    vec4 material;%EOL%    oe_splat_bilinear(tc, env, color, material);%EOL%#endif%EOL%%EOL%    color.a *= oe_layer_opacity;%EOL%}%EOL%%EOL%//............................................................................%EOL%// Main entry point for fragment shader.%EOL%%EOL%// stage global%EOL%mat3 oe_normalMapTBN;%EOL%vec3 vp_Normal;%EOL%%EOL%void oe_splat_complex(inout vec4 color)%EOL%{%EOL%    // Noise coords.%EOL%    float noiseLOD = floor(oe_splat_noiseScale);%EOL%    vec2 noiseCoords = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, noiseLOD);%EOL%%EOL%    oe_SplatEnv env;%EOL%    env.range = oe_splat_range;%EOL%    env.slope = oe_splat_getSlope();%EOL%    env.noise = oe_splat_getNoise(noiseCoords);%EOL%    env.elevation = 0.0;%EOL%%EOL%    // quantize the scale offset so we take the hit in the FS%EOL%    float scaleOffset = float(oe_splat_scaleOffsetInt);%EOL%        %EOL%    // Calculate the 2 LODs we need to blend. We have to do this in the FS because %EOL%    // it's quite possible for a single triangle to span more than 2 LODs.%EOL%    float lod0, lod1;%EOL%    float rangeOuter, rangeInner;%EOL%    oe_splat_getLodBlend(oe_splat_range, lod0, rangeOuter, rangeInner, env.range);%EOL%    %EOL%    // Sample the two LODs:%EOL%    vec4 texel0, texel1;%EOL%    vec4 material0, material1;%EOL%%EOL%    vec2 tc0 = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, lod0 + scaleOffset);%EOL%    env.lod = lod0;%EOL%    oe_splat_bilinear(tc0, env, texel0, material0);%EOL%    %EOL%    vec2 tc1 = oe_terrain_scaleCoordsToRefLOD(oe_layer_tilec.st, lod0 + 1.0 + scaleOffset);%EOL%    env.lod = lod0+1.0;%EOL%    oe_splat_bilinear(tc1, env, texel1, material1);%EOL%%EOL%    // recalcluate blending ratio%EOL%    float lodBlend = clamp((rangeOuter - env.range) / (rangeOuter - rangeInner), 0, 1);%EOL%       %EOL%    // Blend the two samples based on LOD factor:%EOL%    vec4 texel = mix(texel0, texel1, lodBlend);%EOL%%EOL%#ifdef OE_SPLAT_USE_MATERIALS%EOL%    // Blend together the material samples:%EOL%    vec4 material = mix(material0, material1, lodBlend);%EOL%    vec3 n = oe_normalMapTBN * (material.xyz*2.0-1.0);%EOL%    vp_Normal = normalize(vp_Normal + n);%EOL%#endif%EOL%%EOL%    // incorporate the layer's opacity:%EOL%    texel.a *= oe_layer_opacity;%EOL%%EOL%#ifdef OE_TERRAIN_BLEND_IMAGERY%EOL%    // If this is a first image layer, blend with the incoming terrain color.%EOL%    // Otherwise, apply directly and let GL blending do the rest.%EOL%    if (oe_layer_order == 0)%EOL%    {%EOL%        color.rgb = texel.rgb*texel.a + color.rgb*(1.0-texel.a);%EOL%    }%EOL%    else%EOL%    {%EOL%        color = texel;%EOL%    }%EOL%#else%EOL%    // No blending? The output is just the texel value.%EOL%    color = texel;%EOL%#endif // OE_TERRAIN_BLEND_IMAGERY%EOL%}%EOL%%EOL%";

    Util = "Splat.util.glsl";
    _sources[Util] = "#version $GLSL_VERSION_STR%EOL%$GLSL_DEFAULT_PRECISION_FLOAT%EOL%%EOL%#pragma vp_location fragment_coloring%EOL%%EOL%// Number of LOD range. Do not increase this past 25; doing so will result in precision errors%EOL%// and rendering artifacts when the camera is very close to the ground.%EOL%#define LOD_COUNT 26%EOL%%EOL%const float oe_SplatRanges[26] = float[](%EOL%       100000000.0, // 0%EOL%        75000000.0, // 1%EOL%        50000000.0, // 2%EOL%        10000000.0, // 3%EOL%         7500000.0, // 4%EOL%         5000000.0, // 5%EOL%         2500000.0, // 6%EOL%         1000000.0, // 7%EOL%          500000.0, // 8%EOL%          225000.0, // 9%EOL%          150000.0, // 10%EOL%           80000.0, // 11%EOL%           30000.0, // 12%EOL%           14000.0, // 13%EOL%            4000.0, // 14%EOL%            2500.0, // 15%EOL%            1000.0, // 16%EOL%             500.0, // 17%EOL%             250.0, // 18%EOL%             125.0, // 19%EOL%              50.0, // 20%EOL%              25.0, // 21%EOL%              12.0, // 22%EOL%               6.0, // 23%EOL%               3.0, // 24%EOL%               1.0  // 25%EOL%    );%EOL%%EOL%/**%EOL% * Given a camera distance, return the two LODs it falls between and%EOL% * the blend factor [0..1] between then.%EOL% * in  range   = camera distace to fragment%EOL% * in  baseLOD = LOD at which texture scale is 1.0%EOL% * out LOD0    = near LOD%EOL% * out LOD1    = far LOD%EOL% * out blend   = Blend factor between LOD0 and LOD1 [0..1]%EOL% */%EOL%void%EOL%oe_splat_getLodBlend(in float range, out float out_LOD0, out float out_rangeOuter, out float out_rangeInner, out float out_clampedRange)%EOL%{%EOL%    out_clampedRange = clamp(range, oe_SplatRanges[LOD_COUNT-1], oe_SplatRanges[0]);%EOL%%EOL%    out_LOD0 = 0;%EOL%%EOL%    for(int i=0; i<LOD_COUNT-1; ++i)%EOL%    {%EOL%        if ( out_clampedRange < oe_SplatRanges[i] && out_clampedRange >= oe_SplatRanges[i+1] )%EOL%        {%EOL%            out_LOD0 = float(i); //   + baseLOD;%EOL%            break;%EOL%        }%EOL%    }%EOL%%EOL%    out_rangeOuter = oe_SplatRanges[int(out_LOD0)];%EOL%    out_rangeInner = oe_SplatRanges[int(out_LOD0)+1];%EOL%}%EOL%%EOL%";
}


GroundCoverShaders::GroundCoverShaders()
{
    Grass = "Grass.glsl";
    _sources[Grass] = "";
    
    GroundCover_CS = "GroundCover.CS.glsl";
    _sources[GroundCover_CS] = "";

    GroundCover_Billboard = "GroundCover.Billboard.glsl";
    _sources[GroundCover_Billboard] = "";
    
    GroundCover_Model = "GroundCover.Model.glsl";
    _sources[GroundCover_Model] = "@GroundCover.Model.glsl";
}
