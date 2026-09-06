// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "zone/WkfAttachmentZonePolygon.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoPolyLineShape.hpp"
#include "UtoTessellatedPolygonShape.hpp"
#include "UtoUtility.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaSelectionBox.hpp"
#include "VaViewerManager.hpp"
#include "zone/WkfZonePoint.hpp"

namespace wkf
{
AttachmentZonePolygon::AttachmentZonePolygon(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : AttachmentZone(aParent, aViewerPtr, "polygon_zone")
   , mPolylinePtr(nullptr)
   , mPolylineShapeName()
   , mPolygonPtr(nullptr)
   , mPolygonShapeName()
   , mClosed(false)
   , mHideZonePoints(false)
   , mZonePoints()
   , mRelative(true)
   , mPointsLoaded(false)
   , mDepthTest(true)
   , mAntiAliasLines(false)
   , mNeverShowZonePoints(false)
   , mSelectionCleared()
{
   mModifierList.clear();
   mModifierKeys.clear();
   SetName(GetName() + "_" + std::to_string(GetUniqueId()));
   SetType<AttachmentZonePolygon>();
}

AttachmentZonePolygon::~AttachmentZonePolygon()
{
   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      delete *iter;
   }
   mZonePoints.clear();
   mModifierList.clear();
   mModifierKeys.clear();
}

vespa::VaObject* AttachmentZonePolygon::FindSubPart(const vespa::VaAttachmentSubId& aSubId) const
{
   return FindPoint(aSubId.mSubId);
}

std::string AttachmentZonePolygon::GetMenuSelectionString(const vespa::VaAttachmentSubId& aAttachmentSubPartId) const
{
   std::string selectionString;
   if (aAttachmentSubPartId == 0)
   {
      selectionString = GetName();
   }
   else
   {
      for (ZonePoints::const_iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
      {
         if ((*iter)->GetUniqueId() == (unsigned int)(aAttachmentSubPartId.mSubId))
         {
            selectionString = (*iter)->GetName();
            break;
         }
      }
   }
   return selectionString;
}

bool AttachmentZonePolygon::Pick(vespa::VaViewer*                aViewerPtr,
                                 int                             aMouseX,
                                 int                             aMouseY,
                                 bool                            aToggle,
                                 bool                            aDoubleClick,
                                 const vespa::VaAttachmentSubId& aAttachmentSubPartId)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return false;
   }
   bool selected = false;
   int  subpart;

   if (!mZonePoints.empty())
   {
      if (aAttachmentSubPartId.mSubId == 0)
      {
         subpart = (*mZonePoints.begin())->GetUniqueId();
      }
      else
      {
         subpart = aAttachmentSubPartId.mSubId;
      }

      ZonePoints::iterator iter = mZonePoints.end();

      // If we have a valid zone point unique id, then find it.
      if (aAttachmentSubPartId.mSubId != 0)
      {
         iter = std::find_if(mZonePoints.begin(), mZonePoints.end(), ZonePoint::FindPred(subpart));
      }

      // If we found a zone point by unique id, use it. Otherwise, use the last zone point in the list.
      ZonePoint* zonePointPtr = (iter != mZonePoints.end()) ? *iter : *mZonePoints.rbegin();
      if (zonePointPtr != nullptr)
      {
         if (!mPointsLoaded)
         {
            LoadZonePoints();
         }
         selected = aToggle ? !zonePointPtr->GetSelected(aViewerPtr) : true;
         zonePointPtr->SetVisible(selected);
         zonePointPtr->SetSelected(aViewerPtr, selected);
         mSelectionCleared[aViewerPtr->GetUniqueId()] = false;
         selected                                     = true;
      }
      SetHideZonePoints(!selected);
   }
   return selected;
}

bool AttachmentZonePolygon::BoxPick(vespa::VaViewer* aViewerPtr, const vespa::VaSelectionBox& aBox, bool aInside)
{
   bool picked = false;

   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      ZonePoint*               zonePointPtr = *iter;
      const vespa::VaPosition& pos          = zonePointPtr->GetPosition();

      if (!aBox.Contains(pos.GetLat(), pos.GetLon(), pos.GetAlt()) ^ aInside)
      {
         if (!mPointsLoaded)
         {
            LoadZonePoints();
         }
         mSelectionCleared[aViewerPtr->GetUniqueId()] = false;
         zonePointPtr->SetSelected(aViewerPtr, true);
         picked = true;
      }
   }
   SetHideZonePoints(!picked);
   return picked;
}

