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

#ifndef WSFEW_EFFECT_HPP
#define WSFEW_EFFECT_HPP

#include "wsf_mil_export.h"

#include <set>

class UtInput;
class UtInputBlock;
#include "UtLog.hpp"
#include "UtOptional.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
class WsfEW_EA;
class WsfEW_EA_EP;
class WsfEW_EP;
class WsfEW_Effects;
#include "WsfEW_Types.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
class WsfEW_Technique;
class WsfEM_Interaction;


//! WsfEW_Effect is an abstract base class that represents an EW (Electronic Warfare)
//! effect. If provides two methods that are used to affect an EW error.  The first
//! method, 'ApplyUnmitigatedEffect' is responsible for applying the EW effect to the
//! errors. The second method, 'ApplyMitigatedEffect' is responsible for  applying any
//! mitigating effects to the error. WsfEW_Effect works in conjunction with the type
//! list WsfEW_EffectTypes.

class WSF_MIL_EXPORT WsfEW_Effect : public WsfObject
{
public:
   //! The behavior the EW effect exhibits.
   enum Behavior
   {
      cEB_UNDEFINED           = 1,
      cEB_DROP_TRACK          = cEB_UNDEFINED << 1,  //!< Track is dropped.
      cEB_MAINTAIN_TRACK      = cEB_UNDEFINED << 2,  //!< Track is maintained.
      cEB_TRACK_ERROR         = cEB_UNDEFINED << 3,  //!< Track errors are applied.
      cEB_DROP_MESSAGE        = cEB_UNDEFINED << 4,  //!< Message is dropped.
      cEB_MAINTAIN_MESSAGE    = cEB_UNDEFINED << 5,  //!< Message is maintained.
      cEB_MESSAGE_ERROR       = cEB_UNDEFINED << 6,  //!< Message error is applied.
      cEB_JAMMER_POWER_EFFECT = cEB_UNDEFINED << 7,  //!< Jammer power gain effect.
      cEB_SIGNAL_POWER_EFFECT = cEB_UNDEFINED << 8,  //!< Signal power or receiver noise delta gain effect.
      cEB_SLC_DEGRADE_EFFECT  = cEB_UNDEFINED << 9,  //!< Sidelobe canceler degrade effect
      cEB_FALSE_TARGET_EFFECT = cEB_UNDEFINED << 10, //!< False target effect.
      cEB_PULSE_EFFECT        = cEB_UNDEFINED << 11, //!< Pulse jammer effect.
      cEB_AGILITY_EFFECT      = cEB_UNDEFINED << 12, //!< Agility effects are applied.
      cEB_REPEATER_EFFECT     = cEB_UNDEFINED << 13, //!< Repeater jamming effects are applied.
      cEB_REPEATER_UPDATE     = cEB_UNDEFINED << 14, //!< Repeater logic is present and is updated
      cEB_FLOODED_BLOCKED = cEB_UNDEFINED << 15, //!< System is flooded, either track or messages and is causing blockage.
      cEB_FLOODED_MAINTAINED = cEB_UNDEFINED << 16, //!< System is flooded, either track or messages, but is mitigated.
      cEB_JAM_STROBE_EFFECT  = cEB_UNDEFINED << 17, //!< Jam strobe effects are applied.
      cEB_TARGET_REJECT_EFFECT = cEB_UNDEFINED << 18, //!< Reject target effect, used to zero jamming power when applied.
      cEB_NEXT_FREE_MASK = cEB_UNDEFINED << 19 //!< For use by derived classes to specify additional behavior masks.
   };

   //! The signal coherency the EW effect exhibits on the affected system.
   enum Coherency
   {
      cEC_NONE              = 1,
      cEC_NONCOHERENT       = cEC_NONE << 1,
      cEC_COHERENT          = cEC_NONE << 2,
      cEC_NONCOHERENT_PULSE = cEC_NONE << 3,
      cEC_COHERENT_PULSE    = cEC_NONE << 4
   };

   //! The type of modulation the EW effect exhibits for modulated signal effects.
   //! Utilized by inheriting classes.
   enum ModulationType
   {
      cEM_NONE        = 1,
      cEM_CW          = cEM_NONE << 1,
      cEM_COHPULSE    = cEM_NONE << 2,
      cEM_NONCOHPULSE = cEM_NONE << 3,
      cEM_LINEARFM    = cEM_NONE << 4,
      cEM_NONLINEARFM = cEM_NONE << 5,
      cEM_PHASEKEY    = cEM_NONE << 6,
      cEM_PHASEMOD    = cEM_NONE << 7,
      cEM_PHASECODE   = cEM_NONE << 8,
      cEM_ASK         = cEM_NONE << 9,
      cEM_FSK         = cEM_NONE << 10,
      cEM_PSK         = cEM_NONE << 11,
      cEM_BPSK        = cEM_NONE << 12,
      cEM_QPSK        = cEM_NONE << 13,
      cEM_DPSK        = cEM_NONE << 14,
      cEM_QAM         = cEM_NONE << 15
   };

