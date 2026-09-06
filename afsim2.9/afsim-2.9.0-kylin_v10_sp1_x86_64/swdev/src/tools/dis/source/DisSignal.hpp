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

#ifndef DISSIGNAL_HPP
#define DISSIGNAL_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

//! Defines information for transmission of voice, audio, or other data.
class DIS_EXPORT DisSignal : public DisPdu
{
public:
   DisSignal();
   DisSignal(const DisSignal& aSrc);
   DisSignal(DisPdu& aPdu, GenI& aGenI);
   DisSignal(GenI& aGenI);
   ~DisSignal() override;

   DisPdu* Clone() const override;

   int                  GetClass() const override;
   virtual unsigned int IsValid();

   // Enumerations for radio encoding class encoding class (EBV-DOC 9.2.1)
   enum
   {
      EcEncodedAudio        = 0x0000, // bits 15, 14 = (0,0)
      EcRawBinary           = 0x4000, // bits 15, 14 = (0,1)
      EcApplicationSpecific = 0x8000, // bits 15, 14 = (1,0)
      EcDatabaseIndex       = 0xC000  // bits 15, 14 = (1,1)
   };

   // Enumerations for radio signal encoding type (EBV-DOC 9.2.2)
   enum
   {
      Et8bitMuLaw          = 1,
      EtCVSD               = 2,
      EtADPCM              = 3,
      Et16bitLinearPCM     = 4,
      Et8bitLinearPCM      = 5,
      EtVectorQuantization = 6
   };

   // Enumerations for TDL type (EBV-DOC 9.2.4)
   enum
   {
      EtOther        = 0,
      EtMITR         = 6,  // Link16 Legacy Format
      EtACC          = 15, // Abbreviated command and control
      EtKuBandSatcom = 58,
      EtUHFSatcom    = 63,
      EtGC3          = 99,
      EtTadilTales   = 100, // Link16 Standardized Format
      EtGenericMADL  = 253,
      // Our site specific types
      EtGenericIP      = 254,
      EtATCOMResupply  = 1066,
      EtForceStructure = 1067,
      EtFADL           = 1234
   };

   // Accessors

   const DisEntityId& GetOriginatingEntity() const override;

   const DisEntityId& GetEntityId() const;
   DisUint16          GetRadioId() const;
   DisEnum16          GetEncodingScheme() const;
   DisEnum16          GetTDLType() const;
   DisUint32          GetSampleRate() const;
   DisUint16          GetSampleCount() const;

   // GetData returns a pointer to the data portion and the length
   // of the data portion in bits.  The caller can use a GenMemIO
   // object to interpret the data in a platform independent manner.

   // NOTE: THE LENGTH IS IN BITS!!!

   void GetData(const unsigned char*& aDataPtr, DisUint16& aDataLength) const;

   // Mutators

   virtual void SetEntityId(const DisEntityId& aEntityId);
   virtual void SetRadioId(DisUint16 aRadioId);
   virtual void SetEncodingScheme(DisEnum16 aEncodingScheme);
   virtual void SetTDLType(DisEnum16 aTDLType);
   virtual void SetSampleRate(DisUint32 aSampleRate);
   virtual void SetSampleCount(DisUint16 aSampleCount);

   // SetData allows the caller to replace the data portion of the
   // PDU.  The memory pointed to the supplied pointer becomes the
   // property of the object and will be deleted when the objected
   // is deleted or if the pointer is later replaced.

   // NOTE: THE LENGTH IS IN BITS!!!

   virtual void SetData(unsigned char* aDataPtr, DisUint16 aDataLength);

   // SetUserData is like SetData except that the memory pointed to
   // by the supplied pointer remains the property of the caller
   // and is NOT deleted when the object is deleted.

   // NOTE: THE LENGTH IS IN BITS!!!

   virtual void SetUserData(unsigned char* aDataPtr, DisUint16 aDataLength);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

   //! Data Validation
   bool IsValid() const override;

   // Operators

   DisSignal& operator=(const DisSignal& aRhs);

private:
   enum
   {
      EncodedAudio,
      RawBinaryData,
      ApplicationSpecificData,
      DatabaseIndex
   };


   void Copy(const DisSignal& aRhs);
   void DeleteClassData();
   void GetMemberData(GenI& aGenI);
   void SetTheData(unsigned char* aDataPtr, DisUint16 aDataLength);

   /*!
    * Identifies the entity or object to which the radio is attached
    */
   DisEntityId mEntityId;
   /*!
    * Identifies a particular radio that is either associated with an entity, object
    * or is an unattached radio.
    */
   DisUint16 mRadioId;
   /*!
    * Specifies the encoding used in the data field
    */
   DisEnum16 mEncodingScheme;
   /*!
    * Specifies the TDL type
    */
   DisEnum16 mTDLType;
   /*!
    * Specifies either the sample rate or the data rate
    */
   DisUint32 mSampleRate;
   /*!
    * Specifies the number of bits of digital voice audio or digital data
    * being sent
    */
   DisUint16 mDataLength; // in BITS!
   /*!
    * The number of samples associated with this PDU
    */
   DisUint16 mSampleCount;

   unsigned char* mDataPtr;
   bool           mDataIsClassOwned; // true if the data is owned by
                                     // the class and not the user
};

// Accessors

inline const DisEntityId& DisSignal::GetEntityId() const
{
   return mEntityId;
}
inline DisUint16 DisSignal::GetRadioId() const
{
   return mRadioId;
}
inline DisEnum16 DisSignal::GetEncodingScheme() const
{
   return mEncodingScheme;
}
inline DisEnum16 DisSignal::GetTDLType() const
{
   return mTDLType;
}
inline DisUint32 DisSignal::GetSampleRate() const
{
   return mSampleRate;
}
inline DisUint16 DisSignal::GetSampleCount() const
{
   return mSampleCount;
}

// Mutators

inline void DisSignal::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}
inline void DisSignal::SetRadioId(DisUint16 aRadioId)
{
   mRadioId = aRadioId;
}
inline void DisSignal::SetEncodingScheme(DisEnum16 aEncodingScheme)
{
   mEncodingScheme = aEncodingScheme;
}
inline void DisSignal::SetTDLType(DisEnum16 aTDLType)
{
   mTDLType = aTDLType;
}
inline void DisSignal::SetSampleRate(DisUint32 aSampleRate)
{
   mSampleRate = aSampleRate;
}
inline void DisSignal::SetSampleCount(DisUint16 aSampleCount)
{
   mSampleCount = aSampleCount;
}

#endif