vespa::VaAttachment::DeleteAction AttachmentZonePolygon::DeleteSelection(vespa::VaViewer* aViewerPtr)
{
   VaAttachment::DeleteAction deleteAction = VaAttachment::cDELETE_ATTACHMENT;

   ZonePoints::iterator iter = mZonePoints.begin();
   while (iter != mZonePoints.end())
   {
      bool del = false;
      if ((*iter)->GetSelected(aViewerPtr))
      {
         del          = RemoveZonePoint((*iter)->GetUniqueId());
         deleteAction = VaAttachment::cDELETE_NOTHING;
      }

      if (!del)
         ++iter;
      else
         iter = mZonePoints.begin();
   }

   // If there are no more zone points, delete the entity.
   if (mZonePoints.empty())
   {
      deleteAction = VaAttachment::DeleteSelection(aViewerPtr);
   }
   else if (mZonePoints.size() == 1)
   {
      // TODO UNUSED VARIABLE ZonePoint* zonePointPtr = *mZonePoints.begin();
      SetHideZonePoints(false);
   }
   else
   {
      SetHideZonePoints(true);
   }
   return deleteAction;
}

void AttachmentZonePolygon::ClearSelection(vespa::VaViewer* aViewerPtr)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return;
   }
   if ((mSelectionCleared.find(aViewerPtr->GetUniqueId()) == mSelectionCleared.end()) ||
       (!mSelectionCleared[aViewerPtr->GetUniqueId()]))
   {
      mSelectionCleared[aViewerPtr->GetUniqueId()] = true;

      for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
      {
         (*iter)->SetSelected(aViewerPtr, false);
      }
      AttachmentSelect(aViewerPtr, 0, vespa::VaAttachmentSubId());
      if (mZonePoints.size() > 1)
      {
         SetHideZonePoints(true);
      }
      else
      {
         SetHideZonePoints(false);
      }
   }
}

void AttachmentZonePolygon::SetSelected(vespa::VaViewer*                aViewerPtr,
                                        bool                            aSelected,
                                        const vespa::VaAttachmentSubId& aSubId,
                                        bool                            aForce)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return;
   }
   if (aSelected)
   {
      mSelectionCleared[aViewerPtr->GetUniqueId()] = false;
      if (!mPointsLoaded)
      {
         LoadZonePoints();
      }
      if (mZonePoints.size() > 1)
      {
         SetHideZonePoints(!aSelected);
      }
      else
      {
         SetHideZonePoints(false);
      }
   }
   if (aSubId.mSubId > 0)
   {
      ZonePoints::iterator iter = find_if(mZonePoints.begin(), mZonePoints.end(), ZonePoint::FindPred(aSubId.mSubId));
      if (iter != mZonePoints.end())
      {
         (*iter)->SetSelected(aViewerPtr, aSelected);
      }
   }
   else
   {
      ZonePoints::iterator iter;
      for (iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
      {
         (*iter)->SetSelected(aViewerPtr, aSelected);
      }
   }
}

void AttachmentZonePolygon::MoveRelative(UtEntity&               aRefBefore,
                                         UtEntity&               aRefAfter,
                                         int                     aFeedbackFormat,
                                         double                  aFeedbackPrecision,
                                         vespa::MoveRelativeMask aMoveRelativeMask,
                                         vespa::VaViewer*        aViewerPtr)
{
   if (!IsShared())
   {
      double dxy[2] = {0, 0};
      if (mRelative)
      {
         double afterlla[3];
         double beforella[3];
         aRefBefore.GetLocationLLA(beforella[0], beforella[1], beforella[2]);
         aRefAfter.GetLocationLLA(afterlla[0], afterlla[1], afterlla[2]);
         double heading;
         double distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(afterlla[0], afterlla[1], beforella[0], beforella[1], heading, distance);
         dxy[0] = cos(heading * UtMath::cRAD_PER_DEG) * distance;
         dxy[1] = sin(heading * UtMath::cRAD_PER_DEG) * distance;
      }

      for (auto&& zp : mZonePoints)
      {
         if (zp->GetSelected(aViewerPtr))
         {
            vespa::VaPosition pos(zp->GetPosition());
            if (!mRelative)
            {
               double entityLLA[3] = {pos.GetLat(), pos.GetLon(), pos.GetAlt()};
               double ned[3];
               double alt;
               aRefBefore.ConvertLLAToNED(pos.GetLat(), pos.GetLon(), pos.GetAlt(), ned);
               aRefAfter.ConvertNEDToLLA(ned, entityLLA[0], entityLLA[1], alt);

               // Set the entity's new position.
               pos.SetLat(entityLLA[0]);
               pos.SetLatFormat(vespa::VaEnvironment::Instance().GetUnitFormat().mLatLonFormat);
               pos.SetLon(entityLLA[1]);
               pos.SetLonFormat(vespa::VaEnvironment::Instance().GetUnitFormat().mLatLonFormat);
               pos.SetAlt(entityLLA[2]);
               pos.SetAltFormat(vespa::VaEnvironment::Instance().GetUnitFormat().mAltFormat);
               zp->SetPosition(pos);
               zp->UpdatePosition();
            }
            else
            {
               pos.SetX(pos.GetX() + dxy[0]);
               pos.SetY(pos.GetY() + dxy[1]);
               zp->SetPosition(pos);
               zp->UpdatePosition();
            }
         }
      }
      Build();
   }
}

