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
#ifndef L16_MESSAGE31_7_HPP
#define L16_MESSAGE31_7_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J31_7
{

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(31, 7);

   typedef SpareField<28> FieldType1;
   typedef SpareField<29> FieldType2;

   DEFINE_MEMBERS2(Spare, Spare2);
   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS0()
};

} // namespace J31_7
} // namespace Messages
} // namespace WsfL16


#endif
