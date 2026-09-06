// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "aces/WkfAcesDisplayDataContainer.hpp"

wkf::AcesDisplay::DataContainer::DataContainer(const wkf::AcesDisplay::DataContainer& aSrc)
   : mAcesData(aSrc.mAcesData)
   , mAcesTruthPlatforms(aSrc.mAcesTruthPlatforms)
   , mPlatformComments(aSrc.mPlatformComments)
   , mPlatformsWithSAP(aSrc.mPlatformsWithSAP)
   , mSimulationInitialized(aSrc.mSimulationInitialized)
{
}

void wkf::AcesDisplay::DataContainer::SetAcesTruthPlatforms(const std::vector<AcesDisplay::EntityTruth>& aAcesTruthPlatforms)
{
   mAcesTruthPlatforms = aAcesTruthPlatforms;
}

void wkf::AcesDisplay::DataContainer::SetAcesData(const std::map<size_t, AcesData>& aAcesData)
{
   mAcesData = aAcesData;
}

void wkf::AcesDisplay::DataContainer::SetPlatformComments(const std::map<size_t, std::map<double, std::string>>& aPlatformComments)
{
   mPlatformComments = aPlatformComments;
}

void wkf::AcesDisplay::DataContainer::AddPlatformComment(const size_t       aPlatformIndex,
                                                         const double       aSimTime,
                                                         const std::string& aMessage)
{
   mPlatformComments[aPlatformIndex].emplace(aSimTime, aMessage);
}

void wkf::AcesDisplay::DataContainer::AddPlatformWithSAP(const size_t aPlatformIndex)
{
   mPlatformsWithSAP.insert(aPlatformIndex);
}

void wkf::AcesDisplay::DataContainer::AddEntityTruth(EntityTruth& aEntity)
{
   mAcesTruthPlatforms.push_back(aEntity);
}

void wkf::AcesDisplay::DataContainer::RemoveEntityTruth(const size_t aPlatformIndex)
{
   auto indexMatches = [aPlatformIndex](const EntityTruth& entity) { return (entity.index == aPlatformIndex); };

   mAcesTruthPlatforms.erase(std::remove_if(mAcesTruthPlatforms.begin(), mAcesTruthPlatforms.end(), indexMatches),
                             mAcesTruthPlatforms.end());
}
