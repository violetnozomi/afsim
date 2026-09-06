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

#include "GeoIntersectDatabase.hpp"

#include "UtEllipsoidalEarth.hpp"


//! Builds an intersection database from a geodetic shapefile
//!
//! @param aGeoShapeFile the input should be a shapefile in geodetic coordinate system.
//! @param aTreeDepth the desired quadtree depth.  Performance will vary based on density, geometry and size.

// public
GeoIntersectDatabase::GeoIntersectDatabase(GeoShapeFile& aGeoShapeFile, const unsigned int aTreeDepth)
   : mNextMeshId(0)
   , mQueryId(1)
   , mIsValid(true)
{
   mIsValid = aGeoShapeFile.IsValid();

   double centerLat;
   double centerLon;
   double centerAlt;
   double minLat;
   double minLon;
   double minAlt;
   double maxLat;
   double maxLon;
   double maxAlt;
   double minXYZ[3];
   double maxXYZ[3];

   aGeoShapeFile.GetCenter(centerLat, centerLon, centerAlt);

   UtEllipsoidalEarth::ComputeNEDTransform(centerLat,
                                           centerLon,
                                           0,
                                           mTransECEF,
                                           mRefECEF); // build a Cartesian coordinate system
                                                      // for calculations since we are using a
                                                      // NED cs and a quadtree based on the
                                                      // north and east coordinate, this class
                                                      // will be limited in area it is able to
                                                      // represent.  hopefully we won't be
                                                      // modeling urban environments spanning
                                                      // over 90 degrees of the earth!

   aGeoShapeFile.GetLLABoundingBox(minLat, minLon, minAlt, maxLat, maxLon, maxAlt);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, minLat, minLon, 0, minXYZ);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, maxLat, maxLon, 0, maxXYZ);
   UtBoundingBoxI<2> boundingBox(minXYZ, maxXYZ);

   mTreePtr = new UtQuadTree(boundingBox, aTreeDepth); // the depth of 8 is arbitrary and should be tested

   unsigned int meshId = 0;

   for (auto&& shape : aGeoShapeFile.GetShapeList())
   {
      if ((shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygon) ||
          (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonM) ||
          (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonZ))
      {
         meshId = mNextMeshId;
         ++mNextMeshId;

         BuildIntersectMesh(shape, meshId, mTransECEF, mRefECEF);
         mTreePtr->InsertArea(new IntersectMeshDatum(shape, meshId, mTransECEF, mRefECEF));
      }
   }
}

// virtual
//! deconstructor
//!

// public
GeoIntersectDatabase::~GeoIntersectDatabase()
{
   delete mTreePtr;
}

//! Builds an intersection mesh from a GeoShapeFile GeoShape, the mesh is added to the database.
//!
//! @param aShape The source shape
//! @param aMeshId The source mesh id.
//! @param aTransECEF[3][3] A local coordinate system transformation.
//! @param aRefECEF[3] A local coordinate system reference.

// public
void GeoIntersectDatabase::BuildIntersectMesh(GeoShapeFile::Shape& aShape,
                                              const unsigned int   aMeshId,
                                              const double         aTransECEF[3][3],
                                              const double         aRefECEF[3])
{
   std::list<double*> vec;
   double*            pointList;
   double             tempPoint[3];
   IntersectDatum     isectDat;
   size_t             numPoints;
   size_t             index;
   size_t             i;

   for (auto&& part : aShape.GetPartList())
   {
      if (part.mWinding > 0)
      {
         numPoints = part.mPointList.size();
         index     = 0;
         pointList = new double[numPoints * 6];

         for (auto&& pos : part.mPointList)
         {
            UtEllipsoidalEarth::ConvertLLAToLocal(aRefECEF, aTransECEF, pos.x, pos.y, pos.z, tempPoint);
            pointList[3 * index]     = tempPoint[0];
            pointList[3 * index + 1] = tempPoint[1];
            pointList[3 * index + 2] = tempPoint[2];
            UtEllipsoidalEarth::ConvertLLAToLocal(aRefECEF, aTransECEF, pos.x, pos.y, pos.z + aShape.GetHeight(), tempPoint);
            pointList[3 * (index + numPoints)]     = tempPoint[0];
            pointList[3 * (index + numPoints) + 1] = tempPoint[1];
            pointList[3 * (index + numPoints) + 2] = tempPoint[2];
            ++index;
            // get a vertex list in out desired coordinate system
         }
         // 1st add the base
         vec.clear();
         for (i = 0; i < numPoints; ++i)
         {
            vec.push_back(&pointList[i * 3]);
         }
         isectDat.mMesh.AddPolygon(vec);
         // 2nd add the top
         vec.clear();
         for (i = 0; i < numPoints; ++i)
         {
            vec.push_back(&pointList[(2 * numPoints - i - 1) * 3]);
         }
         isectDat.mMesh.AddPolygon(vec);
         // finally add the walls
         for (i = 0; i < numPoints; ++i)
         {
            vec.clear();
            if (i == 0)
            {
               vec.push_back(&pointList[i * 3]);
               vec.push_back(&pointList[(numPoints - 1) * 3]);
               vec.push_back(&pointList[(numPoints * 2 - 1) * 3]);
               vec.push_back(&pointList[(numPoints)*3]);
               isectDat.mMesh.AddPolygon(vec);
            }
            else
            {
               vec.push_back(&pointList[(i - 1) * 3]);
               vec.push_back(&pointList[(i + numPoints - 1) * 3]);
               vec.push_back(&pointList[(i + numPoints) * 3]);
               vec.push_back(&pointList[i * 3]);
               isectDat.mMesh.AddPolygon(vec);
            }
         }

         // build the geometry
         delete[] pointList;
      }
   }
   isectDat.mQueryId = 0;
   mMeshMap[aMeshId] = isectDat;
}

