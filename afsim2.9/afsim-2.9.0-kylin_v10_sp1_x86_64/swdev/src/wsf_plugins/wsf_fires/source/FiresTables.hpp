// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRES_TABLES_HPP
#define FIRES_TABLES_HPP

#include "wsf_fires_export.h"

#include <map>
#include <set>

#include "FiresObserver.hpp"

class WsfScenario;
#include "WsfScenarioExtension.hpp"

//! Provide a top-level interface for including the fires library's types.
namespace Fires
{
class FiresTable;

class WSF_FIRES_EXPORT FiresTables : public WsfScenarioExtension
{
public:
   static FiresTables* Find(const WsfScenario& aScenario);

   FiresTables();
   ~FiresTables() override;

   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;

   virtual void AddType(const std::string& aTypeName);
   virtual bool IsType(const std::string& aType);

   std::map<WsfStringId, WsfStringId>& AeroMap() { return mAeroMap; }
   std::map<WsfStringId, double>&      MassMap() { return mMassMap; }
   std::map<WsfStringId, FiresTable*>& FireTables() { return mTables; }

   FiresTable* GetTable(const std::string& aType) const;

   bool UseFiresMoverExtrapolation() { return mUseFiresMoverExtrapolation; }
   bool UsesMaxOrd(const std::string& aType);
   bool GetMaxOrdAndTOF(const std::string& aType, double aRange, double& aMaxOrd, double& aTOF);

   bool GetElevationAngleAndTOF(const std::string& aType, double aRange, double& aElevationAngle, double& aTOF);

protected:
   bool mUseFiresMoverExtrapolation;
   bool mTestEnabled;

   std::map<WsfStringId, WsfStringId> mAeroMap;
   std::map<WsfStringId, double>      mMassMap;
   std::map<WsfStringId, FiresTable*> mTables;
   std::set<WsfStringId>              mTypes;
};
} // namespace Fires
#endif
