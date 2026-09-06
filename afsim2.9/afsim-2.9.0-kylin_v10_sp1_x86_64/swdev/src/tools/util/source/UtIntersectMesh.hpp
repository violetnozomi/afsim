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

#ifndef UTINTERSECTMESH_HPP
#define UTINTERSECTMESH_HPP

#include "ut_export.h"

#include <list>
#include <map>
#include <set>

#include "UtMatrix.hpp"

// This struct defines a triangle optimized for intersection tests.
struct UtIntersectTriangle
{
   double mV0[3]; // the 'first' point of the triangle
   double mE0[3]; // the 'second' point of the triangle - mV0, mV1 = mV0 + mE0
   double mE1[3]; // the 'third' point of the triangle - mV0, mV2 = mV0 + mE1
};

// This struct defines a surface optimized for intersection tests.
struct UtIntersectSurface
{
   int    mSurfaceCode; // used if you want to encode information into a surface (what kind of surface is it?)
   double mPlane[4]; // the plane equation for the surface of the form 0 = mPlane[0] * x + mPlane[1] * y + mPlane[2] * z + mPlane[3]

   using Triangles     = std::list<UtIntersectTriangle>;
   using TrianglesIter = Triangles::const_iterator;
   Triangles mTriangleList; // a list of triangles that form the surface.  These should all be on mPlane
};

class IsecMeshBoundingBox
{
public:
   virtual ~IsecMeshBoundingBox() = default;

   void   GrowBy(const double aInput[3]);
   double GetMinX() const { return mData[0]; }
   double GetMinY() const { return mData[1]; }
   double GetMinZ() const { return mData[2]; }
   double GetMaxX() const { return mData[3]; }
   double GetMaxY() const { return mData[4]; }
   double GetMaxZ() const { return mData[5]; }

private:
   bool   mInitialized{false};
   double mData[6]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // min x, y, z, max x, y, z
};

// This class defines a mesh optimized for intersection tests.  For drawing, it would not be the best choice.
class UT_EXPORT UtIntersectMesh
{
public:
   UtIntersectMesh();
   UtIntersectMesh(const std::string& aFileName, bool& aSuccess);
   virtual ~UtIntersectMesh() = default;

   void AddPolygon(const std::list<double*>& aPolygon,
                   const int                 aSurfaceCode = 0); // at the moment this is assumed to be a convex polygon

   bool IntersectTest(const double aP0[3],
                      const double aP1[3]); // indicates whether a polygon intersects or not.  It is fastest, but gives
                                            // the least information.
   double Penetration(const double aP0[3],
                      const double aP1[3]); // calculates the penetration distance of the segment into the mesh
   bool   FirstIntersect(const double aP0[3],
                         const double aP1[3],
                         double       aIsect[3]); // returns the first intersection point.
   bool   FirstIntersectAndNormal(const double aP0[3],
                                  const double aP1[3],
                                  double&      aMinDist,
                                  double       aIsect[3],
                                  double       aNormal[3]);
   bool GetDistDotAndCode(const double aP0[3], const double aP1[3], double& aMinDist, double& aDotProduct, int& aSurfaceCode);

   void WriteToFile(const char* aFilename);

   void SetPositionOrientation(double aX, double aY, double aZ, double aRoll, double aPitch, double aYaw);
   int  GetSurfaceCount();
   int  GetSurfaceIdCount();
   int  GetSurfaceId(int aIndex);
   bool HasSurfaceId(int aSurfaceId);
   void GetSurfaceCenter(int aSurfaceId, double& aCenterX, double& aCenterY, double& aCenterZ);

   int  GetSurfaceCode(int aIndex);
   void SetSurfaceCode(int aIndex, int aSurfaceCode);

   using Surfaces     = std::vector<UtIntersectSurface>;
   using SurfacesIter = Surfaces::const_iterator;

   SurfacesIter GetFirstSurface() const;
   SurfacesIter GetLastSurface() const;

private:
   static bool SurfaceIntersect(const UtIntersectSurface& aSurface,
                                const double              aP0[3],
                                const double              aDelta[3],
                                double&                   aRetVal); // this is a check against a single surface.
   void        TransformPoint(double aOut[3], const double aIn[3]);
   void        UntransformPoint(double aOut[3], const double aIn[3]);

   Surfaces      mSurfaceList;
   std::set<int> mSurfaceIdList;

   std::string                        mUniqueCode;
   bool                               mTransformed;
   UtMatrix<double>                   mMatrix;
   UtMatrix<double>                   mInvMatrix;
   std::map<int, IsecMeshBoundingBox> mBoundingBox;
};

#endif
