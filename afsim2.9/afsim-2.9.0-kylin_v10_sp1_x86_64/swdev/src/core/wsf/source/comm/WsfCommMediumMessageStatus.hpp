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

#ifndef WSFCOMMMEDIUMMESSAGESTATUS_HPP
#define WSFCOMMMEDIUMMESSAGESTATUS_HPP

#include "wsf_export.h"

#include "UtScriptAccessible.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommMessage.hpp"
#include "script/WsfScriptAuxDataUtil.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

//! The message status class is a container object used for storing data
//! related to message state when being transmitted over a medium. The
//! container may contain aux data for customization.
class WSF_EXPORT MessageStatus : public UtScriptAccessible, public WsfAuxDataEnabled
{
public:
   MessageStatus(double aSimTime, Message& aMessage, size_t aMode)
      : mMessage(aMessage)
      , mMode(aMode)
      , mTimeStart(aSimTime)
      , mTimeTransmissionEnd(aSimTime)
   {
   }

   ~MessageStatus() override                = default;
   MessageStatus(const MessageStatus& aSrc) = default;

   const char* GetScriptClassName() const override { return "WsfCommMediumMessageStatus"; }

   void SetMessage(const Message& aMessage) { mMessage = aMessage; }
   void SetMode(size_t aMode) { mMode = aMode; }
   void SetLayer(Layer* aLayerPtr) { mLayerPtr = aLayerPtr; }
   void SetIsTransmitting(bool aIsTransmitting) { mTransmitting = aIsTransmitting; }
   void SetIsScheduledDelivery(bool aIsScheduledDelivery) { mScheduledDelivery = aIsScheduledDelivery; }
   void SetTimeStart(double aSimTime) { mTimeStart = aSimTime; }
   void SetTimeTransmissionEnd(double aSimTime) { mTimeTransmissionEnd = aSimTime; }
   void SetTimeDelivery(double aSimTime) { mTimeDelivery = aSimTime; }
   void SetAbortDelivery(bool aAbort) { mAbortDelivery = aAbort; }
   void SetTimeLastXmtrStatusChange(double aSimTime) { mXmtrLastStatusChange = aSimTime; }
   void SetStatusIssued(bool aIssued) { mStatusIssued = aIssued; }

   const Message& GetMessage() const { return mMessage; }
   Message&       GetMessage() { return mMessage; }
   size_t         GetMode() const { return mMode; }
   Layer*         GetLayer() const { return mLayerPtr; }
   bool           IsTransmitting() const { return mTransmitting; }
   bool           IsScheduledDelivery() const { return mScheduledDelivery; }
   double         GetTimeStart() const { return mTimeStart; }
   double         GetTimeTransmissionEnd() const { return mTimeTransmissionEnd; }
   double         GetTimeDelivery() const { return mTimeDelivery; }
   bool           AbortDelivery() const { return mAbortDelivery; }
   double         GetTimeLastXmtrStatusChange() const { return mXmtrLastStatusChange; }
   bool           IsStatusIssued() const { return mStatusIssued; }

private:
   //! The message.
   Message mMessage;

   //! The medium mode associated with this transmission request.
   size_t mMode;

   //! The layer on the transmitter than initiated the send request over
   //! the medium, if applicable.
   Layer* mLayerPtr{nullptr};

   //! Indicates if the message is currently being actively transmitted
   //! by a comm object. Propagation is always occurring while this object
   //! exists. Should be set by call to Medium::EndTransmission().
   bool mTransmitting{true};

   //! Indicates the message has a scheduled message delivery event on a
   //! receiving comm object.
   //! Should be set by call to Medium::PropagateMessage().
   bool mScheduledDelivery{false};

   //! Indicates the time this message began transmitting and propagating.
   double mTimeStart;

   //! Indicates the time this message ceased/will cease transmitting.
   //! Should be set by call to Medium::TransmitMessage().
   //! Initially set to sim time for instantaneous propagation.
   double mTimeTransmissionEnd;

   //! Indicates the time this message will be delivered.
   //! Should be set initially by call to Medium::TransmitMessage().
   double mTimeDelivery{0.0};

   //! Indicates the message is to be aborted and not delivered. If set to true,
   //! the message delivery event will short circuit delivery.
   //! Changes should be made by call to Medium::TerminateTransmission() with
   //! abort parameter set to true.
   bool mAbortDelivery{false};

   //! Indicates the last status update time for the transmitter.
   //! Should be set on call to Medium::TransmitMessage().
   //! Ensures that any updates occurring on the transmitter during transmission
   //! (such as being turned off) are handled.
   double mXmtrLastStatusChange{0.0};

   //! A control variable for correct programmatic handling of pending messages.
   //! Messages may be handled instantaneously or via a time delay with usage
   //! of scheduled simulation events to handle the correct timing. This variable
   //! indicates whether or not feedback has been provided to the utilizing medium
   //! parent on the status of a transmission for proper handling. Query of this
   //! variable indicates whether layer messages need to be provided for correct
   //! functionality.
   bool mStatusIssued{false};
};

class WSF_EXPORT ScriptMediumMessageStatusClass : public UtScriptClass
{
public:
   ScriptMediumMessageStatusClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~ScriptMediumMessageStatusClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetMessage);
   UT_DECLARE_SCRIPT_METHOD(IsTransmitting);
   UT_DECLARE_SCRIPT_METHOD(GetTimeStart);
   UT_DECLARE_SCRIPT_METHOD(GetTimeTransmissionEnd);
   UT_DECLARE_SCRIPT_METHOD(GetTimeDelivery);
   UT_DECLARE_SCRIPT_METHOD(GetDeliveryFailure);

   UT_DECLARE_SCRIPT_METHOD(SetDeliveryFailure);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
