// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALTARGETINGCOSTTYPES_HPP
#define WSFORBITALTARGETINGCOSTTYPES_HPP

#include <functional>
#include <list>
#include <memory>
#include <string>

class UtInput;
class WsfScenario;

#include "wsf_space_export.h"

namespace wsf
{
namespace space
{

class OrbitalTargetingCost;

class WSF_SPACE_EXPORT OrbitalTargetingCostTypes
{
public:
   OrbitalTargetingCostTypes();

   //! Return a modifiable reference to the orbital targeting cost function type list associated with the given @p aScenario.
   static OrbitalTargetingCostTypes& Get(WsfScenario& aScenario);

   //! Return a const reference to the orbital targeting cost function type list associated with the given @p aScenario.
   static const OrbitalTargetingCostTypes& Get(const WsfScenario& aScenario);

   using FactoryFunction = std::function<std::unique_ptr<OrbitalTargetingCost>(const std::string&)>;

   //! Process orbital targeting cost function type input.
   //! @param aInput                   The current input stream.
   //! @param aOrbitalTargetingCostPtr The pointer to the orbital targeting cost function.
   //! @returns true if the current command was recognized and processes; false otherwise.
   bool LoadInstance(UtInput& aInput, std::unique_ptr<OrbitalTargetingCost>& aOrbitalTargetingCostPtr) const;

   //! Add an object factory for creating an instance of a fundamental type.
   //! The provided @p aFactoryFunction should be added only once.
   //! @param aFactoryFunction An object factory for creating instances of a fundamental type.
   void AddObjectFactory(FactoryFunction aFactoryFunction);

   //! Create an orbital targeting cost instance with the given @p aTypeName.
   //! @param aTypeName The name of the type of orbital targeting cost to create.
   //! @returns An instance of the given @p aTypeName is this object recognizes the type; nullptr otherwise.
   std::unique_ptr<OrbitalTargetingCost> CreateInstance(const std::string& aTypeName) const;

private:
   using ObjectFactoryList = std::list<FactoryFunction>;

   ObjectFactoryList mObjectFactoryList;
};

} // namespace space
} // namespace wsf

#endif // WSFORBITALTARGETINGCOSTTYPES_HPP