   //! The target protection type this effect is limited to.
   enum TargetProtectionType
   {
      cTP_ALL,
      cTP_NON_SELF_PROTECT,
      cTP_SELF_PROTECT
   };

   //! The type of system function types the EW effect can affect.
   enum SystemFunction
   {
      cES_UNDEFINED      = 1,
      cES_COMM           = cES_UNDEFINED << 1, //!< Effect is meant for comm systems.
      cES_SENSOR         = cES_UNDEFINED << 2, //!< Effect is meant for sensor systems.
      cES_JAMMER         = cES_UNDEFINED << 3, //!< Effect is meant for jammer systems.
      cES_NEXT_FREE_MASK = cES_UNDEFINED << 4  //!< For use by derived classes to specify additional EW function masks.
   };

   WsfEW_Effect()        = default;
   WsfEW_Effect& operator=(const WsfEW_Effect&) = delete;
   ~WsfEW_Effect() override;

   //! @name Common infrastructure methods.
   //@{

   //! Clone this object.
   //! @return a pointer to the new object.
   WsfEW_Effect* Clone() const override = 0;

   virtual bool Initialize(WsfSimulation& aSimulation);
   bool         ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Effect methods.
   //@{
   // =================================================================================================
   //! Runs the calculation of an effect if it has been unmitigated. This Unmitigated vs Mitigated is based on if there
   //! is a Electronic Protect pointer to counter the attack type of the Electronic Attack pointer.
   //!
   //! @param aSimTime                 [input] The current simulation time.
   //! @param aTargetInteraction       [input] The target interaction for the detection attempt.
   //! @param aJammerToTgtInteraction  [input] The jammer interaction for the jammer xmtr to victim Rcvr.
   //! @param aTgtToJammerInteraction  [input] Optional jammer interaction for the Victim Xmtr to jammer Rcvr.
   //!                                         This will have no value if the jammer Rcvr doesn't exist.
   //! @param aEW_Effects              [input] The currently processing EW effect pointer to prevent
   //!                                         recursion in EW effect processing loops.
   virtual void ProcessUnmitigatedEffect(double                                 aSimTime,
                                         const WsfEM_Interaction&               aTargetInteraction,
                                         const WsfEM_Interaction&               aJammerToTgtInteraction,
                                         const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                         WsfEW_Effects&                         aEW_Effects);

   // =================================================================================================
   //! Runs the calculation of an effect if it has been mitigated. This Unmitigated vs Mitigated is based on if there is
   //! a Electronic Protect pointer to counter the attack type of the Electronic Attack pointer.
   //!
   //! @param aSimTime                 [input] The current simulation time.
   //! @param aTargetInteraction       [input] The target interaction for the detection attempt.
   //! @param aJammerToTgtInteraction  [input] The jammer interaction for the jammer xmtr to victim Rcvr.
   //! @param aTgtToJammerInteraction  [input] Optional jammer interaction for the Victim Xmtr to jammer Rcvr.
   //!                                         This will have no value if the jammer Rcvr doesn't exist.
   //! @param aEW_Effects              [input] The current effect that is being updated.
   //! @param aEW_EffectPtr  [input] The currently processing EW effect pointer to prevent
   //!                                         recursion in EW effect processing loops.
   virtual void ProcessMitigatedEffect(double                                 aSimTime,
                                       const WsfEM_Interaction&               aTargetInteraction,
                                       const WsfEM_Interaction&               aJammerToTgtInteraction,
                                       const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                       WsfEW_Effects&                         aEW_Effects,
                                       WsfEW_Effect*                          aEW_EffectPtr = nullptr);

   //! @name Internal use only effect methods.
   //@{
   virtual void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const = 0;

   virtual void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const = 0;

   // =================================================================================================
   //! Allows the effect to update any parameters needed before running an apply effect method.
   //!
   //! @param aSimTime                 [input] The current simulation time.
   //! @param aTargetInteraction       [input] The target interaction for the detection attempt.
   //! @param aJammerToTgtInteraction  [input] The jammer interaction for the jammer Xmtr to victim Rcvr.
   //! @param aTgtToJammerInteraction  [input] Optional jammer interaction for the Victim Xmtr to jammer Rcvr.
   //!                                         This will have no value if the jammer Rcvr doesn't exist.
   //! @param aEW_Effects              [input] The current effect that is being updated.
   //! @param aEW_EffectPtr  [input] The currently processing EW effect pointer to prevent
   //!                                         recursion in EW effect processing loops.
   virtual void UpdateEffects(double                                 aSimTime,
                              const WsfEM_Interaction&               aTargetInteraction,
                              const WsfEM_Interaction&               aJammerToTgtInteraction,
                              const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                              WsfEW_Effects&                         aEW_Effects,
                              WsfEW_Effect*                          aEW_EffectPtr = nullptr);
   //@}

