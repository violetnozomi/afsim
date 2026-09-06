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
const float cA = 6378137.0;
const float cB = 6356752.3;
const float cE_2 = 0.00669437999014131699445;
const float cEP_2 = 0.00673949674227643495307;
const float cAtmosphereDepth = 100000.0;

uniform mat4 viewMatrix;
uniform mat4 viewMatrixInverse;
uniform vec4 projTexXform;

varying float fFogFragCoord;
varying float Penetration;

void main( void )
{
   gl_Position = ftransform(); // commented for scalar stuff
   vec4 vEye = gl_ModelViewMatrix * gl_Vertex; // this the eye to vertex vector
   vec4 vEarth = viewMatrix * vec4(0.0, 0.0, 0.0, 1.0); // this is the eye to database origin (earth's center) vector
   float viewAngle = acos(dot(normalize(vEye), normalize(vEarth))); // this is the origin=>eye=>vertex angle
   float d = min(-length(vEarth) * sin(viewAngle), 0.0); // this is the length of the view vector projected onto earths perpendicular axis
   float r1 = cA; // earth's radius
   float r2 = cA + cAtmosphereDepth; // atmospheric radius
   float x = sqrt(r2 * r2 - d * d) - sqrt(r1 * r1 - d * d); // total atmospheric penetration along view vector
   float pen = min(length(vEye), x); // actual atmospheric penetration, taking into consideration if the eye is in the atmosphere
   Penetration = pen / sqrt(r2 * r2 - r1 * r1); // normalized actual atmospheric penetration (0 is none, 1 is from outside the atmosphere to an incidental intersection
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
   gl_TexCoord[2] = gl_TextureMatrix[2] * gl_MultiTexCoord2;
   gl_TexCoord[3] = gl_TextureMatrix[3] * gl_MultiTexCoord3;
   vec4 worldCoord = viewMatrixInverse * gl_ModelViewMatrix * gl_Vertex;
   float p = length(vec2(worldCoord));
   float theta = atan(worldCoord.z * cA, p * cB);
   float sinTheta = sin(theta);
   float cosTheta = cos(theta);
   float sinTheta_3 = sinTheta * sinTheta * sinTheta;
   float cosTheta_3 = cosTheta * cosTheta * cosTheta;
   float phi = atan(worldCoord.z + cEP_2 * cB * sinTheta_3, p - cE_2 * cA * cosTheta_3);
   float lambda = atan(worldCoord.y, worldCoord.x);
   vec4 geodetic = vec4(lambda, phi, 0, 0);
   gl_TexCoord[7] = vec4((geodetic.x - projTexXform.x) / projTexXform.y, (geodetic.y - projTexXform.z) / projTexXform.w, 0, 0);
   fFogFragCoord = length(vEye);
}