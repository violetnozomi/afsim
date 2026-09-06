// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtIntersectMesh.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <memory>

#include "UtMatrix.hpp"

//! constructor
//!

// public
UtIntersectMesh::UtIntersectMesh()
   : mUniqueCode{"UNIQUECO"}
   , mTransformed(false)
   , mMatrix(4, 4)
   , mInvMatrix(4, 4)
{
}

//! constructor from file
//!
//! @param aFileName The name of the file to read from.
//! @param aSuccess will be true if a file was found, otherwise false

// public
UtIntersectMesh::UtIntersectMesh(const std::string& aFileName, bool& aSuccess)
   : mMatrix(4, 4)
   , mInvMatrix(4, 4)
{
   mTransformed = false;
   std::ifstream input(aFileName);
   aSuccess = false;

   if (input)
   {
      int numSurface;
      int numTriangles;

      input >> std::setw(8) >> mUniqueCode;

      input >> numSurface;

      for (int i = 0; i < numSurface; ++i)
      {
         UtIntersectSurface tempSurf;
         input >> tempSurf.mSurfaceCode;
         input >> tempSurf.mPlane[0] >> tempSurf.mPlane[1] >> tempSurf.mPlane[2] >> tempSurf.mPlane[3];

         input >> numTriangles;
         for (int j = 0; j < numTriangles; ++j)
         {
            UtIntersectTriangle tempTri;
            input >> tempTri.mV0[0] >> tempTri.mV0[1] >> tempTri.mV0[2];
            input >> tempTri.mE0[0] >> tempTri.mE0[1] >> tempTri.mE0[2];
            input >> tempTri.mE1[0] >> tempTri.mE1[1] >> tempTri.mE1[2];

            mBoundingBox[tempSurf.mSurfaceCode].GrowBy(tempTri.mV0);
            double mv1[3] = {tempTri.mV0[0] + tempTri.mE0[0],
                             tempTri.mV0[1] + tempTri.mE0[1],
                             tempTri.mV0[2] + tempTri.mE0[2]};
            double mv2[3] = {tempTri.mV0[0] + tempTri.mE1[0],
                             tempTri.mV0[1] + tempTri.mE1[1],
                             tempTri.mV0[2] + tempTri.mE1[2]};
            mBoundingBox[tempSurf.mSurfaceCode].GrowBy(mv1);
            mBoundingBox[tempSurf.mSurfaceCode].GrowBy(mv2);
            mSurfaceIdList.insert(tempSurf.mSurfaceCode);
            tempSurf.mTriangleList.push_back(tempTri);
         }
         mSurfaceList.push_back(tempSurf);
      }
      aSuccess = true;
   }
}


//! This method creates a new surface and triangulation inside of the mesh.
//!
//! @param aPolygon This is a list of doubles representing a vertex set.  Each vertex will have 3 coordinates.  It is currently
//!                 assumed that the polygon is convex and has a counter-clockwise winding.

