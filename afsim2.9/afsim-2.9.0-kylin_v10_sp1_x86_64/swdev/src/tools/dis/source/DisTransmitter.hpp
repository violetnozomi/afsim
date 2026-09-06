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

#ifndef DISTRANSMITTER_HPP
#define DISTRANSMITTER_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisModulationType.hpp"
#include "DisPdu.hpp"
#include "DisRadioEntityType.hpp"

class GenI;
class GenO;

//! Defines information about a radio transmitter
class DIS_EXPORT DisTransmitter : public DisPdu
{
public:
   DisTransmitter();
   DisTransmitter(const DisTransmitter& aSrc);
   DisTransmitter(DisPdu& aPdu, GenI& aGenI);
   DisTransmitter(GenI& aGenI);
   ~DisTransmitter() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   enum // Transmit state
   {
      TsOff                  = 0,
      TsOnAndNotTransmitting = 1,
      TsOnAndTransmitting    = 2
   };

   enum // Input source
   {
      IsOther             = 0,
      IsPilot             = 1,
      IsCopilot           = 2,
      IsFirstOfficer      = 3,
      IsDriver            = 4,
      IsLoader            = 5,
      IsGunner            = 6,
      IsCommander         = 7,
      IsDigitalDataDevice = 8,
      IsIntercom          = 9
   };

   // Accessors

   const DisEntityId& GetOriginatingEntity() const override;

   const DisEntityId&        GetEntityId() const;
   DisUint16                 GetRadioId() const;
   const DisRadioEntityType& GetRadioEntityType() const;
   DisEnum8                  GetTransmitState() const;
   DisEnum8                  GetInputSource() const;
   void                      GetLocation(DisFloat64& aX, DisFloat64& aY, DisFloat64& aZ) const;
   void                      GetRelativeLocation(DisFloat32& aX, DisFloat32& aY, DisFloat32& aZ) const;
   DisEnum16                 GetAntennaPatternType() const;
   DisUint64                 GetFrequency() const;
   DisFloat32                GetFrequencyBandwidth() const;
   DisFloat32                GetPower() const;
   const DisModulationType&  GetModulationType() const;
   DisEnum16                 GetCryptoSystem() const;
   DisUint16                 GetCryptoKeyId() const;

   // GetAntennaPattern and GetModulationParameters return a pointer to the
   // data portion and the length of the data portion in octets (bytes).
   // The caller can use a GenMemIO object to interpret the data in a
   // platform independent manner.

   void GetAntennaData(const unsigned char*& aDataPtr, unsigned int& aDataLength);

   void GetModulationData(const unsigned char*& aDataPtr, unsigned int& aDataLength);

   // Mutators

   void SetEntityId(const DisEntityId& aEntityId);
   void SetRadioId(DisUint16 aRadioId);
   void SetRadioEntityType(const DisRadioEntityType& aRadioEntityType);
   void SetTransmitState(DisEnum8 aTransmitState);
   void SetInputSource(DisEnum8 aInputSource);
   void SetLocation(DisFloat64 aX, DisFloat64 aY, DisFloat64 aZ);
   void SetRelativeLocation(DisFloat32 aX, DisFloat32 aY, DisFloat32 aZ);
   void SetAntennaPatternType(DisEnum16 aAntennaPatternType);
   void SetFrequency(DisUint64 aFrequency);
   void SetFrequencyBandwidth(DisFloat32 aFrequencyBandwidth);
   void SetPower(DisFloat32 aPower);
   void SetModulationType(const DisModulationType& aModulationType);
   void SetCryptoSystem(DisEnum16 aCryptoSystem);
   void SetCryptoKeyId(DisUint16 aCryptoKeyId);

   // SetXXXX allow the caller to replace these data portions of the PDU.
   // The memory pointed to the supplied pointer becomes the property of
   // the object and will be deleted when the objected is deleted or if the
   // pointer is later replaced.

   void SetModulationData(unsigned char* aDataPtr, DisUint8 aDataLength);

   void SetAntennaData(unsigned char* aDataPtr, DisUint16 aDataLength);

   // SetUserXXXX is like SetXXXX except that the memory pointed to
   // by the supplied pointer remains the property of the caller
   // and is NOT deleted when the object is deleted.

   void SetUserModulationData(unsigned char* aDataPtr, DisUint8 aDataLength);

   void SetUserAntennaData(unsigned char* aDataPtr, DisUint16 aDataLength);


   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

   bool IsValid() const override;

private:
   // Disallow assignment
   DisTransmitter& operator=(const DisTransmitter& aRhs);

   void Copy(const DisTransmitter& aRhs);

   void DeleteAntennaData();
   void DeleteModulationData();
   void GetMemberData(GenI& aGenI);

