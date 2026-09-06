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

#include "VaZonePoint.hpp"

#include <sstream>

#include "UtoModelShape.hpp"
#include "UtoUtility.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentZonePolygon.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
VaZonePoint::VaZonePoint(VaAttachment& aParent, const VaPosition& aPosition)
   : VaObject()
   , mParent(aParent)
   , mPosition(aPosition)
   , mSourcePosition()
   , mPointPtr(nullptr)
   , mPointShapeName()
   , mSelected()
   , mColor(0, 0, 0, 1)
{
   SetName("zone_point_" + GetUniqueIdString());
   SetType<VaZonePoint>();
}

VaZonePoint::~VaZonePoint()
{
   mParent.RemoveShapeFromParent(mPointShapeName);
}

void VaZonePoint::Load()
{
   const VaEntity& entity = mParent.GetParent();
   if (entity.IsInitialized())
   {
      if (mPointShapeName.empty())
      {
         std::ostringstream  oss;
         static unsigned int uid = 0;
         oss << "ZonePoint_" << uid++;
         mPointShapeName = oss.str().c_str();
      }

      // Remove the previous model.
      mParent.RemoveShapeFromParent(mPointShapeName);
      mPointPtr = nullptr;

      UtoModelShape model = dynamic_cast<UtoModelShape&>(UtoResDBLookup<UtoShape>("WaypointWhite"));
      model.SetShapeFlags(UtoShape::BILLBOARD | UtoShape::AUTO_SCALE, UtoShape::BILLBOARD | UtoShape::AUTO_SCALE);
      model.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
      mPointPtr = dynamic_cast<UtoModelShape*>(mParent.AddShapeToParent(mPointShapeName, model, GetUniqueIdString()));
      if (mPointPtr != nullptr)
      {
         UtColor color(mColor[0], mColor[1], mColor[2], 1.f);
         mPointPtr->SetColor(VaUtils::glToUtoColor(color));
         mPointPtr->SetLighting(false);
         UpdatePosition();
         LoadState();
      }
   }
}

void VaZonePoint::SetPosition(const VaPosition& aPosition, bool aBuild)
{
   mPosition = aPosition;
   UpdatePosition();
   if (aBuild)
   {
      VaAttachmentZonePolygon* poly = dynamic_cast<VaAttachmentZonePolygon*>(&GetParent());
      if (nullptr != poly)
      {
         poly->Rebuild();
      }
   }
   VaEnvironment::Instance().GetCurrentScenario()->SetModified(true);
}

void VaZonePoint::UpdatePosition()
{
   if (mPointPtr != nullptr)
   {
      double xyz[3];
      ComputeXYZ(mPosition, xyz);

      mPointPtr->SetPosition(UtoPosition(xyz[0], xyz[1], xyz[2]));
   }
}

void VaZonePoint::ComputeXYZ(const VaPosition& aPosition, double aXYZ[3])
{
   if (aPosition.PositionIsXY())
   {
      double modelXYZ[3] = {-aPosition.GetX(), aPosition.GetY(), aPosition.GetAlt()};
      mParent.GetParent().ModelToWorld(modelXYZ, aXYZ, true);
   }
   else
   {
      double pxyz[3];
      aPosition.GetSceneXYZ(aXYZ, 0);
      GetParent().GetParent().GetPosition().GetSceneXYZ(pxyz, 0);
      aXYZ[0] -= pxyz[0];
      aXYZ[1] -= pxyz[1];
      aXYZ[2] -= pxyz[2];
   }
}

void VaZonePoint::LoadState()
{
   // By default the state is unselected, so only process the selection if it is true.
   VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
   for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
   {
      bool selected = GetSelected(*it);
      if (selected)
      {
         mSelected[(*it)->GetUniqueId()] = !selected;
         SetSelected(*it, selected);
      }
   }
}

void VaZonePoint::SetVisible(bool aVisible)
{
   if (!mPointShapeName.empty())
   {
      mParent.ShowShape(mPointShapeName, aVisible);
   }
}

bool VaZonePoint::GetSelected(VaViewer* aViewerPtr)
{
   std::map<int, bool>::const_iterator it = mSelected.find(aViewerPtr->GetUniqueId());
   if (it != mSelected.end())
   {
      return it->second;
   }
   return false;
}

void VaZonePoint::SetSelected(VaViewer* aViewerPtr, bool aSelected)
{
   if (!mParent.IsMyViewer(aViewerPtr))
   {
      return;
   }
   if ((mSelected.find(aViewerPtr->GetUniqueId()) == mSelected.end()) || (GetSelected(aViewerPtr) != aSelected))
   {
      mSelected[aViewerPtr->GetUniqueId()] = aSelected;

      // set visually selected
      if (mPointPtr != nullptr)
      {
         float   transparency = aSelected ? 0.5f : 1.0f;
         UtColor color(mColor[0], mColor[1], mColor[2], transparency);
         mPointPtr->SetColor(UtoColor(VaUtils::glToUtoColor(color)), UtoColor::ALPHA);
      }
      mParent.AttachmentSelect(aViewerPtr, (aSelected ? 1 : 0), GetUniqueId());
   }
}

void VaZonePoint::SetColor(const UtColor& aColor)
{
   mColor = aColor;
   if (mPointPtr != nullptr)
   {
      float    alpha = 1.f;
      UtoColor color((unsigned char)(aColor[0] * 255),
                     (unsigned char)(aColor[1] * 255),
                     (unsigned char)(aColor[2] * 255),
                     (unsigned char)(alpha * 255));
      mPointPtr->SetColor(color);
   }
}

void VaZonePoint::GetShapes(VaAttachment::ShapeList& aShapes) const
{
   aShapes.push_back(mPointShapeName);
}

} // namespace vespa
