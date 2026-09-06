// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFLANDCOVER_HPP
#define WSFLANDCOVER_HPP

#include "WsfEnvironment.hpp"

//! Simple Class representing Land Cover
//! Currently, only contains a utility function;
//! may be expanded in the future.
class WSF_EXPORT WsfLandCover
{
public:
   static const std::string& ToString(WsfEnvironment::LandCover aLandCover)
   {
      static const std::string sToString[14] = {"general",
                                                "urban",
                                                "agricultural",
                                                "rangeland_herbaceous",
                                                "rangeland_shrub",
                                                "forest_deciduous",
                                                "forest_coniferous",
                                                "forest_mixed",
                                                "forest_clear_cut",
                                                "forest_block_cut",
                                                "wetland_forested",
                                                "wetland_non_forested",
                                                "barren",
                                                "water"};
      return sToString[aLandCover];
   }
};
#endif
