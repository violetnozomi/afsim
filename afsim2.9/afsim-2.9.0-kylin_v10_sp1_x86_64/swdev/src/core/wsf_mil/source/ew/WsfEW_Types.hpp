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

#ifndef WSFEW_TYPES_HPP
#define WSFEW_TYPES_HPP

#include <vector>

namespace WsfEW_Types
{

//! Note:  EA = Electronic Attack, EP = Electronic Protect
//! (EP is a countermeasure in response to a perceived EA.)
//! Each integer ID below is a WsfStringId string index.
//! The typedef is for software clarity, to distinguish between EA and EP techniques,
//! which are both represented by an int.
//! Each Technique name is mapped to an ID, via the WsfStringId.
using TechniqueId = WsfStringId;

//! A type definition for a collection of EP or EA techniques.
using TechniqueIdVec = std::vector<TechniqueId>;

// typedef std::vector<WsfFalseTarget*>                   FalseTargets;
// typedef std::vector<WsfFalseTarget*>::iterator         FalseTargetsIter;
// typedef std::vector<WsfFalseTarget*>::const_iterator   FalseTargetsConstIter;

//! Start of the EW effect data class, to be plussed up in the future to allow for
//! top level command input instead of effect level inputs. i.e. An EW mission data load
//! class will be implemented and input by the user specific to threat types and associated
//! EW data.
class WSF_MIL_EXPORT EW_EffectData
{
public:
   EW_EffectData()          = default;
   virtual ~EW_EffectData() = default;

   virtual bool ProcessInput(UtInput& aInput) { return false; };
   virtual bool Initialize(WsfSimulation& aSimulation) { return true; };

   virtual EW_EffectData* Clone() const = 0;

   virtual void Reset() {}

protected:
   EW_EffectData(const EW_EffectData& aSrc) = default;
};
} // namespace WsfEW_Types

#endif
