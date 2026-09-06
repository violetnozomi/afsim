// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <logger.hpp>
#include "WsfBMTrackMessage.hpp"
#include <UtException.hpp>

#include <WsfScriptBMTrackMessageClass.hpp>
#include <WsfBMUtils.hpp>
#include <iadsLib/enum.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

//const std::string WsfBMTrackMessage::C_EW_SENS_STR = il::C_EW_SENSOR_TYPE_STR;
//const std::string WsfBMTrackMessage::C_TAR_SENS_STR = il::C_TAR_SENSOR_TYPE_STR;
//const std::string WsfBMTrackMessage::C_TTR_SENS_STR = il::C_TTR_SENSOR_TYPE_STR;

//WsfBMTrackMessage::WsfBMTrackMessage (const UtNullCreate& aNull)
//   : WsfMessage (aNull)
//   , WsfBMManagedTypeWrapper (WsfBMTrackMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::airTargetMessage))
//{
//
//}

WsfBMTrackMessage::WsfBMTrackMessage(WsfSimulation* simPtr)
//: WsfMessage (*simPtr, WsfBMTrackMessage::GetTypeId())
   : WsfMessage(WsfBMTrackMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMTrackMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::airTargetMessage(IADSC2ScenarioExtension::GetGlobalLogger(simPtr->GetScenario()))))
{
   SetSimulation(simPtr);
}

WsfBMTrackMessage::WsfBMTrackMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(WsfBMTrackMessage::GetTypeId(), aPlatformPtr)
   , WsfBMManagedTypeWrapper(WsfBMTrackMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::airTargetMessage(IADSC2ScenarioExtension::GetGlobalLogger(aPlatformPtr->GetScenario()))))
{
}

WsfBMTrackMessage::WsfBMTrackMessage(WsfPlatform* aPlatformPtr, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMTrackMessage::GetTypeId(), aPlatformPtr)
   , WsfBMManagedTypeWrapper(WsfBMTrackMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::AIR_TARGET_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aPlatformPtr->GetScenario())) << "Invalid message type, WsfBMTrackMessage set to wrap non-track message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::AIR_TARGET_MESSAGE);
      throw UtException("Invalid message type, WsfBMTrackMessage set to wrap non-track message.");
   }
}

WsfBMTrackMessage::WsfBMTrackMessage(const WsfBMTrackMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMTrackMessage::GetTypeId(), std::make_shared<il::airTargetMessage>(*aSrc.GetRawMessagePtr()))
{
}

WsfBMTrackMessage::~WsfBMTrackMessage()
{
}

WsfBMTrackMessage* WsfBMTrackMessage::Clone() const
{
   return new WsfBMTrackMessage(*this);
}