// public
void UtIntersectMesh::AddPolygon(const std::list<double*>& aPolygon, const int aSurfaceCode)
{
   UtIntersectSurface surface;
   surface.mSurfaceCode = aSurfaceCode;
   bool first           = true;

   // 1. triangulate aPolygon
   // right now I am assuming convex, but I would like to have a real triangulator here to support concave
   if (aPolygon.size() < 3)
   {
      return;
   }

   auto                p0 = aPolygon.begin();
   auto                p1 = std::next(p0);
   auto                p2 = std::next(p1);
   UtIntersectTriangle tri;

   while (p2 != aPolygon.end())
   {
      tri.mV0[0] = (*p0)[0];
      tri.mV0[1] = (*p0)[1];
      tri.mV0[2] = (*p0)[2];
      tri.mE0[0] = (*p1)[0] - (*p0)[0];
      tri.mE0[1] = (*p1)[1] - (*p0)[1];
      tri.mE0[2] = (*p1)[2] - (*p0)[2];
      tri.mE1[0] = (*p2)[0] - (*p0)[0];
      tri.mE1[1] = (*p2)[1] - (*p0)[1];
      tri.mE1[2] = (*p2)[2] - (*p0)[2];
      if (first)
      {
         surface.mPlane[0] = -(tri.mE0[1] * tri.mE1[2] - tri.mE0[2] * tri.mE1[1]);
         surface.mPlane[1] = -(tri.mE0[2] * tri.mE1[0] - tri.mE0[0] * tri.mE1[2]);
         surface.mPlane[2] = -(tri.mE0[0] * tri.mE1[1] - tri.mE0[1] * tri.mE1[0]);
         surface.mPlane[3] =
            -(tri.mV0[0] * surface.mPlane[0] + tri.mV0[1] * surface.mPlane[1] + tri.mV0[2] * surface.mPlane[2]);
         first = false;
      }

      surface.mTriangleList.push_back(tri);

      mBoundingBox[aSurfaceCode].GrowBy(tri.mV0);
      double mv1[3] = {tri.mV0[0] + tri.mE0[0], tri.mV0[1] + tri.mE0[1], tri.mV0[2] + tri.mE0[2]};
      double mv2[3] = {tri.mV0[0] + tri.mE1[0], tri.mV0[1] + tri.mE1[1], tri.mV0[2] + tri.mE1[2]};
      mBoundingBox[aSurfaceCode].GrowBy(mv1);
      mBoundingBox[aSurfaceCode].GrowBy(mv2);
      mSurfaceIdList.insert(aSurfaceCode);
      ++p1;
      ++p2;
   }

   // 2. add the surface
   mSurfaceList.push_back(surface);
}

//! This performs and intersection check between a line segment and the defined mesh.
//!
//! @param aP0[3] The first vertex of the intersection segment.
//! @param aP1[3] The second vertex of the intersection segment.
//! @return true indicates there was an intersection with the mesh, false otherwise.
//! @note Meshes should be closed and built from convex polygons.

// public
bool UtIntersectMesh::IntersectTest(const double aP0[3],
                                    const double aP1[3]) // indicates whether any mesh polygon intersects or not.
{
   double p0[3] = {aP0[0], aP0[1], aP0[2]};
   double p1[3] = {aP1[0], aP1[1], aP1[2]};
   if (mTransformed)
   {
      TransformPoint(p0, aP0);
      TransformPoint(p1, aP1);
   }

   SurfacesIter it;
   double       delta[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};
   double       dist;

   for (it = mSurfaceList.begin(); it != mSurfaceList.end(); ++it)
   {
      if (SurfaceIntersect(*it, aP0, delta, dist))
      {
         if ((dist >= 0) && (dist <= 1))
         {
            return true;
         }
      }
   }
   return false;
}

//! This method calculates the penetration distance of the segment into the mesh.
//!
//! @param aP0[3] The first vertex of the intersection segment.
//! @param aP1[3] The second vertex of the intersection segment.
//! @return The returned value will be the distance the segment penetrates the mesh.
//! @note Meshes should be closed and built from convex polygons.