void AttachmentZonePolygon::UpdateVisibility()
{
   VaAttachment::UpdateVisibility();
   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      (*iter)->SetVisible(false);
   }
}

bool AttachmentZonePolygon::PrivateInitialize()
{
   // TODO - Remove this once we add the zone panel.
   if (!IsInitialized())
   {
      // SetEnableFill(true);

      // SetLineWidth(4.0);
      // SetClosed(false);

      // UtColor lineColor(1.f, 0.f, 0.f);
      // SetLineColor(lineColor);

      // UtColor fillColor(0.f, 1.f, 0.f, 0.5);
      // SetFillColor(fillColor);
      // PrivateLoad();
      // LoadZonePoints();
   }
   return true;
}

void AttachmentZonePolygon::PrivateInitializeCallbacks()
{
   if (!IsInitialized())
   {
      mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&AttachmentZonePolygon::MapProjectionChangedCB, this));
      // subscribe to parent moved
      mCallbacks.Add(
         vespa::VaObserver::EntityMoved.Connect(&AttachmentZonePolygon::EntityMovedCB, this, GetParent().GetUniqueId()));
      mCallbacks.Add(vespa::VaObserver::TimeUpdated.Connect(&AttachmentZonePolygon::TimeUpdatedCB, this));
   }
}

void AttachmentZonePolygon::PrivateLoad()
{
   if (mPolylineShapeName.empty())
   {
      mPolylineShapeName = "AttachmentPolygon_Edges" + std::to_string(GetUniqueId());
   }

   if (mPolygonShapeName.empty())
   {
      mPolygonShapeName = "AttachmentPolygon_Faces" + std::to_string(GetUniqueId());
   }

   // Remove the previous models.
   RemoveShapeFromParent(mPolylineShapeName);
   mPolylinePtr = nullptr;
   RemoveShapeFromParent(mPolygonShapeName);
   mPolygonPtr = nullptr;

   // Load the polyline.
   UtoPolyLineShape           polyline;
   UtoTessellatedPolygonShape polygon;

   mPolylinePtr = dynamic_cast<UtoPolyLineShape*>(AddShapeToParent(mPolylineShapeName, polyline));
   mPolygonPtr  = dynamic_cast<UtoTessellatedPolygonShape*>(AddShapeToParent(mPolygonShapeName, polygon));

   // Make sure the shape was added.
   if ((mPolylinePtr != nullptr) && (mPolygonPtr != nullptr))
   {
      if (mRelative)
      {
         mPolylinePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
         mPolygonPtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY_YAW_ONLY);
      }
      else
      {
         mPolylinePtr->SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
         mPolygonPtr->SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
      }
      mPolylinePtr->SetAggregation(UtoPolyLineShape::LINES);
      mPolylinePtr->GetTree()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF); // no lighting
      LoadState();
   }
}

void AttachmentZonePolygon::LoadZonePoints()
{
   // If the zone points haven't been loaded, load them.
   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      (*iter)->Load();
   }
}

void AttachmentZonePolygon::LoadState()
{
   SetColor(mColor);
   SetFillColor(mFillColor);
   SetLineWidth(mLineWidth);
   SetLineStyle(mLineStyle);
   SetEnableFillP(mEnableFill, false);
   SetClosed(mClosed, false);
   SetHeight(mHeight, false);
   SetOrientation(mHeading, mPitch, mRoll);
   SetBaseHeight(mBaseHeight, false);
   SetDepthTesting(mDepthTest);
   EnableAntiAliasLines(mAntiAliasLines);
   Rebuild();

   mColorChanged     = false;
   mLineStyleChanged = false;
   mLineWidthChanged = false;
   mFillChanged      = false;
   mClosedChanged    = false;
}

