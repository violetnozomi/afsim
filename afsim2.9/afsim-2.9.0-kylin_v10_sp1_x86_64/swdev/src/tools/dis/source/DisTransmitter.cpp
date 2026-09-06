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

#include "DisTransmitter.hpp"

#include <cassert>

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"

DisTransmitter::DisTransmitter()
   : DisPdu()
   , mEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRadioId(0)
   , mRadioEntityType()
   , mTransmitState(0)
   , mInputSource(0)
   , mLocationX(0.0)
   , mLocationY(0.0)
   , mLocationZ(0.0)
   , mRelativeX(0.0F)
   , mRelativeY(0.0F)
   , mRelativeZ(0.0F)
   , mAntennaPatternType(0)
   , mFrequency(0)
   , mFrequencyBandwidth(0.0F)
   , mPower(0.0F)
   , mModulationType()
   , mCryptoSystem(0)
   , mCryptoKeyId(0)
   , mAntennaDataIsClassOwned(false)
   , mModulationDataIsClassOwned(false)
   , mAntennaDataLength(0)
   , mModulationDataLength(0)
   , mAntennaDataPtr(nullptr)
   , mModulationDataPtr(nullptr)
{
   SetPduType(DisEnum::Pdu::Type::Transmitter);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(104); // 832 bits
}

DisTransmitter::DisTransmitter(const DisTransmitter& aSrc)
   : DisPdu(aSrc)
   , mEntityId(aSrc.mEntityId)
   , mRadioId(aSrc.mRadioId)
   , mRadioEntityType(aSrc.mRadioEntityType)
   , mTransmitState(aSrc.mTransmitState)
   , mInputSource(aSrc.mInputSource)
   , mLocationX(aSrc.mLocationX)
   , mLocationY(aSrc.mLocationY)
   , mLocationZ(aSrc.mLocationZ)
   , mRelativeX(aSrc.mRelativeX)
   , mRelativeY(aSrc.mRelativeY)
   , mRelativeZ(aSrc.mRelativeZ)
   , mAntennaPatternType(aSrc.mAntennaPatternType)
   , mFrequency(aSrc.mFrequency)
   , mFrequencyBandwidth(aSrc.mFrequencyBandwidth)
   , mPower(aSrc.mPower)
   , mModulationType(aSrc.mModulationType)
   , mCryptoSystem(aSrc.mCryptoSystem)
   , mCryptoKeyId(aSrc.mCryptoKeyId)
   , mAntennaDataIsClassOwned(aSrc.mAntennaDataIsClassOwned)
   , mModulationDataIsClassOwned(aSrc.mModulationDataIsClassOwned)
   , mAntennaDataLength(aSrc.mAntennaDataLength)
   , mModulationDataLength(aSrc.mModulationDataLength)
   , mAntennaDataPtr(nullptr)
   , mModulationDataPtr(nullptr)
{
   if (aSrc.mAntennaDataLength != 0)
   {
      mAntennaDataPtr = new unsigned char[mAntennaDataLength];
      memcpy(mAntennaDataPtr, aSrc.mAntennaDataPtr, mAntennaDataLength);
      mAntennaDataIsClassOwned = true;
   }

   if (aSrc.mModulationDataLength != 0)
   {
      mModulationDataPtr = new unsigned char[mModulationDataLength];
      memcpy(mModulationDataPtr, aSrc.mModulationDataPtr, mModulationDataLength);
      mModulationDataIsClassOwned = true;
   }
}

