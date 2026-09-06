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

#ifndef WSFCYBERMITMMESSAGE_HPP
#define WSFCYBERMITMMESSAGE_HPP

#include "wsf_cyber_export.h"

#include <memory>

#include "UtScriptBasicTypes.hpp"
#include "WsfCommMessage.hpp"

namespace wsf
{
namespace cyber
{

class WSF_CYBER_EXPORT MitmMessage : public UtScriptAccessible, public UtReferenceTracked
{
public:
   // A public nested struct for additional information usage
   // via the MitmMessage, for use when a delay is indicated.
   struct DelayParameters
   {
      std::string         mPlatformName{};
      std::string         mCommName{};
      size_t              mMITM_LayerID{0U};
      std::vector<size_t> mNextLayerIDs{};
      bool                mIsSending{true};
   };

   MitmMessage(std::unique_ptr<comm::Message> aMessage, double aDelay, bool aDrop);

   ~MitmMessage() override = default;

   virtual MitmMessage* Clone() const;

   const char* GetScriptClassName() const override;
#undef GetMessage
   comm::Message* GetMessage() const;
   double         GetDelay() const;
   bool           GetDrop() const;

   void SetMessage(std::unique_ptr<comm::Message> aMessage);
   void SetDelay(double aDelay);
   void SetDrop(bool aDrop);

   const DelayParameters& GetDelayParameters() const { return mDelayParameters; }
   void                   SetDelayParameters(const DelayParameters& aParams) { mDelayParameters = aParams; }

private:
   std::unique_ptr<comm::Message> mMessage;
   double                         mDelay;
   bool                           mDrop;
   DelayParameters                mDelayParameters;
};

class WSF_CYBER_EXPORT ScriptMitmMessage : public UtScriptClass
{
public:
   ScriptMitmMessage(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptMitmMessage() override;

   // See the base class for a description of Create, Clone, and Destroy.
   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Each of the exported methods are defined as function objects.
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(Message);
   UT_DECLARE_SCRIPT_METHOD(Delay);
   UT_DECLARE_SCRIPT_METHOD(Drop);
   UT_DECLARE_SCRIPT_METHOD(SetMessage);
   UT_DECLARE_SCRIPT_METHOD(SetDelay);
   UT_DECLARE_SCRIPT_METHOD(SetDrop);
};

} // namespace cyber
} // namespace wsf
#endif
