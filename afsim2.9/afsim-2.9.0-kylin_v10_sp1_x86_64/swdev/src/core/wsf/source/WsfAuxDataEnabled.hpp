// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFAUXDATAENABLED_HPP
#define WSFAUXDATAENABLED_HPP

#include "wsf_export.h"

#include <memory>

#include "UtCallbackN.hpp"
#include "WsfAttributeContainer.hpp"

//! A common base class for 'aux data'-enabled objects throughout the framework,
//! including platforms, platform parts, routes, waypoints, tasks, tracks, and zones.
//! This auxiliary data is not used directly by the core framework, but provides users
//! (through corresponding script interface methods) and model developers a way to
//! attach generic properties to an object. This class provides common input processing,
//! attribute management, and XIO (de)-serialization functionality.
class WSF_EXPORT WsfAuxDataEnabled
{
public:
   WsfAuxDataEnabled()                              = default;
   WsfAuxDataEnabled(const WsfAuxDataEnabled& aSrc) = default;
   WsfAuxDataEnabled& operator                      =(const WsfAuxDataEnabled& aRhs);
   WsfAuxDataEnabled(WsfAuxDataEnabled&&)           = default;
   WsfAuxDataEnabled& operator=(WsfAuxDataEnabled&&) = default;
   virtual ~WsfAuxDataEnabled() noexcept;

   //! @name Auxiliary attribute management.
   //@{
   //! Returns the aux data container.
   //! The non-const versions will emit AuxDataAccessed().
   //! @return A reference to a WsfAttributeContainer object.
   //! @see WsfAttributeContainer
   //! @{
   WsfAttributeContainer&       GetAuxData();
   const WsfAttributeContainer& GetAuxData() const noexcept;

   const WsfAttributeContainer& GetAuxDataConst() const noexcept;
   //! @}

   void SetAuxData(const WsfAttributeContainer& aAuxData);

   //! Delete the aux data container from the object.
   void DeleteAuxData();
   //! Merge the object's aux data with aux data from an existing object.
   void MergeAuxData(const WsfAuxDataEnabled& aSrc);
   //! Update the object's aux data with aux data from an existing object.
   void UpdateAuxData(const WsfAuxDataEnabled& aSrc);

   //! Returns true if this object has aux data
   bool HasAuxData() const;
   //@}

   bool ProcessInput(UtInput& aInput);

   //! (De)serialization for XIO
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mAuxData;
      AuxDataAccessed(this); // this line should be changed to only trigger when aux data is written to
   }

   //! Called whenever aux data is accessed via WsfAuxDataEnabled.
   //! This means that the data may-or-may-not be changed.
   //! The parameter is a pointer to the WsfAuxDataEnabled that was accessed.
   //! @note At the time the callback is called, the data has not necessarily
   //!    been accessed yet.
   //! @note Not called on construction.
   static UtCallbackListN<void(const WsfAuxDataEnabled*)> AuxDataAccessed;

   //! Called whenever a WsfAuxDataEnabled is destroyed.
   //! The parameter is a pointer to the WsfAuxDataEnabled being destroyed.
   //! @note It is UNSAFE to dynamic_cast the pointer.
   static UtCallbackListN<void(const WsfAuxDataEnabled*)> AuxDataDestroyed;

private:
   //! The actual aux data.
   WsfAttributeContainer mAuxData;
};

#endif
