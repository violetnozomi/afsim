#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// unused and untested at this version
uniform sampler2D tex2D;
uniform sampler2D depth;
uniform vec4 v4Viewport;
uniform float fNearPlane;
uniform float fFarPlane;

varying vec4 v3ZeroPos;

const float earthRadius = 6378137.0;
const float atmoDepth = 200000.0;
const float fScaleDepth = 0.25;
const float fSamples = 5.0;
const int nSamples = 5;
const float g = -0.990;
const float g2 = 0.980;

float scale(float fCos)
{
   float x = 1.0 - fCos;
   return fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main(void)
{
   float mydepth = texture2D(depth, gl_TexCoord[0]).r;
   vec4 sceneColor = texture2D(tex2D, gl_TexCoord[0]);
   vec3 light = gl_LightSource[0].position.xyz;
   vec4 ndcPos;
   ndcPos.xy = 2.0 * (gl_FragCoord.xy - v4Viewport.xy) / v4Viewport.zw - 1.0;
   ndcPos.z = mydepth;
   ndcPos.w = 1.0;
//   vec4 clipPos = ndcPos / gl_FragCoord.w;
   vec4 vp = gl_ProjectionMatrixInverse * ndcPos;
   vp.xyz /= vp.w;
   vec3 v3CameraPos = -v3ZeroPos;
   vec3 v3Ray = normalize(vp.xyz);
//   vp.z = (2.0 * fNearPlane) / (fFarPlane + fNearPlane - mydepth * (fFarPlane - fNearPlane));
   float fDepth = max(length(v3CameraPos), earthRadius);
   float costheta = max(dot(v3ZeroPos, v3Ray) / fDepth, 0);
   float sintheta = sqrt(1 - costheta * costheta);
   float pdist = fDepth * sintheta;
   float penetration = 2 * sqrt(max(pow(earthRadius + atmoDepth, 2.0) - pdist * pdist, 0.0));
   float innerpen = 2 * sqrt(max(pow(earthRadius, 2.0) - pdist * pdist, 0.0));
   float outOfAtmosphere = step(0.000000001, penetration);
   float grounded = step(0.000000001, innerpen); // this should be replaced with a check against depthr

   float fNear = sqrt(fDepth * fDepth - pdist * pdist) - 0.5 * penetration;
   float fInnerNear = sqrt(fDepth * fDepth - pdist * pdist) - 0.5 * innerpen;
   float fFar = penetration + fNear;
//   fFar = min(fFar, vp.z);
   fNear = max(fNear, 0.0);
   float fOuterRadius = earthRadius + atmoDepth;
   float fScale = 1 / (fOuterRadius - earthRadius);
   float fScaleOverScaleDepth = fScale / fScaleDepth;
   float fKr4PI = 0.025;
   float fKm4PI = 0.0001257;
   float fKrESun = 0.1;
   float fKmESun = 0.0005;
   vec3 v3InvWavelength = vec3(1.0 / pow(0.650, 4.0), 1.0 / pow(0.570, 4.0), 1.0 / pow(0.475, 4.0));
//////////////////////
   vec3 v3Start = v3CameraPos + v3Ray * fNear;
   fFar = mix(penetration, 0.5 * (penetration - innerpen), grounded); // this should be replaced with depth - fNear for grounded case
   float fStartAngle = dot(v3Ray, v3Start) / min(fDepth, fOuterRadius);
   float fStartDepth = exp(-1.0 / fScaleDepth);
   float fStartOffset = fStartDepth*scale(fStartAngle);
// Initialize the scattering loop variables
   float fSampleLength = fFar / fSamples;
   float fScaledLength = fSampleLength * fScale;
   vec3 v3SampleRay = v3Ray * fSampleLength;
   vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
// Set up ground parameters
   float fGDepth = exp((earthRadius - min(fDepth, fOuterRadius)) / fScaleDepth);
   vec3 v3Pos = v3Ray * fInnerNear - v3ZeroPos; // should be based on depth
   float fGCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
   float fGLightAngle = dot(light, v3Pos) / length(v3Pos);
   float fGCameraScale = scale(fGCameraAngle);
   float fGLightScale = scale(fGLightAngle);
   float fGCameraOffset = fGDepth * fGCameraScale;
   float fGTemp = (fGLightScale + fGCameraScale);
// Now loop through the sample rays
   vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
   vec3 v3Attenuate;
   float fScatter;
   float fGScatter;
   float fUGScatter;
   for(int i=0; i<nSamples; i++)
   {
      float fHeight = length(v3SamplePoint);
      float fDepth = exp(fScaleOverScaleDepth * (earthRadius - fHeight));
      float fLightAngle = dot(light, v3SamplePoint) / fHeight;
      float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
      fUGScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
      fGScatter = fDepth * fGTemp - fGCameraOffset;
      fScatter = fUGScatter * (1 - grounded) + grounded * fGScatter; //mix(fUGScatter, fGScatter, grounded);
      v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
      v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
      v3SamplePoint += v3SampleRay;
   }
// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
   float fCos = dot(light, -v3Ray);
   float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
   vec3 mieinput = fMiePhase * v3FrontColor * fKmESun;
   vec3 rayinput = v3FrontColor * (v3InvWavelength * fKrESun);
   vec4 sColor = vec4(rayinput, 1.0) + vec4(mieinput, 1.0);
   vec4 gColor = vec4((v3FrontColor * (v3InvWavelength * fKrESun + fKmESun)) + 0.25 * v3Attenuate, 1);
   vec4 color = mix(sColor, gColor, grounded);
///////////////////
//   gl_FragColor = vec4(0/*(fFar) / (2 * sqrt(fOuterRadius * fOuterRadius - earthRadius * earthRadius))*/, (innerpen) / earthRadius, 0, 1.0);
   gl_FragColor = sceneColor + outOfAtmosphere * color;
}