   //! @name Jammer management methods.
   //@{

   virtual void RemoveJammerXmtr(unsigned int aXmtrId);
   virtual void RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr);
   //@}

   //! @name System function methods.
   //@{

   //! Set the system type.
   void SetSystemFunctionMask(SystemFunction aSystemFunction) { mSystemFunctionMask = aSystemFunction; }
   void SetSystemFunctionMask(const std::string& aSystemFunctionId);

   //! Set the system type.
   void AddSystemFunctionMask(SystemFunction aSystemFunction) { mSystemFunctionMask |= aSystemFunction; }
   void AddSystemFunctionMask(const std::string& aSystemFunctionId);

   //! Get the system function
   //! @return returns SystemFunction by value.
   unsigned int GetSystemFunctionMask() const { return mSystemFunctionMask; }

   static WsfEW_Effect::SystemFunction ConvertToEW_SystemFunction(WsfEM_Rcvr::RcvrFunction aRcvrFunction);
   static WsfEW_Effect::SystemFunction ConvertToEW_SystemFunction(WsfEM_Xmtr::XmtrFunction aXmtrFunction);
   //@}

   //! @name Coherency Type methods.
   //@{

   //! Set the coherency type of the EW effect.
   //! @param aCoherency The coherency of the EW effect exhibits.
   void SetCoherencyMask(Coherency aCoherency) { mCoherencyMask = aCoherency; }

   //! Add the coherency type of the EW effect.
   //! @param aCoherency The coherency of the EW effect exhibits.
   void AddCoherencyMask(Coherency aCoherency) { mCoherencyMask |= aCoherency; }

   //! Get the coherency of the EW Effect.
   //! @return Returns the coherency type this EW effect exhibits.
   unsigned int GetCoherencyMask() const { return mCoherencyMask; }

   static unsigned int ConvertCoherencyIdToMask(WsfStringId aCoherencyId);
   //@}


   //! @name Behavior methods.
   //@{

   //! Set the behavior of the EW effect.
   //! @param aBehavior The behavior of the EW effect exhibits.
   void SetBehavior(Behavior aBehavior) { mBehavior = aBehavior; }

   //! Get the behavior of the EW Effect.
   //! @return Returns the behavior this EW effect exhibits.
   Behavior GetBehavior() const { return mBehavior; }
   //@}

   //! @name Debug methods.
   //@{

   //! Set the debug flag state.
   //! @param aDebug The debug flag state, 'true' or 'false'.
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   //! Get the debug flag state.
   //! @return Returns the debug flag state, 'true' or 'false'.
   bool DebugEnabled() const { return mDebug; }
   //@}

   //! @name Class pointer methods.
   //@{

   //! Set the Electronic Attack pointer, set to NULL by default.
   //! @param aPtr The pointer to the associated EA class.
   void SetEA_EP_Ptr(WsfEW_EA_EP* aPtr) { mEA_EP_Ptr = aPtr; }

   //! Get the Electronic Attack / Electronic Protect pointer, returns NULL if not set.
   //! @return Returns the associated EA pointer.
   WsfEW_EA_EP* GetEA_EP_Ptr() const { return mEA_EP_Ptr; }

   //! Get the Electronic Attack pointer, returns NULL  if not set or if N/A.
   //! @return Returns the associated EA pointer.
   WsfEW_EA* GetEA_Ptr() const;

   //! Get the Electronic Protect pointer, returns NULL if not set or if N/A.
   //! @return Returns the associated EP pointer.
   WsfEW_EP* GetEP_Ptr() const;

   //! Set the technique pointer associated with this effect.
   //! @return Returns the technique pointer associated with this effect.
   WsfEW_Technique* GetTechniquePtr() const { return mTechniquePtr; }

   //! Set the technique pointer associated with this effect.
   //! @param aPtr The technique pointer associated with this effect.
   void SetTechniquePtr(WsfEW_Technique* aPtr) { mTechniquePtr = aPtr; }
   //@}

   //! Get the current jamming pulse density.
   //! @return The jamming pulse density, not set if it is <= 0.0.
   virtual double GetJammingPulseDensity() const { return 1.0; }
   virtual double GetJammingPulseDensity(WsfEM_Xmtr* aXmtrPtr) const { return 1.0; }

   virtual bool AllowTarget(WsfPlatform* aXmtrPlatform, WsfPlatform* aTargetPlatform) const;

   //! @name Current Id methods.
   //@{

   virtual void UpdateCurrentIds(WsfEM_Rcvr* aRcvrPtr, WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTargetPtr);
   //@}

   WsfSimulation* GetSimulation() const;

   ut::Random& GetRandom() { return mRandom; }

