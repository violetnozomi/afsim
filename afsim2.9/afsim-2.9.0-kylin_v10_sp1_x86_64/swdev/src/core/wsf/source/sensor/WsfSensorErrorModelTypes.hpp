// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFSENSORERRORMODELTYPES_HPP
#define WSFSENSORERRORMODELTYPES_HPP

#include <memory>
#include <vector>

#include "WsfObjectTypeList.hpp"
#include "WsfSensorErrorModel.hpp"

class WsfSensorErrorModelTypes : public WsfObjectTypeList<wsf::SensorErrorModelBase>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfSensorErrorModelTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfSensorErrorModelTypes& Get(const WsfScenario& aScenario);
   //@}

   using FactoryPtr = std::function<std::unique_ptr<wsf::SensorErrorModelBase>(const std::string&)>;

   explicit WsfSensorErrorModelTypes(WsfScenario& aScenario);

   LoadResult LoadType(UtInput& aInput) override;

   WSF_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WSF_EXPORT bool LoadInstance(UtInput& aInput, std::unique_ptr<wsf::SensorErrorModelBase>& aSensorErrorModelPtr);

private:
   std::unique_ptr<wsf::SensorErrorModelBase> CreateInstance(const std::string& aTypeName);

   // The list of object factories.
   using ObjectFactoryList = std::vector<FactoryPtr>;

   ObjectFactoryList mObjectFactoryList;
   int               mUniqueId;
};

#endif
