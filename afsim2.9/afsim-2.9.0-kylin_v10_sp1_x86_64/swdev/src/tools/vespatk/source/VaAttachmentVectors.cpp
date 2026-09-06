// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaAttachmentVectors.hpp"

#include <sstream>
#include <vector>

#include "UtCast.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLLAPos.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSun.hpp"
#include "UtoUtility.hpp"
#include "UtoVectorShape.hpp"
#include "VaAttachmentModel.hpp"
#include "VaEntity.hpp"
#include "VaEphemeris.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaSelectionBox.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
unsigned int VaAttachmentVectors::sId = 0;

//! Method: VaAttachmentVectors
//! Description: VaAttachmentVectors constructor.  VaAttachmentVectors is an attachment
//!              that will display a vector sets when attached to a VaEntity.
//!
//! @param aParent - This parameter is the parent entity. This should always be a valid entity.

// public

VaAttachmentVectors::VaAttachmentVectors(VaEntity& aParent, VaViewer* aViewerPtr)
   : VaAttachment(aParent, aViewerPtr, "vectors")
   , mVelocityVector(-1)
   , mEarthVector(-1)
   , mSunVector(-1)
   , mMoonVector(-1)
   , mNextCustomSetIndex(eVECTOR_STATIC_COUNT)
   , mModelScalar(1.0f)
{
   SetName(GetName() + "-" + VaUtils::ToString<unsigned int>(++sId));
   SetType<VaAttachmentVectors>();

   // can we look around at our parent to find a proper scalar for the vectors?
   vespa::VaAttachmentModel* modelPtr = GetParent().FindFirstAttachmentOfType<vespa::VaAttachmentModel>();
   if (modelPtr)
   {
      mModelScalar = modelPtr->GetModelScale();
      mCallbacks.Add(
         VaObserver::ModelScaleChanged.Connect(&VaAttachmentVectors::ModelScaleChangedCB, this, modelPtr->GetUniqueId()));
   }
   mCallbacks.Add(VaObserver::MapProjectionChanged.Connect(&VaAttachmentVectors::MapProjectionChangedCB, this));
   SetStateVisibility((aViewerPtr->GetMapProjection().projection == UtoMapProjection::Geocentric) ? true : false);
}

//! Method: ~VaAttachmentVectors
//! Description: VaAttachmentVectors destructor.
//!

// public
VaAttachmentVectors::~VaAttachmentVectors()
{
   mCallbacks.Clear();
}

// static
std::string VaAttachmentVectors::GetCategoryTextStatic(int aValue)
{
   switch (aValue)
   {
   case (eVECTOR_BODY_XYZ):
      return "Body Frame";
      break;
   case (eVECTOR_VELOCITY):
      return "Velocity";
      break;
   case (eVECTOR_EARTH):
      return "Earth Nadir";
      break;
   case (eVECTOR_SUN):
      return "Sun";
      break;
   case (eVECTOR_MOON):
      return "Moon";
      break;
   default:
      return "";
      break;
   }
}

unsigned int VaAttachmentVectors::GetCategoryCount()
{
   return ut::safe_cast<unsigned int, int>(mNextCustomSetIndex); // TODO + mUserDefinedCount;
}

std::string VaAttachmentVectors::GetCategoryText(int aValue)
{
   if (aValue < eVECTOR_STATIC_COUNT)
   {
      return GetCategoryTextStatic(aValue);
   }
   else
   {
      return mCustomVectorMap[aValue].mLabel;
   }
}

