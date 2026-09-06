// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  airTargetMessage
 *  @file   airTargetMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#ifndef IADSLIB_AIR_TARGET_MESSAGE_HPP
#define IADSLIB_AIR_TARGET_MESSAGE_HPP

#include <vector>

#include <vclMath/Constants.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>

namespace il
{
class IADSLIB_EXPORT airTargetMessage : public baseMessage
{
   public:

      airTargetMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~airTargetMessage() = default;
      baseMessage* clone() const;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      void setDataTime(const double& DataTime)
      {
         _DataTime = DataTime;
      }

      const double& getDataTime() const
      {
         return _DataTime;
      }

      void setUpdateInterval(const double& UpdateInterval)
      {
         _UpdateInterval = UpdateInterval;
      }

      double getUpdateInterval() const
      {
         return _UpdateInterval;
      }

      void setTrackID(const idRecord& TrackID)
      {
         _TrackID = TrackID;
      }

      const idRecord& getTrackID() const
      {
         return _TrackID;
      }

      const int& getTrackingSystemID() const
      {
         return _TrackID.getSystemUnitID();
      }

      const int& getTrackingSystemTrackID() const
      {
         return _TrackID.getSystemTrackID();
      }

      const std::vector<double>& getLLAVel() const;
      std::vector<double>& getLLAVel();

      void setLLARadsM(const double& lat, const double& lon, const double& alt)
      {
         _LLA[0] = lat;
         _LLA[1] = lon;
         _LLA[2] = alt;
      }

      const double& getLatRads() const
      {
         return _LLA[0];
      }

      const double& getLonRads() const
      {
         return _LLA[1];
      }

      const double& getAltM() const
      {
         return _LLA[2];
      }

      void setVelocityXYZ(const double& Dx,
                           const double& Dy,
                           const double& Dz)
      {
         _LLA[3] = Dx;
         _LLA[4] = Dy;
         _LLA[5] = Dz;
      }

      const double& getDx() const
      {
         return _LLA[3];
      }

      const double& getDy() const
      {
         return _LLA[4];
      }

      const double& getDz() const
      {
         return _LLA[5];
      }

      void setCovarianceMatrix(const std::vector<double>& covar);
      const std::vector<double>& getCovarianceMatrix() const;
      std::vector<double> getCovarianceMatrix();

      void setOrientationRads(const double Psi_rads, const double Theta_rads, const double Phi_rads)
      {
         _Psi_rads = Psi_rads;
         _Theta_rads = Theta_rads;
         _Phi_rads = Phi_rads;
      }

      void setOrientationDegs(const double Psi_degs, const double Theta_degs, const double Phi_degs)
      {
         _Psi_rads = Psi_degs   * vm::DEGREES_TO_RADIANS;
         _Theta_rads = Theta_degs * vm::DEGREES_TO_RADIANS;
         _Phi_rads = Phi_degs   * vm::DEGREES_TO_RADIANS;
      }

      const double getPsiRads() const
      {
         return _Psi_rads;
      }

      const double getThetaRads() const
      {
         return _Theta_rads;
      }

      const double getPhiRads() const
      {
         return _Phi_rads;
      }

      const double getPsiDegs() const
      {
         return _Psi_rads * vm::RADIANS_TO_DEGREES;
      }

      const double getThetaDegs() const
      {
         return _Theta_rads * vm::RADIANS_TO_DEGREES;
      }

      const double getPhiDegs() const
      {
         return _Phi_rads * vm::RADIANS_TO_DEGREES;
      }

      void setHeadingRads(double heading_rads)
      {
         _Heading_rads = heading_rads;
      }

      void setHeadingDegs(double heading_degs)
      {
         _Heading_rads = heading_degs * vm::DEGREES_TO_RADIANS;
      }

      const double getHeadingRads() const
      {
         return _Heading_rads;
      }

      const double getHeadingDegs() const
      {
         return _Heading_rads * vm::RADIANS_TO_DEGREES;
      }

      void setType(const unitTypeRecord& Type)
      {
         _TargetType = Type;
      }

      const unitTypeRecord& getType() const
      {
         return _TargetType;
      }

      void setTrackingStatus(const unsigned char& TrackingStatus)
      {
         _TrackingStatus = TrackingStatus;
      }