DisTransmitter::DisTransmitter(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mAntennaDataIsClassOwned(false)
   , mModulationDataIsClassOwned(false)
   , mAntennaDataLength(0)
   , mModulationDataLength(0)
   , mAntennaDataPtr(nullptr)
   , mModulationDataPtr(nullptr)
{
   GetMemberData(aGenI);
}

DisTransmitter::DisTransmitter(GenI& aGenI)
   : DisPdu(aGenI)
   , mAntennaDataIsClassOwned(false)
   , mModulationDataIsClassOwned(false)
   , mAntennaDataLength(0)
   , mModulationDataLength(0)
   , mAntennaDataPtr(nullptr)
   , mModulationDataPtr(nullptr)
{
   GetMemberData(aGenI);
}

void DisTransmitter::Copy(const DisTransmitter& aSrc)
{
   mEntityId                   = aSrc.mEntityId;
   mRadioId                    = aSrc.mRadioId;
   mRadioEntityType            = aSrc.mRadioEntityType;
   mTransmitState              = aSrc.mTransmitState;
   mInputSource                = aSrc.mInputSource;
   mLocationX                  = aSrc.mLocationX;
   mLocationY                  = aSrc.mLocationY;
   mLocationZ                  = aSrc.mLocationZ;
   mRelativeX                  = aSrc.mRelativeX;
   mRelativeY                  = aSrc.mRelativeY;
   mRelativeZ                  = aSrc.mRelativeZ;
   mAntennaPatternType         = aSrc.mAntennaPatternType;
   mFrequency                  = aSrc.mFrequency;
   mFrequencyBandwidth         = aSrc.mFrequencyBandwidth;
   mPower                      = aSrc.mPower;
   mModulationType             = aSrc.mModulationType;
   mCryptoSystem               = aSrc.mCryptoSystem;
   mCryptoKeyId                = aSrc.mCryptoKeyId;
   mAntennaDataLength          = aSrc.mAntennaDataLength;
   mModulationDataLength       = aSrc.mModulationDataLength;
   mAntennaDataIsClassOwned    = false;
   mModulationDataIsClassOwned = false;

   if (aSrc.mAntennaDataLength != 0)
   {
      mAntennaDataPtr = new unsigned char[mAntennaDataLength];
      memcpy(mAntennaDataPtr, aSrc.mAntennaDataPtr, mAntennaDataLength);
      mAntennaDataIsClassOwned = true;
   }

   if (aSrc.mModulationDataLength != 0)
   {
      mModulationDataPtr = new unsigned char[mModulationDataLength];
      memcpy(mModulationDataPtr, aSrc.mModulationDataPtr, mModulationDataLength);
      mModulationDataIsClassOwned = true;
   }
}

DisTransmitter& DisTransmitter::operator=(const DisTransmitter& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      DeleteAntennaData();
      DeleteModulationData();
      Copy(aRhs);
   }
   return *this;
}

// virtual
DisTransmitter::~DisTransmitter()
{
   DeleteAntennaData();
   DeleteModulationData();
}

// virtual
DisPdu* DisTransmitter::Clone() const
{
   return new DisTransmitter(*this);
}

// virtual
const DisEntityId& DisTransmitter::GetOriginatingEntity() const
{
   return mEntityId;
}

// virtual
int DisTransmitter::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Transmitter;
}

// private
void DisTransmitter::DeleteAntennaData()
{
   if (mAntennaDataPtr != nullptr)
   {
      if (mAntennaDataIsClassOwned)
      {
         delete[] mAntennaDataPtr;
      }
      mAntennaDataPtr = nullptr;
   }
   mAntennaDataLength       = 0;
   mAntennaDataIsClassOwned = false;
}

// private
void DisTransmitter::DeleteModulationData()
{
   if (mModulationDataPtr != nullptr)
   {
      if (mModulationDataIsClassOwned)
      {
         delete[] mModulationDataPtr;
      }
      mModulationDataPtr = nullptr;
   }
   mModulationDataLength       = 0;
   mModulationDataIsClassOwned = false;
}

// GetData returns a pointer to the data portion and the number
// of bytes in the data portion.  The caller can use a GenMemIO
// object to interpret the data in a platform independent manner.

void DisTransmitter::GetAntennaData(const unsigned char*& aDataPtr, unsigned int& aDataLength)
{
   aDataPtr    = mAntennaDataPtr;
   aDataLength = mAntennaDataLength;
}

void DisTransmitter::GetModulationData(const unsigned char*& aDataPtr, unsigned int& aDataLength)
{
   aDataPtr    = mModulationDataPtr;
   aDataLength = mModulationDataLength;
}

// SetData allows the caller to replace the data portion of the
// PDU.  The memory pointed to the supplied pointer becomes the
// property of the object and will be deleted when the objected
// is deleted or if the pointer is later replaced.

void DisTransmitter::SetAntennaData(unsigned char* aDataPtr, DisUint16 aDataLength)
{
   // Verify consistency of data pointer and length.
   assert(((aDataPtr != nullptr) && (aDataLength != 0)) || ((aDataPtr == nullptr) && (aDataLength == 0)));

   DeleteAntennaData();
   mAntennaDataPtr          = aDataPtr;
   mAntennaDataLength       = aDataLength;
   mAntennaDataIsClassOwned = true;
}