protected:
   //! Base effect data used to derive from in inheriting effect classes.
   class WSF_MIL_EXPORT EffectData : public WsfEW_Types::EW_EffectData
   {
   public:
      EffectData()        = default;
      EffectData& operator=(const EffectData&) = delete;
      ~EffectData() override                   = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      WsfEW_Effect* GetEffectPtr() { return mEffectPtr; }
      void          SetEffectPtr(WsfEW_Effect* aEffectPtr) { mEffectPtr = aEffectPtr; }

   protected:
      EffectData(const EffectData& aSrc);

   private:
      WsfEW_Effect* mEffectPtr{nullptr}; //!< Pointer to the owning effect class.
   };

   WsfEW_Effect(const WsfEW_Effect& aSrc);

   //! Print the update message when in debug mode for the given type.
   //! @param aStream       Where to write to.
   //! @param aSimTime      Current simulation time.
   //! @param aEW_EffectPtr Effect pointer for the currently opposing/calculating technique if applicable.
   virtual void PrintUpdateMessage(ut::log::MessageStream& aStream, double aSimTime, WsfEW_Effect* aEW_EffectPtr = nullptr);

   void ResetProtectionTypeFlag(WsfEM_Xmtr* aXmtrPtr, const WsfEM_Interaction& aTargetInteraction, WsfEW_Effects& aEW_Effects);

   //! @name Effect Data methods.
   //@{
   bool                ProcessEffectDataBlock(UtInputBlock& aInputBlock, WsfStringId aSystemTypeId);
   virtual EffectData& GetEffectData(WsfStringId aSystemTypeId);
   void                ResetEffectData(WsfStringId aSystemTypeId){};
   bool                EffectDataExists(WsfStringId aSystemTypeId) const;
   virtual EffectData* NewEffectData() { return new EffectData(); }
   void                InsertNewEffectData(WsfStringId aSystemTypeId, WsfEW_Types::EW_EffectData* aEffectPtr);

   //! Propagate the new effect data pointer to the other classes for local casting.
   //! @note Assumes that the base effect data map has the correct pointer
   //! @param aSystemTypeId The system type id to propagate the pointer for.
   virtual EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId);
   //@}


   unsigned int mSystemFunctionMask{cES_UNDEFINED}; //!< The system type mask that this EW effect can affect.
   Behavior     mBehavior{cEB_UNDEFINED};           //!< The behavior that this EW effect exhibits.
   unsigned int mCoherencyMask{cEC_NONE};           //!< The coherency of this effect wrt to the affected system.
   bool         mDebug{false};                      //!< The debug flag, 'true' for debug, false otherwise.

   // The below values change after each call to UpdateEffects and anytime the UpdateCurrentIds is called.
   WsfStringId mCurrentTargetId{nullptr};       //!< Current target Id.
   WsfStringId mCurrentXmtrPlatformId{nullptr}; //!< Current transmitting platform Id.
   WsfStringId mCurrentRcvrPlatformId{nullptr}; //!< Current receiving platform Id.
   int         mCurrentXmtrUniqueId{0};         //!< Current transmitter unique Id.
   int         mCurrentRcvrUniqueId{0};         //!< Current receiver unique Id.

   //! The type Id of the system being effected, changes with each call to UpdateEffects(...) or UpdateCurrentIds(...)
   WsfStringId mEffectedSystemId{nullptr};

   //! The type Id of the system doing the effecting, changes with each call to UpdateEffects(...) or UpdateCurrentIds(...)
   WsfStringId mEffectingSystemId{nullptr};

   //! Variable to prevent infinite recursion in UpdateEffect.
   bool mUpdatingEffect{true}; // set to true initially to prevent updates before initialization

   using EffectDataMap = std::map<WsfStringId, EffectData*>; // Main effect data map.

   //! Map of the System Type Id to the data required for this effect.
   EffectDataMap mEffectDataMap;

private:
   using TargetAllowRejectSet = std::set<WsfStringId>;

   WsfEW_Technique* mTechniquePtr{nullptr}; //!< Associated technique pointer.
   WsfEW_EA_EP*     mEA_EP_Ptr{nullptr};    //!< Associated EA/EP pointer.

   TargetProtectionType mProtectionType{
      cTP_ALL}; //!< Flag to indicate the type of EA protection this effect exhibits for a given target.
   TargetAllowRejectSet mAllowedTargetSet;  //!< Set of allowed target name/type IDs.
   TargetAllowRejectSet mRejectedTargetSet; //!< Set of rejected target name/type IDs.

   ut::Random mRandom;
};

#endif