// public
double UtIntersectMesh::Penetration(const double aP0[3],
                                    const double aP1[3]) // calculates the penetration distance of the segment into the
                                                         // mesh it is expected that the mesh is closed
{
   double p0[3] = {aP0[0], aP0[1], aP0[2]};
   double p1[3] = {aP1[0], aP1[1], aP1[2]};
   if (mTransformed)
   {
      TransformPoint(p0, aP0);
      TransformPoint(p1, aP1);
   }

   // generate the surface intersection list
   SurfacesIter      it;
   double            dist;
   std::list<double> hitList;
   double            delta[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};

   for (it = mSurfaceList.begin(); it != mSurfaceList.end(); ++it)
   {
      if (SurfaceIntersect(*it, aP0, delta, dist))
      {
         hitList.push_back(dist); // push dist onto surface intersection list
      }
   }
   hitList.sort(); // sort the surface intersection list

   if (hitList.empty())
   {
      return 0;
   }
   else
   {
      bool preSegmentHit = false; // this means that we are inside the mesh, but before the segment.
      bool in            = false;
      std::list<double>::iterator dit;
      double                      inDist  = 0;
      double                      lastHit = 2;
      double                      distSum = 0;
      double                      segLen  = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);

      for (dit = hitList.begin(); dit != hitList.end(); ++dit)
      {
         if (fabs((*dit - lastHit) * segLen) > 0.001) // make sure we have moved at least a milli-unit, otherwise assume
                                                      // these represent an edge or vertex hit
         {
            if (*dit < 0)
            {
               preSegmentHit = !preSegmentHit;
            }
            else if (*dit <= 1)
            {
               if (!in)
               {
                  inDist = *dit;
               }
               else if ((preSegmentHit) && (in))
               {
                  distSum += *dit;
                  preSegmentHit = false;
               }
               else
               {
                  distSum += *dit - inDist;
               }
            }
            else
            {
               if (in)
               {
                  if (preSegmentHit)
                  {
                     distSum       = 1;
                     preSegmentHit = false;
                  }
                  else
                  {
                     distSum += 1 - inDist;
                  }
               }
               break;
            }
            in = !in;
         }
         lastHit = *dit;
      }
      return distSum * segLen;
   }
}


//! this method will return the intersection point nearest to aP0 and between aP0 and aP1.
//!
//! @param aP0[3] The first vertex of the intersection segment.
//! @param aP1[3] The second vertex of the intersection segment.
//! @param mIsect[3] This is the point nearest to aP0 where the segment intersects the mesh.
//! @return true indicates there was an intersection with the mesh, false otherwise.  If false, mIsect contents will be
//! invalid.
//! @note results may be unreliable when the first point is contained in the mesh.

// public
bool UtIntersectMesh::FirstIntersect(const double aP0[3],
                                     const double aP1[3],
                                     double       aIsect[3]) // returns the first intersection point
{
   double p0[3] = {aP0[0], aP0[1], aP0[2]};
   double p1[3] = {aP1[0], aP1[1], aP1[2]};
   if (mTransformed)
   {
      TransformPoint(p0, aP0);
      TransformPoint(p1, aP1);
   }

   SurfacesIter it;
   double       dist;
   double       minDist  = 2;
   double       delta[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};

   for (it = mSurfaceList.begin(); it != mSurfaceList.end(); ++it)
   {
      if (SurfaceIntersect(*it, p0, delta, dist))
      {
         if ((dist < minDist) && (dist >= 0))
         {
            minDist = dist;
         }
      }
   }
   if (minDist > 1)
   {
      return false;
   }
   else
   {
      aIsect[0] = minDist * delta[0] + p0[0];
      aIsect[1] = minDist * delta[1] + p0[1];
      aIsect[2] = minDist * delta[2] + p0[2];
      if (mTransformed)
      {
         UntransformPoint(aIsect, aIsect);
      }
      return true;
   }
}

//
//! this method will return the intersection point nearest to aP0 and between aP0 and aP1.  It also return additional
//! information about the intersection.
//!
//! @param aP0[3] The first vertex of the intersection segment.
//! @param aP1[3] The second vertex of the intersection segment.
//! @param aMinDist This is the segment scaled distance to the first intersection (0 is aP0, 1 is aP1, 0.5 is halfway)
//! @param mIsect[3] This is the point nearest to aP0 where the segment intersects the mesh.
//! @param aNormal[3] This is the normal of the surface intersected with
//! @return true indicates there was an intersection with the mesh, false otherwise.  If false, mIsect contents will be
//! invalid.
//! @note results may be unreliable when the first point is contained in the mesh.

