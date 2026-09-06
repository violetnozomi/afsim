// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfUCI_Utils.hpp"

#include "UtCalendar.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"

#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfTrack.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_InterfaceExtension.hpp"


namespace UCI_Util
{
   wsf::UCI_Interface* GetInterface(WsfPlatform* aPlatformPtr)
   {
      auto extPtr = wsf::UCI_InterfaceExtension::Get(*aPlatformPtr->GetSimulation());
      return extPtr->GetInterface(); // Get the named interface.
   }

   wsf::UCI_Interface* GetInterface(WsfSimulation* aSimulationPtr)
   {
      auto extPtr = wsf::UCI_InterfaceExtension::Get(*aSimulationPtr);
      return extPtr->GetInterface(); // todo is this acceptable or do we need the named interface as above?
   }

   void GetKinematics(uci::type::DateTimeTypeValue& aSimTime,
                      uci::type::KinematicsType&    aKinematics,
                      WsfSensor*         aSensorPtr,
                      const WsfTrack&    aTrack)
   {
      //Set kinematic values of the tracked entity
      double lat;
      double lon;
      double alt;

      aSensorPtr->GetPlatform()->GetLocationLLA(lat, lon, alt);
      lon *= UtMath::cRAD_PER_DEG;
      lat *= UtMath::cRAD_PER_DEG;

      aKinematics.getPosition().enableRelativePosition();
      aKinematics.getPosition().getRelativePosition().getReferencePoint().getPoint().setAltitude(alt);
      aKinematics.getPosition().getRelativePosition().getReferencePoint().getPoint().setLatitude(lat);
      aKinematics.getPosition().getRelativePosition().getReferencePoint().getPoint().setLongitude(lon);
      aKinematics.getPosition().getRelativePosition().getReferencePoint().getPoint().setTimestamp(aSimTime);

      aKinematics.getPosition().getRelativePosition().enableLOS_Uncertainty();
      if (aSensorPtr->GetCurrentMode()->ReportsRange())
      {
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().enableSlantRangeError();
         aKinematics.getPosition().getRelativePosition().getLOS().setSlantRange(aTrack.GetRange());
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().setSlantRangeError(aTrack.GetRangeError());
      }
      if (aSensorPtr->GetCurrentMode()->ReportsBearing())
      {
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().enableBearingError();
         aKinematics.getPosition().getRelativePosition().getLOS().setBearing(UtMath::NormalizeAngle0_TwoPi(aTrack.GetBearing()));
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().setBearingError(UtMath::NormalizeAngle0_TwoPi(aTrack.GetSensorAzimuthError()));
      }
      if (aSensorPtr->GetCurrentMode()->ReportsElevation())
      {
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().enableElevationError();
         aKinematics.getPosition().getRelativePosition().getLOS().setElevation(aTrack.GetElevation());
         aKinematics.getPosition().getRelativePosition().getLOS_Uncertainty().setElevationError(aTrack.GetSensorElevationError());
      }
      if (aSensorPtr->GetCurrentMode()->ReportsLocation())
      {
         aKinematics.getPosition().enableFixedPositionType();
         double lat, lon, alt;
         aTrack.GetLocationLLA(lat, lon, alt);
         lon *= UtMath::cRAD_PER_DEG;
         lat *= UtMath::cRAD_PER_DEG;
         auto& fp = aKinematics.getPosition().getFixedPositionType().getFixedPoint();
         fp.enableAltitude();
         fp.setAltitude(alt);
         fp.setLatitude(lat);
         fp.setLongitude(lon);
         fp.enableTimestamp();
         fp.setTimestamp(aSimTime);
      }
      if (aSensorPtr->GetCurrentMode()->ReportsVelocity())
      {
         UtEntity target;
         double lat, lon, alt;
         aTrack.GetLocationLLA(lat, lon, alt);
         target.SetLocationLLA(lat, lon, alt);
         double vel[3]{ aTrack.GetVelocityWCS().Get(0), aTrack.GetVelocityWCS().Get(1), aTrack.GetVelocityWCS().Get(2) };
         target.SetVelocityWCS(vel);
         double tgtVelocity[3];
         target.GetVelocityNED(tgtVelocity);

         aKinematics.enableVelocity();
         aKinematics.getVelocity().enableDownSpeed();
         aKinematics.getVelocity().setNorthSpeed(tgtVelocity[0]);
         aKinematics.getVelocity().setEastSpeed(tgtVelocity[1]);
         aKinematics.getVelocity().setDownSpeed(tgtVelocity[2]);
      }
   }

