// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisPduEnums.hpp"

#include <sstream>
#include <utility>

#include "DisEnumConversion.hpp"

namespace DisEnum
{
namespace Pdu
{
namespace Version
{
const DisEnum::Conversion<Enum>& GetConversion()
{
   typedef std::pair<Enum, std::string> PAIR;
   typedef std::vector<PAIR>            VECTOR;
   static PAIR                          data[] = {
#define X(Value, String, Enumeration) std::make_pair(Enumeration, String),
#include "DisVersion.xenum"
#undef X
   };
   static const VECTOR                    versions(DisEnum::MakeVector(data));
   static const DisEnum::Conversion<Enum> conv(versions);
   return conv;
}

const bool IsValid(DisEnum8 v)
{
   return (v > Other && v <= IEEE_1278_1_2012);
}
const std::string& ToString(Enum v)
{
   return GetConversion().ToString(v);
}
const Enum ToEnum(const std::string& v)
{
   return GetConversion().ToEnum(v);
}
} // namespace Version
namespace Type
{
const DisEnum::Conversion<Enum>& GetConversion()
{
   typedef std::pair<Enum, std::string> PAIR;
   typedef std::vector<PAIR>            VECTOR;
   static PAIR                          data[] = {
#define X(Value, String, Enumeration) std::make_pair(Enumeration, String),
#include "DisType.xenum"
#undef X
      std::make_pair(IffAtcNavaids, "IFF/ATC/Navaids (Deprecated)"),
      std::make_pair(TransferControl, "Transfer Control (Deprecated)"),
      std::make_pair(DirectedEnergyDamageStatus, "Directed Energy Damage Status (Deprecated)")};
   static const VECTOR                    types(DisEnum::MakeVector(data));
   static const DisEnum::Conversion<Enum> conv(types);
   return conv;
}
const bool IsValid(DisEnum8 t)
{
   return (t <= Attribute || t == Spotted);
}
const std::string& ToString(Enum t)
{
   return GetConversion().ToString(t);
}
const Enum ToEnum(const std::string& t)
{
   return GetConversion().ToEnum(t);
}
} // namespace Type
namespace Family
{
const DisEnum::Conversion<Enum>& GetConversion()
{
   typedef std::pair<Enum, std::string> PAIR;
   typedef std::vector<PAIR>            VECTOR;
   static PAIR                          data[] = {
#define X(Value, String, Enumeration) std::make_pair(Enumeration, String),
#include "DisFamily.xenum"
#undef X
   };
   static const VECTOR                    types(DisEnum::MakeVector(data));
   static const DisEnum::Conversion<Enum> conv(types);
   return conv;
}
const bool IsValid(DisEnum8 f)
{
   return (f <= InformationOperations || f == Experimental);
}
const std::string& ToString(Enum t)
{
   return GetConversion().ToString(t);
}
const Enum ToEnum(const std::string& t)
{
   return GetConversion().ToEnum(t);
}
} // namespace Family
} // namespace Pdu
} // namespace DisEnum