void AttachmentZonePolygon::Build()
{
   if (IsInitialized())
   {
      if ((!mZonePoints.empty()) && (!mRelative))
      {
         vespa::VaPosition p1 = (*mZonePoints.begin())->GetPosition();
         vespa::VaPosition p2 = GetParent().GetPosition();
         if (p1 != p2)
         {
            GetParent().SetPosition(p1);
            return; // setting the parent position should eventually call this function again
         }
      }
      mPolylinePtr->Clear();
      mPolygonPtr->Clear();

      ZonePoints::iterator iter  = mZonePoints.begin();
      ZonePoints::iterator nIter = iter;
      if (nIter != mZonePoints.end())
      {
         ++nIter;
      }

      if (nIter != mZonePoints.end())
      {
         // Update the zone's height (offset normal and distance).

         // Get the zone's centroid and turn into into a normal.
         vespa::VaPosition centroid     = GetCentroid();
         double            normalXYZ[3] = {0, 0, 1};

         if (!mRelative)
         {
            centroid.GetSceneUpVector(normalXYZ, 0);
         }
         UtoPosition normal(normalXYZ[0], normalXYZ[1], normalXYZ[2]);
         normal *= mHeight;
         mPolygonPtr->SetOffset(normal);
         mPolylinePtr->SetSides(normal);

         // Set whether the polygon should be closed. This determines if it
         // the top and bottom faces.
         mPolygonPtr->SetClosed(mClosed);

         // Update the zone's polyline and polygon shapes.

         unsigned int index = 0;
         double       xyz[3];
         ComputeModelXYZ((*iter)->GetPosition(), xyz);

         double offset[3] = {0.0, 0.0, 0.0};
         if (!mRelative)
         {
            GetParent().GetPosition().GetSceneXYZ(offset, 0);
         }

         bool        firstPoint = true;
         UtoPosition last;
         UtoPosition first;
         for (; nIter != mZonePoints.end(); ++nIter, ++iter)
         {
            UtoPosition pos1(xyz[0] - offset[0], xyz[1] - offset[1], xyz[2] - offset[2]);
            if (mRelative)
            {
               pos1.x() = -pos1.x();
            }
            mPolylinePtr->Insert(index, pos1);
            if (GetEnableFill())
            {
               mPolygonPtr->Insert(index, pos1);
            }
            ++index;

            ComputeModelXYZ((*nIter)->GetPosition(), xyz);

            UtoPosition pos2(xyz[0] - offset[0], xyz[1] - offset[1], xyz[2] - offset[2]);
            if (mRelative)
            {
               pos2.x() = -pos2.x();
            }
            mPolylinePtr->Insert(index, pos2);
            if (GetEnableFill())
            {
               mPolygonPtr->Insert(index, pos2);
            }
            ++index;

            last = pos2;
            if (firstPoint)
            {
               first      = pos1;
               firstPoint = false;
            }
         }

         if (mClosed && (mZonePoints.size() > 2))
         {
            mPolylinePtr->Insert(index, last);
            if (GetEnableFill())
            {
               mPolygonPtr->Insert(index, last);
            }
            ++index;

            // Since the zone points are relative to the first point, the first point is (0, 0, 0).
            mPolylinePtr->Insert(index, first);
            if (GetEnableFill())
            {
               mPolygonPtr->Insert(index, first);
            }
            ++index;
         }
         vespa::VaPosition p = GetParent().GetPosition();

         // This will re-tesselate the zone.
         if (GetEnableFill())
         {
            mPolygonPtr->Rebuild();
         }
      }
   }
}

void AttachmentZonePolygon::ComputeModelXYZ(const vespa::VaPosition& aPosition, double aModelXYZ[3]) const
{
   if (aPosition.PositionIsXY())
   {
      aModelXYZ[0] = aPosition.GetX();
      aModelXYZ[1] = aPosition.GetY();
      aModelXYZ[2] = aPosition.GetAlt();
   }
   else
   {
      aPosition.GetSceneXYZ(aModelXYZ, 0);
   }
}

void AttachmentZonePolygon::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   if (aEntityPtr != nullptr)
   {
      if (aEntityPtr->GetUniqueId() == GetParent().GetUniqueId())
      {
         // Notify that each of the zone points that their parent entity has moved.
         for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
         {
            (*iter)->UpdatePosition();
         }
         Build();
      }
   }
}

void AttachmentZonePolygon::SetHeight(const UtLength& aHeight)
{
   SetHeight(aHeight, true);
}

void AttachmentZonePolygon::SetBaseHeight(const UtLength& aBaseHeight)
{
   SetBaseHeight(aBaseHeight, true);
}

