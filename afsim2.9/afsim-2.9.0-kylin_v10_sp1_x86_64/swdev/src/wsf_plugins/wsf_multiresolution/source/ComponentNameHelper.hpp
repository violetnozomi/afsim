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

#ifndef COMPONENT_NAME_HELPER_HPP
#define COMPONENT_NAME_HELPER_HPP

#include "wsf_multiresolution_export.h"

#include <algorithm>
#include <locale>
#include <string>
#include <type_traits>
#include <unordered_map>

// WsfPlatformComponent forward declarations
namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf
class WsfProcessor;
class WsfFuel;
class WsfMover;
class WsfSensor;

// WsfSignature forward declarations
class WsfAcousticSignature;
class WsfInfraredSignature;
class WsfOpticalSignature;
class WsfRadarSignature;

namespace wsf
{
namespace multiresolution
{
constexpr auto cEXTENSION_NAME = "wsf_multiresolution";
constexpr auto cFEATURE_NAME   = "multiresolution";

//! @brief Helper struct for assigning names to platform components.
//! @note For some types, these names could be `cTYPE_KIND` for some types, but it doesn't seem to be consistently
//! defined.
template<typename DerivedComponent>
struct ComponentName
{
   template<class U>
   struct DependentFalse : std::false_type
   {
   };
   static_assert(DependentFalse<DerivedComponent>::value, "Type has no associated name.");
};

//! @brief A wrapper for identifying a platform component within a simulation, which may consist of three names:
//!  platform name, component type, and component name (optional, depending on whether or not a component is unique).
struct PlatformComponentName
{
   std::string mPlatform;
   std::string mComponent;
   std::string mComponentName;
};

//! @brief This name, associated with @a DerivedComponent, is for use in the multiresolution framework. It is set as
//! the name of the platform component instantiated with @a DerivedComponent.
template<typename DerivedComponent>
std::string CommandName()
{
   return std::string("multiresolution_") + ComponentName<DerivedComponent>::cLOWER_CASE_NAME;
}

//! @brief This name, associated with @a DerivedComponent, is used as the base input type for scenario input.
template<typename DerivedComponent>
std::string BaseTypeName()
{
   const std::string upperCaseName = []
   {
      std::string       name = ComponentName<DerivedComponent>::cLOWER_CASE_NAME;
      const std::locale defaultLocale;
      std::transform(name.begin(),
                     name.end(),
                     name.begin(),
                     [&defaultLocale](const char c) { return std::toupper(c, defaultLocale); });
      return name;
   }();
   return "WSF_MULTIRESOLUTION_" + upperCaseName;
}

template<typename DerivedComponent>
constexpr bool RequiresUserInputName()
{
   return ComponentName<DerivedComponent>::cREQUIRES_USER_INPUT_NAME;
}

template<typename DerivedComponent>
constexpr const char* BaseComponentName()
{
   return ComponentName<DerivedComponent>::cLOWER_CASE_NAME;
}

//! @brief Generates a map of each supported multiresolution type name to its required user input flag.
WSF_MULTIRESOLUTION_EXPORT std::unordered_map<std::string, bool> ComponentTypeRequiresNameMap();

//! @name Specializations
//@{
template<>
struct ComponentName<WsfMover>
{
   static constexpr auto cLOWER_CASE_NAME          = "mover";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};

template<>
struct ComponentName<WsfSensor>
{
   static constexpr auto cLOWER_CASE_NAME          = "sensor";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = true;
};

template<>
struct ComponentName<WsfFuel>
{
   static constexpr auto cLOWER_CASE_NAME          = "fuel";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};

template<>
struct ComponentName<wsf::comm::Comm>
{
   static constexpr auto cLOWER_CASE_NAME          = "comm";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = true;
};

template<>
struct ComponentName<WsfProcessor>
{
   static constexpr auto cLOWER_CASE_NAME          = "processor";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = true;
};

template<>
struct ComponentName<WsfAcousticSignature>
{
   static constexpr auto cLOWER_CASE_NAME          = "acoustic_signature";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};

template<>
struct ComponentName<WsfInfraredSignature>
{
   static constexpr auto cLOWER_CASE_NAME          = "infrared_signature";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};

template<>
struct ComponentName<WsfOpticalSignature>
{
   static constexpr auto cLOWER_CASE_NAME          = "optical_signature";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};

template<>
struct ComponentName<WsfRadarSignature>
{
   static constexpr auto cLOWER_CASE_NAME          = "radar_signature";
   static constexpr bool cREQUIRES_USER_INPUT_NAME = false;
};
//@}

namespace detail
{
template<typename Last>
std::unordered_map<std::string, bool> AppendComponentTypeMap(std::unordered_map<std::string, bool>&& componentTypeMap)
{
   componentTypeMap[ComponentName<Last>::cLOWER_CASE_NAME] = ComponentName<Last>::cREQUIRES_USER_INPUT_NAME;
   return std::move(componentTypeMap);
}

template<typename First, typename Second, typename... Rest>
std::unordered_map<std::string, bool> AppendComponentTypeMap(std::unordered_map<std::string, bool>&& componentTypeMap)
{
   return AppendComponentTypeMap<Second, Rest...>(AppendComponentTypeMap<First>(std::move(componentTypeMap)));
}
} // namespace detail
} // namespace multiresolution
} // namespace wsf

#endif
