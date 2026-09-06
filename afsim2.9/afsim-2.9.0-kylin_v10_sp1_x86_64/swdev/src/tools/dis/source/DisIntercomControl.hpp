// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#ifndef DISINTERCOMCONTROL_HPP
#define DISINTERCOMCONTROL_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisIntercomCommunicationsParametersRecord.hpp"
#include "DisPdu.hpp"

/**!
 * The Intercom Control PDU shall be used to communicate the state of a particular intercom device, request
 * an action of another intercom device, or respond to an action request.
 */
class DIS_EXPORT DisIntercomControl : public DisPdu
{
public:
   DisIntercomControl();
   DisIntercomControl(const DisIntercomControl& aSrc);
   DisIntercomControl(const DisPdu& aPdu, GenI& aGenI);
   explicit DisIntercomControl(GenI& aGenI);
   ~DisIntercomControl() = default;

   DisPdu* Clone() const override;
   int     GetClass() const override;
   int     GetFamily() const override;

   // Getters
   DisEnum8           GetControlType() const;
   DisUint8           GetCommunicationsChannelType() const;
   const DisEntityId& GetSourceIntercomReferenceId() const;
   DisUint16          GetMasterIntercomNumber() const;
   DisUint16          GetSourceIntercomNumber() const;
   DisUint8           GetSourceLineId() const;
   DisUint8           GetTransmitPriority() const;
   DisEnum8           GetTransmitLineState() const;
   DisEnum8           GetCommand() const;
   const DisEntityId& GetMasterIntercomReferenceId() const;
   DisUint16          GetMasterChannelId() const;
   DisUint32          GetIntercomParametersLength() const;

   const DisIntercomCommunicationsParametersRecord& GetIntercomParametersRecord() const;

   // Setters
   void SetControlType(DisEnum8 aControlType);
   void SetCommunicationsChannelType(DisUint8 aCommunicationsChannelType);
   void SetSourceIntercomReferenceId(const DisEntityId& aSourceIntercomReferenceId);
   void SetMasterIntercomNumber(DisUint16 aMasterIntercomNumber);
   void SetSourceIntercomNumber(DisUint16 aSourceIntercomNumber);
   void SetSourceLineId(DisUint8 aSourceLineId);
   void SetTransmitPriority(DisUint8 aTransmitPriority);
   void SetTransmitLineState(DisEnum8 aTransmitLineState);
   void SetCommand(DisEnum8 aCommand);
   void SetMasterIntercomReferenceId(const DisEntityId& aMasterIntercomReferenceId);
   void SetMasterChannelId(DisUint16 aMasterChannelId);
   void SetIntercomParametersLength(DisUint32 aIntercomParametersLength);
   void SetIntercomParametersRecord(DisIntercomCommunicationsParametersRecord& aIntercomParametersRecord);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   //! Data Validation
   bool IsValid() const override;

   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI);

   //! Specifies the type of control requested in this PDU.
   DisEnum8 mControlType = 0;

   /**!
    * Identifies the type of communications channel and is composed of two sections:
    * Section 1 (7 bits): 7 bit enumeration specifying the communications type (UID 181)
    * Section 2 (1 bit): 1 bit enumeration specifying the communications class (UID 416)
    * Format: bits 0-6: 7 bit enumeration, bit 7: 1 bit enumeration
    */
   DisUint8 mCommunicationsChannelType = 0;

   //! The source of the intercom
   DisEntityId mSourceIntercomReferenceId;

   //! The specific intercom id of the device being interfaced and/or simulated within an intercom
   DisUint16 mSourceIntercomNumber = 0;

   //! Identifies the line number to which the intercom control refers
   DisUint8 mSourceLineId = 0;

   //! The priority of this message relative to transmissinos from other intercom devices on the same channel.
   DisUint8 mTransmitPriority = 0;

   //! The current state of the line at the intercom source
   DisEnum8 mTransmitLineState = 0;

   //! The details of a request or acknowledge
   DisEnum8 mCommand = 0;

   /**!
    * The master identifier of the entity/object/unattached intercom identifier that has create this intercom channel.
    * For attached intercoms, this field identifies the attached entity or object.
    */
   DisEntityId mMasterIntercomReferenceId;

   //! Identifies the specific intercom device that has created this intercom channel
   DisUint16 mMasterIntercomNumber = 0;

   //! Identifies a unique intercom channel create by this master intercom reference id and master intercom number pair.
   DisUint16 mMasterChannelId = 0;

   //! The length (in octets) of the optional intercom parameters
   DisUint32 mIntercomParametersLength = 0;

   /**!
    * The (optional) intercom parameters which describe additional information including the
    * destination(s) of the intercom control PDU (as required)
    */
   DisIntercomCommunicationsParametersRecord mIntercomParameters;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 28;
};
#endif