void AttachmentZonePolygon::SetHeight(const UtLength& aHeight, bool aBuild)
{
   mHeight = aHeight;
   if (aBuild)
   {
      Build();
   }
}

void AttachmentZonePolygon::SetBaseHeight(const UtLength& aBaseHeight, bool aBuild)
{
   if (mBaseHeight != aBaseHeight)
   {
      mBaseHeight = aBaseHeight;
      ZonePoints::iterator it;
      for (it = mZonePoints.begin(); it != mZonePoints.end(); ++it)
      {
         vespa::VaPosition pos = (*it)->GetPosition();
         pos.SetAlt(aBaseHeight);
         (*it)->SetPosition(pos, aBuild);
      }
   }
}

void AttachmentZonePolygon::SetColor(const UtColor& aColor)
{
   mColor        = aColor;
   mColorChanged = true;
   if (mPolylinePtr != nullptr)
   {
      UtoColor color{static_cast<unsigned char>(mColor[0] * 255),
                     static_cast<unsigned char>(mColor[1] * 255),
                     static_cast<unsigned char>(mColor[2] * 255),
                     static_cast<unsigned char>(mColor[3] * 255)};
      mPolylinePtr->SetColor(color);
   }
   ZonePoints::iterator it;
   for (it = mZonePoints.begin(); it != mZonePoints.end(); ++it)
   {
      (*it)->SetColor(mColor);
   }
}

void AttachmentZonePolygon::SetFillColor(const UtColor& aFillColor)
{
   mFillColor = aFillColor;

   if (mPolygonPtr != nullptr)
   {
      float    alpha = mEnableFill ? 1.0 - mBlendFactor : 0.0;
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));
      mPolygonPtr->EnableLighting(false);
      mPolygonPtr->SetColor(color);
   }
}

void AttachmentZonePolygon::SetLineWidth(double aLineWidth)
{
   mLineWidth        = aLineWidth;
   mLineWidthChanged = true;
   if (mPolylinePtr != nullptr)
   {
      mPolylinePtr->SetWidth(aLineWidth);
   }
}

void AttachmentZonePolygon::SetLineStyle(int aLineStyle)
{
   mLineStyle        = aLineStyle;
   mLineStyleChanged = true;
   switch (mLineStyle)
   {
   case 0:
      mLineStipplePattern = 0xFFFF;
      break;
   case 1:
      mLineStipplePattern = 0xC0C0;
      break;
   case 2:
      mLineStipplePattern = 0xFF00;
      break;
   case 3:
      mLineStipplePattern = 0xF0F0;
      break;
   case 4:
      mLineStipplePattern = 0xFE38;
      break;
   default:
      mLineStipplePattern = 0xFFFF;
      break;
   }

   if (mPolylinePtr != nullptr)
   {
      mPolylinePtr->SetPattern(mLineStipplePattern, mLineStippleFactor);
   }
}

void AttachmentZonePolygon::SetEnableFill(bool aEnableFill)
{
   SetEnableFillP(aEnableFill, true);
}

void AttachmentZonePolygon::SetEnableFillP(bool aEnableFill, bool aBuild)
{
   mEnableFill  = aEnableFill;
   mFillChanged = true;
   SetFillColor(mFillColor);
   if (aBuild)
   {
      Rebuild();
   }
}

void AttachmentZonePolygon::SetOrientation(double aHeading, double aPitch, double aRoll)
{
   mHeading = aHeading;
   mPitch   = aPitch;
   mRoll    = aRoll;

   if (mPolygonPtr)
   {
      mPolygonPtr->SetOrientation(UtoOrientation(mHeading, mPitch, mRoll));
   }
   if (mPolylinePtr)
   {
      mPolylinePtr->SetOrientation(UtoOrientation(mHeading, mPitch, mRoll));
   }
}

void AttachmentZonePolygon::SetClosed(bool aClosed, bool aBuild)
{
   mClosed        = aClosed;
   mClosedChanged = true;
   if (aBuild)
   {
      Rebuild();
   }
}

void AttachmentZonePolygon::SetHideZonePoints(bool aHideZonePoints)
{
   mHideZonePoints = aHideZonePoints || mNeverShowZonePoints;
   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      (*iter)->SetVisible(!mHideZonePoints);
   }
}