   /*!
    * Identifies the entity or object to which the radio is attached
    */
   DisEntityId mEntityId{DisEntityId::ENTITY_ID_UNKNOWN};
   /*!
    * Identifies a particular radio that is either associated with an entity, object,
    * or is an unattached radio
    */
   DisUint16 mRadioId{0};
   /*!
    * Indicates the type of radio being simulated
    */
   DisRadioEntityType mRadioEntityType{};
   /*!
    * Specifies whether a radio is off, powered but not transmitting,
    * or powered and transmitting
    */
   DisEnum8 mTransmitState{0};
   /*!
    * Specifies which operator position or data port is using the radio
    * associated with the entity, or that it represents an audio jamming source
    */
   DisEnum8 mInputSource{0};
   /*!
    * X location of the transmitter
    */
   DisFloat64 mLocationX{0.0};
   /*!
    * Y location of the transmitter
    */
   DisFloat64 mLocationY{0.0};
   /*!
    * Z location of the transmitter
    */
   DisFloat64 mLocationZ{0.0};
   /*!
    * X relative location of the radiating portion of the antenna with respect
    * to the associated entity
    */
   DisFloat32 mRelativeX{0.0f};
   /*!
    * Y relative location of the radiating portion of the antenna with respect
    * to the associated entity
    */
   DisFloat32 mRelativeY{0.0f};
   /*!
    * Z relative location of the radiating portion of the antenna with respect
    * to the associated entity
    */
   DisFloat32 mRelativeZ{0.0f};
   /*!
    * Specifies the type of representation
    */
   DisEnum16 mAntennaPatternType{0};
   /*!
    * Specify the center frequency being used by the radio for transmission if
    * the radio is not capable of frequency hopping or it is not currently
    * in such a mode. Units are in Hz.
    */
   DisUint64 mFrequency{0};
   /*!
    * Identifies the bandpass (Hz) of the radio
    */
   DisFloat32 mFrequencyBandwidth{0.0f};
   /*!
    * Specifies the power of the radio expressed as the average effective radiated power
    * being transmitted or that would be present if transmitting, in units of decibel-milliwatts.
    */
   DisFloat32 mPower{0.0f};
   /*!
    * Specifies the type of modulation use for radio transmission
    */
   DisModulationType mModulationType{};
   /*!
    * Indicates the encryption capabilities of the transmission regardless of whether
    * it is operating in plain or secure communications node.
    */
   DisEnum16 mCryptoSystem{0};
   /*!
    * Identifies the crypto key
    */
   DisUint16 mCryptoKeyId{0};

   // true if the data is owned by the class and not the user
   bool mAntennaDataIsClassOwned{false};
   bool mModulationDataIsClassOwned{false};

   DisUint16 mAntennaDataLength{0};
   DisUint8  mModulationDataLength{0};

   unsigned char* mAntennaDataPtr{nullptr};
   unsigned char* mModulationDataPtr{nullptr};
};

// Accessors

inline const DisEntityId& DisTransmitter::GetEntityId() const
{
   return mEntityId;
}
inline DisUint16 DisTransmitter::GetRadioId() const
{
   return mRadioId;
}
inline const DisRadioEntityType& DisTransmitter::GetRadioEntityType() const
{
   return mRadioEntityType;
}
inline DisEnum8 DisTransmitter::GetTransmitState() const
{
   return mTransmitState;
}
inline DisEnum8 DisTransmitter::GetInputSource() const
{
   return mInputSource;
}
inline void DisTransmitter::GetLocation(DisFloat64& aX, DisFloat64& aY, DisFloat64& aZ) const
{
   aX = mLocationX;
   aY = mLocationY;
   aZ = mLocationZ;
}
inline void DisTransmitter::GetRelativeLocation(DisFloat32& aX, DisFloat32& aY, DisFloat32& aZ) const
{
   aX = mRelativeX;
   aY = mRelativeY;
   aZ = mRelativeZ;
}
inline DisEnum16 DisTransmitter::GetAntennaPatternType() const
{
   return mAntennaPatternType;
}
inline DisUint64 DisTransmitter::GetFrequency() const
{
   return mFrequency;
}
inline DisFloat32 DisTransmitter::GetFrequencyBandwidth() const
{
   return mFrequencyBandwidth;
}
inline DisFloat32 DisTransmitter::GetPower() const
{
   return mPower;
}
inline const DisModulationType& DisTransmitter::GetModulationType() const
{
   return mModulationType;
}
inline DisEnum16 DisTransmitter::GetCryptoSystem() const
{
   return mCryptoSystem;
}
inline DisUint16 DisTransmitter::GetCryptoKeyId() const
{
   return mCryptoKeyId;
}

// Mutators

inline void DisTransmitter::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}
inline void DisTransmitter::SetRadioId(DisUint16 aRadioId)
{
   mRadioId = aRadioId;
}
inline void DisTransmitter::SetRadioEntityType(const DisRadioEntityType& aRadioEntityType)
{
   mRadioEntityType = aRadioEntityType;
}
inline void DisTransmitter::SetTransmitState(DisEnum8 aTransmitState)
{
   mTransmitState = aTransmitState;
}
inline void DisTransmitter::SetInputSource(DisEnum8 aInputSource)
{
   mInputSource = aInputSource;
}
inline void DisTransmitter::SetLocation(DisFloat64 aX, DisFloat64 aY, DisFloat64 aZ)
{
   mLocationX = aX;
   mLocationY = aY;
   mLocationZ = aZ;
}
inline void DisTransmitter::SetRelativeLocation(DisFloat32 aX, DisFloat32 aY, DisFloat32 aZ)
{
   mRelativeX = aX;
   mRelativeY = aY;
   mRelativeZ = aZ;
}
inline void DisTransmitter::SetAntennaPatternType(DisEnum16 aAntennaPatternType)
{
   mAntennaPatternType = aAntennaPatternType;
}
inline void DisTransmitter::SetFrequency(DisUint64 aFrequency)
{
   mFrequency = aFrequency;
}
inline void DisTransmitter::SetFrequencyBandwidth(DisFloat32 aFrequencyBandwidth)
{
   mFrequencyBandwidth = aFrequencyBandwidth;
}
inline void DisTransmitter::SetPower(DisFloat32 aPower)
{
   mPower = aPower;
}
inline void DisTransmitter::SetModulationType(const DisModulationType& aModulationType)
{
   mModulationType = aModulationType;
}
inline void DisTransmitter::SetCryptoSystem(DisEnum16 aCryptoSystem)
{
   mCryptoSystem = aCryptoSystem;
}
inline void DisTransmitter::SetCryptoKeyId(DisUint16 aCryptoKeyId)
{
   mCryptoKeyId = aCryptoKeyId;
}
#endif
