// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_TRACK_MESSAGE_HPP
#define WSF_BM_TRACK_MESSAGE_HPP

#include <memory>

#include <WsfBMCommon.hpp>
#include <WsfMessage.hpp>
#include <iadsLib/airTargetMessage.hpp>
#include "WsfBMGenericTypeWrapper.hpp"
#include <UtVec3.hpp>
#include <WsfGeoPoint.hpp>
#include <WsfTrackId.hpp>
#include <WsfCovariance.hpp>
#include <WsfTrack.hpp>
#include <UtNullCreate.hpp>

class WsfPlatform;

class WsfBMTrackMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      //WsfBMTrackMessage (const UtNullCreate& aNull);
      WsfBMTrackMessage(WsfSimulation* simPtr);
      WsfBMTrackMessage(WsfPlatform* aPlatformPtr);
      WsfBMTrackMessage(WsfPlatform* aPlatformPtr, const std::shared_ptr<il::baseMessage>& message);

      WsfBMTrackMessage(const WsfBMTrackMessage& aSrc);

      virtual ~WsfBMTrackMessage();

      WsfBMTrackMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();
      static WsfStringId GetTypeId();

      void SetUpdateInterval(const double update_interval_s);
      double GetUpdateInterval() const;

      void SetTrackID(const WsfTrackId& track_id);
      int GetTrackingSystemID() const;
      int GetTrackingSystemTrackID() const;

      void SetPosition(const double sim_time_s, const WsfGeoPoint& location);
      const WsfGeoPoint GetPosition() const;
      double GetPositionTime() const;

      void SetVelocityECEF(const UtVec3d& vel);
      const UtVec3d GetVelocityECEF() const;

      void SetCovarianceMatrix(const WsfCovariance& covariance);
      WsfCovariance GetCovarianceMatrix() const;

      void SetHeadingENUDegs(const double heading_deg);
      void SetOrientationWCSDegs(const double psi, const double theta, const double phi);
      double GetOrientationWCSPsiDegs() const;
      double GetOrientationWCSThetaDegs() const;
      double GetOrientationWCSPhiDegs() const;

      void SetTargetType(const std::string& type, const std::string& subtype);
      std::string GetTargetType() const;
      std::string GetTargetSubType() const;

      enum eTrackingStatus { STATUS_NORMAL, STATUS_COASTING, STATUS_TIMEDOUT, STATUS_DROPPING, STATUS_DROPPED };
      void SetTrackingStatus(const eTrackingStatus status);
      eTrackingStatus GetTrackingStatus() const;

      void SetAltitudeReliable(const bool is_reliable);
      bool GetAltitudeReliable() const;

      void SetIFF(const WsfTrack::IFF_Status iff);
      WsfTrack::IFF_Status GetIFF() const;

      void SetManeuveringFlag(const bool is_maneuvering);
      bool GetManeuveringFlag() const;

      void SetQuantity(const int qty);
      int GetQuantity() const;

      void SetJamming(const bool is_jamming);
      bool GetJamming() const;

      void SetJammingPower(const double power);
      double GetJammingPower() const;

      void SetTargetTruthInfo(const WsfPlatform* target);

      //   enum eSensorType { SENS_EW, SENS_TAR, SENS_TTR };
      void SetReportingSensorType(const il::SensorEnum sensor_type);
      il::SensorEnum GetReportingSensorType() const;

      inline std::shared_ptr<const il::airTargetMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::airTargetMessage>(m_wrapped_ptr); }

   private:
      inline std::shared_ptr<il::airTargetMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::airTargetMessage>(m_wrapped_ptr); }

      //   static const std::string C_EW_SENS_STR;
      //   static const std::string C_TAR_SENS_STR;
      //   static const std::string C_TTR_SENS_STR;
};

#endif
