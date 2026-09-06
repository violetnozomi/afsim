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

#ifndef WSFCOMPONENTLISTDEFINES_HPP
#define WSFCOMPONENTLISTDEFINES_HPP

// Some objects (WsfComm, WsfProcessor, and WsfSensor in particular, but there are a few others)
// contain a component list that provides a mechanism to extend the object in an abstract way.
// At strategic places in the object code has to be placed to invoke methods on registered
// components. While the code is not complex, it is laborious to repeat the code in all the
// places. Therefore, these macros are provided to do the work in a common and efficient manner.
//
// The are two assumptions that are made in these macros:
//
// 1) The component list member is visible and is accessible by the name 'mComponents'.
// 2) A typedef exists that defines 'ComponentList' as the type of the component list.
//
// A lot of this could have been done with C++ function templates, but the invocation syntax
// would have been UGLY.
//
// NOTE: All of these perform a HasComponents check for efficiency. This avoid creating the
//       component list iterator for an empty component list.

//! Call FUNCTION(ARG) on each registered component.
//! e.g. WSF_CALL_VOID_COMPONENTS(TurnOn, aSimTime);
#define WSF_CALL_VOID_COMPONENTS(METHOD, ARG) \
   if (mComponents.HasComponents())           \
   {                                          \
      for (auto component : mComponents)      \
      {                                       \
         component->METHOD(ARG);              \
      }                                       \
   }

#endif
