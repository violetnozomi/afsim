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

#ifndef WSFCOMMMEDIUMUNGUIDED_HPP
#define WSFCOMMMEDIUMUNGUIDED_HPP

#include "wsf_export.h"

#include "UtTable.hpp"
#include "WsfCommMediumGuided.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"

namespace wsf
{
namespace comm
{

class Result;

namespace medium
{

//! A mode implementation for common legacy unguided usage.
class WSF_EXPORT ModeUnguided : public ModeGuided
{
public:
   ModeUnguided() = default;
   ModeUnguided(const std::string& aName)
      : ModeGuided(aName)
   {
   }
   ~ModeUnguided() override               = default;
   ModeUnguided(const ModeUnguided& aSrc) = default;
   ModeUnguided* Clone() const override { return new ModeUnguided(*this); }

   const char* GetScriptClassName() const override { return "WsfCommMediumModeUnguided"; }

   bool   ProcessInput(UtInput& aInput) override;
   double GetTransmissionTime(double aSimTime, Comm& aXmtr, Layer* aLayerPtr, Message& aMessage) override;

   bool   GetUseXferRateTable() const { return mUseSNR_XferRateTable; }
   double GetBitErrorProbability() const { return mBitErrorProbability; }
   double GetErrorCorrection() const { return mErrorCorrection; }
   bool   GetUseBER_EbNoTable() const { return mUseBER_EbNoTable; }

protected:
   virtual double CalculateTransferRate(Result& aResult, bool aCanSend);

   //! A table that can be used to provide the message transfer rate based off of the SNR
   //! between the transmitter and receiver using this medium.
   UtTable::Curve mSNR_XferRateTable{};

   //! When set to true, the medium will utilize the mSNR_XferRateTable to determine transfer rate.
   bool mUseSNR_XferRateTable{false};

   //! The expectation value of the BER (bit error rate) in a message, entered as percentage.
   double mBitErrorProbability{0.0};

   //! The value of the Forward Error Correction (FEC) applied to a message to adjust BER and Eb/No, entered as a ratio (dB).
   double mErrorCorrection{1.0};

   //! A table that defines the value of Eb/No versus the Bit Error Rate.
   UtTable::Curve mBER_EbNoTable{};

   //! When set to true, the medium will use the mBER_EbNoTable to determine the message transfer rate.
   bool mUseBER_EbNoTable{false};
};

//! The unguided mode script class.
class WSF_EXPORT ScriptMediumModeUnguidedClass : public ScriptMediumModeGuidedClass
{
public:
   ScriptMediumModeUnguidedClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~ScriptMediumModeUnguidedClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetUseXferRateTable);
   UT_DECLARE_SCRIPT_METHOD(GetBitErrorProbability);
   UT_DECLARE_SCRIPT_METHOD(GetErrorCorrection);
   UT_DECLARE_SCRIPT_METHOD(GetUseBER_EbNoTable);
};

//! This medium extends the base guided medium capabilities to support comm models utilizing
//! unguided mediums. This only provides a baseline for the API expected for legacy support.
//! This should serve as a base for those objects utilizing mediums of this nature that require
//! support for legacy input.
class WSF_EXPORT Unguided : public Guided
{
public:
   static void RegisterFactory(Types& aTypes);

   Unguided(WsfScenario& aScenario);
   ~Unguided() override = default;

   const char*    GetScriptClassName() const override { return "WsfMediumUnguided"; }
   TypeIdentifier GetMediumIdentifier() const override { return TypeIdentifier::cUNGUIDED; }
   Unguided*      Clone() const override;
   bool           ProcessInput(UtInput& aInput) override;

   std::unique_ptr<Mode> CreateDefaultMode() const override { return ut::make_unique<ModeUnguided>(); }

   TransmissionResult TransmitMessage(double aSimTime, Layer* aLayerPtr, Message& aMessage, Comm& aXmtr) override;

   using Guided::EndTransmission;
   void EndTransmission(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier) override;

   void EndPropagation(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier) override;
};

WSF_COMM_MEDIUM_DECLARE_ROLE_TYPE(Unguided, cUNGUIDED);

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
