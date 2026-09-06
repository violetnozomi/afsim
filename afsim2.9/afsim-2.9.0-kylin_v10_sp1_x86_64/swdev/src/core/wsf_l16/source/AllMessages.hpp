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
#ifndef L16_ALLMESSAGES_HPP
#define L16_ALLMESSAGES_HPP

#include "Message10_2.hpp"
#include "Message11_0.hpp"
#include "Message11_1.hpp"
#include "Message11_2.hpp"
#include "Message12_0.hpp"
#include "Message12_1.hpp"
#include "Message12_2.hpp"
#include "Message12_3.hpp"
#include "Message12_4.hpp"
#include "Message12_5.hpp"
#include "Message12_6.hpp"
#include "Message12_7.hpp"
#include "Message13_2.hpp"
#include "Message13_3.hpp"
#include "Message13_4.hpp"
#include "Message13_5.hpp"
#include "Message14_0.hpp"
#include "Message15_0.hpp"
#include "Message2_0.hpp"
#include "Message2_2.hpp"
#include "Message2_3.hpp"
#include "Message2_4.hpp"
#include "Message2_5.hpp"
#include "Message2_6.hpp"
#include "Message31_7.hpp"
#include "Message3_0.hpp"
#include "Message3_1.hpp"
#include "Message3_2.hpp"
#include "Message3_3.hpp"
#include "Message3_4.hpp"
#include "Message3_5.hpp"
#include "Message3_6.hpp"
#include "Message3_7.hpp"
#include "Message7_0.hpp"
#include "Message7_1.hpp"
#include "Message7_3.hpp"
#include "Message9_0.hpp"
#include "Message9_1.hpp"
#include "Messages.hpp"

namespace WsfL16
{
template<typename FN>
inline void MessageSpecialized(WsfL16::Messages::InitialBase& aMsg, FN aFunctor)
{
   using namespace WsfL16::Messages;
   int id = aMsg.GetLabel() * 100 + aMsg.GetSubLabel();
   switch (id)
   {
   case 202:
      aFunctor((J2_2::Initial&)aMsg);
      break;
   case 203:
      aFunctor((J2_3::Initial&)aMsg);
      break;
   case 204:
      aFunctor((J2_4::Initial&)aMsg);
      break;
   case 205:
      aFunctor((J2_5::Initial&)aMsg);
      break;
   case 206:
      aFunctor((J2_6::Initial&)aMsg);
      break;
   case 300:
      aFunctor((J3_0::Initial&)aMsg);
      break;
   case 301:
      aFunctor((J3_2::Initial&)aMsg);
      break;
   case 302:
      aFunctor((J3_2::Initial&)aMsg);
      break;
   case 303:
      aFunctor((J3_3::Initial&)aMsg);
      break;
   case 304:
      aFunctor((J3_4::Initial&)aMsg);
      break;
   case 305:
      aFunctor((J3_5::Initial&)aMsg);
      break;
   case 306:
      aFunctor((J3_6::Initial&)aMsg);
      break;
   case 307:
      aFunctor((J3_7::Initial&)aMsg);
      break;
   case 700:
      aFunctor((J7_0::Initial&)aMsg);
      break;
   case 701:
      aFunctor((J7_1::Initial&)aMsg);
      break;
   case 703:
      aFunctor((J7_3::Initial&)aMsg);
      break;
   case 900:
      aFunctor((J9_0::Initial&)aMsg);
      break;
   case 901:
      aFunctor((J9_1::Initial&)aMsg);
      break;
   case 102:
      aFunctor((J10_2::Initial&)aMsg);
      break;
   case 110:
      aFunctor((J11_0::Initial&)aMsg);
      break;
   case 111:
      aFunctor((J11_1::Initial&)aMsg);
      break;
   case 112:
      aFunctor((J11_2::Initial&)aMsg);
      break;
   case 120:
      aFunctor((J12_0::Initial&)aMsg);
      break;
   case 121:
      aFunctor((J12_1::Initial&)aMsg);
      break;
   case 122:
      aFunctor((J12_2::Initial&)aMsg);
      break;
   case 123:
      aFunctor((J12_3::Initial&)aMsg);
      break;
   case 124:
      aFunctor((J12_4::Initial&)aMsg);
      break;
   case 125:
      aFunctor((J12_5::Initial&)aMsg);
      break;
   case 126:
      aFunctor((J12_6::Initial&)aMsg);
      break;
   case 127:
      aFunctor((J12_7::Initial&)aMsg);
      break;
   case 132:
      aFunctor((J13_2::Initial&)aMsg);
      break;
   case 133:
      aFunctor((J13_3::Initial&)aMsg);
      break;
   case 134:
      aFunctor((J13_4::Initial&)aMsg);
      break;
   case 135:
      aFunctor((J13_5::Initial&)aMsg);
      break;
   case 140:
      aFunctor((J14_0::Initial&)aMsg);
      break;
   case 150:
      aFunctor((J15_0::Initial&)aMsg);
      break;
   case 317:
      aFunctor((J31_7::Initial&)aMsg);
      break;
   }
}
} // namespace WsfL16

#endif