void VaAttachmentVectors::Update(double aTime)
{
   if (mVelocityVector >= 0)
   {
      if (mVectorShapeMap[eVECTOR_VELOCITY].mVisible)
      {
         UtVec3d wcsxyz = GetParent().GetVelocityWCS();
         float   az     = atan2(wcsxyz[1], wcsxyz[0]);
         float   el     = atan2(wcsxyz[2], sqrt(wcsxyz[0] * wcsxyz[0] + wcsxyz[1] * wcsxyz[1]));
         mVectorShapeMap[eVECTOR_VELOCITY].mShape->UpdateVector(mVelocityVector, az, el);
         mVectorShapeMap[eVECTOR_VELOCITY].mPosition = UtVec3f(wcsxyz);
         CallPositionCallback(eVECTOR_VELOCITY, mVectorShapeMap[eVECTOR_VELOCITY].mPosition);
      }
   }
   if (mEarthVector >= 0)
   {
      if (mVectorShapeMap[eVECTOR_EARTH].mVisible)
      {
         double xyz[3];
         GetParent().GetPosition().GetECEF(xyz);
         float az = atan2(-xyz[1], -xyz[0]);
         float el = atan2(-xyz[2], sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]));
         mVectorShapeMap[eVECTOR_EARTH].mShape->UpdateVector(mEarthVector, az, el);
         mVectorShapeMap[eVECTOR_EARTH].mPosition = -(UtVec3f(xyz));
         CallPositionCallback(eVECTOR_EARTH, mVectorShapeMap[eVECTOR_EARTH].mPosition);
      }
   }
   if (mSunVector >= 0)
   {
      if (mVectorShapeMap[eVECTOR_SUN].mVisible)
      {
         double sun[3];
         UtSun::GetSunLocationWCS(VaEphemeris::GetSimTime(), sun);
         float az = atan2(sun[1], sun[0]);
         float el = atan2(sun[2], sqrt(sun[0] * sun[0] + sun[1] * sun[1]));
         mVectorShapeMap[eVECTOR_SUN].mShape->UpdateVector(mSunVector, az, el);
         mVectorShapeMap[eVECTOR_SUN].mPosition = UtVec3f(sun);
         CallPositionCallback(eVECTOR_SUN, mVectorShapeMap[eVECTOR_SUN].mPosition);
         // we are ignoring the position of the platform... the sun is somewhat far away
      }
   }
   if (mMoonVector >= 0)
   {
      if (mVectorShapeMap[eVECTOR_MOON].mVisible)
      {
         double mooneci[3];
         double moonecef[3];
         double xyz[3];
         GetParent().GetPosition().GetECEF(xyz);
         UtMoon::GetLocationECI(VaEphemeris::GetSimTime(), mooneci);
         UtEllipsoidalEarth::ConvertTODToECEF(VaEphemeris::GetSimTime().GetEarthAngle(), mooneci, moonecef);
         UtVec3d moonxyz(moonecef[0] - xyz[0], moonecef[1] - xyz[1], moonecef[2] - xyz[2]);
         mVectorShapeMap[eVECTOR_MOON].mPosition = moonxyz;
         float az                                = atan2(moonxyz[1], moonxyz[0]);
         float el = atan2(moonxyz[2], sqrt(moonxyz[0] * moonxyz[0] + moonxyz[1] * moonxyz[1]));
         mVectorShapeMap[eVECTOR_MOON].mShape->UpdateVector(mMoonVector, az, el);
         CallPositionCallback(eVECTOR_MOON, mVectorShapeMap[eVECTOR_MOON].mPosition);
      }
   }
   for (size_t i = 0; i < mCustomVector.size(); ++i)
   {
      vespa::VaEntity* ent = GetParent().GetScenario()->FindEntity(mCustomVectorTarget[i]);
      if (nullptr != ent)
      {
         double tgtxyz[3];
         double xyz[3];
         GetParent().GetPosition().GetECEF(xyz);
         ent->GetPosition().GetECEF(tgtxyz);
         UtVec3d             tgtd(tgtxyz[0] - xyz[0], tgtxyz[1] - xyz[1], tgtxyz[2] - xyz[2]);
         float               az   = atan2(tgtd[1], tgtd[0]);
         float               el   = atan2(tgtd[2], sqrt(tgtd[0] * tgtd[0] + tgtd[1] * tgtd[1]));
         std::pair<int, int> addr = mCustomVector[i];
         mVectorShapeMap[addr.first].mShape->UpdateVector(addr.second, az, el);
         mVectorShapeMap[addr.first].mPosition = UtVec3f{tgtd};
         CallPositionCallback(addr.first, mVectorShapeMap[addr.first].mPosition);
         mVectorShapeMap[addr.first].mShape->ShowVector(addr.second, ent->IsVisible(GetViewer()));
      }
   }
}