// public
bool UtIntersectMesh::FirstIntersectAndNormal(const double aP0[3],
                                              const double aP1[3],
                                              double&      aMinDist,
                                              double       aIsect[3], // returns the first intersection point
                                              double       aNormal[3])
{
   double p0[3] = {aP0[0], aP0[1], aP0[2]};
   double p1[3] = {aP1[0], aP1[1], aP1[2]};
   if (mTransformed)
   {
      TransformPoint(p0, aP0);
      TransformPoint(p1, aP1);
   }

   aMinDist        = 2;
   double delta[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};

   for (SurfacesIter it = mSurfaceList.begin(); it != mSurfaceList.end(); ++it)
   {
      double dist;
      if (SurfaceIntersect(*it, p0, delta, dist))
      {
         if ((dist < aMinDist) && (dist >= 0))
         {
            aMinDist   = dist;
            aNormal[0] = it->mPlane[0];
            aNormal[1] = it->mPlane[1];
            aNormal[2] = it->mPlane[2];
         }
      }
   }

   if (aMinDist <= 1)
   {
      double len = 1.0 / sqrt(aNormal[0] * aNormal[0] + aNormal[1] * aNormal[1] + aNormal[2] * aNormal[2]);
      aNormal[0] *= len;
      aNormal[1] *= len;
      aNormal[2] *= len;
      aIsect[0] = aMinDist * delta[0] + p0[0];
      aIsect[1] = aMinDist * delta[1] + p0[1];
      aIsect[2] = aMinDist * delta[2] + p0[2];

      if (mTransformed)
      {
         UntransformPoint(aIsect, aIsect);
      }

      return true;
   }
   return false;
}

//
//! this method will return the distance to the first intersect, the dot product of the query segment with the
//! intersection surface normal, and the surface code of the hit surface
//!
//! @param aP0[3] The first vertex of the intersection segment.
//! @param aP1[3] The second vertex of the intersection segment.
//! @param aMinDist This is the TRUE distance to the first intersection
//! @param aDotProduct This is the dot product of the normal of the surface intersected with the query segment
//! @param aSurfaceCode This is the surface code of the intersected surface.
//! @return true indicates there was an intersection with the mesh, false otherwise.  If false, mIsect contents will be
//! invalid.
//! @note results may be unreliable when the first point is contained in the mesh.

// public
bool UtIntersectMesh::GetDistDotAndCode(const double aP0[3],
                                        const double aP1[3],
                                        double&      aMinDist,
                                        double&      aDotProduct,
                                        int&         aSurfaceCode)
{
   double p0[3] = {aP0[0], aP0[1], aP0[2]};
   double p1[3] = {aP1[0], aP1[1], aP1[2]};
   if (mTransformed)
   {
      TransformPoint(p0, aP0);
      TransformPoint(p1, aP1);
   }

   SurfacesIter it;
   double       dist;
   aMinDist         = 2;
   double normal[3] = {0.0, 0.0, 0.0};

   double delta[3] = {p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]};

   for (it = mSurfaceList.begin(); it != mSurfaceList.end(); ++it)
   {
      if (SurfaceIntersect(*it, p0, delta, dist))
      {
         if ((dist < aMinDist) && (dist >= 0))
         {
            aMinDist     = dist;
            aSurfaceCode = it->mSurfaceCode;
            normal[0]    = it->mPlane[0];
            normal[1]    = it->mPlane[1];
            normal[2]    = it->mPlane[2];
         }
      }
   }

   if (aMinDist <= 1)
   {
      double invlen = 1.0 / sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
      normal[0] *= invlen;
      normal[1] *= invlen;
      normal[2] *= invlen;
      double segment[3] = {p0[0] - p1[0], p0[1] - p1[1], p0[2] - p1[2]};

      invlen = 1.0 / sqrt(segment[0] * segment[0] + segment[1] * segment[1] + segment[2] * segment[2]);
      segment[0] *= invlen;
      segment[1] *= invlen;
      segment[2] *= invlen;
      aDotProduct = segment[0] * normal[0] + segment[1] * normal[1] + segment[2] * normal[2];
      aMinDist /= invlen;
      return true;
   }
   return false;
}