void DisTransmitter::SetModulationData(unsigned char* aDataPtr, DisUint8 aDataLength)
{
   // Verify consistency of data pointer and length.
   assert(((aDataPtr != nullptr) && (aDataLength != 0)) || ((aDataPtr == nullptr) && (aDataLength == 0)));

   DeleteModulationData();
   mModulationDataPtr          = aDataPtr;
   mModulationDataLength       = aDataLength;
   mModulationDataIsClassOwned = true;
}

void DisTransmitter::SetUserAntennaData(unsigned char* aDataPtr, DisUint16 aDataLength)
{
   // Verify consistency of data pointer and length.
   assert(((aDataPtr != nullptr) && (aDataLength != 0)) || ((aDataPtr == nullptr) && (aDataLength == 0)));

   DeleteAntennaData();
   mAntennaDataPtr          = aDataPtr;
   mAntennaDataLength       = aDataLength;
   mAntennaDataIsClassOwned = false;
}

void DisTransmitter::SetUserModulationData(unsigned char* aDataPtr, DisUint8 aDataLength)
{
   // Verify consistency of data pointer and length.
   assert(((aDataPtr != nullptr) && (aDataLength != 0)) || ((aDataPtr == nullptr) && (aDataLength == 0)));

   DeleteModulationData();
   mModulationDataPtr          = aDataPtr;
   mModulationDataLength       = aDataLength;
   mModulationDataIsClassOwned = false;
}

// virtual
void DisTransmitter::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisTransmitter::GetMemberData(aGenI);
}

// virtual
DisUint16 DisTransmitter::GetLength()
{
   DisUint16 octetCount = 104 + mModulationDataLength + mAntennaDataLength;
   SetLength(octetCount);
   return octetCount;
}