void AttachmentZonePolygon::SetBlendFactor(float aFactor)
{
   mBlendFactor = aFactor;
   if (mPolylinePtr != nullptr)
   {
      float    alpha = mEnableFill ? 1.0 - mBlendFactor : 0.0;
      UtoColor color((unsigned char)(mFillColor[0] * 255),
                     (unsigned char)(mFillColor[1] * 255),
                     (unsigned char)(mFillColor[2] * 255),
                     (unsigned char)(alpha * 255));

      mPolygonPtr->EnableLighting(false);
      mPolygonPtr->SetColor(color);
   }
}

vespa::VaPosition AttachmentZonePolygon::GetCentroid() const
{
   vespa::VaPosition centroid;

   double xyz[3] = {0.0, 0.0, 0.0};
   for (ZonePoints::const_iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      const vespa::VaPosition& pos((*iter)->GetPosition());
      xyz[0] += pos.GetECEF_X();
      xyz[1] += pos.GetECEF_Y();
      xyz[2] += pos.GetECEF_Z();
   }

   if (!mZonePoints.empty())
   {
      double d = (double)mZonePoints.size();
      xyz[0] /= d;
      xyz[1] /= d;
      xyz[2] /= d;
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      lla[2]   = 0;
      centroid = vespa::VaPosition(lla[0], lla[1], lla[2]);
   }
   return centroid;
}

void AttachmentZonePolygon::AddZonePoint(ZonePoint* aZonePointPtr, vespa::VaViewer* aViewerPtr, bool aAddToSelected, bool aBuild)
{
   // If we have a currently selected zone point, insert the new point after it.
   // Otherwise, append it to the end.
   ZonePoints::iterator selectedZonePointIter = mZonePoints.end();
   if (aAddToSelected)
   {
      for (ZonePoints::reverse_iterator iter = mZonePoints.rbegin(); iter != mZonePoints.rend(); ++iter)
      {
         if ((*iter)->GetSelected(aViewerPtr))
         {
            selectedZonePointIter = iter.base();
            break;
         }
      }
   }
   // Insert the new zone point.
   if (mRelative)
   {
      vespa::VaPosition pos = aZonePointPtr->GetPosition();
      pos                   = GetParent().ConvertToXY(pos);
      pos.SetAlt(aZonePointPtr->GetPosition().GetAlt());
      aZonePointPtr->SetPosition(pos, aBuild);
      // convert pos to a GetParent relative xy
   }
   else
   {
      vespa::VaPosition pos = aZonePointPtr->GetPosition();
      // pos.SetAlt(GetParent().GetPosition().GetAlt());
      aZonePointPtr->SetPosition(pos, aBuild);
   }
   aZonePointPtr->SetColor(mColor);
   mZonePoints.insert(selectedZonePointIter, aZonePointPtr);
   aZonePointPtr->Load();
   if (aBuild)
   {
      Build();
   }
}

bool AttachmentZonePolygon::RemoveZonePoint(int aZonePointUniqueId)
{
   bool removed = false;

   ZonePoints::iterator iter = find_if(mZonePoints.begin(), mZonePoints.end(), ZonePoint::FindPred(aZonePointUniqueId));
   if (iter != mZonePoints.end())
   {
      GetParent().RemoveAttachment(GetUniqueId(), aZonePointUniqueId);
      delete *iter;
      mZonePoints.erase(iter);
      Build();
      removed = true;
   }
   return removed;
}

ZonePoint* AttachmentZonePolygon::FindPoint(int aUniqueId) const
{
   ZonePoints::const_iterator iter = find_if(mZonePoints.begin(), mZonePoints.end(), ZonePoint::FindPred(aUniqueId));
   if (iter != mZonePoints.end())
   {
      return *iter;
   }
   return nullptr;
}

ZonePoint* AttachmentZonePolygon::GetLastPoint()
{
   ZonePoints::iterator iter;
   for (iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      ZonePoints::iterator next = iter;
      ++next;
      if (next == mZonePoints.end())
      {
         return *iter;
      }
   }
   return nullptr;
}

void AttachmentZonePolygon::SetModifier(std::string aKey, std::string aValue)
{
   // bool added;
   // StateTagList::Tag modifiersTag("@@modifiers", "", mTags.GetLeadingWhitespace(), true);
   // mTags.FindOrAddTag(modifiersTag, false, added);
   mModifierList[aKey] = aValue;
   mModifierKeys.push_back(aKey);
}
void AttachmentZonePolygon::RemoveModifier(std::string aKey)
{
   std::map<std::string, std::string>::iterator it;
   it = mModifierList.find(aKey);
   if (it != mModifierList.end())
   {
      mModifierList.erase(it);
   }

   std::vector<std::string>::iterator iter;
   iter = mModifierKeys.begin();
   while (iter != mModifierKeys.end())
   {
      if (*iter == aKey)
      {
         mModifierKeys.erase(iter);
         break;
      }
      ++iter;
   }
}

