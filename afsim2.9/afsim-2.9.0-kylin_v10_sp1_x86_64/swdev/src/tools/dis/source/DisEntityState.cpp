// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisEntityState.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

DisEntityState::DisEntityState()
   : DisPdu()
   , mEntityId()
   , mForceId(0)
   , mEntityPartCount(0)
   , mEntityType()
   , mAlternativeEntityType()
   , mAppearance(0)
   , mDeadreckoningAlgorithm(0)
   , mMarkingCharSet(Unused)
   , mCapabilities(0)
{
   SetPduType(DisEnum::Pdu::Type::EntityState);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityInformationInteraction);
   SetLength(144); // 1152 bits

   unsigned int i;

   for (i = 0; i < 3; ++i)
   {
      mVelocity[i]        = 0.0F;
      mLocation[i]        = 0.0;
      mOrientation[i]     = 0.0F;
      mAcceleration[i]    = 0.0F;
      mAngularVelocity[i] = 0.0F;
   }
   for (i = 0; i < 11; ++i)
   {
      mMarkingChars[i] = '\0';
   }
}

DisEntityState::DisEntityState(const DisEntityState& aSrc)
   : DisPdu(aSrc)
   , mEntityId(aSrc.mEntityId)
   , mForceId(aSrc.mForceId)
   , mEntityPartCount(aSrc.mEntityPartCount)
   , mEntityType(aSrc.mEntityType)
   , mAlternativeEntityType(aSrc.mAlternativeEntityType)
   , mAppearance(aSrc.mAppearance)
   , mDeadreckoningAlgorithm(aSrc.mDeadreckoningAlgorithm)
   , mMarkingCharSet(aSrc.mMarkingCharSet)
   , mCapabilities(aSrc.mCapabilities)
   , mEntityPartList(aSrc.mEntityPartList)
{
   unsigned int i;

   for (i = 0; i < 3; ++i)
   {
      mVelocity[i]        = aSrc.mVelocity[i];
      mLocation[i]        = aSrc.mLocation[i];
      mOrientation[i]     = aSrc.mOrientation[i];
      mAcceleration[i]    = aSrc.mAcceleration[i];
      mAngularVelocity[i] = aSrc.mAngularVelocity[i];
   }
   for (i = 0; i < 11; ++i)
   {
      mMarkingChars[i] = aSrc.mMarkingChars[i];
   }
   GetLength(); // Ensure consistent length
}

DisEntityState::DisEntityState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisEntityState::DisEntityState(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisEntityState::~DisEntityState()
{
   RemoveAllEntityParts();
}

// virtual
DisEntityState* DisEntityState::Clone() const
{
   return new DisEntityState(*this);
}

int DisEntityState::GetClass() const
{
   return (int)DisEnum::Pdu::Type::EntityState;
}

int DisEntityState::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::EntityInformationInteraction);
}

// Virtual
const DisEntityId& DisEntityState::GetOriginatingEntity() const
{
   return mEntityId;
}

void DisEntityState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEntityState::GetLength()
{
   mEntityPartCount      = static_cast<DisUint8>(mEntityPartList.GetSize());
   DisUint16 octetLength = 144 + (16 * mEntityPartCount);
   SetLength(octetLength);
   return octetLength;
}

