// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
float cA = 6378137.0;
float cB = 6356752.3142451794975;
float cE_2 = 0.00669437999014131699445;
float cEP_2 = 0.00673949674227643495307;
float METERS_PER_DEG = 111318.163; // from UtoMapProjection::msEqrFactor
float DEG_PER_RADIAN = 57.2957795;

vec4 GetEqrCoords(vec4 aECEF)
{
   float p = length(aECEF.xy);
   float theta = atan((aECEF.z * cA), (p * cB));
   float cosTheta = cos(theta);
   float sinTheta = sin(theta);
   float cosTheta_3 = cosTheta * cosTheta * cosTheta;
   float sinTheta_3 = sinTheta * sinTheta * sinTheta;
   float phi = atan((aECEF.z + cEP_2 * cB * sinTheta_3), (p - cE_2 * cA * cosTheta_3));
   float lambda = atan(aECEF.y, aECEF.x);
   float sinPhi = sin(phi);
   float R_N = cA / sqrt(1.0 - cE_2 * sinPhi * sinPhi);
   float h = (p / cos(phi)) - R_N;
   if ((sign(lambda) != sign(aECEF.y)) && (aECEF.x < 0.0)) lambda = lambda + sign(aECEF.y) * 6.283185307;
   return vec4(lambda * METERS_PER_DEG * DEG_PER_RADIAN, phi * METERS_PER_DEG * DEG_PER_RADIAN, h, 1);
}
