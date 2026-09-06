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
#ifndef L16_MESSAGESIZECHECKER_HPP
#define L16_MESSAGESIZECHECKER_HPP

#include "FieldTypes.hpp"

//! The following classes ensure the message is the correct size
//! at COMPILE time.  You will get a compile error if the message
//! isn't the right size.  In a good compiler (vc++8), it will give
//! you a total message size, correct message size,
//! and all of the separate fields which make up the message.
namespace WsfL16
{
namespace Messages
{
template<class T>
struct FieldSizeCheck
{
   static const int FieldSize = T::cFIELD_SIZE;
};

template<>
struct FieldSizeCheck<bool>
{
   static const int FieldSize = 0;
};

//! Here we pass in the field sizes just so the compiler
//! will display them.
template<unsigned _MESSAGE_SIZE_,
         unsigned _GOOD_SIZE_,
         unsigned FieldSize1,
         unsigned FieldSize2,
         unsigned FieldSize3,
         unsigned FieldSize4,
         unsigned FieldSize5,
         unsigned FieldSize6,
         unsigned FieldSize7,
         unsigned FieldSize8,
         unsigned FieldSize9,
         unsigned FieldSize10,
         unsigned FieldSize11,
         unsigned FieldSize12,
         unsigned FieldSize13,
         unsigned FieldSize14,
         unsigned FieldSize15,
         unsigned FieldSize16,
         unsigned FieldSize17,
         unsigned FieldSize18,
         unsigned FieldSize19,
         unsigned FieldSize20,
         unsigned FieldSize21,
         unsigned FieldSize22,
         unsigned FieldSize23,
         unsigned FieldSize24,
         unsigned FieldSize25,
         unsigned FieldSize26,
         unsigned FieldSize27,
         unsigned FieldSize28,
         unsigned FieldSize29,
         unsigned FieldSize30,
         unsigned FieldSize31,
         unsigned FieldSize32>
struct MESSAGE_SIZE_NOT_VALID;

template<unsigned OK,
         unsigned FieldSize1,
         unsigned FieldSize2,
         unsigned FieldSize3,
         unsigned FieldSize4,
         unsigned FieldSize5,
         unsigned FieldSize6,
         unsigned FieldSize7,
         unsigned FieldSize8,
         unsigned FieldSize9,
         unsigned FieldSize10,
         unsigned FieldSize11,
         unsigned FieldSize12,
         unsigned FieldSize13,
         unsigned FieldSize14,
         unsigned FieldSize15,
         unsigned FieldSize16,
         unsigned FieldSize17,
         unsigned FieldSize18,
         unsigned FieldSize19,
         unsigned FieldSize20,
         unsigned FieldSize21,
         unsigned FieldSize22,
         unsigned FieldSize23,
         unsigned FieldSize24,
         unsigned FieldSize25,
         unsigned FieldSize26,
         unsigned FieldSize27,
         unsigned FieldSize28,
         unsigned FieldSize29,
         unsigned FieldSize30,
         unsigned FieldSize31,
         unsigned FieldSize32>
struct MESSAGE_SIZE_NOT_VALID<OK,
                              OK,
                              FieldSize1,
                              FieldSize2,
                              FieldSize3,
                              FieldSize4,
                              FieldSize5,
                              FieldSize6,
                              FieldSize7,
                              FieldSize8,
                              FieldSize9,
                              FieldSize10,
                              FieldSize11,
                              FieldSize12,
                              FieldSize13,
                              FieldSize14,
                              FieldSize15,
                              FieldSize16,
                              FieldSize17,
                              FieldSize18,
                              FieldSize19,
                              FieldSize20,
                              FieldSize21,
                              FieldSize22,
                              FieldSize23,
                              FieldSize24,
                              FieldSize25,
                              FieldSize26,
                              FieldSize27,
                              FieldSize28,
                              FieldSize29,
                              FieldSize30,
                              FieldSize31,
                              FieldSize32>
{
};

template<unsigned GOOD_SIZE,
         class c1,
         class c2  = bool,
         class c3  = bool,
         class c4  = bool,
         class c5  = bool,
         class c6  = bool,
         class c7  = bool,
         class c8  = bool,
         class c9  = bool,
         class c10 = bool,
         class c11 = bool,
         class c12 = bool,
         class c13 = bool,
         class c14 = bool,
         class c15 = bool,
         class c16 = bool,
         class c17 = bool,
         class c18 = bool,
         class c19 = bool,
         class c20 = bool,
         class c21 = bool,
         class c22 = bool,
         class c23 = bool,
         class c24 = bool,
         class c25 = bool,
         class c26 = bool,
         class c27 = bool,
         class c28 = bool,
         class c29 = bool,
         class c30 = bool,
         class c31 = bool,
         class c32 = bool>
struct MessageSizeCheck
{
   static const int error_message =
      sizeof(MESSAGE_SIZE_NOT_VALID<
             FieldSizeCheck<c1>::FieldSize + FieldSizeCheck<c2>::FieldSize + FieldSizeCheck<c3>::FieldSize +
                FieldSizeCheck<c4>::FieldSize + FieldSizeCheck<c5>::FieldSize + FieldSizeCheck<c6>::FieldSize +
                FieldSizeCheck<c7>::FieldSize + FieldSizeCheck<c8>::FieldSize + FieldSizeCheck<c9>::FieldSize +
                FieldSizeCheck<c10>::FieldSize + FieldSizeCheck<c11>::FieldSize + FieldSizeCheck<c12>::FieldSize +
                FieldSizeCheck<c13>::FieldSize + FieldSizeCheck<c14>::FieldSize + FieldSizeCheck<c15>::FieldSize +
                FieldSizeCheck<c16>::FieldSize + FieldSizeCheck<c17>::FieldSize + FieldSizeCheck<c18>::FieldSize +
                FieldSizeCheck<c19>::FieldSize + FieldSizeCheck<c20>::FieldSize + FieldSizeCheck<c21>::FieldSize +
                FieldSizeCheck<c22>::FieldSize + FieldSizeCheck<c23>::FieldSize + FieldSizeCheck<c24>::FieldSize +
                FieldSizeCheck<c25>::FieldSize + FieldSizeCheck<c26>::FieldSize + FieldSizeCheck<c27>::FieldSize +
                FieldSizeCheck<c28>::FieldSize + FieldSizeCheck<c29>::FieldSize + FieldSizeCheck<c30>::FieldSize +
                FieldSizeCheck<c31>::FieldSize + FieldSizeCheck<c32>::FieldSize,
             GOOD_SIZE,
             FieldSizeCheck<c1>::FieldSize,
             FieldSizeCheck<c2>::FieldSize,
             FieldSizeCheck<c3>::FieldSize,
             FieldSizeCheck<c4>::FieldSize,
             FieldSizeCheck<c5>::FieldSize,
             FieldSizeCheck<c6>::FieldSize,
             FieldSizeCheck<c7>::FieldSize,
             FieldSizeCheck<c8>::FieldSize,
             FieldSizeCheck<c9>::FieldSize,
             FieldSizeCheck<c10>::FieldSize,
             FieldSizeCheck<c11>::FieldSize,
             FieldSizeCheck<c12>::FieldSize,
             FieldSizeCheck<c13>::FieldSize,
             FieldSizeCheck<c14>::FieldSize,
             FieldSizeCheck<c15>::FieldSize,
             FieldSizeCheck<c16>::FieldSize,
             FieldSizeCheck<c17>::FieldSize,
             FieldSizeCheck<c18>::FieldSize,
             FieldSizeCheck<c19>::FieldSize,
             FieldSizeCheck<c20>::FieldSize,
             FieldSizeCheck<c21>::FieldSize,
             FieldSizeCheck<c22>::FieldSize,
             FieldSizeCheck<c23>::FieldSize,
             FieldSizeCheck<c24>::FieldSize,
             FieldSizeCheck<c25>::FieldSize,
             FieldSizeCheck<c26>::FieldSize,
             FieldSizeCheck<c27>::FieldSize,
             FieldSizeCheck<c28>::FieldSize,
             FieldSizeCheck<c29>::FieldSize,
             FieldSizeCheck<c30>::FieldSize,
             FieldSizeCheck<c31>::FieldSize,
             FieldSizeCheck<c32>::FieldSize>);
};
} // namespace Messages
} // namespace WsfL16

#endif
