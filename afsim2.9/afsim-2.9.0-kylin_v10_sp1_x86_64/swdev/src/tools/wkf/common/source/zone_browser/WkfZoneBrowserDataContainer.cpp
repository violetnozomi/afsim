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

#include "WkfZoneBrowserDataContainer.hpp"

#include <UtMemory.hpp>

extern const std::string wkf::cGLOBAL_NAME = "Global Zones";

wkf::ZoneSetData::ZoneSetData(const ZoneSetData& aSource)
{
   *this = aSource;
}

wkf::ZoneSetData& wkf::ZoneSetData::operator=(const ZoneSetData& aSource)
{
   if (this != &aSource)
   {
      mSetName      = aSource.mSetName;
      mPlatformName = aSource.mPlatformName;
      mZones.clear();
      for (const auto& zonePtr : aSource.mZones)
      {
         mZones.emplace_back(zonePtr->Clone());
      }
      mFillColor = aSource.mFillColor;
      mLineColor = aSource.mLineColor;
   }
   return *this;
}

void wkf::ZoneBrowserDataContainer::ClearData()
{
   mGlobalZones.clear();
   mZoneMap.clear();
   emit ClearZoneData();
}

void wkf::ZoneSetData::AddCircularData(CommonZoneVariables& aCommonData, const CircularZoneVariables& aCircularData)
{
   aCommonData.mZoneName = GetUniqueName();
   mZones.emplace_back(ut::make_unique<CircularZoneData>(aCommonData, aCircularData));
}

void wkf::ZoneSetData::AddEllipticalData(CommonZoneVariables& aCommonData, const EllipticalZoneVariables& aEllipticalData)
{
   aCommonData.mZoneName = GetUniqueName();
   mZones.emplace_back(ut::make_unique<EllipticalZoneData>(aCommonData, aEllipticalData));
}

void wkf::ZoneSetData::AddSphericalData(CommonZoneVariables& aCommonData, const SphericalZoneVariables& aSphericalData)
{
   aCommonData.mZoneName = GetUniqueName();
   mZones.emplace_back(ut::make_unique<SphericalZoneData>(aCommonData, aSphericalData));
}

void wkf::ZoneSetData::AddPolygonalData(CommonZoneVariables& aCommonData, const PolygonalZoneVariables& aPolygonalData)
{
   aCommonData.mZoneName = GetUniqueName();
   mZones.emplace_back(ut::make_unique<PolygonalZoneData>(aCommonData, aPolygonalData));
}

void wkf::ZoneBrowserDataContainer::AddZoneSet(const ZoneSetData& aZoneSet, bool aShow)
{
   auto               platformName = aZoneSet.GetPlatformName();
   auto               zonePtr      = ut::make_unique<ZoneSetData>(aZoneSet);
   const std::string& zoneSetName  = aZoneSet.GetSetName();

   if (platformName == cGLOBAL_NAME)
   {
      auto IsEqual = [zoneSetName](const std::unique_ptr<ZoneSetData>& aDataPtr)
      { return (aDataPtr->GetSetName() == zoneSetName); };

      auto zoneIter = std::find_if(mGlobalZones.begin(), mGlobalZones.end(), IsEqual);
      while (zoneIter != mGlobalZones.end())
      {
         // we found the zone in the list, so remove it so we can add it back in
         // because the zone being added may have had its data updated
         emit ZoneDeleted(zoneIter->get());
         mGlobalZones.erase(zoneIter);

         // since mGlobalZones is a vector, we presume it might contain duplicates (even
         // though it shouldn't, so try again, just to be safe
         zoneIter = std::find_if(mGlobalZones.begin(), mGlobalZones.end(), IsEqual);
      }

      // now we can add the global zone
      mGlobalZones.push_back(std::move(zonePtr));
      emit ZoneAdded(mGlobalZones.back().get(), aShow);
   }
   else
   {
      auto IsEqual = [platformName, zoneSetName](const std::unique_ptr<ZoneSetData>& aDataPtr)
      { return (aDataPtr->GetSetName() == zoneSetName) && (aDataPtr->GetPlatformName() == platformName); };

      auto mapIter = mZoneMap.find(platformName);
      if (mapIter != mZoneMap.end())
      {
         // we found the platform - now see if we can find a zone with the zone name
         // we use a loop to ensure all duplicates are removed - this loop is structured the way it is
         // to ensure that windows will not invalidate the .end() iterator between the iterations of the loop
         bool zoneExists = true; // assume it exists
         while (zoneExists)
         {
            auto zoneIter = std::find_if(mapIter->second.begin(), mapIter->second.end(), IsEqual);
            if (zoneIter != mapIter->second.end())
            {
               // we found a zone with the same name as the zone being added, remove it first as
               // the data for the zone may have changed
               emit ZoneDeleted(zoneIter->get());
               mapIter->second.erase(zoneIter);

               // since each element of mZoneMap is a vector, we presume it might contain
               // duplicates (even though it shouldn't, so try again, just to be safe
               zoneExists = true;
            }
            else
            {
               zoneExists = false;
            }
         }

         // now add it back in
         mapIter->second.push_back(std::move(zonePtr));
         emit ZoneAdded(mapIter->second.back().get(), aShow);
      }
      else
      {
         std::vector<std::unique_ptr<ZoneSetData>> zoneSet;
         zoneSet.emplace_back(std::move(zonePtr));
         mZoneMap.emplace(platformName, std::move(zoneSet));
         emit ZoneAdded(mZoneMap[platformName].back().get(), aShow);
      }
   }
}