//! this method tests for intersections between the given surface (aSurface) and a segment defined by the two points.
//! (actually one point (aP0), and the difference between the second point and the first (aDelta) )
//! two things are returned.  The boolean return value indicates whether the an intersection occurred inside
//! of the surface, this may have happened beyond the segment endpoints, though.  The aRetVal parameter indicates
//! a T calculated from the segment defined by aDelta * T + aP0.  If aRetVal < 0, the intersection happens before
//! aP0, else, if aRetVal <= 1, the intersection happens between aP0 and aP1, else the intersection happens beyond aP1.
//!
//! @param aSurface A surface to check intersections with.
//! @param aP0[3] The first point of the test segment.
//! @param aDelta[3] The second point of the test segment minus the first.
//! @param aRetVal This is the segment scaled distance to the first intersection (0 is aP0, 1 is aP1, 0.5 is halfway)
//! @return true indicates the line defined by aP0 and aDelta intersects aSurface, false indicates they are parallel

// private
bool UtIntersectMesh::SurfaceIntersect(const UtIntersectSurface& aSurface,
                                       const double              aP0[3],
                                       const double              aDelta[3],
                                       double&                   aRetVal)
{
   double denom = (aSurface.mPlane[0] * aDelta[0] + aSurface.mPlane[1] * aDelta[1] + aSurface.mPlane[2] * aDelta[2]);
   if (denom == 0) // segment is parallel to the surface plane, intersection is impossible if we treat coincident
                   // geometries as failure
   {
      return false;
   }

   // this is the place on the segment where an intersection may occur.  The point is calculated a aRetVal * (aP1 - aP0)
   // + aP0. The point may be beyond the endpoints of the segment.
   aRetVal =
      -(aSurface.mPlane[0] * aP0[0] + aSurface.mPlane[1] * aP0[1] + aSurface.mPlane[2] * aP0[2] + aSurface.mPlane[3]) /
      denom;

   // and here it is, the intersection point.
   double I[3] = {aDelta[0] * aRetVal + aP0[0], aDelta[1] * aRetVal + aP0[1], aDelta[2] * aRetVal + aP0[2]};
   double Q[3];
   std::list<UtIntersectTriangle>::const_iterator it;
   for (it = aSurface.mTriangleList.begin(); it != aSurface.mTriangleList.end(); ++it)
   {
      Q[0]          = I[0] - it->mV0[0];
      Q[1]          = I[1] - it->mV0[1];
      Q[2]          = I[2] - it->mV0[2]; // Q is the intersection point (I) relative to the triangles origin point (mV0)
      double dotE0Q = it->mE0[0] * Q[0] + it->mE0[1] * Q[1] + it->mE0[2] * Q[2];
      double dotE1Q = it->mE1[0] * Q[0] + it->mE1[1] * Q[1] + it->mE1[2] * Q[2];

      double E00 = it->mE0[0] * it->mE0[0] + it->mE0[1] * it->mE0[1] + it->mE0[2] * it->mE0[2];
      double E01 = it->mE0[0] * it->mE1[0] + it->mE0[1] * it->mE1[1] + it->mE0[2] * it->mE1[2];
      double E11 = it->mE1[0] * it->mE1[0] + it->mE1[1] * it->mE1[1] +
                   it->mE1[2] * it->mE1[2]; // I thought about pre-calculating these,
                                            // but the memory seems more valuable than
                                            // the speed given that mesh databases
                                            // might get very big

      double sigma0 = E11 * dotE0Q - E01 * dotE1Q;
      double sigma1 = E00 * dotE1Q - E01 * dotE0Q;
      if ((sigma0 >= 0) && (sigma1 >= 0) && (sigma0 + sigma1 <= E00 * E11 - E01 * E01))
      {
         // we have a line intersection in the triangle, and a line segment intersection if aRetVal >= 0 and aRetVal <= 1
         return true;
      }
   }
   // nothing was found to intersect the line, let alone the line segment
   return false;
}

//! Writes the mesh out to a file.
//!
//! @param aFilename The filename to write the mesh out to.

