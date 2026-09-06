// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenInputData.hpp"

#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>

namespace sceneGen
{
/// These are the valid colors that can be assigned to a group of platforms.
/// Helps in quickly validated parsed input files
static const std::set<std::string> cSides = {{"black"}, {"blue"}, {"green"}, {"orange"}, {"red"}, {"white"}, {"yellow"}};

/// @brief Helper function that validates input data and reports error message
///    if color is not handled.
/// @param aColor - the side to error check
/// returns true if valid color, false otherwise
bool ValidColor(const std::string& aColor)
{
   bool result = true;
   auto it     = cSides.find(aColor);

   if (it == cSides.end())
   {
      result        = false;
      auto logError = ut::log::error() << aColor << " not defined as possible side.\n";

      logError.AddNote() << "Valid colors:\n";

      for (auto& color : cSides)
      {
         logError.AddNote() << color;
      }

      logError.AddNote() << '\n';
   }
   return result;
}

void ValidateInput(const SceneGenInputData& aData)
{
   bool valid = true;
   if (aData.mName == "")
   {
      auto logError = ut::log::error() << "Name must be specified. Ex: name bomber";
      valid         = false;
   }
   if (aData.mType == "")
   {
      auto logError = ut::log::error() << "Type must be specified. Ex: type BOMBER";
      valid         = false;
   }
   if (aData.mSide == "")
   {
      auto logError = ut::log::error() << "Side must be specified. Ex: side red";
      valid         = false;
   }
   else if (!ValidColor(aData.mSide))
   {
      valid = false;
   }
   if (aData.mCount <= 0)
   {
      auto logError = ut::log::error() << "Count must be specified and greater than 0. Ex: count 100";
      valid         = false;
   }
   if (std::abs(aData.mStartlat) > cMAX_LAT)
   {
      auto logError = ut::log::error() << "Invalid latitude. Must satisfy ABS(latitude) <= 90.0. Ex: start 40.0 -110.0";
      valid         = false;
   }
   if (std::abs(aData.mStartlon) > cMAX_LON)
   {
      auto logError =
         ut::log::error() << "Invalid longitude. Must satisfy ABS(longitude) <= 180.0. Ex: start 40.0 -110.0";
      valid = false;
   }

   if (!valid)
   {
      throw std::logic_error("Missing required input data.");
   }
}

} // namespace sceneGen
