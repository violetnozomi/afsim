// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @class  combatStatusMessage
*  @file   combatStatusMessage.hpp
*  @author Kenneth R. Sewell III
*          AFRL/RYWF
*          2241 Avionics Circle
*          WPAFB, Ohio 45433-7303
*          937-656-4899 x3004
*
**  Classification: UNCLASSIFIED
*/



#ifndef COMBAT_STATUS_MESSAGE_HPP
#define COMBAT_STATUS_MESSAGE_HPP

#include <iostream>
#include <map>

#include <iadsLib/enum.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>

namespace il
{
class IADSLIB_EXPORT combatStatusMessage : public baseMessage
{
   public:

      struct WeaponSystem
      {
         WeaponSystem()
            : _WSysID(-1), _MunitionsReady(0), _TotalMunitions(0), _TotalFireChannels(0), _NumAllocatedFireChannels(0) { }

         WeaponSystem(idRecord WSysID, unsigned short MunitionsReady, unsigned short TotalMunitions, unsigned short TotalFireChannels, unsigned short NumAllocatedFireChannels)
            : _WSysID(WSysID), _MunitionsReady(MunitionsReady), _TotalMunitions(TotalMunitions), _TotalFireChannels(TotalFireChannels), _NumAllocatedFireChannels(NumAllocatedFireChannels) { }

         /// ID of what system is being referenced
         idRecord _WSysID;

         /// Number of munitions ready to fire
         unsigned short _MunitionsReady;

         // Total number of munitions at site (including prepped)
         unsigned short _TotalMunitions;

         // Max fire channels available for this weapon
         unsigned short _TotalFireChannels;

         // Number of available fire channels for this weapon
         unsigned short _NumAllocatedFireChannels;

      };

      combatStatusMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~combatStatusMessage() = default;
      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      void setStatusTime(const double& time);
      const double getStatusTime() const;

      void setPositionTime(const double& time);
      const double getPositionTime() const;

      void setAssetID(const idRecord& ID);
      const idRecord getAssetID() const;

      void setMaxAssignments(const unsigned short& Num);
      const unsigned short getMaxAssignments() const;

      void setCurrentAssignments(const unsigned short& Num);
      const unsigned short getCurrentAssignments() const;

      void setSystemStatus(const SYSTEM_STATUS SystemStatus);
      SYSTEM_STATUS getSystemStatus() const;

      void setPosition(const double lat_degs, const double lon_degs, const double alt_m_msl);
      bool hasPosition() const;
      bool getPosition(double& lat_degs, double& lon_degs, double& alt_m_msl) const;

      void setVelocity(const double ecef_x_ms, const double ecef_y_ms, const double ecef_z_ms);
      bool hasVelocity() const;
      bool getVelocity(double& ecef_x_ms, double& ecef_y_ms, double& ecef_z_ms) const;


      // 1-based
      size_t getNumWeaponSystems() const;
      bool getNthWeapon(const size_t idx, WeaponSystem& wsys) const;
      void addWeapon(const WeaponSystem& wsys);


   protected:

      /// Timestamp of status data
      double _StatusTime;

      /// Timestamp of positonal data
      double _PositionTime;

      /// ID of system who's status is being sent.
      idRecord _AssetID; // JLK - TTD: 1) change all messages to use an equivalent tuple to the idrecord  2) add fixed-width types to messages!!!

      /// Assignments
      unsigned short _MaxAssignments;
      unsigned short _NumAssignments;

      /// System status
      SYSTEM_STATUS _SystemStatus;


      bool _HasPosition;
      double _LLA_DegsmMSL[3];

      bool _HasVelocity;
      double _ECEF_Vel_ms[3];

      std::map<idRecord, WeaponSystem> _Weapons;
};
}

#endif
