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

#ifndef WSFEW_TECHNIQUE_HPP
#define WSFEW_TECHNIQUE_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

class UtInput;
class UtInputBlock;
class WsfEW_EA;
class WsfEW_EP;
class WsfEW_EA_EP;
#include "WsfEW_Effect.hpp"
#include "WsfEW_Types.hpp"
#include "WsfObject.hpp"

//! A class which encapsulates data and functionality related to Techniques.
//!
//! Each EA and EP technique may have multiple EW Effects they may cause or perceive during
//! operation.  The individual EW Effect objects will be set via ProcessInput() at simulation
//! start time and fixed thereafter.  But in response to changing EA and EP Techniques, a
//! different EW Effect will be referenced for the resulting tracks obscuration.

class WSF_MIL_EXPORT WsfEW_Technique : public WsfObject
{
public:
   //! A vector type to hold list of effects.
   using EffectPtrVec = std::vector<WsfEW_Effect*>;

   WsfEW_Technique();
   WsfEW_Technique& operator=(const WsfEW_Technique&) = delete;
   ~WsfEW_Technique() override;

   //! @name Common infrastructure methods.
   //@{

   //! Clone this object.
   //! @return a pointer to the new object.
   WsfEW_Technique* Clone() const override;

   virtual bool Initialize(WsfSimulation& aSimulation);
   bool         ProcessInput(UtInput& aInput) override;
   //@}


   //! @name Transmitter management methods.
   //@{

   virtual void RemoveJammerXmtr(unsigned int aXmtrId);
   virtual void RemoveJammerXmtr(WsfEM_Xmtr* aXmtrPtr);
   //@}

   //! @name Class pointer methods.
   //@{

   //! Set the Electronic Attack pointer, set to NULL by default.
   //! @param aEA_Ptr The associated EA pointer for the technique.
   void SetEA_EP_Ptr(WsfEW_EA_EP* aEA_Ptr) { mEA_EP_Ptr = aEA_Ptr; }

   //! Get the Electronic Attack / Electronic Protect pointer, returns NULL if not set.
   //! @return Returns the associated EA pointer.
   WsfEW_EA_EP* GetEA_EP_Ptr() const { return mEA_EP_Ptr; }

   //! Get the Electronic Attack pointer, returns NULL if not set or if N/A.
   //! @return Returns the associated EA pointer.
   WsfEW_EA* GetEA_Ptr() const;

   //! Get the Electronic Protect pointer, returns NULL if not set or if N/A.
   //! @return Returns the associated EP pointer.
   WsfEW_EP* GetEP_Ptr() const;
   //@}

   //! @name Status methods
   //@{

   //! Set the default 'on' state
   //! @param aDefaultOn The default state of the technique, 'true' is on & 'false' is off.
   void SetDefaultOn(bool aDefaultOn) { mDefaultOn = aDefaultOn; }

   //! Get the default 'on' state
   bool IsDefaultOn() const { return mDefaultOn; }

   //! Check to see if this technique is externally controlled.
   //! @note Valid only for EP_Techniques.
   virtual bool IsExternallyControlled() const { return false; }

   //! Set the debug flag state
   //! @param aDebug The debug flag state, 'true' or 'false'
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   //! Get the debug flag state
   //! @return Returns the debug flag state, 'true' or 'false'
   bool DebugEnabled() const { return mDebug; }
   //@}

   //! @name Effect methods
   //@{

   bool AddEffect(WsfEW_Effect* aEffectPtr);

   bool EffectIsAvailable(WsfStringId aEffectNameId) const;

   unsigned int GetEffectCount() { return static_cast<unsigned int>(mEffects.size()); }

   WsfEW_Effect* GetEffect(unsigned int aIndex) const;
   WsfEW_Effect* GetEffect(WsfStringId aEffectNameId) const;

   const EffectPtrVec& GetEffects() const { return mEffects; }

   WsfEW_Effect* SelectUnmitigatedEffect(unsigned int aSystemFunctionMask);
   WsfEW_Effect* SelectUnmitigatedEffect(unsigned int aIndex, unsigned int aSystemFunctionMask);

   bool SetDeltaGainEffect(double      aSimTime,
                           WsfStringId aEffectId,
                           bool        aDebug,
                           double      aJammingPowerGain,
                           WsfStringId aSystemTypeId,
                           WsfStringId aSystemFunctionID);

   bool HasFrequencyAgilityEffect() const;

   //@}

   //! @name Technique application methods
   //@{
   bool CanApplyTechnique(unsigned int aSystemFunctionMask) const;

   //! Returns a pointer to the mitigated techniques list for this technique.
   virtual std::vector<WsfEW_Types::TechniqueId>* GetMitigatedTechniqueIds() { return nullptr; }

   //! Returns a pointer to the mitigated techniques list for this technique.
   virtual std::vector<WsfEW_Types::TechniqueId>* GetMitigatedTechniqueClassIds() { return nullptr; }

   //! Returns the user input mitigation flag ID for this EA technique that can be used
   //! to select EP techniques for mitigation.
   virtual WsfStringId GetMitigationClassId() const { return WsfStringId(); }
   //@}

protected:
   WsfEW_Technique(const WsfEW_Technique& aSrc);

   bool FindEffectIndex(WsfStringId aEffectNameId, unsigned int& aIndex) const;

private:
   void SelectDefaultUnmitigatedEffects(EffectPtrVec aEffectList, unsigned int aSystemFunctionMask);

   bool         mDebug;     //!< Debug flag 'true' if debug is enabled, 'false' otherwise
   EffectPtrVec mEffects;   //!< List of the effects associated with the technique.
   bool         mDefaultOn; //!< Variable to define if the technique is on by default 'true', false otherwise.
   WsfEW_EA_EP* mEA_EP_Ptr; //!< Electronic Attack Pointer, NULL if N/A.
};

#endif