//! Tests for binary intersection between the database and a line segment
//!
//! @param aLLA1[3] This is the first end-point of the line segment
//! @param aLLA2[3] This is the second end-point of the line segment
//! @return true indicates a hit was found, false otherwise (clear line-of-sight)

// public
bool GeoIntersectDatabase::IntersectTest(const double aLLA1[3], const double aLLA2[3])
{
   std::list<UtSpatialNode<2>*> nodeList;
   int                          id;
   double                       xyz1[3];
   double                       xyz2[3];

   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA1[0], aLLA1[1], aLLA1[2], xyz1);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA2[0], aLLA2[1], aLLA2[2], xyz2);

   mTreePtr->Search(xyz1, xyz2, nodeList);

   std::list<UtSpatialNode<2>*>::iterator it;
   SpatialData                            data;
   SpatialData::iterator                  dit;
   const IntersectMeshDatum*              isectDatum;

   for (it = nodeList.begin(); it != nodeList.end(); ++it)
   {
      data = (*it)->GetData();

      for (dit = data.begin(); dit != data.end(); ++dit)
      {
         isectDatum = static_cast<const IntersectMeshDatum*>(*dit);
         id         = isectDatum->GetMeshId();
         if (mMeshMap[id].mQueryId != mQueryId)
         {
            mMeshMap[id].mQueryId = mQueryId;
            if (mMeshMap[id].mMesh.IntersectTest(xyz1, xyz2))
            {
               ++mQueryId;
               return true;
            }
         }
      }
   }
   ++mQueryId;
   return false;
}

//! Returns the first intersection point and the normal of the surface intersected.  The normal is in database
//! coordinate system, so this may not be useful, yet.
//!
//! @param aLLA1[3] This is the first end-point of the line segment in geodetic cs
//! @param aLLA2[3] This is the second end-point of the line segment in geodetic cs
//! @param aMinDist This is the segment scaled distance to the first intersection (0 is aLLA1, 1 is aLLA2, 0.5 is halfway)
//! @param aIsect[3] This is the intersection closest to aLLA1 in geodetic cs.
//! @param aNormal[3] This is the normal in local CS.
//! @return true indicates and intersection, false otherwise.  aNormal, aMinDist, and aIsect will be unreliable on a false return.

// public
bool GeoIntersectDatabase::FirstIntersectAndNormal(const double aLLA1[3],
                                                   const double aLLA2[3],
                                                   double       aMinDist,
                                                   double       aIsect[3],
                                                   double       aNormal[3])
{
   aMinDist                            = 2.0;
   bool                         retval = false;
   std::list<UtSpatialNode<2>*> nodeList;
   int                          id;
   double                       xyz1[3];
   double                       xyz2[3];
   double                       dist = 2.0;
   double                       isectPoint[3];
   double                       local[3];
   double                       normal[3];

   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA1[0], aLLA1[1], aLLA1[2], xyz1);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA2[0], aLLA2[1], aLLA2[2], xyz2);

   mTreePtr->Search(xyz1, xyz2, nodeList);

   std::list<UtSpatialNode<2>*>::iterator it;
   SpatialData                            data;
   SpatialData::iterator                  dit;
   const IntersectMeshDatum*              isectDatum;

   int nodeCount = 0;
   int meshCount = 0;

   for (it = nodeList.begin(); it != nodeList.end(); ++it)
   {
      ++nodeCount;
      data = (*it)->GetData();

      for (dit = data.begin(); dit != data.end(); ++dit)
      {
         ++meshCount;
         isectDatum = static_cast<const IntersectMeshDatum*>(*dit);
         id         = isectDatum->GetMeshId();
         if (mMeshMap[id].mQueryId != mQueryId)
         {
            mMeshMap[id].mQueryId = mQueryId;
            if (mMeshMap[id].mMesh.FirstIntersectAndNormal(xyz1, xyz2, dist, isectPoint, normal))
            {
               if (dist < aMinDist)
               {
                  aMinDist   = dist;
                  local[0]   = isectPoint[0];
                  local[1]   = isectPoint[1];
                  local[2]   = isectPoint[2];
                  aNormal[0] = normal[0];
                  aNormal[1] = normal[1];
                  aNormal[2] = normal[2];
               }
               retval = true;
            }
         }
      }
   }
   ++mQueryId;
   UtEllipsoidalEarth::ConvertLocalToLLA(mRefECEF, mTransECEF, local, aIsect[0], aIsect[1], aIsect[2]);
   return retval;
}

