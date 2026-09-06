// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ORBITPREVIEWMANAGER_HPP
#define ORBITPREVIEWMANAGER_HPP

#include "wsf_spaceg_export.h"

#include <map>

#include <QModelIndex>

namespace wsfg
{

namespace spaceg
{

class OrbitPreviewEntity;
class OrbitalMissionModel;

class WSF_SPACEG_EXPORT OrbitPreviewManager
{
public:
   OrbitPreviewManager()
      : mPreviews{}
   {
   }
   ~OrbitPreviewManager() = default;

   void ClearOrbitPreviews();
   void UpdateOrbitPreviews(const OrbitalMissionModel& aModel, const QModelIndex& aIndex);
   void RecreateOrbitPreviews(const OrbitalMissionModel& aModel);

private:
   void HideOrDeletePreview(const OrbitalMissionModel& aModel);
   void UpdateSingleOrbitPreview(const OrbitalMissionModel& aModel, const QModelIndex& aIndex);
   void UpdateAllDescendants(const OrbitalMissionModel& aModel, const QModelIndex& aParentIndex);

   std::map<QModelIndex, OrbitPreviewEntity*> mPreviews;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITPREVIEWMANAGER_HPP