void VaAttachmentVectors::UpdateVisibility()
{
   for (auto&& vectorShape : mVectorShapeMap)
   {
      bool visible = IsVisible() && vectorShape.second.mVisible;
      vectorShape.second.mShape->Show(visible, *(GetViewer()->GetViewer()));
   }
}

void VaAttachmentVectors::ShowVector(int aVectorId, bool aState)
{
   std::map<int, Vector>::iterator it = mVectorShapeMap.find(aVectorId);
   if (it != mVectorShapeMap.end())
   {
      it->second.mShape->Show(aState && GetParent().IsVisible(GetViewer()), *(GetViewer()->GetViewer()));
      it->second.mVisible = aState;
   }
   else if (aState)
   {
      Load(aVectorId);
      // create a new shape
      if (aVectorId == eVECTOR_BODY_XYZ)
      {
         mVectorShapeMap[aVectorId].mShape->AddVector(0, 0, osg::Vec4(1, 0, 0, 1));
         mVectorShapeMap[aVectorId].mShape->AddVector(static_cast<float>(UtMath::cPI_OVER_2), 0.0f, osg::Vec4(0, 1, 0, 1));
         mVectorShapeMap[aVectorId].mShape->AddVector(0.0f, static_cast<float>(UtMath::cPI_OVER_2), osg::Vec4(0, 0, 1, 1));
         mVectorShapeMap[aVectorId].mVisible = true;

         if (!GetParent().IsVisible(GetViewer()))
         {
            mVectorShapeMap[aVectorId].mShape->Show(false, *(GetViewer()->GetViewer()));
         }
      }
      else if (aVectorId == eVECTOR_VELOCITY)
      {
         UtVec3d wcsxyz  = GetParent().GetVelocityWCS();
         float   az      = atan2(wcsxyz[1], wcsxyz[0]);
         float   el      = atan2(wcsxyz[2], sqrt(wcsxyz[0] * wcsxyz[0] + wcsxyz[1] * wcsxyz[1]));
         mVelocityVector = mVectorShapeMap[aVectorId].mShape->AddVector(az, el, osg::Vec4(1, 1, 1, 1));
         mVectorShapeMap[aVectorId].mVisible  = true;
         mVectorShapeMap[aVectorId].mPosition = wcsxyz;
         CallPositionCallback(aVectorId, mVectorShapeMap[aVectorId].mPosition);
         if (!GetParent().IsVisible(GetViewer()))
         {
            mVectorShapeMap[aVectorId].mShape->Show(false, *(GetViewer()->GetViewer()));
         }
      }
      else if (aVectorId == eVECTOR_EARTH)
      {
         double xyz[3];
         GetParent().GetPosition().GetECEF(xyz);
         float az     = atan2(-xyz[1], -xyz[0]);
         float el     = atan2(-xyz[2], sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]));
         mEarthVector = mVectorShapeMap[aVectorId].mShape->AddVector(az, el, osg::Vec4(0, 1, 1, 1));
         mVectorShapeMap[aVectorId].mVisible      = true;
         mVectorShapeMap[eVECTOR_EARTH].mPosition = -(UtVec3f(xyz));
         ;
         CallPositionCallback(eVECTOR_EARTH, mVectorShapeMap[eVECTOR_EARTH].mPosition);
         if (!GetParent().IsVisible(GetViewer()))
         {
            mVectorShapeMap[aVectorId].mShape->Show(false, *(GetViewer()->GetViewer()));
         }
      }
      else if (aVectorId == eVECTOR_SUN)
      {
         double sun[3];
         UtSun::GetSunLocationWCS(VaEphemeris::GetSimTime(), sun);
         float az   = atan2(sun[1], sun[0]);
         float el   = atan2(sun[2], sqrt(sun[0] * sun[0] + sun[1] * sun[1]));
         mSunVector = mVectorShapeMap[aVectorId].mShape->AddVector(az, el, osg::Vec4(1, 1, 0, 1));
         mVectorShapeMap[aVectorId].mVisible  = true;
         mVectorShapeMap[aVectorId].mPosition = UtVec3f(sun);
         CallPositionCallback(aVectorId, mVectorShapeMap[aVectorId].mPosition);
         if (!GetParent().IsVisible(GetViewer()))
         {
            mVectorShapeMap[aVectorId].mShape->Show(false, *(GetViewer()->GetViewer()));
         }
      }
      else if (aVectorId == eVECTOR_MOON)
      {
         // TODO_NOT_USED double earthAngle = 0;
         double mooneci[3];
         double moonecef[3];
         double xyz[3];
         GetParent().GetPosition().GetECEF(xyz);
         UtMoon::GetLocationECI(VaEphemeris::GetSimTime(), mooneci);
         UtEllipsoidalEarth::ConvertTODToECEF(VaEphemeris::GetSimTime().GetEarthAngle(), mooneci, moonecef);
         UtVec3d moonxyz(moonecef[0] - xyz[0], moonecef[1] - xyz[1], moonecef[2] - xyz[2]);
         mVectorShapeMap[aVectorId].mPosition = moonxyz;
         float az                             = atan2(moonxyz[1], moonxyz[0]);
         float el    = atan2(moonxyz[2], sqrt(moonxyz[0] * moonxyz[0] + moonxyz[1] * moonxyz[1]));
         mMoonVector = mVectorShapeMap[aVectorId].mShape->AddVector(az, el, osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
         CallPositionCallback(aVectorId, mVectorShapeMap[aVectorId].mPosition);
         mVectorShapeMap[aVectorId].mVisible = true;
         if (!GetParent().IsVisible(GetViewer()))
         {
            mVectorShapeMap[aVectorId].mShape->Show(false, *(GetViewer()->GetViewer()));
         }
      }
      else
      {
      }
   }
   UpdateVisibility();
}