   uci::type::SubsystemStateEnum::EnumerationItem GetSubsystemState(WsfPlatformPart* aPartPtr)
   {
      uci::type::SubsystemStateEnum::EnumerationItem retVal = uci::type::SubsystemStateEnum::UCI_OFF;
      if (aPartPtr->IsOperational() &&
          aPartPtr->IsTurnedOn())
      {
         //The sensor is available if it is operational and turned on.
         retVal = uci::type::SubsystemStateEnum::EnumerationItem::UCI_OPERATE;
      }

      return retVal;
   }

   std::string GetSubsystemStateString(const uci::type::SubsystemStateEnum::EnumerationItem& aState)
   {
      std::string state = "UNKNOWN";
      switch (aState)
      {
         case uci::type::SubsystemStateEnum::EnumerationItem::UCI_OFF:
            state = "OFF";
            break;
         case uci::type::SubsystemStateEnum::EnumerationItem::UCI_OPERATE:
            state = "OPERATE";
            break;
         default:
            break;
      }

      return state;
   }

   // Convert Double Data Time to UCI Date Time
   uci::type::DateTimeTypeValue ConvertDoubleToUCI_DateTime(double aTime)
   {
      // set to the converted value
      return static_cast<uint64_t>(aTime * sNANOSECONDS_PER_SECOND);
   }

   double ConvertUCI_DateTimeToDouble(uci::type::DateTimeTypeValue aUCI_DateTime)
   {
      return (aUCI_DateTime / sNANOSECONDS_PER_SECOND);
   }

   //Converts the absolute sim time to the required time stamp format
   uci::type::DateTimeTypeValue GetTimeStamp(WsfSimulation& aSimulation,
                                             double         aSimTime)
   {
      //The time stamp requires number of seconds from 1970-01-01 00:00:00
      static std::unique_ptr<UtCalendar> sUnixTimeStart { nullptr };
      if (sUnixTimeStart == nullptr)
      {
         sUnixTimeStart = ut::make_unique<UtCalendar>();
         sUnixTimeStart->SetDate(1970, 1, 1);
         sUnixTimeStart->SetTime(0.0);
      }
      UtCalendar currentTime;
      // if (aSimulation.GetDateTime().UsingSystemTime())
      // {
      //    currentTime.SetCurrentDateAndTime();
      // }
      // else
      // {
         currentTime = aSimulation.GetDateTime().GetCurrentTime(aSimTime);
      // }
      return ConvertDoubleToUCI_DateTime(currentTime.GetTimeSince(*sUnixTimeStart));
   }

   std::string RemoveAngleBrackets(const std::string& aString)
   {
      std::string retStr{ aString };
      retStr.erase(std::remove(retStr.begin(), retStr.end(), '>'), retStr.end());
      retStr.erase(std::remove(retStr.begin(), retStr.end(), '<'), retStr.end());
      return retStr;
   }

   uci::type::StandardIdentityEnum::EnumerationItem GetStandardIdentity(const WsfTrack& aTrack)
   {
      auto retVal = uci::type::StandardIdentityEnum::UCI_UNKNOWN;
      switch (aTrack.GetIFF_Status())
      {
      case WsfTrack::cIFF_FOE:
         retVal = uci::type::StandardIdentityEnum::UCI_HOSTILE;
         break;
      case WsfTrack::cIFF_FRIEND:
         retVal = uci::type::StandardIdentityEnum::UCI_FRIEND;
         break;
      case WsfTrack::cIFF_NEUTRAL:
         retVal = uci::type::StandardIdentityEnum::UCI_NEUTRAL;
         break;
      default:
         break;
      }
	   return retVal;
   }

   bool CheckLimits(const WsfArticulatedPart* aArticulatedPartPtr,
                    double&                   aAz,
                    double&                   aEl)
   {
      if (aAz <= sTOLERANCE && aAz >= -sTOLERANCE)
      {
         aAz = 0;
      }

      if (aEl <= sTOLERANCE && aEl >= -sTOLERANCE)
      {
         aEl = 0;
      }

      return ((aAz >= aArticulatedPartPtr->GetMinAzCue() || aArticulatedPartPtr->GetMinAzCue() - aAz <= sTOLERANCE) &&
              (aAz <= aArticulatedPartPtr->GetMaxAzCue() || aAz - aArticulatedPartPtr->GetMaxAzCue() <= sTOLERANCE) &&
              (aEl >= aArticulatedPartPtr->GetMinElCue() || aArticulatedPartPtr->GetMinElCue() - aEl <= sTOLERANCE) &&
              (aEl <= aArticulatedPartPtr->GetMaxElCue() || aEl - aArticulatedPartPtr->GetMaxElCue() <= sTOLERANCE));      
   }
} // namespace UCI_Util