void AttachmentZonePolygon::Translated(double aDeltaLat, double aDeltaLon)
{
   ZonePoints::iterator iter;
   for (iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      vespa::VaPosition pos((*iter)->GetPosition());
      pos.Translate(aDeltaLat, aDeltaLon);
      (*iter)->SetPosition(pos);
   }
}

void AttachmentZonePolygon::MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection)
{
   for (ZonePoints::iterator iter = mZonePoints.begin(); iter != mZonePoints.end(); ++iter)
   {
      (*iter)->UpdatePosition();
   }
   Build();
}

void AttachmentZonePolygon::GetShapesForSubpart(const vespa::VaAttachmentSubId& aSubpartId, ShapeList& aShapes) const
{
   if (aSubpartId == 0)
   {
      GetShapes(aShapes);
   }
   else
   {
      FindPoint(aSubpartId.mSubId)->GetShapes(aShapes);
   }
}

double AttachmentZonePolygon::GetArea() const
{
   if (mClosed)
   {
      if (mRelative)
      {
         double tArea = 0;
         double x1;
         double x2;
         double y1;
         double y2;

         ZonePoints::const_iterator it    = mZonePoints.begin();
         ZonePoint*                 zp    = *it;
         vespa::VaPosition          a     = zp->GetPosition();
         vespa::VaPosition          b     = zp->GetPosition();
         bool                       first = true;
         for (it = mZonePoints.begin(); it != mZonePoints.end(); ++it)
         {
            ZonePoint* zp = *it;
            a             = zp->GetPosition();
            if (!first)
            {
               x1 = a.GetX();
               y1 = a.GetY();
               x2 = b.GetX();
               y2 = b.GetY();
               tArea += ((x1 * y2) - (x2 * y1));
            }
            else
            {
               first = false;
            }
            b = a;
         }
         it = mZonePoints.begin();
         zp = *it;
         a  = zp->GetPosition();

         x1 = a.GetX();
         x2 = b.GetX();
         y1 = a.GetY();
         y2 = b.GetY();
         tArea += ((x1 * y2) - (x2 * y1));

         // Converting for m2 to km2.
         return fabs(0.5 * tArea);
      }
      else
      {
         vespa::VaPosition refpos = GetCentroid();
         double            tecef33[3][3];
         double            recef3[3];
         UtEllipsoidalEarth::ComputeNEDTransform(refpos.GetLat(), refpos.GetLon(), 0, tecef33, recef3);
         double                     tArea = 0;
         double                     xyz1[3];
         double                     xyz2[3];
         bool                       first = true;
         ZonePoint*                 zp    = *(mZonePoints.begin());
         vespa::VaPosition          a     = zp->GetPosition();
         vespa::VaPosition          b     = zp->GetPosition();
         ZonePoints::const_iterator it;
         for (it = mZonePoints.begin(); it != mZonePoints.end(); ++it)
         {
            zp = *it;
            a  = zp->GetPosition();
            if (!first)
            {
               UtEllipsoidalEarth::ConvertLLAToLocal(recef3, tecef33, a.GetLat(), a.GetLon(), a.GetAlt(), xyz1);
               UtEllipsoidalEarth::ConvertLLAToLocal(recef3, tecef33, b.GetLat(), b.GetLon(), b.GetAlt(), xyz2);
               tArea += ((xyz1[0] * xyz2[1]) - (xyz2[0] * xyz1[1]));
            }
            else
            {
               first = false;
            }
            b = a;
         }
         zp = *(mZonePoints.begin());
         a  = zp->GetPosition();

         UtEllipsoidalEarth::ConvertLLAToLocal(recef3, tecef33, a.GetLat(), a.GetLon(), a.GetAlt(), xyz1);
         UtEllipsoidalEarth::ConvertLLAToLocal(recef3, tecef33, b.GetLat(), b.GetLon(), b.GetAlt(), xyz2);
         tArea += ((xyz1[0] * xyz2[1]) - (xyz2[0] * xyz1[1]));

         // Converting for m2 to km2.
         return fabs(0.5 * tArea);
      }
   }
   else
   {
      return 0;
   }
}