bool VaAttachmentVectors::IsShown(int aVectorId)
{
   std::map<int, Vector>::iterator it = mVectorShapeMap.find(aVectorId);
   if (it != mVectorShapeMap.end())
   {
      return it->second.mVisible;
   }
   return false;
}

void VaAttachmentVectors::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection)
{
   if (aViewerId == GetViewer()->GetUniqueId())
   {
      SetStateVisibility((aProjection->projection == UtoMapProjection::Geocentric) ? true : false);
   }
}

void VaAttachmentVectors::Load(int aVectorId)
{
   mCustomVectorMap[aVectorId].mLabel = GetCategoryText(aVectorId);

   // Remove the previous shape.
   RemoveShapeFromParent(mCustomVectorMap[aVectorId].mLabel + std::to_string(GetUniqueId()));
   mVectorShapeMap[aVectorId].mShape = nullptr;

   // Load the shape.
   UtoVectorShape vector;

   mVectorShapeMap[aVectorId].mShape = dynamic_cast<UtoVectorShape*>(
      AddShapeToParent(mCustomVectorMap[aVectorId].mLabel + std::to_string(GetUniqueId()), vector));
   mVectorShapeMap[aVectorId].mVisible = true;
   mVectorShapeMap[aVectorId].mShape->SetScalar(20 * mModelScalar);

   if (aVectorId == eVECTOR_BODY_XYZ)
   {
      mVectorShapeMap[aVectorId].mShape->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
   }
}