// private
void DisTransmitter::GetMemberData(GenI& aGenI)
{
   DisUint8  pad8(0);
   DisUint16 pad16(0);

   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DeleteAntennaData(); // Release existing class-owned data
   DeleteModulationData();

   aGenI >> mEntityId;
   aGenI >> mRadioId;
   aGenI >> mRadioEntityType;
   aGenI >> mTransmitState;
   aGenI >> mInputSource;
   aGenI >> pad16;
   aGenI >> mLocationX >> mLocationY >> mLocationZ;
   aGenI >> mRelativeX >> mRelativeY >> mRelativeZ;
   aGenI >> mAntennaPatternType;
   aGenI >> mAntennaDataLength;
   aGenI >> mFrequency;
   aGenI >> mFrequencyBandwidth;
   aGenI >> mPower;
   aGenI >> mModulationType;
   aGenI >> mCryptoSystem;
   aGenI >> mCryptoKeyId;
   aGenI >> mModulationDataLength;
   aGenI >> pad8 >> pad16;

   // Read modulation and antenna data parameters.  Note that we take special
   // care to ensure we don't try to read past the end of the data in the case
   // we get sent a bad PDU.  If we do get sent a bad PDU and subsequently
   // try to send it, we will send enough data to match what was supposed to
   // be sent...

   DisUint16 lengthRead = 104; // 832 bits
   if (mModulationDataLength != 0)
   {
      mModulationDataIsClassOwned = true;
      mModulationDataPtr          = new unsigned char[mModulationDataLength];
      DisUint16 dataLength        = 0;
      if (lengthRead < lengthToRead)
      {
         dataLength = lengthToRead - dataLength;
         if (dataLength > mModulationDataLength)
         {
            dataLength = mModulationDataLength;
         }
      }
      aGenI.GetArray(mModulationDataPtr, dataLength);
      lengthRead += dataLength;
   }

   if (mAntennaDataLength != 0)
   {
      mAntennaDataIsClassOwned = true;
      mAntennaDataPtr          = new unsigned char[mAntennaDataLength];
      DisUint16 dataLength     = 0;
      if (lengthRead < lengthToRead)
      {
         dataLength = lengthToRead - dataLength;
         if (dataLength > mAntennaDataLength)
         {
            dataLength = mAntennaDataLength;
         }
      }
      aGenI.GetArray(mAntennaDataPtr, dataLength);
      lengthRead += dataLength;
   }

   // Skip 'extra' data
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisTransmitter::Put(GenO& aGenO)
{
   DisUint8  pad8(0);
   DisUint16 pad16(0);

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mEntityId;
   aGenO << mRadioId;
   aGenO << mRadioEntityType;
   aGenO << mTransmitState;
   aGenO << mInputSource;
   aGenO << pad16;
   aGenO << mLocationX << mLocationY << mLocationZ;
   aGenO << mRelativeX << mRelativeY << mRelativeZ;
   aGenO << mAntennaPatternType;
   aGenO << mAntennaDataLength;
   aGenO << mFrequency;
   aGenO << mFrequencyBandwidth;
   aGenO << mPower;
   aGenO << mModulationType;
   aGenO << mCryptoSystem;
   aGenO << mCryptoKeyId;
   aGenO << mModulationDataLength;
   aGenO << pad8 << pad16;

   if (mModulationDataLength != 0)
   {
      aGenO.PutArray(mModulationDataPtr, mModulationDataLength);
   }

   if (mAntennaDataLength != 0)
   {
      aGenO.PutArray(mAntennaDataPtr, mAntennaDataLength);
   }
}

void DisTransmitter::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisTransmitter::ToString() const
{
   using namespace std;
   std::ostringstream  ss("");
   static const string UNITS[] = {" Hz)", " KHz)", " MHz)", " GHz)", " THz)"};

   static const string ANT_PATTERN_TYPES[] = {" (Omni-directional)\n", " (Beam)\n", " (Spherical harmonic)\n"};

   DisFloat64 locationX = 0.0;
   DisFloat64 locationY = 0.0;
   DisFloat64 locationZ = 0.0;

   GetLocation(locationX, locationY, locationZ);

   DisFloat32 relativeLocationX = 0.0f;
   DisFloat32 relativeLocationY = 0.0f;
   DisFloat32 relativeLocationZ = 0.0f;

   GetRelativeLocation(relativeLocationX, relativeLocationY, relativeLocationZ);

   DisFloat64 lat = 0.0;
   DisFloat64 lon = 0.0;
   DisFloat64 alt = 0.0;

   double positionECEF[3] = {locationX, locationY, locationZ};
   UtEllipsoidalEarth::ConvertECEFToLLA(positionECEF, lat, lon, alt);
   alt *= UtMath::cFT_PER_M;

   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n'
      << "Orig. Entity:    " << setprecision(0) << GetOriginatingEntity().ToString() << '\n'
      << "Entity Id:       " << setprecision(0) << GetEntityId().ToString() << '\n'
      << "Radio Id:        " << setprecision(0) << static_cast<unsigned int>(GetRadioId()) << '\n'
      << "Radio Ent. Kind: " << setprecision(0) << static_cast<unsigned int>(GetRadioEntityType().GetEntityKind()) << '\n'
      << "Domain:          " << setprecision(0) << static_cast<unsigned int>(GetRadioEntityType().GetDomain()) << '\n'
      << "Country:         " << setprecision(0) << static_cast<unsigned int>(GetRadioEntityType().GetCountry()) << '\n'
      << "Category:        " << setprecision(0) << static_cast<unsigned int>(GetRadioEntityType().GetCategory()) << '\n'
      << "Nomenclature V.: " << setprecision(0)
      << static_cast<unsigned int>(GetRadioEntityType().GetNomenclatureVersion()) << '\n'
      << "Nomenclature:    " << setprecision(0) << static_cast<unsigned int>(GetRadioEntityType().GetNomenclature()) << '\n'
      << "Transmit State:  " << setprecision(0) << static_cast<unsigned int>(GetTransmitState()) << " ";

   switch (static_cast<unsigned int>(GetTransmitState()))
   {
   case DisTransmitter::TsOff:
      ss << "(Off)\n";
      break;
   case DisTransmitter::TsOnAndNotTransmitting:
      ss << "(On and not transmitting)\n";
      break;
   case DisTransmitter::TsOnAndTransmitting:
      ss << "(On and transmitting)\n";
      break;
   default:
      ss << "(Unknown)\n";
      break;
   }

   ss << "Input source:    " << setprecision(0) << static_cast<unsigned int>(GetInputSource()) << " ";

   switch (static_cast<unsigned int>(GetInputSource()))
   {
   case DisTransmitter::IsOther:
      ss << "(Other)\n";
      break;
   case DisTransmitter::IsPilot:
      ss << "(Pilot)\n";
      break;
   case DisTransmitter::IsCopilot:
      ss << "(CoPilot)\n";
      break;
   case DisTransmitter::IsFirstOfficer:
      ss << "(FirstOfficer)\n";
      break;
   case DisTransmitter::IsDriver:
      ss << "(Driver)\n";
      break;
   case DisTransmitter::IsLoader:
      ss << "(Loader)\n";
      break;
   case DisTransmitter::IsGunner:
      ss << "(Gunner)\n";
      break;
   case DisTransmitter::IsCommander:
      ss << "(Commander)\n";
      break;
   case DisTransmitter::IsDigitalDataDevice:
      ss << "(Digital Data Device)\n";
      break;
   case DisTransmitter::IsIntercom:
      ss << "(Intercom)\n";
      break;
   default:
      ss << "(Unknown)\n";
      break;
   }

   double       frequency  = static_cast<double>(GetFrequency());
   double       freq_short = frequency;
   unsigned int freq_units = 0;
   while (freq_short > 1000.0 && freq_units < 5)
   {
      freq_short /= 1000.0;
      ++freq_units;
   }
   double       range       = GetFrequencyBandwidth();
   double       range_short = range;
   unsigned int range_units = 0;
   while (range_short > 1000.0 && freq_units < 5)
   {
      range_short /= 1000.0;
      ++range_units;
   }
   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "Location:       " << fixed << setprecision(0) << locationX << ", " << locationY << ", " << locationZ
      << " m ECEF\n"
      << "                " << fixed << setprecision(6) << lat << "N, " << lon << "E, " << setprecision(0) << alt
      << " ft MSL\n"
      << "Rel. Location:  " << setprecision(6) << relativeLocationX << ", " << relativeLocationY << ", "
      << relativeLocationZ << " m ECS\n"
      << "Ant. Pat. Type: " << setprecision(0) << static_cast<unsigned int>(GetAntennaPatternType());
   if (GetAntennaPatternType() < 4)
   {
      ss << ANT_PATTERN_TYPES[GetAntennaPatternType()];
   }
   ss << '\n';

   if (freq_units > 0)
   {
      ss << "Frequency:       " << fixed << setprecision(0) << frequency << " Hz (" << setprecision(3) << freq_short
         << UNITS[freq_units] << '\n';
   }
   else
   {
      ss << "Frequency:       " << fixed << setprecision(0) << frequency << " Hz\n";
   }
   if (range_units > 0)
   {
      ss << "Freq. Bwidth:    " << fixed << setprecision(0) << range << " Hz (" << setprecision(3) << range_short
         << UNITS[range_units] << '\n';
   }
   else
   {
      ss << "Freq. Bwidth:    " << fixed << setprecision(0) << frequency << " Hz\n";
   }
   float power_dBm = GetPower();
   float power_W   = (float)pow(10.0, ((power_dBm - 30.0) / 10.0));
   ss << "Power:           " << fixed << setprecision(3) << power_dBm << " dBm (" << setprecision(1) << power_W << " W)\n"
      << "Spread Spectrum: " << setprecision(0) << static_cast<unsigned int>(GetModulationType().GetSpreadSpectrum())
      << '\n'
      << "Major:           " << setprecision(0) << static_cast<unsigned int>(GetModulationType().GetMajor()) << '\n'
      << "Detail:          " << setprecision(0) << static_cast<unsigned int>(GetModulationType().GetDetail()) << '\n'
      << "System:          " << setprecision(0) << static_cast<unsigned int>(GetModulationType().GetSystem()) << '\n'
      << "Crypto System:   " << setprecision(0) << static_cast<unsigned int>(GetCryptoSystem()) << '\n'
      << "Crypto Key Id:   " << setprecision(0) << static_cast<unsigned int>(GetCryptoKeyId());
   ss.imbue(tmp);

   return ss.str();
}

bool DisTransmitter::IsValid() const
{
   bool isValid = true;
   if (!DisPdu::IsValid())
   {
      isValid = false;
   }
   if (!mEntityId.IsValid())
   {
      isValid = false;
   }
   if (!mRadioEntityType.IsValid())
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocationX))
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocationY))
   {
      isValid = false;
   }
   if (!ValidateScalar(mLocationZ))
   {
      isValid = false;
   }
   if (!ValidateScalar(mRelativeX))
   {
      isValid = false;
   }
   if (!ValidateScalar(mRelativeY))
   {
      isValid = false;
   }
   if (!ValidateScalar(mRelativeZ))
   {
      isValid = false;
   }
   if (!ValidateScalar(static_cast<double>(mFrequency)))
   {
      isValid = false;
   }
   if (!ValidateScalar(mFrequencyBandwidth))
   {
      isValid = false;
   }
   if (!ValidateScalar(mPower))
   {
      isValid = false;
   }
   if (!mModulationType.IsValid())
   {
      isValid = false;
   }
   return isValid;
}