//! This method calculates the penetration distance of the segment into the database.
//!
//! @param aLLA1[3] This is the first end-point of the line segment in geodetic cs
//! @param aLLA2[3] This is the second end-point of the line segment in geodetic cs
//! @return the penetration distance.  This is the sum of pentrations with all meshes, overlap is possible, so you could get a
//!         return longer than the segment length.

// public
double GeoIntersectDatabase::Penetration(const double aLLA1[3],
                                         const double aLLA2[3]) // calculates the penetration distance of the segment
                                                                // into the database
{
   double                       retval = 0;
   std::list<UtSpatialNode<2>*> nodeList;

   int    id;
   double xyz1[3];
   double xyz2[3];

   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA1[0], aLLA1[1], aLLA1[2], xyz1);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLLA2[0], aLLA2[1], aLLA2[2], xyz2);

   mTreePtr->Search(xyz1, xyz2, nodeList);

   std::list<UtSpatialNode<2>*>::iterator it;
   SpatialData                            data;
   SpatialData::iterator                  dit;
   const IntersectMeshDatum*              isectDatum;

   int nodeCount = 0;
   int meshCount = 0;

   for (it = nodeList.begin(); it != nodeList.end(); ++it)
   {
      ++nodeCount;
      data = (*it)->GetData();

      for (dit = data.begin(); dit != data.end(); ++dit)
      {
         ++meshCount;
         isectDatum = static_cast<const IntersectMeshDatum*>(*dit);
         id         = isectDatum->GetMeshId();
         if (mMeshMap[id].mQueryId != mQueryId)
         {
            mMeshMap[id].mQueryId = mQueryId;
            retval += mMeshMap[id].mMesh.Penetration(xyz1, xyz2);
         }
      }
   }
   ++mQueryId;
   return retval;
}

//! constructor for the quadtree datum.  It is a bounding area and meshid.
//!
//! @param aShape The shape, used to get a bounding area
//! @param aMeshId The unique mesh id.
//! @param aTransECEF[3][3] A local CS transform.
//! @param aRefECEF[3] A local CS reference.

// public
IntersectMeshDatum::IntersectMeshDatum(GeoShapeFile::Shape& aShape,
                                       const unsigned int   aMeshId,
                                       const double         aTransECEF[3][3],
                                       const double         aRefECEF[3])
{
   double minLat;
   double minLon;
   double minAlt;
   double maxLat;
   double maxLon;
   double maxAlt;
   double minXYZ[3];
   double maxXYZ[3];
   aShape.GetLLABoundingBox(minLat, minLon, minAlt, maxLat, maxLon, maxAlt);
   UtEllipsoidalEarth::ConvertLLAToLocal(aRefECEF, aTransECEF, minLat, minLon, minAlt, minXYZ);
   UtEllipsoidalEarth::ConvertLLAToLocal(aRefECEF, aTransECEF, maxLat, maxLon, maxAlt, maxXYZ);
   mBoundingBox.SetLeast(minXYZ);
   mBoundingBox.SetGreatest(maxXYZ);
   mMeshId = aMeshId;
}

//! destructor
//!

// public
IntersectMeshDatum::~IntersectMeshDatum() {}

// virtual
//! Indicates whether the datum is entirely in aBox
//!
//! @param aBox The bounding box checked against.
//! @return true is this is in aBox completely, false otherwise

// public
bool IntersectMeshDatum::IsWhollyContainedIn(const UtBoundingBox& aBox) const
{
   unsigned int i;
   for (i = 0; i < mBoundingBox.GetDimension(); ++i)
   {
      if (mBoundingBox.GetLeast(i) < aBox.GetLeast(i))
      {
         return false;
      }
      if (mBoundingBox.GetGreatest(i) > aBox.GetGreatest(i))
      {
         return false;
      }
   }
   return true;
}

// virtual
//! Indicates whether the datum overlaps aBox
//!
//! @param aBox The bounding box to check against.
//! @return true indicates an intersection between the areas, false otherwise.

// public
bool IntersectMeshDatum::IsPartiallyContainedIn(const UtBoundingBox& aBox) const
{
   return mBoundingBox.Intersects(aBox);
}
