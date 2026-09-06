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
#ifndef L16_MESSAGE7_3_HPP
#define L16_MESSAGE7_3_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J7_3
{

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(7, 3, 1)
   typedef TrackNumberAddresseeField FieldType1;
   typedef TrackNumberAddresseeField FieldType2;
   typedef TrackNumberAddresseeField FieldType3;
   typedef TrackNumberAddresseeField FieldType4;
   typedef SpareField<3>             FieldType5;

   DEFINE_MEMBERS5(Addressee1, Addressee2, Addressee3, Addressee4, Spare);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(7, 3, 2)
   typedef LetterNumberField FieldType1;
   typedef LetterNumberField FieldType2;
   typedef LetterNumberField FieldType3;
   typedef LetterNumberField FieldType4;
   typedef LetterNumberField FieldType5;
   typedef LetterNumberField FieldType6;
   typedef LetterNumberField FieldType7;
   typedef LetterNumberField FieldType8;
   typedef LetterNumberField FieldType9;
   typedef LetterNumberField FieldType10;
   typedef SpareField<3>     FieldType11;

   DEFINE_MEMBERS11(Char1, Char2, Char3, Char4, Char5, Char6, Char7, Char8, Char9, Char10, Spare);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(7, 3, 3)
   typedef LetterNumberField FieldType1;
   typedef LetterNumberField FieldType2;
   typedef LetterNumberField FieldType3;
   typedef LetterNumberField FieldType4;
   typedef LetterNumberField FieldType5;
   typedef LetterNumberField FieldType6;
   typedef LetterNumberField FieldType7;
   typedef LetterNumberField FieldType8;
   typedef LetterNumberField FieldType9;
   typedef LetterNumberField FieldType10;
   typedef SpareField<3>     FieldType11;

   DEFINE_MEMBERS11(Char11, Char12, Char13, Char14, Char15, Char16, Char17, Char18, Char19, Char20, Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(7, 3)
   typedef ActionPointerField        FieldType1;
   typedef TextIndicatorField        FieldType2;
   typedef LatitudeField<18>         FieldType3;
   typedef LongitudeField<19>        FieldType4;
   typedef TrackNumberAddresseeField FieldType5;

   DEFINE_MEMBERS5(Action, TextIndicator, Latitude, Longitude, TrackNumber);

   DEFINE_CONTINUATIONS3(1, 2, 3)
   DEFINE_EXTENSIONS0()
};

} // namespace J7_3
} // namespace Messages
} // namespace WsfL16


#endif
