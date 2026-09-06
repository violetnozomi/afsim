// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#include "VaAttachmentAngles.hpp"

#include <QApplication>
#include <QDesktopWidget>

#include "UtLLAPos.hpp"
#include "VaEntity.hpp"

namespace vespa
{
VaAttachmentAngles::VaAttachmentAngles(VaEntity& aParentEntity, VaViewer* aViewerPtr)
   : VaAttachment(aParentEntity, aViewerPtr, "angles")
{
   SetType<VaAttachmentAngles>();
}

void VaAttachmentAngles::AddEntry(vespa::AngleEntry& aEntry, const std::string& aUniqueId)
{
   aEntry.SetSectorFromParent(AddShapeToParent("Angle_" + aUniqueId, aEntry.GeSectorForTree()));
   mAngleEntries.emplace(aUniqueId, std::move(aEntry));
}

void VaAttachmentAngles::AddEntry(const UtVec3f&     aFirstAngle,
                                  const UtVec3f&     aSecondAngle,
                                  const QColor&      aColor,
                                  const std::string& aUniqueId,
                                  const std::string& aName,
                                  float              aRadius)
{
   vespa::AngleEntry angleEntry{aFirstAngle, aSecondAngle, aColor, aName, aRadius};
   AddEntry(angleEntry, aUniqueId);
}

void VaAttachmentAngles::Update(double aTime)
{
   for (auto& angle : mAngleEntries)
   {
      angle.second.Update();
   }
}

void VaAttachmentAngles::UpdatePositionContainingVector(int aVectorId, UtVec3f aNewPosition)
{
   for (auto& angle : mAngleEntries)
   {
      auto position = angle.first.find(std::to_string(aVectorId));
      if (position != std::string::npos)
      {
         if (position == 0) // first vector
         {
            angle.second.UpdateStartVector(aNewPosition);
         }
         else // second
         {
            angle.second.UpdateEndVector(aNewPosition);
         }
      }
   }
}

const std::map<const std::string, AngleEntry>& VaAttachmentAngles::GetEntries() const
{
   return mAngleEntries;
}

void VaAttachmentAngles::ToggleEntryVisibility(const std::string& aEntryId)
{
   bool isVisible = mAngleEntries[aEntryId].IsAngleVisible();
   mAngleEntries[aEntryId].SetVisibility(!isVisible);
}

void VaAttachmentAngles::ToggleEntryFill(const std::string& aEntryId)
{
   bool isFilling = mAngleEntries[aEntryId].IsModeFill();
   mAngleEntries[aEntryId].ShouldFill(!isFilling);
}

const AngleEntry* VaAttachmentAngles::GetEntry(const std::string& aUniqueId) const
{
   if (mAngleEntries.find(aUniqueId) != mAngleEntries.end())
   {
      return &mAngleEntries.at(aUniqueId);
   }
   return nullptr;
}
} // namespace vespa
