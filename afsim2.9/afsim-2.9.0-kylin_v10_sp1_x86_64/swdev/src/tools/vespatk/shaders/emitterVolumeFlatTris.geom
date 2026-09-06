#version 150
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 11) out;
uniform mat4 osg_ViewMatrix;

in vec3 ecefCoord[];
float cM_PER_DEGREE_EQ = 111318.163;
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
   vec2 c0 = ecefCoord[0].xy;
   vec2 c1 = ecefCoord[1].xy;
   vec2 c2 = ecefCoord[2].xy;

   if (c0.x < 0 && c0.y < 0)
   {
      negIndices[negCount++] = 0;
   }
   else if (c0.x < 0 && c0.y > 0)
   {
      posIndices[posCount++] = 0;
   }

   if (c1.x < 0 && c1.y < 0)
   {
      negIndices[negCount++] = 1;
   }
   else if (c1.x < 0 && c1.y > 0)
   {
      posIndices[posCount++] = 1;
   }

   if (c2.x < 0 && c2.y < 0)
   {
      negIndices[negCount++] = 2;
   }
   else if (c2.x < 0 && c2.y > 0)
   {
      posIndices[posCount++] = 2;
   }
   // Only if the triangle crosses the dateline
   return (posCount + negCount < 3) || posCount == 3 || negCount == 3;
}

void EmitFrom(vec4 aPosition)
{
      vec4 p = gl_ProjectionMatrix * osg_ViewMatrix * aPosition;
      p.z = min(p.w, p.z);
	  gl_Position = p;
      EmitVertex();
}

void PassThrough()
{
   for (int i = 0; i < 3; ++i)
   {
      EmitFrom(gl_in[i].gl_Position);
   }
   EndPrimitive();
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

   // Depending on the case, copy the input triangles into the canonical
   // labeling with vertex 0 the vertex on the opposite side of the branch
   // from the other two. Also, map the x-coordinate of the negative side
   // points around the globe so the differences go the short way.
   vec4 pt0;
   vec4 pt1;
   vec4 pt2;

   if (negCount == 1)
   {
      pt0 = gl_in[negIndices[0]].gl_Position;
      pt0.x += cBRANCH_ADJUST;
      pt1 = gl_in[posIndices[0]].gl_Position;
      pt2 = gl_in[posIndices[1]].gl_Position;
   }
   else if (posCount == 1)
   {
      pt0 = gl_in[posIndices[0]].gl_Position;
      pt1 = gl_in[negIndices[0]].gl_Position;
      pt1.x += cBRANCH_ADJUST;
      pt2 = gl_in[negIndices[1]].gl_Position;
      pt2.x += cBRANCH_ADJUST;
   }
   else
   {
      // should not happen; do nothing
      return;
   }

   // pt0 -> pt1 crosses the branch
   float lambda_1_minus = (cBRANCH_POINT - pt0.x) / (pt1.x - pt0.x);
   vec4 pta = mix(pt0, pt1, lambda_1_minus);

   float lambda_1_plus = (cBRANCH_POINT - pt0.x) / (pt1.x - pt0.x);
   vec4 ptb = mix(pt0, pt1, lambda_1_plus);

   // pt0 -> pt2 crosses the branch
   float lambda_2_minus = (cBRANCH_POINT - pt0.x) / (pt2.x - pt0.x);
   vec4 ptc = mix(pt0, pt2, lambda_2_minus);

   float lambda_2_plus = (cBRANCH_POINT - pt0.x) / (pt2.x - pt0.x);
   vec4 ptd = mix(pt0, pt2, lambda_2_plus);
   

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
   EmitFrom(pt0);
   EmitFrom(pta);
   EmitFrom(ptc);
   EndPrimitive();

   EmitFrom(pt1);
   EmitFrom(ptb);
   EmitFrom(pt2);
   EmitFrom(ptd);
   EndPrimitive();
}

void main()
{
   if (ClassifyVertices())
   {
      PassThrough();
   }
   else
   {
      SplitProcessing();
   }
}
