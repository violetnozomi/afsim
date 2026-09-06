#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 UTO_Viewport;
uniform vec3 vtk_sunVec;

varying vec4 v4ZeroPos;
varying vec3 sunLight;

const float earthRadius = 6378137.0;
const float atmoDepth = 6578137.0;
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

void main()
{
   vec4 ndcPos;
   ndcPos.xy = 2.0 * (gl_FragCoord.xy - UTO_Viewport.xy) / UTO_Viewport.zw - 1.0;
   ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
   ndcPos.w = 1.0;
   vec4 vp = gl_ProjectionMatrixInverse * ndcPos;
   vp.xyz /= vp.w;
   vec3 v3CameraPos = -v4ZeroPos.xyz;
   vec3 v3Ray = normalize(vp.xyz);  // this is a vector to the fragment we are looking at
   float fDepth = length(v3CameraPos);
   float costheta = dot(v4ZeroPos.xyz, v3Ray) / fDepth; // theta is the angle defined by the fragment, the eyepoint, and the center of the earth
   float sintheta = sqrt(1.0 - costheta * costheta);

   float fNear = 0.0;
   float fFar = 0.0;
   if (fDepth < atmoDepth)
   {
      fNear = 0.0;
	  fFar = sqrt(atmoDepth * atmoDepth - fDepth * fDepth * sintheta * sintheta) + fDepth * costheta;
   }
   else
   {
      float pdist = fDepth * sintheta;
      float pen = 2.0 * sqrt(pow(atmoDepth, 2.0) - pow(pdist, 2.0));
      fNear = sqrt(pow(fDepth, 2.0) - pow(pdist, 2.0)) - 0.5 * pen;
      fFar = fNear + pen;
   }

   float fOuterRadius = atmoDepth;
   float fScale = 1.0 / (fOuterRadius - earthRadius);
   float fScaleOverScaleDepth = fScale / fScaleDepth;
   float fKr4PI = 0.025;
   float fKm4PI = 0.0001257;
   float fKrESun = 0.1;
   float fKmESun = 0.0005;
   vec3 v3InvWavelength = vec3(1.0 / pow(0.650, 4.0), 1.0 / pow(0.570, 4.0), 1.0 / pow(0.475, 4.0));
//////////////////////
   vec3 v3Start = v3CameraPos + v3Ray * fNear;
   float fStartAngle = dot(v3Ray, v3Start) / min(fDepth, fOuterRadius);
   float fStartDepth = exp(-1.0 / fScaleDepth);
   float fStartOffset = fStartDepth*scale(fStartAngle);
// Initialize the scattering loop variables
   float fSampleLength = (fFar - fNear) / fSamples;
   float fScaledLength = fSampleLength * fScale;
   vec3 v3SampleRay = v3Ray * fSampleLength;
   vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
   vec3 v3Pos = v3Ray * fNear - v4ZeroPos.xyz; // should be based on depth
   vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
   vec3 v3Attenuate;
   float fScatter;
   for(int i=0; i<nSamples; i++)
   {
      float fHeight = length(v3SamplePoint);
      float fDepth = exp(fScaleOverScaleDepth * (earthRadius - fHeight));
      float fLightAngle = dot(sunLight, v3SamplePoint) / fHeight;
      float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
      fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
      v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
      v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
      v3SamplePoint += v3SampleRay;
   }
// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
   float fCos = dot(sunLight, -v3Ray);
   float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
   vec3 mieinput = fMiePhase * v3FrontColor * fKmESun;
   vec3 rayinput = v3FrontColor * (v3InvWavelength * fKrESun);
   vec4 sColor = vec4(rayinput, 1.0) + vec4(mieinput, 1.0);
   gl_FragColor = vec4(pow(sColor.r * 0.25, 0.5), pow(sColor.g * 0.25, 0.5), pow(sColor.b * 0.30, 0.5), pow(sColor.b, 0.1));
}
