// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef OBJ_POINT_COLLECTION_HPP
#define OBJ_POINT_COLLECTION_HPP

#include <vector>

#include "UtCast.hpp"
#include "GeometryObjFile.hpp"
#include "UtVec3dX.hpp"
#include "Vehicle.hpp"

namespace Designer
{
   class ObjRingPoint
   {
      public:

         ObjRingPoint(double x, double y, double z);
         virtual ~ObjRingPoint();

         void SetPreviousRingPoint(ObjRingPoint* aPreviousRingPoint);
         void SetNextRingPoint(ObjRingPoint* aNextRingPoint);

         void SetPreviousLongitudinalPoint(ObjRingPoint* aPreviousLongitudinalPoint);
         void SetNextLongitudinalPoint(ObjRingPoint* aNextLongitudinalPoint);

         void CalculateNormal();
         void Draw(GeometryObjFile& aGeometryObjFile);

         UtVec3dX GetPoint() { return mPointVec; }
         UtVec3dX GetNormal() { return mNormalVec; }

      protected:
         UtVec3dX      mPointVec;
         UtVec3dX      mNormalVec;
         ObjRingPoint* mPreviousRingPoint         = nullptr;
         ObjRingPoint* mNextRingPoint             = nullptr;
         ObjRingPoint* mPreviousLongitudinalPoint = nullptr;
         ObjRingPoint* mNextLongitudinalPoint     = nullptr;
   };

   class ObjPointRing
   {
      public:

         explicit ObjPointRing(bool aCircularList);
         virtual ~ObjPointRing();

         void AddPoint(double x, double y, double z);

         void SetPreviousRing(ObjPointRing* aPreviousRing);
         void SetNextRing(ObjPointRing* aNextRing);

         ObjRingPoint* GetPoint(size_t aIndex);

         void CalculatePointConnections();
         void CalculateNormals();
         void Draw(GeometryObjFile& aGeometryObjFile);

      protected:
         std::vector<ObjRingPoint*> mPointList;
         ObjPointRing*              mPreviousRing   = nullptr;
         ObjPointRing*              mNextRing       = nullptr;
         bool                       mListIsCircular = false;
   };

   class ObjPointCollection
   {
      public:

         ObjPointCollection(bool aCircularList, GeometryObjFile& aGeometryObjFile);
         virtual ~ObjPointCollection();

         void AddRing();
         void AddPoint(double x, double y, double z);
         void CalculatePointCollection();
         void Draw();

      protected:
         std::vector<ObjPointRing*> mObjPointRingList;
         size_t                     mCurrentRingIndex     = ut::npos;
         bool                       mCollectionIsCircular = false;
         GeometryObjFile&           mGeometryObjFile;
   };
}

#endif // !OBJ_POINT_COLLECTION_HPP