// Note: This function is currently unsafe to use in the wkf zone browser dock widget
void wkf::ZoneBrowserDataContainer::UpdateData(const ZoneSetData& aZoneSet)
{
   auto zoneSet = aZoneSet;
   RemoveData(zoneSet.GetSetName(), zoneSet.GetPlatformName());
   if (!zoneSet.GetZones().empty())
   {
      AddZoneSet(zoneSet);
   }
   emit ZoneUpdated(zoneSet.GetSetName(), zoneSet.GetPlatformName());
}

void wkf::ZoneBrowserDataContainer::UpdateColor(const ZoneSetData& aZoneSet)
{
   auto        zoneSet      = aZoneSet;
   std::string platformName = zoneSet.GetPlatformName();
   std::string setName      = zoneSet.GetSetName();

   auto IsEqual = [platformName, setName](const std::unique_ptr<ZoneSetData>& aDataPtr)
   { return (aDataPtr->GetSetName() == setName) && (aDataPtr->GetPlatformName() == platformName); };

   if (platformName == cGLOBAL_NAME)
   {
      auto it = std::find_if(mGlobalZones.begin(), mGlobalZones.end(), IsEqual);
      if (it != mGlobalZones.end())
      {
         it->get()->SetLineColor(aZoneSet.GetLineColor());
         it->get()->SetFillColor(aZoneSet.GetFillColor());
      }
   }
   else
   {
      auto mapIter = mZoneMap.find(platformName);
      if (mapIter != mZoneMap.end())
      {
         auto zoneIter = std::find_if((*mapIter).second.begin(), (*mapIter).second.end(), IsEqual);
         if (zoneIter != (*mapIter).second.end())
         {
            zoneIter->get()->SetLineColor(aZoneSet.GetLineColor());
            zoneIter->get()->SetFillColor(aZoneSet.GetFillColor());
         }
      }
   }
   emit ZoneColorUpdated(setName, platformName);
}

void wkf::ZoneBrowserDataContainer::UpdatePreview(CommonZoneVariables          aCommonData,
                                                  const CircularZoneVariables& aCircularData,
                                                  const std::string&           aSetName,
                                                  const std::string&           aPlatformName)
{
   if (mPreviewZonePtr != nullptr)
   {
      emit PreviewPendingDeletion();
   }
   ZoneSetData zoneSet(aSetName, aPlatformName);
   zoneSet.AddCircularData(aCommonData, aCircularData);
   mPreviewZonePtr = ut::make_unique<ZoneSetData>(zoneSet);
   emit PreviewUpdated();
}

void wkf::ZoneBrowserDataContainer::UpdatePreview(CommonZoneVariables            aCommonData,
                                                  const EllipticalZoneVariables& aEllipticalData,
                                                  const std::string&             aSetName,
                                                  const std::string&             aPlatformName)
{
   if (mPreviewZonePtr != nullptr)
   {
      emit PreviewPendingDeletion();
   }
   ZoneSetData zoneSet(aSetName, aPlatformName);
   zoneSet.AddEllipticalData(aCommonData, aEllipticalData);
   mPreviewZonePtr = ut::make_unique<ZoneSetData>(zoneSet);
   emit PreviewUpdated();
}

