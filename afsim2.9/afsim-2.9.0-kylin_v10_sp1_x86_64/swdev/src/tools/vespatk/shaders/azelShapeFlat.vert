#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// unused and untested with this shader version
float cA = 6378137.0;
float cB = 6356752.3142451794975;
float cE_2 = 0.00669437999014131699445;
float cEP_2 = 0.00673949674227643495307;
float METERS_PER_DEG = 111319.4908;
float DEG_PER_RADIAN = 57.2957795;
varying vec4 vertColor;
uniform mat4 osg_ViewMatrix;
uniform vec3 geocentricxyz;
uniform vec3 geocentricrpy;

void main( void )
{
   float ca = cos(geocentricrpy.x);
   float sa = sin(geocentricrpy.x); // roll
   float cb = cos(geocentricrpy.y);
   float sb = sin(geocentricrpy.y); // pitch
   float cc = cos(geocentricrpy.z);
   float sc = sin(geocentricrpy.z); // heading

   mat3 bodyToWorld = mat3(cb * ca,                   cb * sa,                   -sb,
                           sc * sb * ca - cc * sa,    sc * sb * sa + cc * ca,    sc * cb,
                           cc * sb * ca + sc * sa,    cc * sb * sa - sc * ca,    cc * cb);

   mat3 pitchFix = mat3(0, 0, 1, 0, -1, 0, -1, 0, 0);

   vec4 v = gl_Vertex;
   float cel = cos(v.y);
   vec3 vbody = vec3(cel * cos(v.x) * v.z, cel * sin(v.x) * v.z, sin(v.y) * v.z).zyx;
   vertColor = gl_Color;
   vec3 gcpos = bodyToWorld * pitchFix * vbody + geocentricxyz;

   float p = length(gcpos.xy);
   float theta = atan((gcpos.z * cA), (p * cB));
   float cosTheta = cos(theta);
   float sinTheta = sin(theta);
   float cosTheta_3 = cosTheta * cosTheta * cosTheta;
   float sinTheta_3 = sinTheta * sinTheta * sinTheta;
   float phi = atan((gcpos.z + cEP_2 * cB * sinTheta_3), (p - cE_2 * cA * cosTheta_3));
   float lambda = atan2(gcpos.y, gcpos.x);
   float cosPhi = cos(phi);
   float sinPhi = sin(phi);
   float R_N = cA / sqrt(1.0 - cE_2 * sinPhi * sinPhi);
   float h = (p / cosPhi) - R_N;
   if ((sign(lambda) != sign(geocentricxyz.y)) && (geocentricxyz.x < 0)) lambda = lambda + sign(geocentricxyz.y) * 6.283185307;
   vec4 scenepos = vec4(lambda * METERS_PER_DEG * DEG_PER_RADIAN, phi * METERS_PER_DEG * DEG_PER_RADIAN, h, 1);

   gl_Position = gl_ProjectionMatrix * osg_ViewMatrix * scenepos;
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
