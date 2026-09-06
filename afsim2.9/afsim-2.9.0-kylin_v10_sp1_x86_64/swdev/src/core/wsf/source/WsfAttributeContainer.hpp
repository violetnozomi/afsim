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

#ifndef WSFATTRIBUTECONTAINER_HPP
#define WSFATTRIBUTECONTAINER_HPP

#include "wsf_export.h"

class UtAttributeBase;
#include "UtAttributeContainer.hpp"
class UtInput;

//! A WsfAttributeContainer that can be used to load attributes from input files.
//! (see WsfAttributeContainer::ProcessInput).
class WSF_EXPORT WsfAttributeContainer : public UtAttributeContainer
{
public:
   //! Make a copy of this attribute container.
   //! Instead of calling this directly, users should use ut::clone.
   WsfAttributeContainer* Clone() const override { return new WsfAttributeContainer(*this); }

   //! Process input from a generic source.
   //! Input can be of a standard type (bool, int, double);
   //! or, it can be a generic "attribute", of a name that has been
   //! registered via the WsfAttributeContainer::RegisterPrototype method.
   //! See the WSF User Input Guide for more information about valid
   //! inputs.
   //!   @param aInput The UtInput object from which commands are extracted.
   virtual bool ProcessInput(UtInput& aInput);

   static bool LoadType(UtInput& aInput); // processes "struct" input, saves the type
   static void ClearTypes();

protected:
   void ParseAssignment(UtInput& aInput, std::string& aName, std::string& aValue);

private:
   bool ProcessAttribute(UtInput& aInput);

   static const WsfAttributeContainer* ProcessStruct(UtInput& aInput); // registers type into base class static member

   static bool mPrototypesRegistered;
};

namespace WsfUtil
{
// Template function to extract an aux_data value from a system (WsfPlatform, WsfPlatformPart, etc...)
template<typename SYSTEM, typename ATTRIBUTE_TYPE>
inline bool GetAuxValue(SYSTEM* aSystem, const std::string& aLabel, ATTRIBUTE_TYPE& aValue)
{
   bool found(false);
   if (aSystem->HasAuxData())
   {
      found = aSystem->GetAuxDataConst().TryGet(aLabel, aValue);
   }
   return found;
}

template<typename SYSTEM, typename ATTRIBUTE_TYPE>
inline bool GetAuxValue(SYSTEM& aSystem, const std::string& aLabel, ATTRIBUTE_TYPE& aValue)
{
   return GetAuxValue(&aSystem, aLabel, aValue);
}
} // namespace WsfUtil

#endif
