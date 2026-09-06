// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERMITMLAYER_HPP
#define WSFCYBERMITMLAYER_HPP

#include <functional>
#include <memory>

#include "UtOptional.hpp"
#include "WsfComm.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCyberMitmMessage.hpp"
#include "WsfMessage.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{

//! The MITM_Layer enables the indicated functionality of the cyber MITM effect
//! via usage as a typical communications layer. This layer can be injected
//! on any comm's protocol stack (assuming comm usage of the protocol stack).
//! The base class is intended usage of distinct send and receive
//! layers. This is done to ensure compatibility with legacy functionality regarding
//! the preservation of ordering of multiple potential MITM effects on a single comm,
//! as the layers are encountered in different orders depending on a receive or send.
class MITM_Layer : public comm::LayerImp
{
public:
   MITM_Layer(std::shared_ptr<WsfScriptContext> aContextPtr,
              UtScript*                         aOnSendReceivePtr,
              bool                              aIsSend,
              ut::optional<size_t>              aExfiltrateToPlatform = ut::nullopt)
      : mContextPtr(aContextPtr)
      , mOnSendReceivePtr(aOnSendReceivePtr)
      , mIsSend(aIsSend)
      , mExfiltrateToPlatform(aExfiltrateToPlatform)
   {
   }

   ~MITM_Layer() override = default;

   MITM_Layer(const MITM_Layer& aSrc) = default;
   MITM_Layer(MITM_Layer&& aSrc)      = default;
   MITM_Layer& operator=(const MITM_Layer& aRhs) = default;
   MITM_Layer& operator=(MITM_Layer&& aRhs) = default;

   MITM_Layer* Clone() const override;

   bool Initialize(double /*aSimTime*/) override { return true; }
   bool ProcessInput(UtInput& /*aInput*/) override { return false; }
   bool IsSend() const { return mIsSend; }

   bool ProcessLayerMessage(double aSimTime, comm::layer::Message aLayerMessage, comm::Message* aCommMessagePtr) override;

   bool Send(double aSimTime, size_t aLayerIndex, comm::Message& aMessage, bool& aNotifyNextLayer) override;

   bool Receive(double aSimTime, comm::Comm* aXmtrPtr, comm::Message& aMessage, bool& aNotifyNextLayer) override;

   static void ResolveDelay(double                       aSimTime,
                            comm::LayerImp&              aLayer,
                            std::unique_ptr<MitmMessage> aMessage,
                            comm::Comm*                  aXmtrPtr = nullptr);

protected:
   void Exfiltrate(double aSimTime, const comm::Message& aMessage);

   std::shared_ptr<WsfScriptContext> mContextPtr{nullptr};
   UtScript*                         mOnSendReceivePtr{nullptr};
   bool                              mIsSend{true};
   ut::optional<size_t>              mExfiltrateToPlatform{ut::nullopt};
   ut::optional<wsf::comm::Address>  mExfiltrateToAddress{ut::nullopt};
};

} // namespace cyber
} // namespace wsf

#endif
