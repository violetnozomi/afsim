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

#ifndef WSFORBITALPROPAGATORTYPES_HPP
#define WSFORBITALPROPAGATORTYPES_HPP

#include "wsf_space_export.h"

#include <functional>
#include <list>
#include <memory>
#include <string>

class UtInput;
class UtOrbitalPropagatorBase;
class WsfScenario;

class WSF_SPACE_EXPORT WsfOrbitalPropagatorTypes
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WsfOrbitalPropagatorTypes&       Get(WsfScenario& aScenario);
   static const WsfOrbitalPropagatorTypes& Get(const WsfScenario& aScenario);
   //@}

   using FactoryPtr        = std::function<std::unique_ptr<UtOrbitalPropagatorBase>(const std::string&)>;
   using ObjectFactoryList = std::list<FactoryPtr>;

   bool LoadInstance(UtInput& aInput, std::unique_ptr<UtOrbitalPropagatorBase>& aOrbitalPropagatorPtr) const;

   void                                     AddObjectFactory(FactoryPtr aFactoryPtr);
   std::unique_ptr<UtOrbitalPropagatorBase> CreateInstance(const std::string& aTypeName) const;

private:
   ObjectFactoryList mObjectFactoryList;
};

#endif