      const unsigned char& getTrackingStatus() const
      {
         return _TrackingStatus;
      }

      void setAltitudeReliable(const bool& Reliable = true)
      {
         if (Reliable)
         {
            _AltitudeReliable = 1;
         }
         else
         {
            _AltitudeReliable = 0;
         }
      }

      void setAltitudeUnreliable()
      {
         _AltitudeReliable = 0;
      }

      bool isAltitudeReliable() const
      {
         return (_AltitudeReliable > 0);
      }

      void setIFF(const eIFFType IFF)
      {
         _IFF = IFF;
      }

      const eIFFType& getIFF() const
      {
         return _IFF;
      }

      void setManeuvering(const unsigned char& Maneuvering)
      {
         _Maneuvering = Maneuvering;
      }

      const unsigned char& getManeuvering() const
      {
         return _Maneuvering;
      }

      void setQuantity(const unsigned short& Quantity)
      {
         _Quantity = Quantity;
      }

      const unsigned short& getQuantity() const
      {
         return _Quantity;
      }

      void setReactionAgainstTarget(const unsigned char& ReactionAgainstTarget)
      {
         _ReactionAgainstTarget = ReactionAgainstTarget;
      }

      const unsigned char& getReactionAgainstTarget() const
      {
         return _ReactionAgainstTarget;
      }

      void setJamming(const unsigned char& Jamming)
      {
         _Jamming = Jamming;
      }

      const unsigned char& getJamming() const
      {
         return _Jamming;
      }

      void setJammingPower(const double& JammingPower)
      {
         _JammingPower = JammingPower;
      }

      const double& getJammingPower() const
      {
         return _JammingPower;
      }

      void setDisID(const unsigned short& site,
                     const unsigned short& app,
                     const unsigned short& id);
      void setDisID(const std::vector<unsigned short>& id);
      const std::vector<unsigned short>& getDisID() const;
      std::vector<unsigned short>& getDisID();

      inline void setTargetTruthName(const std::string& Name) { _TargetTruthName = Name; }
      inline const std::string& getTargetTruthName() const { return _TargetTruthName; }

      inline void setReportingSensorType(const std::string& type) { _ReportingSensorType = type; }
      inline const std::string& getReportingSensorType() const { return _ReportingSensorType; }

      inline void setTargetTruthID(const idRecord& record) { _TargetTruthID = record; }
      inline idRecord getTargetTruthID() const { return _TargetTruthID; }

      inline void setTargetTruthSide(const std::string& Side) { _TargetTruthSide = Side; }
      inline const std::string& getTargetTruthSide() const { return _TargetTruthSide; }

   protected:
      /// Timestamp of message data.
      double _DataTime;

      /// Time between expected updates. (sec)
      double _UpdateInterval;

      // Track is uniquely identified by System Unit ID & Track Number
      idRecord _TrackID;

      /// Latitude in radians
      /// Longitude in radians
      /// Altitude in meters.
      /// XYZ Velocity vector (m/s)
      std::vector<double> _LLA;

      /// Position/velocity covariance matrix 6x6
      std::vector<double> _Covariance;

      /// Orientation (rad) wrt ECEF
      double _Psi_rads;
      double _Theta_rads;
      double _Phi_rads;

      /// Heading (radians)
      double _Heading_rads;

      /// Type/Subtype describing target type.
      unitTypeRecord  _TargetType;

      /// Track is: New, Lost, Dropped, Extrapolated, Update...
      unsigned char _TrackingStatus;

      /// Indicates whether altitude is reliable.
      unsigned char _AltitudeReliable;

      /// IFF - Unidentified, Foriegn, Friendly, ...
      eIFFType _IFF;

      /// Target maneuvering: None or type of maneuver
      unsigned char _Maneuvering;

      /// Quantity of targets in this track.
      unsigned short _Quantity;

      /// No reaction, Air intercept, SAM engagement, Air and SAM, ...
      unsigned char _ReactionAgainstTarget;

      /// Type of jamming - None, Active, Passive, Combined.
      unsigned char _Jamming;

      /// Jamming power in dB
      double _JammingPower;

      std::vector<unsigned short> _DisID;

      std::string _TargetTruthName;
      idRecord    _TargetTruthID;
      std::string _TargetTruthSide;

      std::string _ReportingSensorType;
};
}

#endif
