// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \file   trackRecord.hpp
 *  \author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/trackRecord.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <vclMath/ECEFToGeodetic.hpp>
#include <vclMath/GreatCircle.hpp>

namespace il
{

IADSLIB_EXPORT std::shared_ptr<trackRecord> FindTrack(const idRecord& track_id, const trackMap& tracks)
{
   auto track = tracks.find(track_id);
   return track != tracks.end() ? track->second : std::shared_ptr<trackRecord>();
}

trackRecord::trackRecord()
   : _AltitudeReliable(false)
   , _TimeCollected(-1)
   , _Time(-1)
   , _UpdateInterval(-1)
   , _TrackingStatus(UPDATE_TIMED_OUT)
   , _Psi_rads(0)
   , _Theta_rads(0)
   , _Phi_rads(0)
   , _Heading_rads(0)
   , _JammingPower(0)
   , _Quantity(0)
   , _EstimatedStrength(0)
   , _IFF(E_IFF_UNKNOWN)
   , _TruthName("Unknown")
   , _ReportingSensorType("Unknown")
{

}

bool trackRecord::operator==(const trackRecord& record) const
{
   return (_ID == record._ID);
}

bool trackRecord::operator<(const trackRecord& record) const
{
   return (_ID < record._ID);
}

void trackRecord::setID(const idRecord& ID)
{
   _ID = ID;
}

const idRecord& trackRecord::getID() const
{
   return _ID;
}

idRecord& trackRecord::getID()
{
   return _ID;
}

const idRecord& trackRecord::getSendingSystemID() const
{
   return _SendingSystemID;
}

void trackRecord::setPosition(const positionRecord& Position)
{
   _Position = Position;
}

const positionRecord& trackRecord::getPosition() const
{
   return _Position;
}

positionRecord& trackRecord::getPosition()
{
   return _Position;
}

void trackRecord::setVelocityVector(const double& dX,
                                    const double& dY,
                                    const double& dZ)
{
   _VelocityVector.set(dX, dY, dZ);
   _Velocity = sqrt(_VelocityVector.normSquared());
}

void trackRecord::setVelocityVector(const vm::Vector3<double>& Velocity)
{
   _VelocityVector = Velocity;
   _Velocity = sqrt(_VelocityVector.normSquared());
}

const vm::Vector3<double>& trackRecord::getVelocityVector() const
{
   return _VelocityVector;
}

vm::Vector3<double>& trackRecord::getVelocityVector()
{
   return _VelocityVector;
}

void trackRecord::setTime(const double& Time)
{
   _Time = Time;
}

const double trackRecord::getTime() const
{
   return _Time;
}

const double trackRecord::getUpdateInterval() const
{
   return _UpdateInterval;
}

void trackRecord::processMessage(const airTargetMessage& atm)
{
   _ID = atm.getTrackID();
   _Time = atm.getDataTime();
   _UpdateInterval = atm.getUpdateInterval();
   _TrackingStatus = atm.getTrackingStatus();
   _Position.setLLA(atm.getLatRads(), atm.getLonRads(), atm.getAltM());

   setVelocityVector(atm.getDx(), atm.getDy(), atm.getDz());
   _CovarianceMatrix = atm.getCovarianceMatrix();
   _Heading_rads = atm.getHeadingRads();
   _Type = atm.getType();

   _JammingPower = atm.getJammingPower();
   _Quantity = atm.getQuantity();
   _SendingSystemID = atm.getSenderID();

   _AltitudeReliable = atm.isAltitudeReliable();
   _IFF = atm.getIFF();

   _TruthName = atm.getTargetTruthName();
   _TruthID = atm.getTargetTruthID();

   _ReportingSensorType = atm.getReportingSensorType();
}

void trackRecord::setType(const unitTypeRecord& Type)
{
   _Type = Type;
}

const unitTypeRecord& trackRecord::getType() const
{
   return _Type;
}


positionRecord trackRecord::getPropagatedPosition(const double currentTime) const
{
   double deltaTime = currentTime - _Time;

   positionRecord new_position = _Position;
   vm::Vector3<double> deltaVector(_VelocityVector * deltaTime);
   new_position.setXYZ(_Position.getXYZ() + deltaVector);

   // Calculate the predicted distance traveled in meters
   double range = sqrt(deltaVector.normSquared());

   // Round earth altitude Drop off wrt distance traveled in meters
   double dropOffDistance = std::sqrt(std::pow(range, 2) + std::pow(vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES, 2)) - vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES;

   vm::Vector3<double> lla(new_position.getLLA());
   lla.setAlt(lla.getAlt() - dropOffDistance);

   // Subtract the drop off distance from the predicted altitude
   new_position.setLLA(lla);

   return new_position;
}

void trackRecord::extrapolateTrack(const double deltaTime)
{
   if (deltaTime < 0.0)
   {
      return;
   }

   vm::Vector3<double> deltaVector(_VelocityVector * deltaTime);

   _Position.setXYZ(_Position.getXYZ() + deltaVector);
   _Time = _Time + deltaTime;

   // Calculate the predicted distance traveled in meters
   double range = sqrt(deltaVector.normSquared());

   // Round earth altitude Drop off wrt distance traveled in meters
   double dropOffDistance =
      std::sqrt(std::pow(range, 2) +
                std::pow(vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES, 2)) -
      vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES;

   vm::Vector3<double> lla(_Position.getLLA());
   lla.setAlt(lla.getAlt() - dropOffDistance);

   // Subtract the drop off distance from the predicted altitude
   _Position.setLLA(lla);
}

trackRecord trackRecord::extrapolateTrack(const trackRecord& st, const double deltaTime)
{
   trackRecord newTrack(st);
   newTrack.extrapolateTrack(deltaTime);

   return newTrack;
}

bool trackRecord::generateAirTargetMessage(airTargetMessage& atm) const
{
   atm.setTrackID(_ID);
   atm.setDataTime(_Time);
   atm.setUpdateInterval(_UpdateInterval);
   atm.setTrackingStatus(_TrackingStatus);
   atm.setLLARadsM(_Position.getLatRads(), _Position.getLonRads(), _Position.getAltM());
   atm.setVelocityXYZ(_VelocityVector.getX(),
                      _VelocityVector.getY(),
                      _VelocityVector.getZ());
   atm.setCovarianceMatrix(_CovarianceMatrix);
   // no separate heading setting - wrapped up in orientation
   atm.setOrientationRads(_Psi_rads, _Theta_rads, _Phi_rads);
   atm.setHeadingRads(_Heading_rads);
   atm.setType(_Type);
   atm.setJammingPower(_JammingPower);
   atm.setQuantity(_Quantity);
   atm.setAltitudeReliable(_AltitudeReliable);
   atm.setIFF(_IFF);
   atm.setTargetTruthName(_TruthName);
   atm.setTargetTruthID(_TruthID);
   atm.setTargetTruthSide(_TruthSide);
   atm.setReportingSensorType(_ReportingSensorType);

   return true;
}


const double trackRecord::getVelocity() const
{
   return _Velocity;
}

void trackRecord::setCovarianceMatrix(const std::vector<double>& covar)
{
   _CovarianceMatrix = covar;
}

const std::vector<double>& trackRecord::getCovarianceMatrix() const
{
   return _CovarianceMatrix;
}

std::vector<double> trackRecord::getCovarianceMatrix()
{
   return _CovarianceMatrix;
}

void trackRecord::setHeadingRads(const double Heading_rads)
{
   _Heading_rads = Heading_rads;
}

const double trackRecord::getHeadingRads() const
{
   return _Heading_rads;
}

void trackRecord::setHeadingDegs(const double Heading_degs)
{
   _Heading_rads = Heading_degs * vm::DEGREES_TO_RADIANS;
}

const double trackRecord::getHeadingDegs() const
{
   return _Heading_rads * vm::RADIANS_TO_DEGREES;
}

void trackRecord::setTrackingStatus(const unsigned char& TrackingStatus)
{
   _TrackingStatus = TrackingStatus;
}

const unsigned char trackRecord::getTrackingStatus() const
{
   return _TrackingStatus;
}

double trackRecord::distance(const trackRecord& track) const
{
   return (_Position.getXYZ() - track._Position.getXYZ()).norm();
}

double trackRecord::distance(const il::positionRecord& position) const
{
   return (_Position.getXYZ() - position.getXYZ()).norm();
}

void trackRecord::setIFF(const eIFFType IFF)
{
   _IFF = IFF;
}

const eIFFType trackRecord::getIFF() const
{
   return _IFF;
}

void trackRecord::setTargetTruthName(const std::string& Name)
{
   _TruthName = Name;
}

const std::string& trackRecord::getTargetTruthName() const
{
   return _TruthName;
}

void trackRecord::setTargetTruthID(const idRecord& record)
{
   _TruthID = record;
}

idRecord trackRecord::getTargetTruthID() const
{
   return _TruthID;
}

void trackRecord::setTargetTruthSide(const std::string& Side)
{
   _TruthSide = Side;
}

const std::string& trackRecord::getTargetTruthSide() const
{
   return _TruthSide;
}

void trackRecord::setReportingSensorType(const std::string& type)
{
   _ReportingSensorType = type;
}

const std::string& trackRecord::getReportingSensorType() const
{
   return _ReportingSensorType;
}

il::SensorEnum trackRecord::getReportingSensorTypeEnum() const
{
   return getSensorTypeEnumFromString(_ReportingSensorType);
}


const double trackRecord::getJammingPower() const
{
   return _JammingPower;
}

void trackRecord::calculateStrength(const double SimTime,
                                    const trackMap& tracks,
                                    const double maxGroupingDistance)
{
   _EstimatedStrength = 1;

   auto thisEstimatedPosition = getPropagatedPosition(SimTime);

   for (auto it = tracks.begin(); it != tracks.end(); ++it)
   {
      auto& track = it->second;

      // ensure we're not double dipping by comparing to ourselves
      if (_ID != track->getID())
      {
         auto otherEstimatedPosition = track->getPropagatedPosition(SimTime);
         // Calculate great-circle distance in meters.
         auto distance = vm::GreatCircle<double>::calculateDistanceInMeters(thisEstimatedPosition.getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                            thisEstimatedPosition.getLLA().getLon() * vm::RADIANS_TO_DEGREES,
                                                                            otherEstimatedPosition.getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                            otherEstimatedPosition.getLLA().getLon() * vm::RADIANS_TO_DEGREES);

         if (distance <= maxGroupingDistance)
         {
            ++_EstimatedStrength;
         }
      }
   }
}

const unsigned int trackRecord::getStrength() const
{
   return _EstimatedStrength;
}

const unsigned int trackRecord::getQuantity() const
{
   return _Quantity;
}

} // namespace il