void wkf::ZoneBrowserDataContainer::UpdatePreview(CommonZoneVariables           aCommonData,
                                                  const SphericalZoneVariables& aSphericalData,
                                                  const std::string&            aSetName,
                                                  const std::string&            aPlatformName)
{
   if (mPreviewZonePtr != nullptr)
   {
      emit PreviewPendingDeletion();
   }
   ZoneSetData zoneSet(aSetName, aPlatformName);
   zoneSet.AddSphericalData(aCommonData, aSphericalData);
   mPreviewZonePtr = ut::make_unique<ZoneSetData>(zoneSet);
   emit PreviewUpdated();
}

void wkf::ZoneBrowserDataContainer::UpdatePreview(CommonZoneVariables           aCommonData,
                                                  const PolygonalZoneVariables& aPolygonalData,
                                                  const std::string&            aSetName,
                                                  const std::string&            aPlatformName)
{
   if (mPreviewZonePtr != nullptr)
   {
      emit PreviewPendingDeletion();
   }
   ZoneSetData zoneSet(aSetName, aPlatformName);
   zoneSet.AddPolygonalData(aCommonData, aPolygonalData);
   mPreviewZonePtr = ut::make_unique<ZoneSetData>(zoneSet);
   emit PreviewUpdated();
}

void wkf::ZoneBrowserDataContainer::RemovePreview()
{
   emit PreviewPendingDeletion();
   mPreviewZonePtr.reset(nullptr);
}

void wkf::ZoneBrowserDataContainer::RemoveData(const std::string& aZoneName, const std::string& aPlatformName)
{
   auto IsEqual = [aPlatformName, aZoneName](const std::unique_ptr<ZoneSetData>& aDataPtr)
   { return (aDataPtr->GetSetName() == aZoneName) && (aDataPtr->GetPlatformName() == aPlatformName); };

   if (aPlatformName == cGLOBAL_NAME)
   {
      auto it = std::find_if(mGlobalZones.begin(), mGlobalZones.end(), IsEqual);
      if (it != mGlobalZones.end())
      {
         emit ZoneDeleted(it->get());
         mGlobalZones.erase(it);
      }
   }
   else
   {
      auto mapIter = mZoneMap.find(aPlatformName);
      if (mapIter != mZoneMap.end())
      {
         auto zoneIter = std::find_if((*mapIter).second.begin(), (*mapIter).second.end(), IsEqual);
         if (zoneIter != (*mapIter).second.end())
         {
            emit ZoneDeleted((*zoneIter).get());
            mapIter->second.erase(zoneIter);

            if (mapIter->second.empty())
            {
               mZoneMap.erase(mapIter);
            }
         }
      }
   }
}

void wkf::ZoneBrowserDataContainer::RemovePlatform(const std::string& aPlatformName)
{
   mZoneMap.erase(aPlatformName);
   emit PlatformRemoved(aPlatformName);
}

wkf::ZoneSetData* wkf::ZoneBrowserDataContainer::GetGlobalZone(const std::string& aZoneName) const
{
   wkf::ZoneSetData* retVal{nullptr};
   auto              zoneIter = std::find_if(mGlobalZones.begin(),
                                mGlobalZones.end(),
                                [&](const std::unique_ptr<wkf::ZoneSetData>& aZoneDataPtr)
                                { return (aZoneDataPtr->GetSetName() == aZoneName); });
   if (zoneIter != mGlobalZones.end())
   {
      retVal = (*zoneIter).get();
   }

   return retVal;
}

wkf::ZoneSetData* wkf::ZoneBrowserDataContainer::GetLocalZone(const std::string& aPlatformName,
                                                              const std::string& aZoneName) const
{
   wkf::ZoneSetData* retValPtr{nullptr};
   for (const auto& iter : mZoneMap)
   {
      if (iter.first == aPlatformName)
      {
         auto zoneIter = std::find_if(iter.second.begin(),
                                      iter.second.end(),
                                      [&](const std::unique_ptr<wkf::ZoneSetData>& aZoneDataPtr)
                                      { return (aZoneDataPtr->GetSetName() == aZoneName); });
         if (zoneIter != iter.second.end())
         {
            retValPtr = (*zoneIter).get();
            break;
         }
      }
   }

   return retValPtr;
}
