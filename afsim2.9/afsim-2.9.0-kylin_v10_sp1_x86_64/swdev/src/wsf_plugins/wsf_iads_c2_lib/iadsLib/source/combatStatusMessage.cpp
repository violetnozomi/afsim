// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  \class  combatStatusMessage
*  \file   combatStatusMessage.cpp
*  \author Kenneth R. Sewell III \n
*          AFRL/RYWF \n
*          2241 Avionics Circle \n
*          WPAFB, Ohio 45433-7303 \n
*          937-656-4899 x3004 \n
*
**  Classification: UNCLASSIFIED
*/

#include <iostream>

#include <logger.hpp>
#include <iadsLib/combatStatusMessage.hpp>
#include <iadsLib/enum.hpp>

namespace il
{

/********************************************************************/

combatStatusMessage::combatStatusMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : baseMessage(aGlobalLogger, COMBAT_STATUS_MESSAGE)
   , _StatusTime(0.0)
   , _PositionTime(0.0)
   , _MaxAssignments(0)
   , _NumAssignments(0)
   , _SystemStatus(STATUS_RED)
   , _HasPosition(false)
   , _HasVelocity(false)
{
   _LLA_DegsmMSL[0] = _LLA_DegsmMSL[1] = _LLA_DegsmMSL[2] = 0;

   _ECEF_Vel_ms[0] = _ECEF_Vel_ms[1] = _ECEF_Vel_ms[2] = 0;
}

baseMessage* combatStatusMessage::clone() const
{
   return new combatStatusMessage(*this);
}

void combatStatusMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void combatStatusMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}


void combatStatusMessage::logSTD(std::ostream& os) const
{
   os << "Combat Status Message" << "\n"
      << "---------------------" << "\n";
   baseMessage::logSTD(os);
   os << "           Status Time: " << _StatusTime << "\n"
      << "           Asset ID: " <<  _AssetID << "\n"
      << "Assignments (curr/max): " << _NumAssignments << "/" << _MaxAssignments << "\n"
      << "       System Status: " << static_cast<unsigned int>(_SystemStatus)
      << "       Has Position?: " << (_HasPosition ? "True" : "False") << std::endl
      << "       Podition Time: " << _PositionTime << std::endl
      << "Lat/Lon/Alt (degs/m MSL): " << _LLA_DegsmMSL[0] << "\\" << _LLA_DegsmMSL[1] << "\\" << _LLA_DegsmMSL[2] << std::endl
      << "       Has Velocity?: " << (_HasVelocity ? "True" : "False") << std::endl
      << "ECEF Vel (m/s): " << _ECEF_Vel_ms[0] << "\\" << _ECEF_Vel_ms[1] << "\\" << _ECEF_Vel_ms[2] << std::endl
      << "       Total Weapons: " << _Weapons.size() << std::endl;

   for (auto it = _Weapons.begin(); it != _Weapons.end(); ++it)
   {
      auto& weapon = it->second;
      os << "          Weapon ID: " << weapon._WSysID << " Missiles Ready: " << weapon._MunitionsReady << std::endl;
   }

   os  << "\n";
}

void combatStatusMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Combat Status Message" << ","
      << _StatusTime << ","
      << _AssetID << ","
      << _NumAssignments << "," << _MaxAssignments << ","
      << static_cast<unsigned int>(_SystemStatus) << ","
      << (_HasPosition ? "True" : "False") << ","
      << _PositionTime << ","
      << _LLA_DegsmMSL[0] << "," << _LLA_DegsmMSL[1] << "," << _LLA_DegsmMSL[2] << ","
      << (_HasVelocity ? "True" : "False") << ","
      << _ECEF_Vel_ms[0] << "," << _ECEF_Vel_ms[1] << "," << _ECEF_Vel_ms[2] << ","
      << _Weapons.size();

   for (auto it = _Weapons.begin(); it != _Weapons.end(); ++it)
   {
      auto& weapon = it->second;
      os << "," << weapon._WSysID << "," << weapon._MunitionsReady;
   }

   os << "\n";
}


void combatStatusMessage::setStatusTime(const double& time)
{
   _StatusTime = time;
}

const double combatStatusMessage::getStatusTime() const
{
   return _StatusTime;
}

void combatStatusMessage::setPositionTime(const double& time)
{
   _PositionTime = time;
}


const double combatStatusMessage::getPositionTime() const
{
   return _PositionTime;
}

void combatStatusMessage::setAssetID(const idRecord& ID)
{
   _AssetID = ID;
}

const idRecord combatStatusMessage::getAssetID() const
{
   return _AssetID;
}

void combatStatusMessage::setMaxAssignments(const unsigned short& Num)
{
   _MaxAssignments = Num;
}

const unsigned short combatStatusMessage::getMaxAssignments() const
{
   return _MaxAssignments;
}

void combatStatusMessage::setCurrentAssignments(const unsigned short& Num)
{
   _NumAssignments = Num;
}

const unsigned short combatStatusMessage::getCurrentAssignments() const
{
   return _NumAssignments;
}


void combatStatusMessage::setSystemStatus(const SYSTEM_STATUS SystemStatus)
{
   _SystemStatus = SystemStatus;
}

SYSTEM_STATUS combatStatusMessage::getSystemStatus() const
{
   return _SystemStatus;
}

void combatStatusMessage::setPosition(const double lat_degs, const double lon_degs, const double alt_m_msl)
{
   _LLA_DegsmMSL[0] = lat_degs;
   _LLA_DegsmMSL[1] = lon_degs;
   _LLA_DegsmMSL[2] = alt_m_msl;

   _HasPosition = true;
}

bool combatStatusMessage::hasPosition() const
{
   return _HasPosition;
}

bool combatStatusMessage::getPosition(double& lat_degs, double& lon_degs, double& alt_m_msl) const
{
   if (_HasPosition)
   {
      lat_degs =  _LLA_DegsmMSL[0];
      lon_degs =  _LLA_DegsmMSL[1];
      alt_m_msl = _LLA_DegsmMSL[2];
   }

   return _HasPosition;
}


void combatStatusMessage::setVelocity(const double ecef_x_ms, const double ecef_y_ms, const double ecef_z_ms)
{
   _ECEF_Vel_ms[0] = ecef_x_ms;
   _ECEF_Vel_ms[1] = ecef_y_ms;
   _ECEF_Vel_ms[2] = ecef_z_ms;

   _HasVelocity = true;
}


bool combatStatusMessage::hasVelocity() const
{
   return _HasVelocity;
}

bool combatStatusMessage::getVelocity(double& ecef_x_ms, double& ecef_y_ms, double& ecef_z_ms) const
{
   if (_HasVelocity)
   {
      ecef_x_ms = _ECEF_Vel_ms[0];
      ecef_y_ms = _ECEF_Vel_ms[1];
      ecef_z_ms = _ECEF_Vel_ms[2];
   }

   return _HasVelocity;
}


size_t combatStatusMessage::getNumWeaponSystems() const
{
   return _Weapons.size();
}

// 1-based
bool combatStatusMessage::getNthWeapon(const size_t idx, WeaponSystem& wsys) const
{
   bool valid_weapon = false;

   size_t curridx = 1;
   for (auto iter = _Weapons.cbegin(); iter != _Weapons.cend() && !valid_weapon; ++iter, ++curridx)
   {
      if (curridx == idx)
      {
         wsys = iter->second;
         valid_weapon = true;
      }
   }
   return valid_weapon;
}

void combatStatusMessage::addWeapon(const WeaponSystem& wsys)
{
   _Weapons[wsys._WSysID] = wsys;
}

} // namespace il