int VaAttachmentVectors::AddVector(const std::string& aTitle, float aRed, float aGreen, float aBlue, float aAlpha)
{
   UtoVectorShape vector;
   int            idx = mNextCustomSetIndex;
   ++mNextCustomSetIndex;
   mCustomVectorMap[idx].mLabel = aTitle;
   mVectorShapeMap[idx].mShape  = dynamic_cast<UtoVectorShape*>(
      AddShapeToParent(mCustomVectorMap[idx].mLabel + std::to_string(GetUniqueId()), vector));
   mVectorShapeMap[idx].mVisible = true;
   mVectorShapeMap[idx].mShape->SetScalar(20 * mModelScalar);
   mCustomVectorMap[idx].mColor = osg::Vec4(aRed, aGreen, aBlue, aAlpha);
   return idx;
}

void VaAttachmentVectors::ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   // If boresight does not exist, create it. Otherwise update it and make it visible.
   if (mBoresightIdTitles.count(aLabel) == 0 || mVectorShapeMap.find(mBoresightIdTitles[aLabel]) == mVectorShapeMap.end())
   {
      int idx = mNextCustomSetIndex++;

      mBoresightIdTitles[aLabel] = idx;

      mCustomVectorMap[idx].mLabel = aLabel;
      mCustomVectorMap[idx].mColor = osg::Vec4(0, 1.0f, 1.0f, 1.0f);

      UtoVectorShape                     vectorShape;
      vespa::VaAttachmentVectors::Vector vector;
      vector.mShape = dynamic_cast<UtoVectorShape*>(
         AddShapeToParent(mCustomVectorMap[idx].mLabel + std::to_string(GetUniqueId()), vectorShape));
      vector.mShape->AddVector(aAzimuth, -aElevation, mCustomVectorMap[idx].mColor);
      vector.mShape->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      vector.mVisible = true;
      vector.mShape->SetScalar(20 * mModelScalar);
      mVectorShapeMap[idx] = std::move(vector);

      UpdateBoresight(aLabel, aAzimuth, aElevation);
   }
   else
   {
      auto it = mVectorShapeMap.find(mBoresightIdTitles[aLabel]);
      UpdateBoresight(aLabel, aAzimuth, aElevation);
      it->second.mShape->Show(GetParent().IsVisible(GetViewer()), *(GetViewer()->GetViewer()));
      it->second.mVisible = true;
   }
}

void VaAttachmentVectors::UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   auto it = mVectorShapeMap.find(mBoresightIdTitles[aLabel]);
   if (it != mVectorShapeMap.end())
   {
      it->second.mShape->UpdateVector(0, aAzimuth, -aElevation);

      osg::Vec4d posECS(cos(aAzimuth) * cos(-aElevation), sin(aAzimuth) * cos(-aElevation), sin(-aElevation), 1);

      // Calculate conversion matrix from ECS to WCS
      osg::Matrix ECS_ToWCS_Transform;
      auto        entity = GetParent().ToUtEntity();
      GetViewer()->GetViewer()->Projection()->GetECS_Frame(entity, ECS_ToWCS_Transform);
      ECS_ToWCS_Transform = osg::Matrix::inverse(ECS_ToWCS_Transform);

      osg::Vec4d posWCS    = ECS_ToWCS_Transform.postMult(posECS);
      it->second.mPosition = UtVec3d(posWCS[0], posWCS[1], posWCS[2]);

      CallPositionCallback(mBoresightIdTitles[aLabel], it->second.mPosition);
   }
}

void VaAttachmentVectors::HideBoresight(const std::string& aLabel)
{
   auto it = mVectorShapeMap.find(mBoresightIdTitles[aLabel]);
   if (it != mVectorShapeMap.end())
   {
      it->second.mShape->Show(false, *(GetViewer()->GetViewer()));
      it->second.mVisible = false;
   }
}

int VaAttachmentVectors::GetBoresightId(const std::string& aTitle)
{
   auto it = mBoresightIdTitles.find(aTitle);
   if (it != mBoresightIdTitles.end())
   {
      return it->second;
   }
   return -1;
}

