// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  weaponsControlMessage
 *  \file   weaponsControlMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/weaponsControlMessage.hpp>

namespace il
{

weaponsControlMessage::weaponsControlMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   :baseMessage(aGlobalLogger, WEAPONS_CONTROL_MESSAGE),
   _DataTime(0.0),
   _PrepareMissileCommand(0),
   _PrepareMissileQuantity(0)
{
}


baseMessage* weaponsControlMessage::clone() const
{
   return new weaponsControlMessage(*this);
}

void weaponsControlMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void weaponsControlMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}


void weaponsControlMessage::logSTD(std::ostream& os) const
{
   os << std::fixed
      << "Weapons Control Message" << "\n"
      << "-----------------------" << "\n";
   baseMessage::logSTD(os);
   os << "Data Time: " << _DataTime << "\n"
      << "Prepare Missile Command: " << _PrepareMissileCommand << "\n"
      << "Prepare Missile Quantity: " << _PrepareMissileQuantity << "\n";
}

void weaponsControlMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Weapons Control Message" << ","
      << _DataTime << ","
      << _PrepareMissileCommand << ","
      << _PrepareMissileQuantity;
}

} // namespace il
