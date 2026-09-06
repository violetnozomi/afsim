// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISINTERCOMSIGNAL_HPP
#define DISINTERCOMSIGNAL_HPP

#include <vector>

#include "DisEncodingSchemeRecord.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

/**!
 * The Intercom Signal PDU shall be used to convey the audio or digital data that is used to communicate
 * between simulated intercom devices.This is one of two methods that may be used to represent an intercom.
 * The other method uses the Transmitter and Signal PDUs
 */
class DIS_EXPORT DisIntercomSignal : public DisPdu
{
public:
   DisIntercomSignal();
   DisIntercomSignal(const DisIntercomSignal& aSrc);
   DisIntercomSignal(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIntercomSignal(GenI& aGenI);
   ~DisIntercomSignal() override = default;

   DisIntercomSignal* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Getters
   const DisEntityId&             GetOriginatingEntity() const override;
   const DisEntityId&             GetIntercomReferenceId() const;
   DisUint16                      GetIntercomNumber() const;
   const DisEncodingSchemeRecord& GetEncodingScheme() const;
   DisEnum16                      GetTDLType() const;
   DisUint32                      GetSampleRate() const;
   DisUint16                      GetSampleCount() const;

   // GetData returns a pointer to the data portion and the length
   // of the data portion in bits.  The caller can use a GenMemIO
   // object to interpret the data in a platform independent manner.

   // NOTE: THE LENGTH IS IN BITS!!!
   void GetData(std::vector<unsigned char>& aData, DisUint16& aDataLength) const;

   void GetData(std::vector<unsigned char>& aData, DisUint16& aDataLength);

   std::vector<unsigned char> GetData() const;

   // Setters
   virtual void SetIntercomReferenceId(const DisEntityId& aIntercomReferenceId);
   virtual void SetIntercomNumber(DisUint16 aIntercomNumber);
   virtual void SetEncodingScheme(const DisEncodingSchemeRecord& aEncodingScheme);
   virtual void SetTDLType(DisEnum16 aTDLType);
   virtual void SetSampleRate(DisUint32 aSampleRate);
   virtual void SetSampleCount(DisUint16 aSampleCount);

   // SetData allows the caller to replace the data portion of the
   // PDU.  The memory pointed to the supplied pointer becomes the
   // property of the object and will be deleted when the objected
   // is deleted or if the pointer is later replaced.

   // NOTE: THE LENGTH IS IN BITS!!!
   /**!
    * Sets the data for this PDU given a source and a length (in bits).
    * NOTE: If there is a data size mismatch between the source and the length (in bits)
    *       provided by the caller, then this method will return false.
    */
   virtual bool SetData(std::vector<unsigned char> aData, DisUint16 aDataLength);

   std::string         GetStringId() const override;
   virtual std::string ToString() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;


   // Data Validation
   virtual unsigned int IsValid();
   bool                 IsValid() const override;

   // Operators
   DisIntercomSignal& operator=(const DisIntercomSignal& aRhs);

private:
   //! Copies the contents of a given DisIntercomSignal PDU from a source to this PDU
   void Copy(const DisIntercomSignal& aSrc);
   void DeleteClassData();
   void GetMemberData(GenI& aGenI) override;
   bool SetTheData(std::vector<unsigned char> aData, DisUint16 aDataLength);

   //! (For attached intercoms) Identifies an entity or object to which the intercom is attached.
   DisEntityId mIntercomReferenceId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! The particular intercom that is associated with the an entity or object, or is unattached.
   DisUint16 mIntercomNumber = 0;

   //! Encoding scheme
   DisEncodingSchemeRecord mEncodingScheme = 0;

   //! TDL type
   DisUint16 mTDLType = 0;

   //! Sample or data rate
   DisUint32 mSampleRate = 0;

   //! Length of the data field NOTE: in bits
   DisUint16 mDataLength = 0;

   //! Sample count
   DisUint16 mSampleCount = 0;

   //! Data associated with this PDU
   std::vector<unsigned char> mData;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 20;
};
#endif
