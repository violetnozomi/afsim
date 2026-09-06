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
#ifndef L16_MESSAGE12_1_HPP
#define L16_MESSAGE12_1_HPP
#include "MessageDefines.hpp"


namespace WsfL16
{
namespace Messages
{
//! The J12.1 Vector message is used by C2 JUs to send vector information and
//! vector discretes specifically to air units operating on its net. Vectors
//! are given for interception of air targets, navigation, and air traffic
//! control.
namespace J12_1
{

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 1, 0)
   typedef TimeSecondsField       FieldType1;
   typedef SpareField<25>         FieldType2; // we cant represent fields larger
   typedef SpareField<26>         FieldType3; // than 32 bits.  so split it up.
   typedef ReceiptComplianceField FieldType4;
   typedef RecurrenceRateField    FieldType5;
   DEFINE_MEMBERS5(SecondsToIntercept, Spare, Spare2, ReceiptCompliance, RecurrenceRate);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 1);

   typedef TrackNumberAddresseeField FieldType1;
   typedef AngleField                FieldType2;
   typedef SpareField<3>             FieldType3;
   typedef AltitudeField<13>         FieldType4;
   typedef VectorField               FieldType5;
   typedef SpeedField                FieldType6;

   DEFINE_MEMBERS6(TrackNumber, Course, Spare, Altitude, VectorType, Speed);

   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J12_1
} // namespace Messages
} // namespace WsfL16

#endif