void VaAttachmentVectors::RemoveVector(int aIndex)
{
   mCustomVectorMap.erase(aIndex);
   std::string shapeName = mVectorShapeMap[aIndex].mShape->GetName();
   RemoveShapeFromParent(shapeName);
   mVectorShapeMap.erase(aIndex);

   // clean up mCustomVector
   std::vector<std::pair<int, int>> temp;
   for (auto&& it : mCustomVector)
   {
      if (it.first != aIndex)
      {
         temp.push_back(it);
      }
   }
   mCustomVector = temp;
}

void VaAttachmentVectors::AddVectorToEntity(int aIndex, VaEntity* aEntity)
{
   if (aEntity->GetUniqueId() != GetParent().GetUniqueId())
   {
      double tgtxyz[3];
      double xyz[3];
      GetParent().GetPosition().GetECEF(xyz);
      aEntity->GetPosition().GetECEF(tgtxyz);
      UtVec3d tgtd(tgtxyz[0] - xyz[0], tgtxyz[1] - xyz[1], tgtxyz[2] - xyz[2]);
      float   az  = atan2(tgtd[1], tgtd[0]);
      float   el  = atan2(tgtd[2], sqrt(tgtd[0] * tgtd[0] + tgtd[1] * tgtd[1]));
      int     idx = mCustomVector.size();
      mCustomVector.emplace_back(aIndex,
                                 mVectorShapeMap[aIndex].mShape->AddVector(az, el, mCustomVectorMap[aIndex].mColor));
      mCustomVectorTarget.push_back(aEntity->GetUniqueId());
      if (!aEntity->IsVisible(GetViewer()))
      {
         mVectorShapeMap[aIndex].mShape->ShowVector(mCustomVector[idx].second, false);
      }
      mVectorShapeMap[aIndex].mPosition = UtVec3f(tgtd);
      CallPositionCallback(aIndex, mVectorShapeMap[aIndex].mPosition);
   }
}

void VaAttachmentVectors::RemoveVectorToEntity(int aIndex, VaEntity* aEntity)
{
   std::set<unsigned int> remSet;
   int                    idx = 0;
   for (auto& it : mCustomVectorTarget)
   {
      if (it == aEntity->GetUniqueId())
      {
         mVectorShapeMap[aIndex].mShape->RemoveVector(mCustomVector[idx].second);
         mVectorShapeMap[aIndex].mVisible = false;
         remSet.insert(idx);
      }
      ++idx;
   }
   int count = 0;
   for (auto& it : remSet)
   {
      mCustomVector.erase(mCustomVector.begin() + it - count);
      mCustomVectorTarget.erase(mCustomVectorTarget.begin() + it - count);
      ++count;
   }
}

UtVec3f VaAttachmentVectors::GetVectorPosition(int aIndex)
{
   if (mVectorShapeMap.find(aIndex) != mVectorShapeMap.end())
   {
      return mVectorShapeMap.at(aIndex).mPosition;
   }
   return UtVec3f{};
}

float VaAttachmentVectors::GetVectorScale(int aIndex) const
{
   if (mVectorShapeMap.find(aIndex) != mVectorShapeMap.end())
   {
      return mVectorShapeMap.at(aIndex).mShape->GetScalar();
   }
   return -1;
}

void VaAttachmentVectors::SubscribeToPositionUpdate(const std::function<void(int, UtVec3f)>& aCallBack)
{
   mPositionUpdatedCallback = aCallBack;
   mIsCallbackPositionSet   = true;
}

void VaAttachmentVectors::CallPositionCallback(int aVectorId, const UtVec3f& aPosition)
{
   if (mIsCallbackPositionSet)
   {
      mPositionUpdatedCallback(aVectorId, aPosition);
   }
}

void VaAttachmentVectors::ModelScaleChangedCB(float aScalar)
{
   mModelScalar = aScalar;
   for (auto vectorShape : mVectorShapeMap)
   {
      vectorShape.second.mShape->SetScalar(20 * mModelScalar);
   }
}

} // end namespace vespa.
