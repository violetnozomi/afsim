// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfTargetPointOptions.hpp"

#include <string>

#include "UtInput.hpp"
#include "WsfLibrationTargetPoint.hpp"
#include "WsfNTW_OffsetTargetPoint.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfRIC_OffsetTargetPoint.hpp"
#include "WsfSpaceKinematicStateTargetPoint.hpp"
#include "WsfTimeOffsetTargetPoint.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"

namespace wsf
{
namespace space
{

TargetPointOptions::TargetPointOptions(const WsfTrackId& aLocalTrackId)
   : mLocalTrackId{aLocalTrackId}
{
}

TargetPointOptions::TargetPointOptions(LibrationPoint::System aSystem, LibrationPoint::Point aPoint)
{
   SetLibrationPoint(aSystem, aPoint);
}

TargetPointOptions::TargetPointOptions(const UtVec3d& aPosition, const UtVec3d& aVelocity)
{
   SetKinematicState(aPosition, "m", aVelocity, "m/s");
}

//! Process commands in the input stream for this object.
bool TargetPointOptions::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "platform")
   {
      std::string name;
      aInput.ReadValue(name);
      SetPlatformName(name);
   }
   else if (command == "libration_point")
   {
      // get the system
      std::string system;
      aInput.ReadCommand(system);
      auto lpSystem = LibrationPoint::GetSystemFromIdentifier(system);
      if (lpSystem == LibrationPoint::System::cUNKNOWN)
      {
         std::stringstream err{};
         err << "Unknown libration point system: '" << system << "'";
         throw UtInput::BadValue(aInput, err.str());
      }

      // get the point
      std::string point;
      aInput.ReadCommand(point);
      auto lpPoint = LibrationPoint::GetPointFromIdentifier(point);
      if (lpPoint == LibrationPoint::Point::cUNKNOWN)
      {
         std::stringstream err{};
         err << "Unknown libration point: '" << point << "'";
         throw UtInput::BadValue(aInput, err.str());
      }

      SetLibrationPoint(lpSystem, lpPoint);
   }
   else if (command == "eci_position")
   {
      UtVec3d posVec;
      for (int i = 0; i < 3; ++i)
      {
         aInput.ReadValue(posVec[i]);
      }
      std::string units;
      aInput.ReadValue(units);
      SetKinematicState(posVec, units, UtVec3d{}, "m/s");
   }
   else if (command == "eci_state")
   {
      UtVec3d posVec;
      for (int i = 0; i < 3; ++i)
      {
         aInput.ReadValue(posVec[i]);
      }
      std::string posUnits;
      aInput.ReadValue(posUnits);
      UtVec3d velVec;
      for (int i = 0; i < 3; ++i)
      {
         aInput.ReadValue(velVec[i]);
      }
      std::string velUnits;
      aInput.ReadValue(velUnits);
      SetKinematicState(posVec, posUnits, velVec, velUnits);
   }
   else if (command == "offset" || command == "position_offset")
   {
      std::string frameIdentifier;
      aInput.ReadCommand(frameIdentifier);
      auto frame = wsf::space::OrbitalReferenceFrameFromIdentifier(frameIdentifier);
      if (frame == wsf::space::OrbitalReferenceFrame::cUNKNOWN)
      {
         std::stringstream err{};
         err << "Unknown position offset frame '" << frameIdentifier << "'.";
         throw UtInput::BadValue(aInput, err.str());
      }
      else if (!ValidOffsetFrame(frame))
      {
         std::stringstream err{};
         err << "Unsupported position offset frame '" << frameIdentifier << "'.";
         throw UtInput::BadValue(aInput, err.str());
      }
      UtVec3d offsetVec;
      for (int i = 0; i < 3; ++i)
      {
         aInput.ReadValue(offsetVec[i]);
      }
      std::string units;
      aInput.ReadValue(units);
      SetPositionOffset(offsetVec, units, frame);
   }
   else if (command == "velocity_offset")
   {
      std::string frameIdentifier;
      aInput.ReadCommand(frameIdentifier);
      auto frame = wsf::space::OrbitalReferenceFrameFromIdentifier(frameIdentifier);
      if (frame == wsf::space::OrbitalReferenceFrame::cUNKNOWN)
      {
         std::stringstream err{};
         err << "Unknown velocity offset frame '" << frameIdentifier << "'.";
         throw UtInput::BadValue(aInput, err.str());
      }
      else if (!ValidOffsetFrame(frame))
      {
         std::stringstream err{};
         err << "Unsupported velocity offset frame '" << frameIdentifier << "'.";
         throw UtInput::BadValue(aInput, err.str());
      }
      UtVec3d offsetVec;
      for (int i = 0; i < 3; ++i)
      {
         aInput.ReadValue(offsetVec[i]);
      }
      std::string units;
      aInput.ReadValue(units);
      SetVelocityOffset(offsetVec, units, frame);
   }
   else if (command == "offset_time")
   {
      aInput.ReadUnitValue(mOffsetTime);
      mHasOffsetTime = true;
   }
   else if (command == "lag_time")
   {
      aInput.ReadUnitValue(mLagTime);
      mHasLagTime = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

std::unique_ptr<OrbitalTargetPoint> TargetPointOptions::InstantiatePoint(const UtCalendar& aEpoch) const
{
   std::unique_ptr<OrbitalTargetPoint> retvalPtr = FormInitialTarget(aEpoch);
   retvalPtr                                     = FormOffsetTarget(std::move(retvalPtr));
   return retvalPtr;
}

bool TargetPointOptions::IsValid() const
{
   bool retval{false};
   switch (mTargetType)
   {
   case Type::cPLATFORM:
      retval = !GetPlatformName().empty();
      break;
   case Type::cTRACK:
      retval = !GetLocalTrackId().IsNull();
      break;
   case Type::cLIBRATION_POINT:
      retval = wsf::space::LibrationPoint::ValidLibrationPoint(GetLP_System(), GetLP_Point());
      break;
   case Type::cPROVIDED_TARGET:
      retval = mTargetPoint != nullptr;
      break;
   case Type::cKINEMATIC_STATE:
      // Any values for the kinematic state are allowed.
      retval = true;
      break;
   default:
      break;
   }
   return retval;
}

UtVec3d TargetPointOptions::GetPositionOffsetInUnits() const
{
   UtVec3d retval;
   for (int i = 0; i < 3; ++i)
   {
      UtLengthValue temp{mPositionOffset[i], UtUnitLength::cMETERS};
      retval[i] = temp.GetAsUnit(mPositionOffsetUnits);
   }
   return retval;
}

UtVec3d TargetPointOptions::GetVelocityOffsetInUnits() const
{
   UtVec3d retval;
   for (int i = 0; i < 3; ++i)
   {
      UtSpeedValue temp{mVelocityOffset[i], UtUnitSpeed::cMETERS_PER_SECOND};
      retval[i] = temp.GetAsUnit(mVelocityOffsetUnits);
   }
   return retval;
}

UtVec3d TargetPointOptions::GetKinematicStatePositionInUnits() const
{
   UtVec3d retval;
   for (int i = 0; i < 3; ++i)
   {
      UtLengthValue temp{mKinematicStatePosition[i], UtUnitLength::cMETERS};
      retval[i] = temp.GetAsUnit(mKinematicStatePositionUnits);
   }
   return retval;
}

UtVec3d TargetPointOptions::GetKinematicStateVelocityInUnits() const
{
   UtVec3d retval;
   for (int i = 0; i < 3; ++i)
   {
      UtSpeedValue temp{mKinematicStateVelocity[i], UtUnitSpeed::cMETERS_PER_SECOND};
      retval[i] = temp.GetAsUnit(mKinematicStateVelocityUnits);
   }
   return retval;
}

void TargetPointOptions::SetPositionOffset(const UtVec3d&                    aOffset,
                                           const std::string&                aOffsetUnits,
                                           wsf::space::OrbitalReferenceFrame aOffsetFrame)
{
   if (ValidOffsetFrame(aOffsetFrame))
   {
      for (int i = 0; i < 3; ++i)
      {
         UtLengthValue component{aOffset[i], aOffsetUnits};
         mPositionOffset[i] = component;
      }
      mPositionOffsetUnits = aOffsetUnits;
      mPositionOffsetFrame = aOffsetFrame;
      mHasPositionOffset   = true;
   }
}

void TargetPointOptions::SetVelocityOffset(const UtVec3d&                    aOffset,
                                           const std::string&                aOffsetUnits,
                                           wsf::space::OrbitalReferenceFrame aOffsetFrame)
{
   if (ValidOffsetFrame(aOffsetFrame))
   {
      for (int i = 0; i < 3; ++i)
      {
         UtSpeedValue component{aOffset[i], aOffsetUnits};
         mVelocityOffset[i] = component;
      }
      mVelocityOffsetUnits = aOffsetUnits;
      mVelocityOffsetFrame = aOffsetFrame;
      mHasVelocityOffset   = true;
   }
}

void TargetPointOptions::ClearPositionOffset()
{
   mPositionOffset      = UtVec3d{};
   mPositionOffsetUnits = std::string{};
   mPositionOffsetFrame = wsf::space::OrbitalReferenceFrame::cUNKNOWN;
   mHasPositionOffset   = false;
}

void TargetPointOptions::ClearVelocityOffset()
{
   mVelocityOffset      = UtVec3d{};
   mVelocityOffsetUnits = std::string{};
   mVelocityOffsetFrame = wsf::space::OrbitalReferenceFrame::cUNKNOWN;
   mHasVelocityOffset   = false;
}

void TargetPointOptions::SetOffsetTime(const UtTimeValue& aTimeOffset)
{
   mOffsetTime    = aTimeOffset;
   mHasOffsetTime = true;
}

void TargetPointOptions::ClearOffsetTime()
{
   mOffsetTime    = 0.0;
   mHasOffsetTime = false;
}

void TargetPointOptions::SetLagTime(const UtTimeValue& aLagTime)
{
   mLagTime    = aLagTime;
   mHasLagTime = true;
}

void TargetPointOptions::ClearLagTime()
{
   mLagTime    = 0.0;
   mHasLagTime = false;
}

void TargetPointOptions::SetPlatformName(const std::string& aPlatformName)
{
   mPlatformName = aPlatformName;
   mTargetType   = Type::cPLATFORM;
   mTargetPoint  = nullptr;
}

void TargetPointOptions::SetLocalTrackId(const WsfTrackId& aTrackId)
{
   mLocalTrackId = aTrackId;
   mTargetType   = Type::cTRACK;
   mTargetPoint  = nullptr;
}

void TargetPointOptions::SetLibrationPoint(LibrationPoint::System aSystem, LibrationPoint::Point aPoint)
{
   mTargetType = Type::cLIBRATION_POINT;
   if (LibrationPoint::ValidLibrationPoint(aSystem, aPoint))
   {
      mLP_System = aSystem;
      mLP_Point  = aPoint;
   }
   mTargetPoint = nullptr;
}

void TargetPointOptions::SetTargetPoint(std::unique_ptr<wsf::space::OrbitalTargetPoint> aTargetPoint)
{
   mTargetType = Type::cPROVIDED_TARGET;
   if (aTargetPoint)
   {
      mTargetPoint = std::move(aTargetPoint);
   }
}

void TargetPointOptions::SetKinematicState(const UtVec3d&     aPositionInertial,
                                           const std::string& aPositionUnits,
                                           const UtVec3d&     aVelocityInertial,
                                           const std::string& aVelocityUnits)
{
   mTargetType = Type::cKINEMATIC_STATE;
   for (int i = 0; i < 3; ++i)
   {
      UtLengthValue length{aPositionInertial[i], aPositionUnits};
      mKinematicStatePosition[i] = length;
      UtSpeedValue speed{aVelocityInertial[i], aVelocityUnits};
      mKinematicStateVelocity[i] = speed;
   }
   mKinematicStatePositionUnits = aPositionUnits;
   mKinematicStateVelocityUnits = aVelocityUnits;
}

bool TargetPointOptions::ValidateParameterRanges(const std::string& aExecutingPlatformName,
                                                 const std::string& aManeuverType) const
{
   bool retval{true};
   if (HasPlatformTarget())
   {
      if (mPlatformName == aExecutingPlatformName)
      {
         auto logger = ut::log::error() << "Event would target the executing platform.";
         logger.AddNote() << "Type: " << aManeuverType;
         logger.AddNote() << "Executing Platform: " << aExecutingPlatformName;
         retval = false;
      }
   }
   else if (HasTrackTarget())
   {
      if (!mLocalTrackId.IsNull() && mLocalTrackId.GetOwningPlatformId() != aExecutingPlatformName)
      {
         auto logger = ut::log::error() << "Event targets a track not owned by executing platform.";
         logger.AddNote() << "Type: " << aManeuverType;
         logger.AddNote() << "Executing Platform: " << aExecutingPlatformName;
         retval = false;
      }
   }
   else if (HasLibrationPointTarget())
   {
      if (!LibrationPoint::ValidLibrationPoint(mLP_System, mLP_Point))
      {
         auto logger = ut::log::error() << "Event targets invalid libration point.";
         logger.AddNote() << "Type: " << aManeuverType;
         logger.AddNote() << "Executing Platform: " << aExecutingPlatformName;
         retval = false;
      }
   }
   else if (HasProvidedTarget())
   {
      if (!mTargetPoint)
      {
         auto logger = ut::log::error() << "Event targets null point.";
         logger.AddNote() << "Type: " << aManeuverType;
         logger.AddNote() << "Executing Platform: " << aExecutingPlatformName;
         retval = false;
      }
   }
   else if (!HasKinematicStateTarget())
   {
      auto logger = ut::log::error() << "Must specify a target platform, track, libration point or kinematic state.";
      logger.AddNote() << "Type: " << aManeuverType;
      retval = false;
   }
   if (HasPositionOffset() && !ValidOffsetFrame(mPositionOffsetFrame))
   {
      auto logger = ut::log::error() << "Must specify a valid position offset frame.";
      logger.AddNote() << "Position offset frame: " << wsf::space::OrbitalReferenceFrameToIdentifier(mPositionOffsetFrame);
      retval = false;
   }
   if (HasVelocityOffset() && !ValidOffsetFrame(mVelocityOffsetFrame))
   {
      auto logger = ut::log::error() << "Must specify a valid velocity offset frame.";
      logger.AddNote() << "Velocity offset frame: " << wsf::space::OrbitalReferenceFrameToIdentifier(mVelocityOffsetFrame);
      retval = false;
   }
   return retval;
}

bool TargetPointOptions::ValidOffsetFrame(wsf::space::OrbitalReferenceFrame aFrame)
{
   return aFrame == wsf::space::OrbitalReferenceFrame::cRIC || aFrame == wsf::space::OrbitalReferenceFrame::cNTW;
}

std::unique_ptr<OrbitalTargetPoint> TargetPointOptions::FormInitialTarget(const UtCalendar& aEpoch) const
{
   std::unique_ptr<OrbitalTargetPoint> retvalPtr{nullptr};

   switch (mTargetType)
   {
   case Type::cPLATFORM: // Intentional fall-through
   case Type::cTRACK:    //
   {
      if (mTargetPropagator)
      {
         mTargetPropagator->Initialize(aEpoch);
         retvalPtr = ut::make_unique<PropagatorTargetPoint>(std::move(mTargetPropagator));
      }
      else
      {
         auto logger = ut::log::error() << "No propagation solution for specified target.";
         logger.AddNote() << "Target propagator must be supplied before forming initial point.";
      }
   }
   break;
   case Type::cLIBRATION_POINT:
   {
      if (wsf::space::LibrationPoint::ValidLibrationPoint(mLP_System, mLP_Point))
      {
         retvalPtr = ut::make_unique<wsf::space::LibrationTargetPoint>(mLP_System, mLP_Point);
      }
      else
      {
         auto logger = ut::log::error() << "Invalid target specification.";
         logger.AddNote() << "Unknown libration point system or point.";
      }
   }
   break;
   case Type::cPROVIDED_TARGET:
      retvalPtr = std::move(mTargetPoint);
      break;
   case Type::cKINEMATIC_STATE:
      retvalPtr = ut::make_unique<wsf::space::KinematicStateTargetPoint>(mKinematicStatePosition, mKinematicStateVelocity);
      break;
   default:
   {
      auto logger = ut::log::error() << "Unknown target type during initialization of Target-class maneuver";
      logger.AddNote() << "Target Type: " << static_cast<int>(mTargetType);
   }
   break;
   }

   return retvalPtr;
}

std::unique_ptr<OrbitalTargetPoint> TargetPointOptions::FormOffsetTarget(std::unique_ptr<OrbitalTargetPoint> aPointPtr) const
{
   std::unique_ptr<OrbitalTargetPoint> retvalPtr{nullptr};

   if ((HasLagTime() || HasOffsetTime()) && !mManeuveringPropagator)
   {
      auto logger = ut::log::error() << "Incomplete target specification.";
      logger.AddNote() << "Lag or offset time requires specification of maneuvering propagator.";
   }
   else
   {
      retvalPtr = std::move(aPointPtr);

      // If there is an offset_time, the initial target needs to be projected forward by the
      // offset_time, the target then needs to be offset, and then it needs to be projected
      // back to the present time. If there is a lag time, that will apply after any of the
      // other targeting. The only complication if there is both is that the final step must
      // sum the offset_time and lag_time (rather than creating two TimeOffsetTargetPoint
      // classes, this creates just one).
      if (retvalPtr && (HasOffsetTime() && HasKinematicOffset()))
      {
         retvalPtr = ut::make_unique<TimeOffsetTargetPoint>(std::move(retvalPtr),
                                                            ut::clone(mManeuveringPropagator.get()),
                                                            -1.0 * mOffsetTime);
      }
      if (retvalPtr && HasKinematicOffset())
      {
         retvalPtr = FormKinematicOffsetTarget(std::move(retvalPtr));
      }
      if (retvalPtr && ((HasOffsetTime() && HasKinematicOffset()) || HasLagTime()))
      {
         retvalPtr = ut::make_unique<TimeOffsetTargetPoint>(std::move(retvalPtr),
                                                            ut::clone(mManeuveringPropagator.get()),
                                                            mOffsetTime + mLagTime);
      }
   }

   return retvalPtr;
}

//! Form an offset target point from the provided base target.
std::unique_ptr<OrbitalTargetPoint>
TargetPointOptions::FormKinematicOffsetTarget(std::unique_ptr<OrbitalTargetPoint> aTargetPtr) const
{
   std::unique_ptr<OrbitalTargetPoint> posPtr{nullptr};
   if (HasPositionOffset())
   {
      switch (mPositionOffsetFrame)
      {
      case wsf::space::OrbitalReferenceFrame::cRIC:
      {
         auto ricTargetPtr = ut::make_unique<RIC_OffsetTargetPoint>(std::move(aTargetPtr));
         ricTargetPtr->SetPositionOffset(GetPositionOffset());
         posPtr = std::move(ricTargetPtr);
         break;
      }
      case wsf::space::OrbitalReferenceFrame::cNTW:
      {
         auto ntwTargetPtr = ut::make_unique<NTW_OffsetTargetPoint>(std::move(aTargetPtr));
         ntwTargetPtr->SetPositionOffset(GetPositionOffset());
         posPtr = std::move(ntwTargetPtr);
         break;
      }
      case wsf::space::OrbitalReferenceFrame::cINERTIAL: // Intentional fall-through
      case wsf::space::OrbitalReferenceFrame::cUNKNOWN:  //
      default:                                           //
         throw std::runtime_error{"Forming offset target when no valid position offset frame is specified."};
         break;
      }
   }
   else
   {
      posPtr = std::move(aTargetPtr);
   }

   std::unique_ptr<OrbitalTargetPoint> retvalPtr{nullptr};
   if (HasVelocityOffset())
   {
      switch (mVelocityOffsetFrame)
      {
      case wsf::space::OrbitalReferenceFrame::cRIC:
      {
         auto ricTargetPtr = ut::make_unique<RIC_OffsetTargetPoint>(std::move(posPtr));
         ricTargetPtr->SetVelocityOffset(GetVelocityOffset());
         retvalPtr = std::move(ricTargetPtr);
         break;
      }
      case wsf::space::OrbitalReferenceFrame::cNTW:
      {
         auto ntwTargetPtr = ut::make_unique<NTW_OffsetTargetPoint>(std::move(posPtr));
         ntwTargetPtr->SetVelocityOffset(GetVelocityOffset());
         retvalPtr = std::move(ntwTargetPtr);
         break;
      }
      case wsf::space::OrbitalReferenceFrame::cINERTIAL: // Intentional fall-through
      case wsf::space::OrbitalReferenceFrame::cUNKNOWN:  //
      default:                                           //
         throw std::runtime_error{"Forming offset target when no valid velocity offset frame is specified."};
         break;
      }
   }
   else
   {
      retvalPtr = std::move(posPtr);
   }

   return retvalPtr;
}

} // namespace space
} // namespace wsf
