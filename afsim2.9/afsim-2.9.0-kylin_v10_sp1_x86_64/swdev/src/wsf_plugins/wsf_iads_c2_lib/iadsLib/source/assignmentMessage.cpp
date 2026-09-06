// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  assignmentMessage
 *  \file   assignmentMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <stdint.h>
#include <logger.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/util.hpp>

namespace il
{

assignmentMessage::assignmentMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : baseMessage(aGlobalLogger, ASSIGNMENT_MESSAGE)
   , _Delegation(false)
   , _SelfDefenseAssignment(false)
   , _Ready(false)
   , _AssignTime(0)
   , _ShotDoctrine(SHOOT_1)
   , _ShotExpiry(0)
   , _MaxProjectionTime(0)
   , _MaxProjectionTimeIncrement(10)
   , _Reason(NEW_ASSIGNMENT)
   , m_shots_fired_during_assignment(0)
{
}

assignmentMessage::assignmentMessage(const assignmentMessage& am)
   : baseMessage(am)
     // Track ID //
   , _ReferenceTrackID(am._ReferenceTrackID)
   , _LocalTrackID(am._LocalTrackID)
     // Assigned ID //
   , _AssignedID(am._AssignedID)
     // Bookkeeping Unit IDs //
   , _InitiatingUnitID(am._InitiatingUnitID)
   , _AssigningUnitID(am._AssigningUnitID)
     // Flags
   , _Delegation(am._Delegation)
   , _SelfDefenseAssignment(am._SelfDefenseAssignment)
   , _Ready(am._Ready)
     // Other
   , _AssignTime(am._AssignTime)
   , _ShotDoctrine(am._ShotDoctrine)
   , _ShotExpiry(am._ShotExpiry)
   , _MaxProjectionTime(am._MaxProjectionTime)
   , _MaxProjectionTimeIncrement(am._MaxProjectionTimeIncrement)
   , _Reason(am._Reason)
   , m_shots_fired_during_assignment(am.m_shots_fired_during_assignment)
{
   if (am.m_status)
   {
      m_status = std::shared_ptr<assignmentACKMessage>(new assignmentACKMessage(*am.m_status));
   }
}

assignmentMessage* assignmentMessage::clone() const
{
   return new assignmentMessage(*this);
}

assignmentMessage& assignmentMessage::operator=(const assignmentMessage& src)
{
   if (this != &src)
   {
      baseMessage::operator=(src);

      _ReferenceTrackID = src._ReferenceTrackID;
      _LocalTrackID = src._LocalTrackID;
      _AssignedID = src._AssignedID;
      _InitiatingUnitID = src._InitiatingUnitID;
      _AssigningUnitID = src._AssigningUnitID;

      _Delegation = src._Delegation;
      _SelfDefenseAssignment = src._SelfDefenseAssignment;
      _Ready = src._Ready;
      _AssignTime = src._AssignTime;
      _ShotDoctrine = src._ShotDoctrine;
      _ShotExpiry = src._ShotExpiry;
      _Reason = src._Reason;

      m_status = std::shared_ptr<assignmentACKMessage>(new assignmentACKMessage(*src.m_status));
   }

   return *this;
}



// Keep assignments at the unit, track tuple level -- ignore weapon systems since we don't double assign
bool assignmentMessage::operator<(const assignmentMessage& message) const
{
   if (_AssignedID.getID() != message._AssignedID.getID())
   {
      return (_AssignedID.getID() < message._AssignedID.getID());
   }
   else if (_ReferenceTrackID.getSystemUnitID() != message._ReferenceTrackID.getSystemUnitID())
   {
      return (_ReferenceTrackID.getSystemUnitID() < message._ReferenceTrackID.getSystemUnitID());
   }
   else
   {
      return (_ReferenceTrackID.getSystemTrackID() < message._ReferenceTrackID.getSystemTrackID());
   }
}

bool assignmentMessage::operator==(const assignmentMessage& message) const
{
   return (_AssignedID.getID() == message._AssignedID.getID() &&
           _ReferenceTrackID == message._ReferenceTrackID);
}

bool assignmentMessage::operator!=(const assignmentMessage& message) const
{
   return !(*this == message);
}


void assignmentMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void assignmentMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}


