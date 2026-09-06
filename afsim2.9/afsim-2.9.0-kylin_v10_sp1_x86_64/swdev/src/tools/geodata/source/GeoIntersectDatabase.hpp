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

#ifndef GeoIntersectDatabase_hpp
#define GeoIntersectDatabase_hpp

#include "geodata_export.h"

#include <map>

#include "GeoShapeFile.hpp"
#include "UtIntersectMesh.hpp"
#include "UtSpatialTree.hpp"

// this class represents an environment of intersectable meshes.
class GEODATA_EXPORT GeoIntersectDatabase
{
public:
   GeoIntersectDatabase(GeoShapeFile& aGeoShapeFile, const unsigned int aTreeDepth);
   ~GeoIntersectDatabase();

   void BuildIntersectMesh(GeoShapeFile::Shape& aShape,
                           const unsigned int   aMeshId,
                           const double         aTransECEF[3][3],
                           const double         aRefECEF[3]);

   bool IntersectTest(const double aLLA1[3], const double aLLA2[3]);

   bool FirstIntersectAndNormal(const double aLLA1[3],
                                const double aLLA2[3],
                                double       aMinDist,
                                double       aIsect[3],
                                double       aNormal[3]);

   double Penetration(const double aLLA1[3],
                      const double aLLA2[3]); // calculates the penetration distance of the segment into the database
   bool   IsValid() { return mIsValid; }

private:
   double mTransECEF[3][3];
   double mRefECEF[3];

   struct IntersectDatum
   {
      unsigned int    mQueryId;
      UtIntersectMesh mMesh;
   };
   UtQuadTree*                            mTreePtr;
   std::map<unsigned int, IntersectDatum> mMeshMap;

   unsigned int mNextMeshId;
   unsigned int mQueryId;
   bool         mIsValid;
};

// this class converts a shape into a mesh
// it then sits in the tree and provides query tests
class IntersectMeshDatum : public UtQuadDatum
{
public:
   IntersectMeshDatum(GeoShapeFile::Shape& aShape,
                      const unsigned int   aMeshId,
                      const double         aTransECEF[3][3],
                      const double         aRefECEF[3]);
   ~IntersectMeshDatum() override;

   bool IsPoint() const override { return false; }

   bool IsWhollyContainedIn(const UtBoundingBox& aBox) const override;

   bool IsPartiallyContainedIn(const UtBoundingBox& aBox) const override;

   const UtBoundingBoxI<2> GetBoundingBox() const { return mBoundingBox; }
   unsigned int            GetMeshId() const { return mMeshId; }

private:
   unsigned int      mMeshId;
   UtBoundingBoxI<2> mBoundingBox;
};

#endif // GeoIntersectDatabase_hpp