void DisEntityState::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   int       i;
   DisUint32 pad32;
   DisUint16 pad16;
   DisUint8  pad8;
   DisUint32 capabilities;
   DisUint32 appearance;

   RemoveAllEntityParts();

   mEntityId.Get(aGenI);
   aGenI >> mForceId;
   aGenI >> mEntityPartCount;
   mEntityType.Get(aGenI);
   mAlternativeEntityType.Get(aGenI);
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> appearance;
   aGenI >> mDeadreckoningAlgorithm;
   aGenI >> pad32 >> pad32 >> pad32 >> pad16 >> pad8;
   aGenI >> mAcceleration[0] >> mAcceleration[1] >> mAcceleration[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];

   aGenI >> mMarkingCharSet;
   for (i = 0; i < 11; ++i)
   {
      aGenI >> mMarkingChars[i];
   }

   aGenI >> capabilities;

   mAppearance.Set(appearance);
   mCapabilities.Set(capabilities);
   if (mEntityPartCount != 0)
   {
      mEntityPartList.Reserve(mEntityPartCount);
      for (i = 0; i < mEntityPartCount; ++i)
      {
         DisEntityPart* ptr = new DisEntityPart();
         ptr->Get(aGenI);
         mEntityPartList.Add(ptr);
      }
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisEntityState::Put(GenO& aGenO)
{
   DisUint32 pad32 = 0;
   DisUint16 pad16 = 0;
   DisUint8  pad8  = 0;

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   mEntityId.Put(aGenO);
   aGenO << mForceId;
   aGenO << mEntityPartCount;
   mEntityType.Put(aGenO);
   mAlternativeEntityType.Put(aGenO);
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mAppearance.Value();
   aGenO << mDeadreckoningAlgorithm;
   aGenO << pad32 << pad32 << pad32 << pad16 << pad8;
   aGenO << mAcceleration[0] << mAcceleration[1] << mAcceleration[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];

   aGenO << mMarkingCharSet;
   for (int i = 0; i < 11; ++i)
   {
      aGenO << mMarkingChars[i];
   }

   aGenO << mCapabilities.Value();
   for (auto it = mEntityPartList.GetBegin(); it != mEntityPartList.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

std::string DisEntityState::GetEntityPartCountStringValue(int aEnumValue) const
{
   return "No Parts attached";
}

bool DisEntityState::IsValid() const
{
   return DisPdu::IsValid() && mEntityId.IsValid() && mEntityType.IsValid() && mAlternativeEntityType.IsValid() &&
          ValidateScalarArray(mVelocity, 3) && ValidateScalarArray(mLocation, 3) &&
          ValidateScalarArray(mAcceleration, 3) && ValidateScalarArray(mAngularVelocity, 3) &&
          mEntityPartList.IsValid() && ValidateScalar(mOrientation[0]) && !(fabs(mOrientation[0]) > UtMath::cTWO_PI) &&
          ValidateScalar(mOrientation[1]) && !(fabs(mOrientation[1]) > UtMath::cTWO_PI) &&
          ValidateScalar(mOrientation[2]) && !(fabs(mOrientation[2]) > UtMath::cTWO_PI) &&
          !(UtVec3f::Magnitude(mVelocity) > 3000000000.0);
}

std::string DisEntityState::ToString() const
{
   using namespace std;
   using namespace DisEnum;

   DisFloat32 velocityX = 0.0f;
   DisFloat32 velocityY = 0.0f;
   DisFloat32 velocityZ = 0.0f;

   GetVelocity(velocityX, velocityY, velocityZ);

   DisFloat64 locationX = 0.0;
   DisFloat64 locationY = 0.0;
   DisFloat64 locationZ = 0.0;

   GetLocation(locationX, locationY, locationZ);

   DisFloat32 psi   = 0.0f;
   DisFloat32 theta = 0.0f;
   DisFloat32 phi   = 0.0f;

   GetOrientation(psi, theta, phi);

   double hdg   = 0.0f;
   double pitch = 0.0f;
   double roll  = 0.0f;

   UtEntity entity;
   double   position[3] = {locationX, locationY, locationZ};
   entity.SetLocationWCS(position);
   entity.SetOrientationWCS(psi, theta, phi);
   entity.GetOrientationNED(hdg, pitch, roll);
   roll *= UtMath::cDEG_PER_RAD;
   pitch *= UtMath::cDEG_PER_RAD;
   hdg *= UtMath::cDEG_PER_RAD;
   hdg = UtMath::NormalizeAngle0_360(hdg);

   DisFloat32 psid   = psi * (DisFloat32)UtMath::cDEG_PER_RAD;
   DisFloat32 thetad = theta * (DisFloat32)UtMath::cDEG_PER_RAD;
   DisFloat32 phid   = phi * (DisFloat32)UtMath::cDEG_PER_RAD;

   DisFloat32 accelerationX = 0.0f;
   DisFloat32 accelerationY = 0.0f;
   DisFloat32 accelerationZ = 0.0f;

   GetAcceleration(accelerationX, accelerationY, accelerationZ);

   DisFloat32 angularVelocityX = 0.0f;
   DisFloat32 angularVelocityY = 0.0f;
   DisFloat32 angularVelocityZ = 0.0f;

   GetAngularVelocity(angularVelocityX, angularVelocityY, angularVelocityZ);

   DisFloat32 velocityMag = static_cast<float>(
      sqrtf(velocityX * velocityX + velocityY * velocityY + velocityZ * velocityZ) * UtMath::cNMPH_PER_MPS);

   DisFloat64 lat = 0.0;
   DisFloat64 lon = 0.0;
   DisFloat64 alt = 0.0;

   double positionECEF[3] = {locationX, locationY, locationZ};
   UtEllipsoidalEarth::ConvertECEFToLLA(positionECEF, lat, lon, alt);
   alt *= UtMath::cFT_PER_M;

   unsigned int       appearance = static_cast<unsigned int>(GetAppearance());
   std::ostringstream ss("");
   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n';

   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Entity Id:      " << setprecision(0) << GetEntityId().ToString() << '\n'
      << "Force Id:       " << setprecision(0) << static_cast<unsigned int>(GetForceId()) << '\n'
      << "Part Count:     " << setprecision(0) << static_cast<unsigned int>(GetEntityPartCount()) << '\n'
      << "Entity Type:    " << setprecision(0) << GetEntityType().ToString() << '\n'
      << "Alt. Ent. Type: " << setprecision(0) << GetAlternativeEntityType().ToString() << '\n'
      << "Velocity:       " << setprecision(1) << velocityX << ", " << velocityY << ", " << velocityZ << " ("
      << velocityMag << "kts)" << '\n'
      << "Location:       " << setprecision(0) << locationX << ", " << locationY << ", " << locationZ << " m ECEF\n"
      << "                " << setprecision(6) << lat << "N, " << lon << "E, " << setprecision(0) << alt << " ft MSL\n"
      << "Orientation:\n"
      << "    " << setprecision(4) << "ECEF (Phi Theta Psi): " << setw(7) << phi << ", " << setw(7) << theta << ", "
      << setw(7) << psi << " rad\n"
      << "    " << setprecision(2) << "                      " << setw(7) << phid << ", " << setw(7) << thetad << ", "
      << setw(7) << psid << " deg\n"
      << "    " << setprecision(2) << "NED (Roll Pitch Hdg): " << setw(7) << roll << ", " << setw(7) << pitch << ", "
      << setw(7) << hdg << " deg\n";
   ss.imbue(tmp);
   ss << "Appearance:                 " << setprecision(0) << appearance << '\n';

   if (GetEntityType().GetEntityKind() == DisEntityType::EKT_PLATFORM_KIND)
   {
      if (GetEntityType().GetDomain() == DisEntityType::EDT_LAND_DOMAIN)
      {
         DisEntityStateEnums::LandPlatformAppearance* lpa =
            reinterpret_cast<DisEntityStateEnums::LandPlatformAppearance*>(&appearance);
         ss << "   Paint Scheme:             " << static_cast<unsigned int>(lpa->paintScheme) << '\n';
         ss << "   Mobility Kill:            " << static_cast<unsigned int>(lpa->mobilityKill) << '\n';
         ss << "   Fire Power Kill:          " << static_cast<unsigned int>(lpa->firePowerKill) << '\n';
         ss << "   Damage:                   " << static_cast<unsigned int>(lpa->damage) << '\n';
         ss << "   Smoke:                    " << static_cast<unsigned int>(lpa->smoke) << '\n';
         ss << "   Trailing Effects:         " << static_cast<unsigned int>(lpa->trailingEffects) << '\n';
         ss << "   Hatch State:              " << static_cast<unsigned int>(lpa->hatchState) << '\n';
         ss << "   Head Lights On:           " << static_cast<unsigned int>(lpa->headLightsOn) << '\n';
         ss << "   Tail Lights On:           " << static_cast<unsigned int>(lpa->tailLightsOn) << '\n';
         ss << "   Brake Lights On:          " << static_cast<unsigned int>(lpa->brakeLightsOn) << '\n';
         ss << "   Is Flaming:               " << static_cast<unsigned int>(lpa->isFlaming) << '\n';
         ss << "   Launcher Raised:          " << static_cast<unsigned int>(lpa->launcherRaised) << '\n';
         ss << "   Camouflage Type:          " << static_cast<unsigned int>(lpa->comouflageType) << '\n';
         ss << "   Concealed:                " << static_cast<unsigned int>(lpa->concealed) << '\n';
         ss << "   Unused:                   " << static_cast<unsigned int>(lpa->bit20_unused) << '\n';
         ss << "   Frozen Status:            " << static_cast<unsigned int>(lpa->frozenStatus) << '\n';
         ss << "   Powerplant On:            " << static_cast<unsigned int>(lpa->powerPlantOn) << '\n';
         ss << "   Deactivated:              " << static_cast<unsigned int>(lpa->deactivated) << '\n';
         ss << "   Tent Extended:            " << static_cast<unsigned int>(lpa->tentExtended) << '\n';
         ss << "   Ramp Down:                " << static_cast<unsigned int>(lpa->rampDown) << '\n';
         ss << "   Blackout Lights On:       " << static_cast<unsigned int>(lpa->blackoutLightsOn) << '\n';
         ss << "   Blackout Brake Lights On: " << static_cast<unsigned int>(lpa->blackoutBreakLightsOn) << '\n';
         ss << "   Spotlights On:            " << static_cast<unsigned int>(lpa->spotLightsOn) << '\n';
         ss << "   Interior Lights On:       " << static_cast<unsigned int>(lpa->interiorLightsOn) << '\n';
         ss << "   Surrender State:          " << static_cast<unsigned int>(lpa->surrenderState) << '\n';
         ss << "   Unused:                   " << static_cast<unsigned int>(lpa->bit31_unused) << '\n';
      }
      else if (GetEntityType().GetDomain() == DisEntityType::EDT_AIR_DOMAIN)
      {
         DisEntityStateEnums::AirPlatformAppearance* apa =
            reinterpret_cast<DisEntityStateEnums::AirPlatformAppearance*>(&appearance);
         ss << "   Paint Scheme:            " << static_cast<unsigned int>(apa->paintScheme) << '\n';
         ss << "   Propulsion Kill:         " << static_cast<unsigned int>(apa->propulsionKill) << '\n';
         ss << "   Unused:                  " << static_cast<unsigned int>(apa->bit2_unused) << '\n';
         ss << "   Damage:                  " << static_cast<unsigned int>(apa->damage) << '\n';
         ss << "   Smoke:                   " << static_cast<unsigned int>(apa->smoke) << '\n';
         ss << "   Trailing Effects:        " << static_cast<unsigned int>(apa->trailingEffects) << '\n';
         ss << "   Canopy State:            " << static_cast<unsigned int>(apa->canopyState) << '\n';
         ss << "   Landing Lights On:       " << static_cast<unsigned int>(apa->landingLightsOn) << '\n';
         ss << "   Navigation Lights On:    " << static_cast<unsigned int>(apa->navigationLightsOn) << '\n';
         ss << "   AntiCollision Lights On: " << static_cast<unsigned int>(apa->antiCollisionLightsOn) << '\n';
         ss << "   Is Flaming:              " << static_cast<unsigned int>(apa->isFlaming) << '\n';
         ss << "   Afterburner On:          " << static_cast<unsigned int>(apa->afterburnerOn) << '\n';
         ss << "   Sig Db Index:            " << static_cast<unsigned int>(apa->sig_db_index) << '\n';
         ss << "   Frozen Status:           " << static_cast<unsigned int>(apa->frozenStatus) << '\n';
         ss << "   Powerplant On:           " << static_cast<unsigned int>(apa->powerPlantOn) << '\n';
         ss << "   Deactivated:             " << static_cast<unsigned int>(apa->deactivated) << '\n';
         ss << "   Formation Lights:        " << static_cast<unsigned int>(apa->formationLights) << '\n';
         ss << "   Unused:                  " << static_cast<unsigned int>(apa->bits25_27_unused) << '\n';
         ss << "   Spotlights On:           " << static_cast<unsigned int>(apa->spotLightsOn) << '\n';
         ss << "   Interior Lights On:      " << static_cast<unsigned int>(apa->interiorLightsOn) << '\n';
         ss << "   Unused:                  " << static_cast<unsigned int>(apa->bits30_31_unused) << '\n';
      }
      else if (GetEntityType().GetDomain() == DisEntityType::EDT_SURFACE_DOMAIN)
      {
         DisEntityStateEnums::SurfacePlatformAppearance* spa =
            reinterpret_cast<DisEntityStateEnums::SurfacePlatformAppearance*>(&appearance);
         ss << "   Paint Scheme:       " << static_cast<unsigned int>(spa->paintScheme) << '\n';
         ss << "   Mobility Kill:      " << static_cast<unsigned int>(spa->mobilityKill) << '\n';
         ss << "   Unused:             " << static_cast<unsigned int>(spa->bit2_unused) << '\n';
         ss << "   Damage:             " << static_cast<unsigned int>(spa->damage) << '\n';
         ss << "   Smoke:              " << static_cast<unsigned int>(spa->smoke) << '\n';
         ss << "   Trailing Effects:   " << static_cast<unsigned int>(spa->trailingEffects) << '\n';
         ss << "   Unused:             " << static_cast<unsigned int>(spa->bits9_11_unused) << '\n';
         ss << "   Running Lights On:  " << static_cast<unsigned int>(spa->runningLightsOn) << '\n';
         ss << "   Unused:             " << static_cast<unsigned int>(spa->bits13_14_unused) << '\n';
         ss << "   Is Flaming:         " << static_cast<unsigned int>(spa->isFlaming) << '\n';
         ss << "   Bit 16:             " << static_cast<unsigned int>(spa->bit16) << '\n';
         ss << "   Sig Db Index:       " << static_cast<unsigned int>(spa->sig_db_index) << '\n';
         ss << "   Frozen Status:      " << static_cast<unsigned int>(spa->frozenStatus) << '\n';
         ss << "   Powerplant On:      " << static_cast<unsigned int>(spa->powerPlantOn) << '\n';
         ss << "   Deactivated:        " << static_cast<unsigned int>(spa->deactivated) << '\n';
         ss << "   Unused:             " << static_cast<unsigned int>(spa->bits24_27_unused) << '\n';
         ss << "   Spotlights On:      " << static_cast<unsigned int>(spa->spotLightsOn) << '\n';
         ss << "   Interior Lights On: " << static_cast<unsigned int>(spa->interiorLightsOn) << '\n';
         ss << "   Unused:             " << static_cast<unsigned int>(spa->bits30_31_unused) << '\n';
      }
      else if (GetEntityType().GetDomain() == DisEntityType::EDT_SUBSURFACE_DOMAIN)
      {
         DisEntityStateEnums::SubSurfacePlatformAppearance* spa =
            reinterpret_cast<DisEntityStateEnums::SubSurfacePlatformAppearance*>(&appearance);
         ss << "   Paint Scheme:      " << static_cast<unsigned int>(spa->paintScheme) << '\n';
         ss << "   Mobility Kill:     " << static_cast<unsigned int>(spa->mobilityKill) << '\n';
         ss << "   Unused:            " << static_cast<unsigned int>(spa->bit2_unused) << '\n';
         ss << "   Damage:            " << static_cast<unsigned int>(spa->damage) << '\n';
         ss << "   Smoke:             " << static_cast<unsigned int>(spa->smoke) << '\n';
         ss << "   Unused:            " << static_cast<unsigned int>(spa->bits7_8_unused) << '\n';
         ss << "   Hatch State:       " << static_cast<unsigned int>(spa->hatchState) << '\n';
         ss << "   Running Lights On: " << static_cast<unsigned int>(spa->runningLightsOn) << '\n';
         ss << "   Unused:            " << static_cast<unsigned int>(spa->bits13_14_unused) << '\n';
         ss << "   Is Flaming:        " << static_cast<unsigned int>(spa->isFlaming) << '\n';
         ss << "   Bit 16:            " << static_cast<unsigned int>(spa->bit16) << '\n';
         ss << "   Sig Db Index:      " << static_cast<unsigned int>(spa->sig_db_index) << '\n';
         ss << "   Frozen Status:     " << static_cast<unsigned int>(spa->frozenStatus) << '\n';
         ss << "   Powerplant On:     " << static_cast<unsigned int>(spa->powerPlantOn) << '\n';
         ss << "   Deactivated:       " << static_cast<unsigned int>(spa->deactivated) << '\n';
         ss << "   Unused:            " << static_cast<unsigned int>(spa->bits24_31_unused) << '\n';
      }
      else if (GetEntityType().GetDomain() == DisEntityType::EDT_SPACE_DOMAIN)
      {
         DisEntityStateEnums::SpacePlatformAppearance* spa =
            reinterpret_cast<DisEntityStateEnums::SpacePlatformAppearance*>(&appearance);
         ss << "   Paint Scheme:  " << static_cast<unsigned int>(spa->paintScheme) << '\n';
         ss << "   Mobility Kill: " << static_cast<unsigned int>(spa->mobilityKill) << '\n';
         ss << "   Unused:        " << static_cast<unsigned int>(spa->bit2_unused) << '\n';
         ss << "   Damage:        " << static_cast<unsigned int>(spa->damage) << '\n';
         ss << "   Smoke:         " << static_cast<unsigned int>(spa->smoke) << '\n';
         ss << "   Unused:        " << static_cast<unsigned int>(spa->bits7_14_unused) << '\n';
         ss << "   Is Flaming:    " << static_cast<unsigned int>(spa->isFlaming) << '\n';
         ss << "   Bit 16:        " << static_cast<unsigned int>(spa->bit16) << '\n';
         ss << "   Sig Db Index:  " << static_cast<unsigned int>(spa->sig_db_index) << '\n';
         ss << "   Frozen Status: " << static_cast<unsigned int>(spa->frozenStatus) << '\n';
         ss << "   Powerplant On: " << static_cast<unsigned int>(spa->powerPlantOn) << '\n';
         ss << "   Deactivated:   " << static_cast<unsigned int>(spa->deactivated) << '\n';
         ss << "   Unused:        " << static_cast<unsigned int>(spa->bits24_31_unused) << '\n';
      }
   }

   ss.imbue(locale(""));
   ss << "Dead Reck.:    " << setprecision(0) << static_cast<unsigned int>(GetDeadreckoningAlgorithm()) << '\n'
      << "Acceleration:  " << setprecision(2) << accelerationX << ", " << accelerationY << ", " << accelerationZ
      << " (m/s^2)\n"
      << "Ang. Velocity: " << setprecision(2) << angularVelocityX << ", " << angularVelocityY << ", "
      << angularVelocityZ << " (rad/s)\n"
      << "               " << setprecision(2) << angularVelocityX * UtMath::cDEG_PER_RAD << ", "
      << angularVelocityY * UtMath::cDEG_PER_RAD << ", " << angularVelocityZ * UtMath::cDEG_PER_RAD << " (deg/s)\n"
      << "Marking Set:   " << setprecision(0) << static_cast<unsigned int>(GetMarkingCharSet()) << '\n'
      << "Capabilities:  " << setprecision(0) << static_cast<unsigned int>(GetCapabilities());
   ss.imbue(tmp);
   return ss.str();
}