double AttachmentZonePolygon::GetLength() const
{
   if (mZonePoints.empty())
   {
      return 0;
   }
   ZonePoints::const_iterator it     = mZonePoints.begin();
   ZonePoint*                 zp     = *it;
   vespa::VaPosition          a      = zp->GetPosition();
   vespa::VaPosition          b      = zp->GetPosition();
   bool                       first  = true;
   double                     length = 0;
   for (it = mZonePoints.begin(); it != mZonePoints.end(); ++it)
   {
      zp = *it;
      a  = zp->GetPosition();
      if (!first)
      {
         if (mRelative)
         {
            length += sqrt((a.GetX() - b.GetX()) * (a.GetX() - b.GetX()) + (a.GetY() - b.GetY()) * (a.GetY() - b.GetY()));
         }
         else
         {
            length += sqrt((a.GetECEF_X() - b.GetECEF_X()) * (a.GetECEF_X() - b.GetECEF_X()) +
                           (a.GetECEF_Y() - b.GetECEF_Y()) * (a.GetECEF_Y() - b.GetECEF_Y()));
         }
      }
      else
      {
         first = false;
      }
      b = a;
   }
   if (mClosed)
   {
      it = mZonePoints.begin();
      zp = *it;
      a  = zp->GetPosition();

      if (mRelative)
      {
         length += sqrt((a.GetX() - b.GetX()) * (a.GetX() - b.GetX()) + (a.GetY() - b.GetY()) * (a.GetY() - b.GetY()));
      }
      else
      {
         length += sqrt((a.GetECEF_X() - b.GetECEF_X()) * (a.GetECEF_X() - b.GetECEF_X()) +
                        (a.GetECEF_Y() - b.GetECEF_Y()) * (a.GetECEF_Y() - b.GetECEF_Y()));
      }
   }
   return length;
}

void AttachmentZonePolygon::TimeUpdatedCB(double aTime, double aRate, double aMinTime, double aMaxTime)
{
   // if (mSlewToPlayer)
   //{
   //    Rebuild();
   // }

   if (mSlewToPlayer)
   {
      if (mSlewEntityPtr == nullptr)
      {
         mSlewEntityPtr = vespa::VaEnvironment::Instance().FindEntity(mSlewEntityName);
         if (mSlewEntityPtr == nullptr)
         {
            mSlewToPlayer = false;
         }
      }
      if (mSlewToPlayer)
      {
         double            bearingRdrTgt;
         double            bearingTgtRdr;
         double            heading = GetParent().GetHeading();
         double            rotate;
         UtEntity          slewPlayer;
         UtEntity          radar;
         vespa::VaPosition playerPos = GetParent().GetPosition();
         vespa::VaPosition slewPos   = mSlewEntityPtr->GetPosition();

         slewPlayer.SetLocationLLA(slewPos.GetLat(), slewPos.GetLon(), slewPos.GetAlt());
         slewPlayer.SetOrientationNED(mSlewEntityPtr->GetHeading() * UtMath::cRAD_PER_DEG, 0.0, 0.0);

         radar.SetLocationLLA(playerPos.GetLat(), playerPos.GetLon(), playerPos.GetAlt());
         radar.SetOrientationNED(heading * UtMath::cRAD_PER_DEG, 0.0, 0.0);

         double relLoc[3];
         double relEl;

         radar.GetRelativeLocationWCS(&slewPlayer, relLoc);
         radar.ComputeAspect(relLoc, bearingRdrTgt, relEl);
         bearingRdrTgt = bearingRdrTgt * UtMath::cDEG_PER_RAD + heading;

         slewPlayer.GetRelativeLocationWCS(&radar, relLoc);
         slewPlayer.ComputeAspect(relLoc, bearingTgtRdr, relEl);
         bearingTgtRdr = bearingTgtRdr * UtMath::cDEG_PER_RAD;

         heading = bearingRdrTgt - bearingTgtRdr;
         // rotate = 360.0 - heading;
         rotate = heading;
         GetParent().SetHeading(rotate);
      }
      else if (!mSlewEntityName.empty())
      {
         mSlewToPlayer = true;
      }
   }
}

void AttachmentZonePolygon::SetDepthTesting(bool aDepthTest)
{
   mDepthTest = aDepthTest;

   if (mPolygonPtr != nullptr)
   {
      mPolygonPtr->SetDepthTesting(mDepthTest);
   }
   if (mPolylinePtr != nullptr)
   {
      mPolylinePtr->SetDepthTesting(mDepthTest);
   }
}

void AttachmentZonePolygon::EnableAntiAliasLines(bool aEnable)
{
   mAntiAliasLines = aEnable;

   if (mPolylinePtr != nullptr)
   {
      mPolylinePtr->EnableAntiAliasLines(mAntiAliasLines);
   }
}

} // namespace wkf
