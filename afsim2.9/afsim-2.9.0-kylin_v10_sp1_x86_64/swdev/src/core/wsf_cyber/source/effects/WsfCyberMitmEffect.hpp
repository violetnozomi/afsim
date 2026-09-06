// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERMITMEFFECT_HPP
#define WSFCYBERMITMEFFECT_HPP

#include <memory>
#include <unordered_map>

#include "WsfCyberEffect.hpp"

class UtInput;
class UtScript;
#include "WsfNamed.hpp"
class WsfScenario;
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{
class Comm;

} // namespace comm

namespace cyber
{

class MitmEffect : public Effect
{
public:
   MitmEffect(WsfScenario& aScenario);
   ~MitmEffect() override = default;

   //! Deleted to prevent use
   MitmEffect& operator=(const MitmEffect& aRhs) = delete;

   //! @name Common infrastructure methods.
   //@{
   //! Clone this object.
   //! @return a pointer to the new object.
   MitmEffect* Clone() const override;

   bool Initialize(Engagement& aEngagement) override;

   bool ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //@{
   //! Apply the effect to the victim.
   //! @param aSimTime The current simulation time.
   //! @param aEngagement Reference to the engagement object.
   void Attack(double aSimTime, Engagement& aEngagement) override;

   //! Restore the victim to its pre-attack state.
   //! This simulates the victim successfully recovering from an attack.
   //! @param aSimTime The current simulation time.
   //! @param aEngagement Reference to the engagement object.
   void Restore(double aSimTime, Engagement& aEngagement) override;
   //@}

protected:
   MitmEffect(const MitmEffect& aSrc);

private:
   class EffectType : public WsfNamed
   {
   public:
      struct CommMITM_Data
      {
         CommMITM_Data(comm::Comm* aCommPtr, size_t aSendLayerID, size_t aReceiveLayerID)
            : mCommPtr(aCommPtr)
            , mSendLayerID(aSendLayerID)
            , mReceiveLayerID(aReceiveLayerID)
         {
         }

         comm::Comm* mCommPtr;
         size_t      mSendLayerID;
         size_t      mReceiveLayerID;
      };

      EffectType() = default;
      EffectType(UtInput& aInput, WsfScriptContext& aContext, const std::string& aName);
      void Attack(double aSimTime, Engagement& aEngagement, const std::shared_ptr<WsfScriptContext>& aContextPtr);
      void Restore(Engagement& aEngagement);

   private:
      //! Returns the index where insertion of the MITM layers should occur.
      //! If the index is equal to the size of the current stack, this indicates
      //! the layer should be added on the end of the stack.
      size_t GetInsertionIndex(const comm::Comm& aComm) const;

      //! Removes injected MITM layers.
      void RemoveMITM_Layers();

      UtScript*                  mOnSend    = nullptr;
      UtScript*                  mOnReceive = nullptr;
      std::vector<std::string>   mComms;
      std::vector<CommMITM_Data> mAttackedComms;
      bool                       mAllComms   = false;
      bool                       mExfiltrate = false;
   };

   class EffectTypeFactory
   {
   public:
      EffectType CreateEffect(const Engagement& aEngagement) const;
      void       AddPlatformTypeEffect(const std::string& aPlatformType, const EffectType& aEffect);
      void       AddPlatformEffect(const std::string& aPlatform, const EffectType& aEffect);
      void       SetDefaultEffect(const EffectType& aEffect);

   private:
      std::unordered_map<std::string, EffectType> mPlatformTypeEffect;
      std::unordered_map<std::string, EffectType> mPlatformEffect;
      EffectType                                  mDefaultEffect;
   };

   std::shared_ptr<WsfScriptContext> mContextPtr;
   EffectTypeFactory                 mEffectFactory;
   std::map<size_t, EffectType>      mActiveEffects;
};

} // namespace cyber
} // namespace wsf

#endif