// public
void UtIntersectMesh::WriteToFile(const char* aFilename)
{
   std::ofstream output(aFilename);
   if (output)
   {
      output << std::fixed;
      output << mUniqueCode << '\n';
      output << mSurfaceList.size() << '\n';
      for (const auto& surface : mSurfaceList)
      {
         output << ' ' << surface.mSurfaceCode << '\n';
         output << ' ' << surface.mPlane[0] << ' ' << surface.mPlane[1] << ' ' << surface.mPlane[2] << ' '
                << surface.mPlane[3] << '\n';
         output << ' ' << surface.mTriangleList.size() << '\n';
         for (const auto& tri : surface.mTriangleList)
         {
            output << "  " << tri.mV0[0] << ' ' << tri.mV0[1] << ' ' << tri.mV0[2] << '\n';
            output << "  " << tri.mE0[0] << ' ' << tri.mE0[1] << ' ' << tri.mE0[2] << '\n';
            output << "  " << tri.mE1[0] << ' ' << tri.mE1[1] << ' ' << tri.mE1[2] << '\n';
         }
      }
   }
}

// public
void UtIntersectMesh::SetPositionOrientation(double aX, double aY, double aZ, double aRoll, double aPitch, double aYaw)
{
   mTransformed = true;
   double cx    = cos(aRoll);
   double sx    = sin(aRoll);
   double cy    = cos(aPitch);
   double sy    = sin(aPitch);
   double cz    = cos(aYaw);
   double sz    = sin(aYaw);

   double dvector[16] = {cy * cz,
                         sx * sy * cz - cx * sz,
                         cx * sy * cz + sx * sy,
                         aX,
                         cy * sz,
                         sx * sy * sz + cx * cz,
                         cx * sy * sz - sx * cz,
                         aY,
                         -sy,
                         sx * cy,
                         cx * cy,
                         aZ,
                         0,
                         0,
                         0,
                         1};

   mMatrix.CopyFrom(dvector);
   mInvMatrix = mMatrix;
   mInvMatrix.Invert();
}

void UtIntersectMesh::TransformPoint(double aOut[3], const double aIn[3])
{
   double matArray[16];
   mInvMatrix.CopyTo(matArray, 16);
   aOut[0] = aIn[0] * matArray[0] + aIn[1] * matArray[1] + aIn[2] * matArray[2] + matArray[3];
   aOut[1] = aIn[0] * matArray[4] + aIn[1] * matArray[5] + aIn[2] * matArray[6] + matArray[7];
   aOut[2] = aIn[0] * matArray[8] + aIn[1] * matArray[9] + aIn[2] * matArray[10] + matArray[11];
}

void UtIntersectMesh::UntransformPoint(double aOut[3], const double aIn[3])
{
   double matArray[16];
   mMatrix.CopyTo(matArray, 16);
   aOut[0] = aIn[0] * matArray[0] + aIn[1] * matArray[1] + aIn[2] * matArray[2] + matArray[3];
   aOut[1] = aIn[0] * matArray[4] + aIn[1] * matArray[5] + aIn[2] * matArray[6] + matArray[7];
   aOut[2] = aIn[0] * matArray[8] + aIn[1] * matArray[9] + aIn[2] * matArray[10] + matArray[11];
}

int UtIntersectMesh::GetSurfaceCount()
{
   return static_cast<int>(mSurfaceList.size());
}

int UtIntersectMesh::GetSurfaceIdCount()
{
   return static_cast<int>(mSurfaceIdList.size());
}

int UtIntersectMesh::GetSurfaceId(int aIndex)
{
   int                     i = 0;
   std::set<int>::iterator it;
   for (it = mSurfaceIdList.begin(); it != mSurfaceIdList.end(); ++it)
   {
      if (i == aIndex)
      {
         return (*it);
      }
      ++i;
   }
   return -1;
}

void UtIntersectMesh::GetSurfaceCenter(int aSurfaceId, double& aCenterX, double& aCenterY, double& aCenterZ)
{
   std::map<int, IsecMeshBoundingBox>::const_iterator bbit = mBoundingBox.find(aSurfaceId);

   if (bbit != mBoundingBox.end())
   {
      const IsecMeshBoundingBox& bbox = bbit->second;

      aCenterX = 0.5 * (bbox.GetMinX() + bbox.GetMaxX());
      aCenterY = 0.5 * (bbox.GetMinY() + bbox.GetMaxY());
      aCenterZ = 0.5 * (bbox.GetMinZ() + bbox.GetMaxZ());
   }
}

