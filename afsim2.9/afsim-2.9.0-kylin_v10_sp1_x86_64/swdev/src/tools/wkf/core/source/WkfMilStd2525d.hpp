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
#ifndef WKFMILSTCICON_HPP
#define WKFMILSTCICON_HPP

#include "wkf_export.h"

#include <string>
#include <vector>

#include <QMap>

#include "WkfDefines.hpp"

class WKF_EXPORT WkfMilStd2525d
{
public:
   WkfMilStd2525d(const QStringList& aFileList);

   struct SymbolIdentificationCode
   {
      std::string mVersion       = "10";
      std::string mStdIdentity   = "00";
      std::string mSymbolSet     = "00";
      std::string mStatus        = "0";
      std::string mHQDummy       = "0";
      std::string mAmplifier     = "00";
      std::string mEntity        = "00";
      std::string mEntityType    = "00";
      std::string mEntitySubtype = "00";
      std::string mSector1Mod    = "00";
      std::string mSector2Mod    = "00";
      std::string mExtra         = ".png";
      bool        mMilPath       = false;

      std::string ToString() const;

      bool operator==(const SymbolIdentificationCode& rhs) const { return (ToString() == rhs.ToString()); }
   };

   // This is the 2nd bit of the Standard Identity
   enum class Side
   {
      UNKNOWN,
      FRIENDLY,
      HOSTILE,
      NEUTRAL
   };

   struct PartialInput
   {
      PartialInput(Side aSide, const wkf::SpatialDomain& aSpatialDomain, const std::string& aEntityType)
         : mSide(aSide)
         , mSpatialDomain(aSpatialDomain)
         , mEntityType(aEntityType)
      {
      }

      Side               mSide{Side::UNKNOWN};
      wkf::SpatialDomain mSpatialDomain{wkf::SpatialDomain::SPATIAL_DOMAIN_UNKNOWN};
      std::string        mEntityType;
   };

   // Returns the best image model match based on the input.
   // Also adds it to VTK Model database if it is not already added.
   std::string GetModelName(const PartialInput& aInput) const;
   std::string GetModelName(const SymbolIdentificationCode& aSIDC) const;

   static Side GetSideFromTeam(const std::string& aTeam);

private:
   // If none match exactly, an empty string is returned.
   std::string GetFilePath(const SymbolIdentificationCode& aSIDC) const;
   // Returns the full path to a model file that is the either the exact file requested, or a similar file
   std::string GetFilePathForClosestMatch(const SymbolIdentificationCode& aSIDC) const;
   // Returns the full path to a model file that is the either the exact file requested, or a similar file
   std::string GetFilePathForClosestMatch(const std::string& aMilStdString) const;
   // Get an SIDC from a Partial Input
   SymbolIdentificationCode GetSIDC_FromInput(const PartialInput& aInput) const;
   // Determine what the closest SIDC loaded is to the SIDC provided
   SymbolIdentificationCode GetClosestMatch(const SymbolIdentificationCode& aSIDC) const;
   SymbolIdentificationCode GetClosestMatchFromList(const SymbolIdentificationCode&              aSIDC,
                                                    const std::vector<SymbolIdentificationCode>& aSIDC_list) const;
   void                     LoadMilStdIcons();
   void                     SetResourcePath();

   std::string                           mMilDir;
   std::string                           mSiteDir;
   std::vector<SymbolIdentificationCode> mCompleteMilStdFileList;
   // Map of platform icons (icon command on platforms) to a string representation of a SIDC
   QMap<QString, std::string> mIconMappings;
};
#endif