void assignmentMessage::logSTD(std::ostream& os) const
{
   os << "Assignment Message" << "\n"
      << "------------------" << "\n";
   baseMessage::logSTD(os);
   os << "Assignment Time: " << _AssignTime << "\n"
      << "Assigned System ID: " << _AssignedID.getID() << " Specific System ID: " << _AssignedID.getSubID() << "\n"
      << "Reference Track ID: " << _ReferenceTrackID.getSystemUnitID() << ":" << _ReferenceTrackID.getSystemTrackID() << std::endl
      << "Local Track ID: " << _LocalTrackID.getSystemUnitID() << ":" << _LocalTrackID.getSystemTrackID()
      << "\n"
      << "Reason: " << static_cast<uint32_t>(_Reason) << "\n"
      << "Shot doctrine: " << static_cast<uint32_t>(_ShotDoctrine)
      << "\n";
}

void assignmentMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Assignment Message" << ","
      << _AssignTime << ","
      << _AssignedID.getID() << ","
      << _AssignedID.getSubID() << ","
      << _ReferenceTrackID.getSystemUnitID() << "," << _ReferenceTrackID.getSystemTrackID() << ","
      << _LocalTrackID.getSystemUnitID() << "," << _LocalTrackID.getSystemTrackID() << ","
      << static_cast<uint32_t>(_Reason) << ","
      << static_cast<uint32_t>(_ShotDoctrine) << "\n";
}

void assignmentMessage::create(const double simTime,
                               const eShotDoctrine shot_doctrine,
                               const idRecord& initiating_unit_id,
                               const idRecord& assigning_unit_id,
                               const idRecord& reference_track_number,
                               const idRecord& weapon_unit_id,
                               const idRecord& weapon_id,
                               const double max_shot_time,
                               const double max_projection_time,
                               const double max_projection_time_increment,
                               const bool  is_self_defense,
                               const bool is_delegation)
{
   // Track ID
   _ReferenceTrackID = _LocalTrackID = reference_track_number;
   // Assigned unit/weapon system
   _AssignedID = GetAssignedIDFromWeaponIDs(weapon_unit_id, weapon_id);

   _InitiatingUnitID = initiating_unit_id;
   _AssigningUnitID = assigning_unit_id;

   _Delegation = is_delegation;

   _SelfDefenseAssignment = is_self_defense;

   _Ready = true; // not really sure why HELIOS has this field

   _AssignTime = simTime;

   _ShotDoctrine = shot_doctrine;

   _ShotExpiry = max_shot_time;

   _Reason = NEW_ASSIGNMENT;

   _MaxProjectionTime = max_projection_time;

   _MaxProjectionTimeIncrement = max_projection_time_increment;
}

void assignmentMessage::setReferenceTrackID(const idRecord& track_num)
{
   _ReferenceTrackID = track_num;
}

const idRecord& assignmentMessage::getReferenceTrackID() const
{
   return _ReferenceTrackID;
}

void assignmentMessage::setLocalTrackID(const idRecord& track_num)
{
   _LocalTrackID = track_num;
}

const idRecord& assignmentMessage::getLocalTrackID() const
{
   return _LocalTrackID;
}

void assignmentMessage::setAssignedID(const idRecord& value)
{
   _AssignedID = value;
}

idRecord assignmentMessage::getAssignedID() const
{
   return _AssignedID;
}

void assignmentMessage::setCuedID(const idRecord& value)
{
   _CuedID = value;
}

idRecord assignmentMessage::getCuedID() const
{
   return _CuedID;
}

void assignmentMessage::setInitiatingID(const idRecord& value)
{
   _InitiatingUnitID = value;
}

idRecord assignmentMessage::getInitiatingID() const
{
   return _InitiatingUnitID;
}


void assignmentMessage::setAssigningID(const idRecord& value)
{
   _AssigningUnitID = value;
}

idRecord assignmentMessage::getAssigningID() const
{
   return _AssigningUnitID;
}


void assignmentMessage::setSelfDefenseFlag(const bool& value)
{
   _SelfDefenseAssignment = value;
}

bool assignmentMessage::getSelfDefenseFlag() const
{
   return _SelfDefenseAssignment;
}


void assignmentMessage::setAssignTime(const double& value)
{
   _AssignTime = value;
}

double assignmentMessage::getAssignTime() const
{
   return _AssignTime;
}

void assignmentMessage::setShotDoctrine(const eShotDoctrine& value)
{
   _ShotDoctrine = value;
}

eShotDoctrine assignmentMessage::getShotDoctrine() const
{
   return _ShotDoctrine;
}

void assignmentMessage::setMaxFiringTime(const double& value)
{
   _ShotExpiry = value;
}

double assignmentMessage::getMaxFiringTime() const
{
   return _ShotExpiry;
}

void assignmentMessage::setMaxProjectionTime(const double& value)
{
   _MaxProjectionTime = value;
}