UtIntersectMesh::SurfacesIter UtIntersectMesh::GetFirstSurface() const
{
   return mSurfaceList.begin();
}

UtIntersectMesh::SurfacesIter UtIntersectMesh::GetLastSurface() const
{
   return mSurfaceList.end();
}

int UtIntersectMesh::GetSurfaceCode(int aIndex)
{
   if ((aIndex >= 0) && (aIndex < static_cast<int>(mSurfaceList.size())))
   {
      return mSurfaceList[aIndex].mSurfaceCode;
   }

   return -1;
}

void UtIntersectMesh::SetSurfaceCode(int aIndex, int aSurfaceCode)
{
   if ((aIndex >= 0) && (aIndex < static_cast<int>(mSurfaceList.size())))
   {
      UtIntersectSurface& surface = mSurfaceList[aIndex];

      if (surface.mSurfaceCode != aSurfaceCode)
      {
         int oldCode          = surface.mSurfaceCode;
         surface.mSurfaceCode = aSurfaceCode;

         // Update bounding box of old surface code, removing the code if it does not exist anymore
         mBoundingBox.erase(oldCode);
         bool found = false;
         for (SurfacesIter sit = mSurfaceList.begin(); sit != mSurfaceList.end(); ++sit)
         {
            if (sit->mSurfaceCode == oldCode)
            {
               found = true;
               for (const auto& tri : sit->mTriangleList)
               {
                  mBoundingBox[oldCode].GrowBy(tri.mV0);
                  double mv1[3] = {tri.mV0[0] + tri.mE0[0], tri.mV0[1] + tri.mE0[1], tri.mV0[2] + tri.mE0[2]};
                  double mv2[3] = {tri.mV0[0] + tri.mE1[0], tri.mV0[1] + tri.mE1[1], tri.mV0[2] + tri.mE1[2]};
                  mBoundingBox[oldCode].GrowBy(mv1);
                  mBoundingBox[oldCode].GrowBy(mv2);
               }
            }
         }

         if (!found)
         {
            mSurfaceIdList.erase(oldCode);
         }

         // Add new surface code to surface id list
         mSurfaceIdList.insert(aSurfaceCode);

         // Update bounding box of new surface code
         for (UtIntersectSurface::TrianglesIter trit = surface.mTriangleList.begin(); trit != surface.mTriangleList.end();
              ++trit)
         {
            mBoundingBox[aSurfaceCode].GrowBy(trit->mV0);
            double mv1[3] = {trit->mV0[0] + trit->mE0[0], trit->mV0[1] + trit->mE0[1], trit->mV0[2] + trit->mE0[2]};
            double mv2[3] = {trit->mV0[0] + trit->mE1[0], trit->mV0[1] + trit->mE1[1], trit->mV0[2] + trit->mE1[2]};
            mBoundingBox[aSurfaceCode].GrowBy(mv1);
            mBoundingBox[aSurfaceCode].GrowBy(mv2);
         }
      }
   }
}

bool UtIntersectMesh::HasSurfaceId(int aSurfaceId)
{
   return (mSurfaceIdList.find(aSurfaceId) != mSurfaceIdList.end());
}

void IsecMeshBoundingBox::GrowBy(const double aInput[3])
{
   if (!mInitialized)
   {
      mData[0] = mData[3] = aInput[0];
      mData[1] = mData[4] = aInput[1];
      mData[2] = mData[5] = aInput[2];
      mInitialized        = true;
   }
   else
   {
      mData[0] = (mData[0] < aInput[0]) ? mData[0] : aInput[0];
      mData[1] = (mData[1] < aInput[1]) ? mData[1] : aInput[1];
      mData[2] = (mData[2] < aInput[2]) ? mData[2] : aInput[2];
      mData[3] = (mData[3] > aInput[0]) ? mData[3] : aInput[0];
      mData[4] = (mData[4] > aInput[1]) ? mData[4] : aInput[1];
      mData[5] = (mData[5] > aInput[2]) ? mData[5] : aInput[2];
   }
}
