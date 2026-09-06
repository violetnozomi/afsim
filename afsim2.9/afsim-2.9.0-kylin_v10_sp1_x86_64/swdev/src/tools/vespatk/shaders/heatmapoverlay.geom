#version 150
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#pragma import_defines (USE_FLAT_MAP_PROJECTION)

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 7) out;

uniform mat4 osg_ModelViewProjectionMatrix;

in float colorValue[];
out float fieldValue;

float cM_PER_DEGREE_EQ = 111318.163;
float cBRANCH_LIMIT    = 160.0 * cM_PER_DEGREE_EQ;
float cLAMBDA_EPSILON  = 1.0;
float cBRANCH_POINT    = 180.0 * cM_PER_DEGREE_EQ;
float cBRANCH_ADJUST   = 360.0 * cM_PER_DEGREE_EQ;

ivec3 negIndices = ivec3(-1, -1, -1);
ivec3 posIndices = ivec3(-1, -1, -1);
int negCount = 0;
int posCount = 0;

bool ClassifyVertices()
{
   // Count how many points of the triangle are close to the coordinate branch
   // from the positive and negative sides.
   float c0 = gl_in[0].gl_Position.x;
   float c1 = gl_in[1].gl_Position.x;
   float c2 = gl_in[2].gl_Position.x;

   if (c0 < -cBRANCH_LIMIT)
   {
      negIndices[negCount++] = 0;
   }
   else if (c0 > cBRANCH_LIMIT)
   {
      posIndices[posCount++] = 0;
   }

   if (c1 < -cBRANCH_LIMIT)
   {
      negIndices[negCount++] = 1;
   }
   else if (c1 > cBRANCH_LIMIT)
   {
      posIndices[posCount++] = 1;
   }

   if (c2 < -cBRANCH_LIMIT)
   {
      negIndices[negCount++] = 2;
   }
   else if (c2 > cBRANCH_LIMIT)
   {
      posIndices[posCount++] = 2;
   }

   // Only if the triangle has three vertices near the branch, not all three
   // of which are on the same side, do we need to split the input triangle.
   return posCount + negCount < 3 || posCount == 3 || negCount == 3;
}

void SplitProcessing()
{
   //     |    2    The input triangle is 0-1-2. If one of the three initial
   //     |   /|    vertices is across the coordinate branch from the other two,
   //     |  / |    then three output triangles are required. The first in a strip
   //     | /  |    by itself is 0-a-c. The second and third are in a strip
   //     |d   |    b-1-d-2. Both sides of the overlay are drawn because it may
   //    c| \  |    be at significant altitude, so no special care needs to be
   //   / |  \ |    taken to assure the winding order of the output triangles is
   //  /  |   \|    correct.
   // 0--a|b---1
   //     |
   //     | <--- coordinate branch

   vec4 pt0;
   vec4 pt1;
   vec4 pt2;
   float val0;
   float val1;
   float val2;

   // Depending on the case, copy the input triangles into the canonical
   // labeling with vertex 0 the vertex on the opposite side of the branch
   // from the other two. Also, map the x-coordinate of the negative side
   // points around the globe so the differences go the short way.
   if (negCount == 1)
   {
      pt0 = gl_in[negIndices[0]].gl_Position;
      pt0.x += cBRANCH_ADJUST;
      val0 = colorValue[negIndices[0]];
      pt1 = gl_in[posIndices[0]].gl_Position;
      val1 = colorValue[posIndices[0]];
      pt2 = gl_in[posIndices[1]].gl_Position;
      val2 = colorValue[posIndices[1]];
   }
   else if (posCount == 1)
   {
      pt0 = gl_in[posIndices[0]].gl_Position;
      val0 = colorValue[posIndices[0]];
      pt1 = gl_in[negIndices[0]].gl_Position;
      pt1.x += cBRANCH_ADJUST;
      val1 = colorValue[negIndices[0]];
      pt2 = gl_in[negIndices[1]].gl_Position;
      pt2.x += cBRANCH_ADJUST;
      val2 = colorValue[negIndices[1]];
   }
   else
   {
      // should not happen; do nothing
      return;
   }

   // pt0 -> pt1 crosses the branch
   float lambda_1_minus = (cBRANCH_POINT - cLAMBDA_EPSILON - pt0.x) / (pt1.x - pt0.x);
   vec4 pta = mix(pt0, pt1, lambda_1_minus);
   float vala = mix(val0, val1, lambda_1_minus);

   float lambda_1_plus = (cBRANCH_POINT + cLAMBDA_EPSILON - pt0.x) / (pt1.x - pt0.x);
   vec4 ptb = mix(pt0, pt1, lambda_1_plus);
   float valb = mix(val0, val1, lambda_1_plus);

   // pt0 -> pt2 crosses the branch
   float lambda_2_minus = (cBRANCH_POINT - cLAMBDA_EPSILON - pt0.x) / (pt2.x - pt0.x);
   vec4 ptc = mix(pt0, pt2, lambda_2_minus);
   float valc = mix(val0, val2, lambda_2_minus);

   float lambda_2_plus = (cBRANCH_POINT + cLAMBDA_EPSILON - pt0.x) / (pt2.x - pt0.x);
   vec4 ptd = mix(pt0, pt2, lambda_2_plus);
   float vald = mix(val0, val2, lambda_2_plus);

   // Map the x-coordinates of the negative side points back into the negative
   // side.
   if (negCount == 1)
   {
      pt0.x -= cBRANCH_ADJUST;
      pta.x -= cBRANCH_ADJUST;
      ptc.x -= cBRANCH_ADJUST;
   }
   else if (posCount == 1)
   {
      pt1.x -= cBRANCH_ADJUST;
      pt2.x -= cBRANCH_ADJUST;
      ptb.x -= cBRANCH_ADJUST;
      ptd.x -= cBRANCH_ADJUST;
   }

   // First, the single triangle by the lone vertex [0-a-c].
   gl_Position = osg_ModelViewProjectionMatrix * pt0;
   fieldValue = val0;
   EmitVertex();

   gl_Position = osg_ModelViewProjectionMatrix * pta;
   fieldValue = vala;
   EmitVertex();

   gl_Position = osg_ModelViewProjectionMatrix * ptc;
   fieldValue = valc;
   EmitVertex();

   EndPrimitive();

   // Second, the two triangles on the other side [b-1-d-2].
   gl_Position = osg_ModelViewProjectionMatrix * ptb;
   fieldValue = valb;
   EmitVertex();

   gl_Position = osg_ModelViewProjectionMatrix * pt1;
   fieldValue = val1;
   EmitVertex();

   gl_Position = osg_ModelViewProjectionMatrix * ptd;
   fieldValue = vald;
   EmitVertex();

   gl_Position = osg_ModelViewProjectionMatrix * pt2;
   fieldValue = val2;
   EmitVertex();

   EndPrimitive();
}

void PassThrough()
{
   for (int i = 0; i < 3; ++i)
   {
      gl_Position = osg_ModelViewProjectionMatrix * gl_in[i].gl_Position;
      fieldValue = colorValue[i];
      EmitVertex();
   }
   EndPrimitive();
}

void main()
{
#ifdef USE_FLAT_MAP_PROJECTION
   if (ClassifyVertices())
   {
      PassThrough();
   }
   else
   {
      SplitProcessing();
   }
#else
   PassThrough();
#endif
}
