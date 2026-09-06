// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_MESSAGE9_1_HPP
#define L16_MESSAGE9_1_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J9_1
{

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(9, 1)

   typedef TrackNumberAddresseeField FieldType1;
   typedef TrackNumberField          FieldType2;
   typedef FireMethodField           FieldType3;
   typedef EngagementActionField     FieldType4;
   typedef ProbabilityOfKillField    FieldType5;
   typedef TrackingStatusField       FieldType6;
   typedef SpareField<2>             FieldType7;
   typedef ReceiptComplianceField    FieldType8;
   typedef RecurrenceRateField       FieldType9;

   DEFINE_MEMBERS9(Addressee,
                   TrackNumber,
                   FireMethod,
                   EngagementAction,
                   ProbabilityOfKill,
                   TrackingStatus,
                   Spare,
                   ReceiptCompliance,
                   RecurrenceRate);

   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS0()
};

} // namespace J9_1
} // namespace Messages
} // namespace WsfL16


#endif
