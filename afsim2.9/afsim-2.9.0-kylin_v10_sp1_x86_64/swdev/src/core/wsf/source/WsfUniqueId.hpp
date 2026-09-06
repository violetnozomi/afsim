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

#ifndef WSFUNIQUEID_HPP
#define WSFUNIQUEID_HPP

#include "wsf_export.h"
class WsfSimulation;

//! A class that maintains a unique identifier for an object.
//!
//! It is sometimes beneficial for simulation objects to have the concept of a unique identifier
//! that distinguishes it from all other objects. This provides such a capability. Unique identifiers
//! are NEVER reused within a given simulation instance.
//!
//! A unique identifier is an unsigned integer so it is very efficient key for a map or a hash_map.
//!
//! To use this capability, a class must do two things:
//! - Inherit from this class.
//! - Invoke the AssignUniqueId method from within their Initialize method when being added to a
//!   simulation.
//!
//! Unique identifiers are valid ONLY within the context of a simulation. Objects that are not part
//! of a simulation will return a value of 0.

class WSF_EXPORT WsfUniqueId
{
public:
   //! Construtor initializes the unique identifier to 0.
   WsfUniqueId()
      : mUniqueId(0)
   {
   }

   //! Copy constructor initializes the unique identifier to 0.
   WsfUniqueId(const WsfUniqueId& /* aSrc */)
      : mUniqueId(0)
   {
   }

   //! The assignment operator does nothing as it must preserve the unique
   //! identification of the target.
   WsfUniqueId& operator=(const WsfUniqueId& /* aRhs */) { return *this; }

   //! Get the unique identifier assigned to this object.
   unsigned int GetUniqueId() const { return mUniqueId; }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mUniqueId;
   }

protected:
   void AssignUniqueId(WsfSimulation* aSimulationPtr);

private:
   //! The unique ID that is assigned to this object.
   unsigned int mUniqueId;
};

#endif
