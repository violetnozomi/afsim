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

#ifndef WSFCOMPONENTROLES_HPP
#define WSFCOMPONENTROLES_HPP

#include <type_traits>

//! Struct template providing compile-time constant representing component role. It is compile-time error to
//! instantiate this template. Types that define component roles must specialize the template using the helper
//! macro below.
template<class T>
struct WsfComponentRole
{
   template<class U>
   struct always_false : std::false_type
   {
   };
   static_assert(
      always_false<T>::value,
      "Type has no component role registered. Use the WSF_DECLARE_COMPONENT_ROLE_TYPE macro to register the type.");
};

//! Helper macro to declare a component role for a type by specializing WsfComponentRole template. If TYPE is in a
//! namespace, the macro has to be outside the namespace:
//! namespace wsf
//! {
//!    namespace comm
//!    {
//!       class Comm { ... };
//!       ...
//!    }
//! }
//! WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::Comm, cWSF_COMPONENT_COMM)

#define WSF_DECLARE_COMPONENT_ROLE_TYPE(TYPE, ROLE)                                                    \
   template<>                                                                                          \
   struct WsfComponentRole<TYPE> : std::integral_constant<int, ROLE>                                   \
   {                                                                                                   \
      static_assert(std::is_base_of<WsfComponent, TYPE>::value,                                        \
                    "Cannot register component role for type that does not derive from WsfComponent"); \
      static_assert(value > 0, "Component role must be > 0");                                          \
   };

//! Helper variable template can be used when the code base is moved to c++14 or further.
//! Until then, struct template below must be used.
// template <class T>
// constexpr int cCOMPONENT_ROLE = WsfComponentRole<T>::value;

template<class T>
struct cCOMPONENT_ROLE
{
   constexpr operator int() const noexcept { return WsfComponentRole<T>::value; }
};

//! The standard component roles.
//!
//! Every component type has one or more 'roles', each of which is assigned a number. These numbers
//! are defined by committee (much like DIS enumerations), with several guidelines as follows:
//!
//! - Numbers   0-199 are reserved for components that are part of the AFSIM core (directory 'wsf')
//! - Numbers 200-999 are reserved for components that are part of AFSIM but are located in other
//!   directories. These numbers are not published here for security reasons, but are agreed to and
//!   defined in their respective directories.
//! - Numbers 1000 and above are reserved for allocation to sites.
//!
//! A component may have multiple roles that it returns in the method WsfComponent::GetComponentRoles.
//! The first entry in the list is its 'primary' role while others are used represent other roles
//! in the inheritance hierarchy of the component.
//!
//! For instance, WsfSensor returns the following list:
//!
//! \code
//!    static const int roles[] = { cWSF_COMPONENT_SENSOR, cWSF_COMPONENT_ARTICULATED_PART,
//!                                 cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL };
//! \endcode
//!
//! The definitions of multiple roles allows one to invoke WsfComponent::QueryInterface to retrieve
//! a pointer to a specific interface.

enum
{
   //! A special entry to indicate the end of the list of roles assumed by a component.
   cWSF_COMPONENT_NULL = 0,

   cWSF_COMPONENT_PLATFORM = 1, //!< The component is or derives from WsfPlatform

   //! @name Components that are part of a platform.
   //@{
   cWSF_COMPONENT_PLATFORM_PART     = 2,  //!< The component derives from WsfPlatformPart.
   cWSF_COMPONENT_ARTICULATED_PART  = 3,  //!< The component derives from WsfArticulatedPart.
   cWSF_COMPONENT_MOVER             = 4,  //!< A 'mover' component (WsfMover)
   cWSF_COMPONENT_FUEL              = 5,  //!< A 'fuel' component (WsfFuel)
   cWSF_COMPONENT_COMM              = 6,  //!< A 'comm' component (WsfComm)
   cWSF_COMPONENT_COMM_ROUTER       = 7,  //!< A 'router' component (WsfCommRouter)
   cWSF_COMPONENT_PROCESSOR         = 8,  //!< A 'processor' component (WsfProcessor)
   cWSF_COMPONENT_SENSOR            = 9,  //!< A 'sensor' component (WsfSensor)
   cWSF_COMPONENT_VISUAL            = 10, //!< A 'visual_part' component (WsfVisualPart)
   cWSF_COMPONENT_TRACK_MANAGER     = 11, //!< A 'track manager' component (WsfTrackManager)
   cWSF_COMPONENT_NAVIGATION_ERRORS = 12, //!< A 'navigation_error' component (WsfNavigationErrors)
   cWSF_COMPONENT_COMMAND_CHAIN     = 13, //!< A 'command_chain' component (WsfCommandChain)
   cWSF_COMPONENT_ZONE              = 14, //!< A 'zone' component (WsfZone)
   cWSF_COMPONENT_GEOPOINT          = 15, //!< A 'geopoint' component (WsfGeoPoint)
   cWSF_COMPONENT_CALLBACK          = 16, //!< A 'callback' component (WsfCallback)
   cWSF_COMPONENT_INTERSECT_MESH    = 17, //!< A 'intersect_mesh" component (WsfIntersectMesh)
   cWSF_COMPONENT_THERMAL_SYSTEM    = 18, //!< A 'thermal system' component
                                          //@}

