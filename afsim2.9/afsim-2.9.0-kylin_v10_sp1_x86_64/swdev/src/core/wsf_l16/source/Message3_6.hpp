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
#ifndef L16_MESSAGE3_6_HPP
#define L16_MESSAGE3_6_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_6
{
// Continuation1,2,3,4,5 not yet defined.

class VelocityField : public NormalField<14, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField&                          operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)(MetersToFeet(aVal) / 3.33));
      return *this;
   }
   operator double() const { return FeetToMeters(3.33 * DecodeTC(mValue)); }
};

class PositionField : public NormalField<23, NoStatementMid>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   PositionField&                 operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)(MetersToFeet(aVal) / 10.0));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue) * 10.0); }
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 6, 0);

   typedef PositionField                      FieldType1;
   typedef VelocityField                      FieldType2;
   typedef PositionField                      FieldType3;
   typedef IntegerFieldNS<5, NoStatementZero> FieldType4;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType5;
   DEFINE_MEMBERS5(WcsX, WcsVelX, WcsY, SpaceAmplify, AmplifyConfidence);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 6, 1);

   typedef VelocityField                      FieldType1;
   typedef PositionField                      FieldType2;
   typedef VelocityField                      FieldType3;
   typedef BooleanField                       FieldType4;
   typedef BooleanField                       FieldType5;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType6;
   typedef SpareField<12>                     FieldType7;
   DEFINE_MEMBERS7(WcsVelY, WcsZ, WcsVelZ, IsTrackLost, IsBoosting, DataIndicator, Spare);
};

class Extension2 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 6, 2);
   typedef DisSiteField        FieldType1;
   typedef DisApplicationField FieldType2;
   typedef DisEntityIdField    FieldType3;
   typedef SpareField<20>      FieldType4;

   DEFINE_MEMBERS4(DisSite, DisApplication, DisEntityId, Spare);
};

// Note: This is just the outline of the message.  All fields are integers; need to expand
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 6, 1);
   typedef IntegerField<10> FieldType1;
   typedef IntegerField<10> FieldType2;
   typedef IntegerField<10> FieldType3;
   typedef IntegerField<10> FieldType4;
   typedef IntegerField<10> FieldType5;
   typedef IntegerField<1>  FieldType6;
   typedef IntegerField<10> FieldType7;
   typedef IntegerField<1>  FieldType8;
   typedef IntegerField<1>  FieldType9;

   DEFINE_MEMBERS9(SigmaX_Position,
                   SigmaY_Position,
                   SigmaZ_Position,
                   CovarianceElement22,
                   CovarianceElement33,
                   Element33Sign,
                   AbsoluteValueOfCovariance,
                   SignOfCovariance12,
                   SignOfCovariance13);
};


class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 6);

   typedef BooleanField                       FieldType1;
   typedef BooleanField                       FieldType2;
   typedef BooleanField                       FieldType3;
   typedef BooleanField                       FieldType4;
   typedef BooleanField                       FieldType5;
   typedef TrackNumberField                   FieldType6;
   typedef MinuteField                        FieldType7;
   typedef SecondField                        FieldType8;
   typedef TrackQualityField1                 FieldType9;
   typedef IdentityField                      FieldType10;
   typedef BooleanField                       FieldType11;
   typedef IntegerFieldNS<6, NoStatementZero> FieldType12;
   typedef IntegerFieldNS<7, NoStatementZero> FieldType13;
   // typedef IntegerFieldNS<12, NoStatementZero> FieldType12;
   // typedef SpareField<1>                       FieldType13;

   DEFINE_MEMBERS13(IsExerciseTrackUnit,
                    IsForceTell,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    IsSpecificType,
                    TrackNumber,
                    Minute,
                    Second,
                    TrackQuality,
                    Identity,
                    IsIdentityDifferent,
                    SpacePlatform,
                    SpaceActivity); // SpaceSpecificType, Spare);
   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS3();
};

} // namespace J3_6
} // namespace Messages
} // namespace WsfL16


#endif