const char* WsfBMTrackMessage::GetScriptClassName() const
{
   return WsfScriptBMTrackMessageClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMTrackMessage::GetBaseClassName()
{
   return WsfScriptBMTrackMessageClass::BASE_CLASS_NAME;
}

WsfStringId WsfBMTrackMessage::GetTypeId()
{
   return "WsfBMTrackMessage";
}

// Accessor implementations
void WsfBMTrackMessage::SetUpdateInterval(const double update_interval_s)
{
   GetMutableRawMessagePtr()->setUpdateInterval(update_interval_s);
}

double WsfBMTrackMessage::GetUpdateInterval() const
{
   return GetRawMessagePtr()->getUpdateInterval();
}

void WsfBMTrackMessage::SetTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

int WsfBMTrackMessage::GetTrackingSystemID() const
{
   return GetRawMessagePtr()->getTrackingSystemID();
}

int WsfBMTrackMessage::GetTrackingSystemTrackID() const
{
   return GetRawMessagePtr()->getTrackingSystemTrackID();
}

void WsfBMTrackMessage::SetPosition(const double sim_time_s, const WsfGeoPoint& location)
{
   GetMutableRawMessagePtr()->setDataTime(sim_time_s);
   GetMutableRawMessagePtr()->setLLARadsM(location.GetLat() * UtMath::cRAD_PER_DEG, location.GetLon() * UtMath::cRAD_PER_DEG, location.GetAlt());
}

const WsfGeoPoint WsfBMTrackMessage::GetPosition() const
{
   return WsfGeoPoint(GetRawMessagePtr()->getLatRads() * UtMath::cDEG_PER_RAD, GetRawMessagePtr()->getLonRads() * UtMath::cDEG_PER_RAD, GetRawMessagePtr()->getAltM());
}

double WsfBMTrackMessage::GetPositionTime() const
{
   return GetRawMessagePtr()->getDataTime();
}

void WsfBMTrackMessage::SetVelocityECEF(const UtVec3d& vel)
{
   GetMutableRawMessagePtr()->setVelocityXYZ(vel[0], vel[1], vel[2]);
}

const UtVec3d WsfBMTrackMessage::GetVelocityECEF() const
{
   return UtVec3d(GetRawMessagePtr()->getDx(), GetRawMessagePtr()->getDy(), GetRawMessagePtr()->getDz());
}

void WsfBMTrackMessage::SetCovarianceMatrix(const WsfCovariance& covariance)
{
   std::vector<double> cov_vector(covariance.GetSize());
   covariance.CopyTo(cov_vector.data(), covariance.GetSize());

   GetMutableRawMessagePtr()->setCovarianceMatrix(cov_vector);
}

WsfCovariance WsfBMTrackMessage::GetCovarianceMatrix() const
{
   std::vector<double> cov_vector = GetRawMessagePtr()->getCovarianceMatrix();

   WsfCovariance covariance(static_cast<unsigned int>(cov_vector.size()) / 2,
                            static_cast<unsigned int>(cov_vector.size()) / 2);
   covariance.CopyTo(cov_vector.data());

   return covariance;
}

void WsfBMTrackMessage::SetHeadingENUDegs(const double heading_deg)
{
   GetMutableRawMessagePtr()->setHeadingDegs(heading_deg);
}

void WsfBMTrackMessage::SetOrientationWCSDegs(const double psi, const double theta, const double phi)
{
   GetMutableRawMessagePtr()->setOrientationDegs(psi, theta, phi);
}

double WsfBMTrackMessage::GetOrientationWCSPsiDegs() const
{
   return GetRawMessagePtr()->getPsiDegs();
}

double WsfBMTrackMessage::GetOrientationWCSThetaDegs() const
{
   return GetRawMessagePtr()->getThetaDegs();
}

double WsfBMTrackMessage::GetOrientationWCSPhiDegs() const
{
   return GetRawMessagePtr()->getPhiDegs();
}

void WsfBMTrackMessage::SetTargetType(const std::string& type, const std::string& subtype)
{
   WsfStringId type_id(type), subtype_id(subtype);
   GetMutableRawMessagePtr()->setType(il::unitTypeRecord(WsfStringInt(type_id), type, WsfStringInt(subtype_id), subtype));
}

std::string WsfBMTrackMessage::GetTargetType() const
{
   return GetRawMessagePtr()->getType().getTypeStr();
}

std::string WsfBMTrackMessage::GetTargetSubType() const
{
   return GetRawMessagePtr()->getType().getSubTypeStr();
}

void WsfBMTrackMessage::SetTrackingStatus(const eTrackingStatus status)
{
   il::TRACK_UPDATE_STATUS il_status = il::UPDATE_NORMAL;
   switch (status)
   {
      case STATUS_NORMAL:
         il_status = il::UPDATE_NORMAL;
         break;
      case STATUS_COASTING:
         il_status = il::UPDATE_COASTING;
         break;
      case STATUS_TIMEDOUT:
         il_status = il::UPDATE_TIMED_OUT;
         break;
      case STATUS_DROPPING:
         il_status = il::UPDATE_DROPPING;
         break;
      case STATUS_DROPPED:
         il_status = il::UPDATE_DROPPED;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::SetTrackingStatus(): Unknown status: " << status << std::endl;
         break;
   }

   GetMutableRawMessagePtr()->setTrackingStatus(il_status);
}

WsfBMTrackMessage::eTrackingStatus WsfBMTrackMessage::GetTrackingStatus() const
{
   eTrackingStatus status = STATUS_NORMAL;
   switch (GetRawMessagePtr()->getTrackingStatus())
   {
      case il::UPDATE_NORMAL:
         status = STATUS_NORMAL;
         break;
      case il::UPDATE_COASTING:
         status = STATUS_COASTING;
         break;
      case il::UPDATE_TIMED_OUT:
         status = STATUS_TIMEDOUT;
         break;
      case il::UPDATE_DROPPING:
         status = STATUS_DROPPING;
         break;
      case il::UPDATE_DROPPED:
         status = STATUS_DROPPED;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::GetTrackingStatus(): Unknown status: " << GetRawMessagePtr()->getTrackingStatus() << std::endl;
         break;
   }

   return status;
}

void WsfBMTrackMessage::SetAltitudeReliable(const bool is_reliable)
{
   GetMutableRawMessagePtr()->setAltitudeReliable(is_reliable);
}

bool WsfBMTrackMessage::GetAltitudeReliable() const
{
   return GetRawMessagePtr()->isAltitudeReliable();
}

void WsfBMTrackMessage::SetIFF(const WsfTrack::IFF_Status iff)
{
   il::eIFFType il_iff = il::E_IFF_UNKNOWN;
   switch (iff)
   {
      case WsfTrack::cIFF_UNKNOWN:
      case WsfTrack::cIFF_AMBIGUOUS: // warning, this could be potentiall lossy
         il_iff = il::E_IFF_UNKNOWN;
         break;
      case WsfTrack::cIFF_FRIEND:
         il_iff = il::E_IFF_FRIENDLY;
         break;
      case WsfTrack::cIFF_FOE:
         il_iff = il::E_IFF_HOSTILE;
         break;
      case WsfTrack::cIFF_NEUTRAL:
         il_iff = il::E_IFF_NEUTRAL;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::SetIFF(): Unknown iff: " << iff << std::endl;
         break;
   }

   GetMutableRawMessagePtr()->setIFF(il_iff);
}

WsfTrack::IFF_Status WsfBMTrackMessage::GetIFF() const
{
   WsfTrack::IFF_Status iff = WsfTrack::cIFF_UNKNOWN;
   switch (GetRawMessagePtr()->getIFF())
   {
      case il::E_IFF_UNKNOWN:
         iff = WsfTrack::cIFF_UNKNOWN;
         break;
      case il::E_IFF_FRIENDLY:
         iff = WsfTrack::cIFF_FRIEND;
         break;
      case il::E_IFF_HOSTILE:
         iff = WsfTrack::cIFF_FOE;
         break;
      case il::E_IFF_NEUTRAL:
         iff = WsfTrack::cIFF_NEUTRAL;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::GetIFF(): Unknown iff: " << GetRawMessagePtr()->getIFF() << std::endl;
         break;
   }

   return iff;
}

void WsfBMTrackMessage::SetManeuveringFlag(const bool is_maneuvering)
{
   GetMutableRawMessagePtr()->setManeuvering(is_maneuvering);
}

bool WsfBMTrackMessage::GetManeuveringFlag() const
{
   return GetRawMessagePtr()->getManeuvering();
}

void WsfBMTrackMessage::SetQuantity(const int qty)
{
   GetMutableRawMessagePtr()->setQuantity(qty);
}

int WsfBMTrackMessage::GetQuantity() const
{
   return GetRawMessagePtr()->getQuantity();
}

void WsfBMTrackMessage::SetJamming(const bool is_jamming)
{
   GetMutableRawMessagePtr()->setJamming(is_jamming);
}

bool WsfBMTrackMessage::GetJamming() const
{
   return GetRawMessagePtr()->getJamming();
}

void WsfBMTrackMessage::SetJammingPower(const double power)
{
   GetMutableRawMessagePtr()->setJammingPower(power);
}

double WsfBMTrackMessage::GetJammingPower() const
{
   return GetRawMessagePtr()->getJammingPower();
}

void WsfBMTrackMessage::SetTargetTruthInfo(const WsfPlatform* target)
{
   auto mutable_msg = GetMutableRawMessagePtr();
   if (target)
   {
      mutable_msg->setTargetTruthName(target->GetName());
      mutable_msg->setTargetTruthID(WsfBMUtils::GetUnitIDRecordFromPlatform(target));
   }
   else
   {
      mutable_msg->setTargetTruthName("** unknown target **");
      mutable_msg->setTargetTruthID(il::idRecord());
   }
}

void WsfBMTrackMessage::SetReportingSensorType(const il::SensorEnum sensor_type)
{
   switch (sensor_type)
   {
      case il::SENSOR_EW:
         GetMutableRawMessagePtr()->setReportingSensorType(il::C_EW_SENSOR_TYPE_STR);
         break;
      case il::SENSOR_TAR:
         GetMutableRawMessagePtr()->setReportingSensorType(il::C_TAR_SENSOR_TYPE_STR);
         break;
      case il::SENSOR_TTR:
         GetMutableRawMessagePtr()->setReportingSensorType(il::C_TTR_SENSOR_TYPE_STR);
         break;
      case il::SENSOR_RWR:
         GetMutableRawMessagePtr()->setReportingSensorType(il::C_RWR_SENSOR_TYPE_STR);
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::SetReportingSensorType(): Unknown sensor type: " << sensor_type << std::endl;
         break;
   }
}

il::SensorEnum WsfBMTrackMessage::GetReportingSensorType() const
{
   auto sensor_type_str = GetRawMessagePtr()->getReportingSensorType();
   il::SensorEnum sensor_type = il::SENSOR_EW;
   if (sensor_type_str == il::C_EW_SENSOR_TYPE_STR)
   {
      sensor_type = il::SENSOR_EW;
   }
   else if (sensor_type_str == il::C_TAR_SENSOR_TYPE_STR)
   {
      sensor_type = il::SENSOR_TAR;
   }
   else if (sensor_type_str == il::C_TTR_SENSOR_TYPE_STR)
   {
      sensor_type = il::SENSOR_TTR;
   }
   else if (sensor_type_str == il::C_RWR_SENSOR_TYPE_STR)
   {
      sensor_type = il::SENSOR_RWR;
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMTrackMessage::GetReportingSensorType(): Unknown sensor type: " << sensor_type_str << std::endl;
   }

   return sensor_type;
}
