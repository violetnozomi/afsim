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

#include "ObjPointCollection.hpp"

#include "UtMath.hpp"

namespace Designer
{

ObjRingPoint::ObjRingPoint(double x, double y, double z)
   : mPointVec(x, y, z)
   , mNormalVec(0.0, 0.0, 0.0)
{
}

ObjRingPoint::~ObjRingPoint()
{
}

void ObjRingPoint::SetPreviousRingPoint(ObjRingPoint* aPreviousRingPoint)
{
   mPreviousRingPoint = aPreviousRingPoint;
}

void ObjRingPoint::SetNextRingPoint(ObjRingPoint* aNextRingPoint)
{
   mNextRingPoint = aNextRingPoint;
}

void ObjRingPoint::SetPreviousLongitudinalPoint(ObjRingPoint* aPreviousLongitudinalPoint)
{
   mPreviousLongitudinalPoint = aPreviousLongitudinalPoint;
}

void ObjRingPoint::SetNextLongitudinalPoint(ObjRingPoint* aNextLongitudinalPoint)
{
   mNextLongitudinalPoint = aNextLongitudinalPoint;
}

void ObjRingPoint::CalculateNormal()
{
   UtVec3dX vector1(0.0, 0.0, 0.0);
   UtVec3dX vector2(0.0, 0.0, 0.0);
   UtVec3dX vector3(0.0, 0.0, 0.0);
   UtVec3dX vector4(0.0, 0.0, 0.0);

   if (mPreviousRingPoint != nullptr)
   {
      if (mNextLongitudinalPoint != nullptr)
      {
         UtVec3dX vecA = mPreviousRingPoint->GetPoint() - mPointVec;
         UtVec3dX vecB = mNextLongitudinalPoint->GetPoint() - mPointVec;
         vector1 = vecA.Cross(vecB);
         vector1.Normalize();
      }
      if (mPreviousLongitudinalPoint != nullptr)
      {
         UtVec3dX vecA = mPreviousRingPoint->GetPoint() - mPointVec;
         UtVec3dX vecB = mPreviousLongitudinalPoint->GetPoint() - mPointVec;
         vector4 = vecB.Cross(vecA);
         vector4.Normalize();
      }
   }
   if (mNextRingPoint != nullptr)
   {
      if (mNextLongitudinalPoint != nullptr)
      {
         UtVec3dX vecA = mNextRingPoint->GetPoint() - mPointVec;
         UtVec3dX vecB = mNextLongitudinalPoint->GetPoint() - mPointVec;
         vector2 = vecB.Cross(vecA);
         vector2.Normalize();
      }
      if (mPreviousLongitudinalPoint != nullptr)
      {
         UtVec3dX vecA = mNextRingPoint->GetPoint() - mPointVec;
         UtVec3dX vecB = mPreviousLongitudinalPoint->GetPoint() - mPointVec;
         vector3 = vecA.Cross(vecB);
         vector3.Normalize();
      }
   }

   mNormalVec.Set(0.0, 0.0, 0.0);

   mNormalVec = vector1 + vector2 + vector3 + vector4;
   mNormalVec.Normalize();
}

void ObjRingPoint::Draw(GeometryObjFile& aGeometryObjFile)
{
   if ((mNextLongitudinalPoint != nullptr) && (mNextRingPoint != nullptr))
   {
      if (mNextLongitudinalPoint->mNextRingPoint != nullptr)
      {
         aGeometryObjFile.ObjBegin();

         aGeometryObjFile.ObjNormal3d(mNormalVec.X(), mNormalVec.Y(), mNormalVec.Z());
         aGeometryObjFile.ObjVertex3d(mPointVec.X(), mPointVec.Y(), mPointVec.Z());

         UtVec3dX pt2 = mNextLongitudinalPoint->GetPoint();
         UtVec3dX norm2 = mNextLongitudinalPoint->GetNormal();
         aGeometryObjFile.ObjNormal3d(norm2.X(), norm2.Y(), norm2.Z());
         aGeometryObjFile.ObjVertex3d(pt2.X(), pt2.Y(), pt2.Z());

         UtVec3dX pt3 = mNextLongitudinalPoint->mNextRingPoint->GetPoint();
         UtVec3dX norm3 = mNextLongitudinalPoint->mNextRingPoint->GetNormal();
         aGeometryObjFile.ObjNormal3d(norm3.X(), norm3.Y(), norm3.Z());
         aGeometryObjFile.ObjVertex3d(pt3.X(), pt3.Y(), pt3.Z());

         UtVec3dX pt4 = mNextRingPoint->GetPoint();
         UtVec3dX norm4 = mNextRingPoint->GetNormal();
         aGeometryObjFile.ObjNormal3d(norm4.X(), norm4.Y(), norm4.Z());
         aGeometryObjFile.ObjVertex3d(pt4.X(), pt4.Y(), pt4.Z());

         aGeometryObjFile.ObjEnd();
      }
   }
}

ObjPointRing::ObjPointRing(bool aCircularList)
   : mListIsCircular(aCircularList)
{
}

ObjPointRing::~ObjPointRing()
{
   for (auto& pointPtr : mPointList)
   {
      delete pointPtr;
   }

   mPointList.clear();
}

void ObjPointRing::AddPoint(double x, double y, double z)
{
   mPointList.push_back(new ObjRingPoint(x, y, z));
}

void ObjPointRing::SetPreviousRing(ObjPointRing* aPreviousRing)
{
   mPreviousRing = aPreviousRing;
}

void ObjPointRing::SetNextRing(ObjPointRing* aNextRing)
{
   mNextRing = aNextRing;
}

ObjRingPoint* ObjPointRing::GetPoint(size_t aIndex)
{
   if (!mPointList.empty())
   {
      if (aIndex < mPointList.size())
      {
         return mPointList.at(aIndex);
      }
   }

   return nullptr;
}

void ObjPointRing::CalculatePointConnections()
{
   size_t num = mPointList.size();

   for (size_t i = 0; i < num ; ++i)
   {
      if (i == 0)
      {
         // First item in list

         if (mListIsCircular)
         {
            // Wrap around to the last point in ring
            mPointList.at(i)->SetPreviousRingPoint(mPointList.at(num - 1));
         }

         // Connect to the next point in the ring
         mPointList.at(i)->SetNextRingPoint(mPointList.at(i + 1));
      }
      else if (i == (num - 1))
      {
         // Last item in list

         // Connect to the previous point in the ring
         mPointList.at(i)->SetPreviousRingPoint(mPointList.at(i - 1));

         if (mListIsCircular)
         {
            // Wrap around to the first point in ring
            mPointList.at(i)->SetNextRingPoint(mPointList.at(0));
         }
      }
      else
      {
         // Connect to the previous point in the ring
         mPointList.at(i)->SetPreviousRingPoint(mPointList.at(i - 1));
         // Connect to the next point in the ring
         mPointList.at(i)->SetNextRingPoint(mPointList.at(i + 1));
      }

      // Connect to the point in the previous ring
      if (mPreviousRing != nullptr)
      {
         ObjRingPoint* prevPoint = mPreviousRing->GetPoint(i);
         if (prevPoint != nullptr)
         {
            mPointList.at(i)->SetPreviousLongitudinalPoint(prevPoint);
         }
      }

      // Connect to the point in the next ring
      if (mNextRing != nullptr)
      {
         ObjRingPoint* nextPoint = mNextRing->GetPoint(i);
         if (nextPoint != nullptr)
         {
            mPointList.at(i)->SetNextLongitudinalPoint(nextPoint);
         }
      }
   }
}

void ObjPointRing::CalculateNormals()
{
   for (auto& pointPtr : mPointList)
   {
      pointPtr->CalculateNormal();
   }
}

void ObjPointRing::Draw(GeometryObjFile& aGeometryObjFile)
{
   for (auto& pointPtr : mPointList)
   {
      pointPtr->Draw(aGeometryObjFile);
   }
}

ObjPointCollection::ObjPointCollection(bool aCircularList, GeometryObjFile& aGeometryObjFile)
   : mCollectionIsCircular(aCircularList)
   , mGeometryObjFile(aGeometryObjFile)
{
}

ObjPointCollection::~ObjPointCollection()
{
   for (auto& ringPtr : mObjPointRingList)
   {
      delete ringPtr;
   }

   mObjPointRingList.clear();
}

void ObjPointCollection::AddRing()
{
   mCurrentRingIndex = mObjPointRingList.size();
   mObjPointRingList.push_back(new ObjPointRing(mCollectionIsCircular));
}

void ObjPointCollection::AddPoint(double x, double y, double z)
{
   if (mCurrentRingIndex != ut::npos)
   {
      mObjPointRingList.at(mCurrentRingIndex)->AddPoint(x, y, z);
   }
}

void ObjPointCollection::CalculatePointCollection()
{
   // Tell each ring about its neighbors
   ObjPointRing* prevRing = nullptr;
   for (auto& ringPtr : mObjPointRingList)
   {
      // Check for a previous ring
      if (prevRing != nullptr)
      {
         // Tell the previous ring this is its "next"
         prevRing->SetNextRing(ringPtr);

         // Tell this ring of its "previous"
         ringPtr->SetPreviousRing(prevRing);
      }

      // Remember this ring
      prevRing = ringPtr;
   }

   // Tell each ring to calculate its connections
   for (auto& ringPtr : mObjPointRingList)
   {
      ringPtr->CalculatePointConnections();
   }

   // Tell each ring to calculate its normals
   for (auto& ringPtr : mObjPointRingList)
   {
      ringPtr->CalculateNormals();
   }
}

void ObjPointCollection::Draw()
{
   for (auto& ringPtr : mObjPointRingList)
   {
      ringPtr->Draw(mGeometryObjFile);
   }
}

} // namespace Designer