double assignmentMessage::getMaxProjectionTime() const
{
   return _MaxProjectionTime;
}

void assignmentMessage::setMaxProjectionTimeIncrement(const double& value)
{
   _MaxProjectionTimeIncrement = value;
}

double assignmentMessage::getMaxProjectionTimeIncrement() const
{
   return _MaxProjectionTimeIncrement;
}

void assignmentMessage::setAssignmentReason(const AssignmentReason& value)
{
   _Reason = value;
}

assignmentMessage::AssignmentReason assignmentMessage::getAssignmentReason() const
{
   return _Reason;
}

std::string assignmentMessage::getAssignmentReasonString() const
{
   std::string reason;

   switch (_Reason)
   {
      case NEW_ASSIGNMENT:
         reason = "New Assignment";
         break;
      case RETRANSMIT:
         reason = "Retransmit";
         break;
      case UPDATE:
         reason = "Update";
         break;
      case REASSIGNMENT:
         reason = "Reassignment";
         break;
      case CANCEL:
         reason = "Cancel";
         break;
      default:
         std::stringstream strmerror;
         strmerror << "assignmentMessage::getAssignmentReasonString(): Unknown reason enumeration: " << _Reason;
         throw std::runtime_error(strmerror.str());
         break;
   }

   return reason;
}

int assignmentMessage::getRequestedMissilesToCommit() const
{
   return il::GetMissileCountFromDoctrine(_ShotDoctrine, mGlobalLogger);
}

//////////////////////////////
// status-related interface //
//////////////////////////////
il::assignmentACKMessage::eAssignmentStatus assignmentMessage::getAssignmentStatus() const
{
   assignmentACKMessage::eAssignmentStatus status = assignmentACKMessage::UNACKNOWLEDGED;
   if (m_status)
   {
      status = m_status->getStatus();
   }

   return status;
}

void assignmentMessage::setStatus(const std::shared_ptr<assignmentACKMessage>& status)
{
   m_status = status;

   // do any extra handling for specific statuses
   if (status->getStatus() == assignmentACKMessage::FIRING)
   {
      incrementShotsFired();
   }
}


bool assignmentMessage::isAssignmentComplete() const
{
   bool is_complete = false;

   // test for the case of the assignment status being directly cancelled, if so, it is complete
   if (getAssignmentReason() == CANCEL)
   {
      return is_complete = true;
   }

   // check assignment status directly if assignment hasn't been cancelled
   switch (getAssignmentStatus())
   {
      case assignmentACKMessage::UNACKNOWLEDGED:
      case assignmentACKMessage::WILCO:
      case assignmentACKMessage::COMITTED:
      case assignmentACKMessage::CUE_TAR:
      case assignmentACKMessage::CUE_TTR:
      case assignmentACKMessage::TAR_SEARCHING:
      case assignmentACKMessage::TTR_SEARCHING:
      case assignmentACKMessage::TAR_TRACKING:
      case assignmentACKMessage::TTR_TRACKING:
      case assignmentACKMessage::FIRING:
      case assignmentACKMessage::MISS:
      case assignmentACKMessage::CHANGED_ASSIGNED_UNIT:
      case assignmentACKMessage::UPDATED_SHOT_DOCTRINE:
         is_complete = false;
         break;

      case assignmentACKMessage::KILL:
      case assignmentACKMessage::CANCELLED:
      case assignmentACKMessage::HAVCO_SUCCESS:
      case assignmentACKMessage::HAVCO_FAILURE:
      case assignmentACKMessage::CANTCO:
         is_complete = true;
         break;

      default:
         std::stringstream strm_error;
         strm_error << "assignmentMessage::isAssignmentComplete(): Unrecognized assignment status (" << getAssignmentStatus() << ")\n";
         HCL_FATAL_LOGGER(mGlobalLogger) << strm_error.str();
         throw (strm_error.str());
         break;
   }

   return is_complete;
}

bool assignmentMessage::wasSuccess() const
{
   return isAssignmentComplete() &&
          (getAssignmentStatus() == assignmentACKMessage::KILL || getAssignmentStatus() == assignmentACKMessage::HAVCO_SUCCESS);
}

int assignmentMessage::getSalvosFired() const
{
   return m_shots_fired_during_assignment;
}

void assignmentMessage::incrementShotsFired()
{
   if (_ShotDoctrine == SHOOT_2)
   {
      m_shots_fired_during_assignment += 2;
   }
   else
   {
      m_shots_fired_during_assignment++;
   }
}

} // namespace il