namespace DisEnum
{
namespace Pdu
{

const std::string& Status::TEI::ToString(DisEnum8 aTEI)
{
   static const std::string STR[] = {"No Difference", "Difference"};
   return STR[(aTEI & 0x01)];
}

const std::string& Status::LVC::ToString(DisEnum8 aLVC)
{
   static const std::string STR[] = {"No Statement", "Live", "Virtual", "Constructive"};
   return STR[(aLVC & 0x03)];
}

const std::string& Status::CEI::ToString(DisEnum8 aCEI)
{
   static const std::string STR[] = {"Not Coupled", "Coupled"};
   return STR[(aCEI & 0x01)];
}

const std::string& Status::FTI::ToString(DisEnum8 aFTI)
{
   static const std::string STR[] = {"Munition", "Expendable"};
   return STR[(aFTI & 0x01)];
}

const std::string& Status::DTI::ToString(DisEnum8 aDTI)
{
   static const std::string STR[] = {"Munition", "Expendable", "Non-munition Explosion"};
   size_t                   index = IsValid(aDTI) ? static_cast<size_t>(aDTI) : 0;
   return STR[index];
}

const std::string& Status::RAI::ToString(DisEnum8 aRAI)
{
   static const std::string STR[] = {"No Statement", "Unattached", "Attached"};
   size_t                   index = IsValid(aRAI) ? static_cast<size_t>(aRAI) : 0;
   return STR[index];
}

const std::string& Status::IAI::ToString(DisEnum8 aIAI)
{
   static const std::string STR[] = {"No Statement", "Unattached", "Attached"};
   size_t                   index = IsValid(aIAI) ? static_cast<size_t>(aIAI) : 0;
   return STR[index];
}

const std::string& Status::ISM::ToString(DisEnum8 aISM)
{
   static const std::string STR[] = {"Regeneration", "Interactive"};
   return STR[(aISM & 0x01)];
}

const std::string& Status::AII::ToString(DisEnum8 aAII)
{
   static const std::string STR[] = {"Not Active", "Active"};
   return STR[(aAII & 0x01)];
}

bool Status::IsValid(DisEnum8 aPduStatus, DisEnum8 aPduType)
{
   switch (aPduType)
   {
   case Type::EntityState:
   case Type::Emission:
   case Type::Designator:
   case Type::EnvironmentalProcess:
   case Type::EntityStateUpdate:
   {
      // TEI, LVC, and CEI used
      // All possible bits are defined
      break;
   }
   case Type::Fire:
   {
      // TEI not defined,
      // LVC, CEI, and FTI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      if (tei != 0)
         return false;
      break;
   }
   case Type::Detonation:
   {
      // TEI not defined,
      // LVC, CEI, and DTI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      if (tei != 0)
         return false;
      DisEnum8 dti = (aPduStatus >> 3) & 0x03;
      if (!DTI::IsValid(dti))
         return false;
      break;
   }
   case Type::Transmitter:
   case Type::Signal:
   case Type::Receiver:
   {
      // TEI, LVC, CEI, and RAI are defined
      DisEnum8 rai = (aPduStatus >> 3) & 0x03;
      if (!RAI::IsValid(rai))
         return false;
      break;
   }
   case Type::IntercomSignal:
   case Type::IntercomControl:
   {
      // TEI, LVC, CEI, and IAI are defined
      DisEnum8 iai = (aPduStatus >> 3) & 0x03;
      if (!IAI::IsValid(iai))
         return false;
      break;
   }
   case Type::IFF:
   {
      // TEI, LVC, CEI, ISM, and AII are defined
      // All possible bits are defined
      break;
   }
   case Type::Attribute:
   {
      // TEI and CEI are not defined
      // LVC is defined
      DisEnum8 tei = aPduStatus & 0x01;
      if (tei != 0)
         return false;
      DisEnum8 cei = (aPduStatus >> 2) & 0x01;
      if (cei != 0)
         return false;
      break;
   }
   default:
   {
      // TEI is not defined
      // LVC and CEI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      if (tei != 0)
         return false;
      break;
   }
   }
   return true;
}

std::string Status::ToString(DisEnum8 aPduStatus, DisEnum8 aPduType)
{
   std::stringstream s;
   switch (aPduType)
   {
   case Type::EntityState:
   case Type::Emission:
   case Type::Designator:
   case Type::EnvironmentalProcess:
   case Type::EntityStateUpdate:
   {
      // TEI, LVC, and CEI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      s << "Transferred Entity Indicator:   " << static_cast<DisUint16>(tei) << TEI::ToString(tei) << '\n';
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      break;
   }
   case Type::Fire:
   {
      // TEI not defined,
      // LVC, CEI, and FTI are defined
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      DisEnum8 fti = (aPduStatus >> 3) & 0x01;
      s << "Fire Type Indicator:            " << static_cast<DisUint16>(fti) << FTI::ToString(fti) << '\n';
      break;
   }
   case Type::Detonation:
   {
      // TEI not defined,
      // LVC, CEI, and DTI are defined
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      DisEnum8 dti = (aPduStatus >> 3) & 0x03;
      s << "Detonation Type Indicator:      " << static_cast<DisUint16>(dti) << DTI::ToString(dti) << '\n';
      break;
   }
   case Type::Transmitter:
   case Type::Signal:
   case Type::Receiver:
   {
      // TEI, LVC, CEI, and RAI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      s << "Transferred Entity Indicator:   " << static_cast<DisUint16>(tei) << TEI::ToString(tei) << '\n';
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      DisEnum8 rai = (aPduStatus >> 3) & 0x03;
      s << "Radio Attached Indicator:       " << static_cast<DisUint16>(rai) << RAI::ToString(rai) << '\n';
      break;
   }
   case Type::IntercomSignal:
   case Type::IntercomControl:
   {
      // TEI, LVC, CEI, and IAI are defined
      DisEnum8 tei = aPduStatus & 0x01;
      s << "Transferred Entity Indicator:   " << static_cast<DisUint16>(tei) << TEI::ToString(tei) << '\n';
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      DisEnum8 iai = (aPduStatus >> 3) & 0x03;
      s << "Intercom Attached Indicator:    " << static_cast<DisUint16>(iai) << IAI::ToString(iai) << '\n';
      break;
   }
   case Type::IFF:
   {
      // TEI, LVC, CEI, ISM, and AII are defined
      // All possible bits are defined
      DisEnum8 tei = aPduStatus & 0x01;
      s << "Transferred Entity Indicator:   " << static_cast<DisUint16>(tei) << TEI::ToString(tei) << '\n';
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      DisEnum8 ism = (aPduStatus >> 3) & 0x01;
      s << "IFF Simulation Mode:            " << static_cast<DisUint16>(ism) << ISM::ToString(ism) << '\n';
      DisEnum8 aii = (aPduStatus >> 4) & 0x01;
      s << "Active Interrogation Indicator: " << static_cast<DisUint16>(aii) << AII::ToString(aii) << '\n';
      break;
   }
   case Type::Attribute:
   {
      // TEI and CEI are not defined
      // LVC is defined
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      break;
   }
   default:
   {
      // TEI is not defined
      // LVC and CEI are defined
      DisEnum8 lvc = (aPduStatus >> 1) & 0x03;
      s << "LVC Indicator:                  " << static_cast<DisUint16>(lvc) << LVC::ToString(lvc) << '\n';
      DisEnum8 cei = (aPduStatus >> 3) & 0x01;
      s << "Coupled Extension Indicator:    " << static_cast<DisUint16>(cei) << CEI::ToString(cei) << '\n';
      break;
   }
   }
   return s.str();
}

} // namespace Pdu
} // namespace DisEnum