   //! @name Core components that are a part of the comm framework
   //@{
   cWSF_COMPONENT_COMM_HW                        = 61,
   cWSF_COMPONENT_COMM_XIO                       = 62,
   cWSF_COMPONENT_COMM_PROTOCOL                  = 63,
   cWSF_COMPONENT_COMM_PROTOCOL_IGMP             = 64,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL           = 65,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_LEGACY    = 66,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_RIPv2     = 67,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF      = 68,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST = 69,
   cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_AD_HOC    = 70,
   cWSF_COMPONENT_COMM_MEDIUM_CONTAINER_COMM     = 71,
   cWSF_COMPONENT_COMM_MEDIUM_CONTAINER_ROUTER   = 72,
   //@}

   //! @name Object which by themselves are not components, but which contain a component list.
   //! The host of a component list must have a role in order to use component factories (e.g.:
   //! WsfComponentFactory).
   //@{
   cWSF_COMPONENT_EM_XMTR = 98, //!< WsfEM_Xmtr
   cWSF_COMPONENT_EM_RCVR = 99, //!< WSFEM_Rcvr
                                //@}

   //! @name Core components that are not part of a platform
   //@{
   cWSF_COMPONENT_EM_INTERACTION            = 100, //!< An extension to WsfEM_Interaction
   cWSF_COMPONENT_PROCESSOR_COMPONENT       = 101, //!< An extension to WsfProcessor
   cWSF_COMPONENT_TASK_MANAGER              = 102, //!< An extension to WsfTaskManager
   cWSF_COMPONENT_TASK_MANAGER_COMPONENT    = 103, //!< A Component on a WsfTaskManager
   cWSF_COMPONENT_SENSOR_COMPONENT          = 104, //!< A Component on a WsfSensor
   cWSF_COMPONENT_SENSOR_EXCLUSION          = 105, //!< A Solar or lunar exclusion angle component
   cWSF_COMPONENT_SENSOR_MOON_LOS           = 106, //!< A Component enabling Moon LOS checks
   cWSF_COMPONENT_SOLAR_ILLUMINATION        = 107, //!< Solar Illumination command sensor component
   cWSF_COMPONENT_SOLAR_ELEVATION_AT_TARGET = 108  //!< A solar elevation from target sensor component
                                                   //@}
};

//! These are some reference component initialization orders. Gaps are left so new components can reference
//! these values and insert themselves where needed. Note that unless an order is assigned, all components
//! with no order will be assigned a value of 0 and will be initialized in the order in the input file.
//!
//! These are signed ints so they have values of about [-2 billion, +2 billion].

enum
{
   //! Command chains aren't dependent on anything, but something may be dependent on them.
   cWSF_INITIALIZE_ORDER_COMMAND_CHAIN = -1000000000,
   //! The track manager must be initialized prior to the mover.
   cWSF_INITIALIZE_ORDER_TRACK_MANAGER = -900000000,
   //! The mover must be initialized prior to just about everything else because it defines the initial location.
   cWSF_INITIALIZE_ORDER_MOVER = -800000000,
   //! @name WSF 1.x ordering...
   //! The following impose initialization order that is the same as WSF 1.x This isn't strictly necessary but it
   //! makes regression testing possible.
   //@{
   cWSF_INITIALIZE_ORDER_FUEL              = -790000000,
   cWSF_INITIALIZE_ORDER_NAVIGATION_ERRORS = -780000000,
   cWSF_INITIALIZE_ORDER_ROUTER            = -740000000,
   cWSF_INITIALIZE_ORDER_COMM              = -700000000,
   cWSF_INITIALIZE_ORDER_PROCESSOR         = -600000000,
   cWSF_INITIALIZE_ORDER_SENSOR            = -500000000,
   cWSF_INITIALIZE_ORDER_VISUAL_PART       = -400000000,
   cWSF_INITIALIZE_ORDER_RESERVED          = -300000000,
   //@}
   //! Zones generally aren't dependent on anything except position, but something *may* be dependent on them.
   cWSF_INITIALIZE_ORDER_ZONE = -100000000 //!< 'zones' components
};
#endif
