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

#include "WsfConstellationManager.hpp"

#include "WsfSimulation.hpp"

//! Add a constellation to this manager.
//!
//! This will add the given constellation to this manager if there is not already
//! a constellation with the name of the given @p aConstellationPtr. This returns
//! true if the constellation was successfully added to the manager, or false
//! otherwise.
//!
//! \param aConstellationPtr - The constellation to add.
//! \return                  - true if the constellation was successfully added; false otherwise.
bool WsfConstellationManager::AddConstellation(std::unique_ptr<WsfConstellation> aConstellationPtr)
{
   bool retval{false};
   auto iter = mConstellations.find(aConstellationPtr->GetName());
   if (iter == mConstellations.end())
   {
      mConstellations[aConstellationPtr->GetName()] = std::move(aConstellationPtr);
      retval                                        = true;
   }
   return retval;
}

//! Return the constellation with the given name, if it exists.
//!
//! \param aConstellationName - The name of the constellation to find.
//! \return                   - The constellation if there is a constellation with that name; nullptr otherwise.
WsfConstellation* WsfConstellationManager::FindConstellation(const std::string& aConstellationName) const
{
   WsfConstellation* retvalPtr{nullptr};
   auto              iter = mConstellations.find(aConstellationName);
   if (iter != mConstellations.end())
   {
      retvalPtr = iter->second.get();
   }
   return retvalPtr;
}

//! Return the constellations defined in the current simulation.
std::vector<WsfConstellation*> WsfConstellationManager::GetConstellations() const
{
   std::vector<WsfConstellation*> retval{};
   for (auto& i : mConstellations)
   {
      retval.push_back(i.second.get());
   }
   return retval;
}

//! Accessor to get a handle to the instance of this extension for the given @p aSimulation.
WsfConstellationManager& WsfConstellationManager::Get(const WsfSimulation* aSimulation)
{
   return static_cast<WsfConstellationManager&>(aSimulation->GetExtension(cEXTENSION_NAME));
}
