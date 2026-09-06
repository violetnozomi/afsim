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

#ifndef DISPDU_HPP
#define DISPDU_HPP

#include "dis_export.h"
#include "ut_export.h"
#include "util_script_export.h"

#include <iomanip>
#include <iosfwd>
#include <string>
#include <vector>

#include <memory.h>

#include "DisPduProcessI.hpp"
#include "DisTypes.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtReferenceTracked.hpp"
#include "UtScriptAccessible.hpp"

class GenI;
class GenO;
class DisAttribute;
class DisEntityId;
class DisPduFactory;
class UtScriptContext;

/*!
 * This class is the base class and header for all the PDUs implemented in DIS. The only exception to this
 * is the DisLE (Live Entity) PDU which does not use this class but a different class as its header.
 */
class DIS_EXPORT DisPdu : public UtScriptAccessible
{
public:
   // This method reads enough of the input stream to
   // determine the type of PDU being read.  It then creates the proper PDU
   // from a user provided factory class derived from DisPduFactory or if
   // no factory pointer is provided, the PDU is created from a
   // internally generated DisPduFactory.  Once the proper PDU is generated,
   // it is populated with the data from the input stream.
   //
   // The caller owns the returned PDU and is responsible for its destruction.
   static DisPdu* Create(GenI& aGenI, DisPduFactory* aDisPduFactory = nullptr);
   //! Default Constructor
   DisPdu();
   //! Logger Constructor
   explicit DisPdu(const std::vector<std::string>& aLoggerResultVector);
   //! Copy Constructor
   DisPdu(const DisPdu& aSrc);
   //! Stream Constructor
   explicit DisPdu(GenI& aGenI);
   //! Destructor
   ~DisPdu() override;

   //! Creates an exact deep copy of this PDU
   virtual DisPdu* Clone() const;

   //----------------------- Script accessibility methods -----------------------
   virtual const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

   // Return the C++ class on which this PDU is constructed.  This may be
   // different than the value returned by GetPduType() as some PDU's have
   // not yet been mapped to a C++ class (other than DisOther).  Also, it
   // may be desirable in the future to map more than one PDU type to a
   // specific class (such as the reliable and non-reliable forms).
   virtual int GetClass() const;
   /**
    * Returns the family type of this PDU
    */
   virtual int GetFamily() const;
   // Boolean accessors for use in tracking the "reliable" PDU forms.
   bool IsInitiator() const;
   bool IsRespondent() const;
   bool IsReliability() const;

   // These two methods may be overridden to fit the local needs of the user.
   // Does nothing in this base class, but is typically a "PDU Received" action.
   // Return zero to permit memory pointer to be kept.  Non-zero indicates
   // delete.
   virtual int Process();

   // This method does nothing in this class
   virtual int DoIt();

   // This method may be overridden to fit the local needs of the user.
   // Does nothing in this base class, but is typically a time-queued
   // "Request Timed Out" action.
   virtual void TimedOut();

   // To be implemented by several PDU types to link a request with a response.
   virtual DisUint32 GetRequestId() const;

   // Accessor to determine if this PDU will be implementing a "reliable"
   // (iterative) send, until a response or acknowledgment is received.
   virtual DisEnum8 GetReliabilityService() const;

   // Most derived classes should be able to tell who sent this PDU to
   // the best of their ability, but can return sNO_ENTITY if unknown.
   virtual const DisEntityId& GetOriginatingEntity() const;

   // Some derived classes should be able to tell who received this PDU,
   // as indicated in the data attributes, but not all can.  The base
   // accessor will return sNO_ENTITY if not overridden.
   virtual const DisEntityId& GetReceivingEntity() const;

   // Accessors
   //! Protocol Version
   /*!
    *  This field shall specify the version of protocol used in this PDU.
    */
   DisEnum8 GetProtocolVersion() const;
   //! Exercise Identifier
   /*!
    *  This field shall specify the exercise to which the PDU pertains.
    */
   DisUint8 GetExerciseIdentifier() const;
   //! PDU Type
   /*!
    *  This field shall indicate the type of PDU that follows.
    */
   DisEnum8 GetPduType() const;
   //! Protocol Family
   /*!
    *  This field shall indicate the family of protocols to which the PDU
    *  belongs.
    */
   DisEnum8 GetProtocolFamily() const;
   //! Timestamp
   /*!
    *  This field shall specify the time that the data in the PDU was generated.
    */
   DisUint32 GetTimestamp() const;

   // Returns length of just DisPdu in Octets
   static DisUint16 GetBaseLength();

   // This is virtual because some PDU's have lengths that can be
   // dynamically updated and there is no other single hook that can
   // give us the length.
   virtual DisUint16 GetLength();

   //! PDU Status
   /*!
    *  This field shall specify the PDU status related to one or more PDU Types.
    *
    *  The PDU Status record shall be used to indicate status information that
    *  either (1) affects the processing of a specific PDU as a whole without
    *  regard to any specific data field in the PDU, (2) provides information
    *  related to the interpretation of one or more data fields or their
    *  content, or (3) provides information that affects the processing of an
    *  entity, other object, or environmental process associated with a PDU.
    *  This record shall be defined as an 8-bit record.  Fields not applicable
    *  for a specific PDU type shall be set to zero.
    *
    *  -------------------------------------------------------------------------
    *  |       |                           | Field Bit Position                |
    *  |  PDU  |  PDU Name                 |-----------------------------------|
    *  |  Type |                           |  5  |  4  |  3  |  2  |  1  |  0  |
    *  |-----------------------------------------------------------------------|
    *  |    1  | Entity State              |  -  |  -  | CEI |    LVC    | TEI |
    *  |    2  | Fire                      |  -  | FTI | CEI |    LVC    |  -  |
    *  |    3  | Detonation                |    DTI    | CEI |    LVC    |  -  |
    *  |  4-22 |  All PDUs                 |  -  |  -  | CEI |    LVC    |  -  |
    *  |   23  | Electromagnetic Emission  |  -  |  -  | CEI |    LVC    | TEI |
    *  |   24  | Designator                |  -  |  -  | CEI |    LVC    | TEI |
    *  |   25  | Transmitter               |    RAI    | CEI |    LVC    | TEI |
    *  |   26  | Signal                    |    RAI    | CEI |    LVC    | TEI |
    *  |   27  | Receiver                  |    RAI    | CEI |    LVC    | TEI |
    *  |   28  | IFF                       | AII | ISM | CEI |    LVC    | TEI |
    *  | 29-30 |  All PDUs                 |  -  |  -  | CEI |    LVC    |  -  |
    *  |   31  | Intercom Signal           |    IAI    | CEI |    LVC    | TEI |
    *  |   32  | Intercom Control          |    IAI    | CEI |    LVC    | TEI |
    *  | 33-40 |  All PDUs                 |  -  |  -  | CEI |    LVC    |  -  |
    *  |   41  | Environmental Process     |  -  |  -  | CEI |    LVC    | TEI |
    *  | 42-66 |  All PDUs                 |  -  |  -  | CEI |    LVC    |  -  |
    *  |   67  | Entity State Update       |  -  |  -  | CEI |    LVC    | TEI |
    *  | 67-71 |  All PDUs                 |  -  |  -  | CEI |    LVC    |  -  |
    *  |   72  | Attribute                 |  -  |  -  |  -  |    LVC    |  -  |
    *  -------------------------------------------------------------------------
    *  | Legend:                                                               |
    *  |    AII - Active Interrogation Indicator                               |
    *  |    CEI - Coupled Extension Indicator                                  |
    *  |    TEI - Transferred Entity Indicator                                 |
    *  |    LVC - LVC Indicator                                                |
    *  |    FTI - Fire Type Indicator                                          |
    *  |    DTI - Detonation Type Indicator                                    |
    *  |    RAI - Radio Attached Indicator                                     |
    *  |    IAI - Intercom Attached Indicator                                  |
    *  |    ISM - IFF Simulation Mode                                          |
    *  -------------------------------------------------------------------------
    */
   DisEnum8 GetPduStatus() const;

   //! Transferred Entity Indicator (TEI)
   /*!
    *  The Transferred Entity Indicator field shall indicate whether the
    *  simulation identified in the primary Entity ID field (Site, Application)
    *  is the same one that owns the entity -- No Difference (0) or whether a
    *  different simulation owns the entity -- Difference (1).
    */
   DisEnum8 GetTransferredEntityIndicator() const;

   //! LVC Indicator (LVC)
   /*!
    *  The LVC Indicator field shall indicate whether the data contained in the
    *  PDU is related to a Live (1), Virtual (2), or Constructive (3) entity.
    *  If the LVC designation is not able to be determined, this field shall be
    *  set to No Statement (0).  The LVC Indicator field shall occupy bits
    *  1 and 2 of the PDU Status record for any PDU type that implements this
    *  field.
    */
   DisEnum8 GetLvcIndicator() const;

   //! Subprotocol Number
   /*!
    *  This field shall specify the subprotocol to be used to decode the PDU.
    *
    *  The Subprotocol Number shall be used with a pdu in the LE Information/Interaction
    *  protocol family in place of the PDU Status field.
    */
   DisEnum8 GetSubprotocolNumber() const;

   //! Coupled Extension Indicator (CEI)
   /*!
    *  The Coupled Extension Indicator field shall be used to indicate whether
    *  the PDU is Coupled (1) or Not Coupled (0) with an Attribute PDU.  The
    *  CEI field shall occupy bit 3 of the PDU Status record for any PDU type
    *  that implements this field.
    */
   DisEnum8 GetCoupledExtensionIndicator() const;

   //! Returns a pointer to the coupled Attribute PDU extending this PDU.
   //!
   //!  \return  Pointer to the Attribute PDU, or null if this PDU is not
   //!           coupled.
   //!
   const DisAttribute* GetCoupledExtension() const;

   const int GetMaxPduSizeOctets() const;

   const int GetMaxPduSizeBits() const;

   virtual bool IsValid() const;
   virtual void HandleReadError(ut::log::MessageStream& aStream);

   // Mutators

   void SetProtocolVersion(DisEnum8 aProtocolVersion);
   void SetExerciseIdentifier(DisUint8 aExerciseId);
   void SetTimestamp(DisUint32 aTimestamp);
   void SetTransferredEntityIndicator(bool isTransferred = true);
   void SetLvcIndicator(DisEnum8 aLvcIndicator);

   //! Called by LE Information/Interaction PDUs instead of SetPduStatus
   void SetSubprotocolNumber(DisEnum8 aNumber);

   //! Couples the Attribute PDU to extend this PDU.
   //! Couple this PDU to the attribute PDU.
   //!
   //! \warn This PDU will take ownership of the coupled PDU,
   //!       and will delete it when this PDU is destroyed.
   bool CoupleExtension(DisAttribute* aAttribute);

   // Input/output

   virtual void Get(GenI& aGenI);
   virtual void Put(GenO& aGenO);

   // Time helper functions

   // Get the seconds after the hour from a raw time stamp and
   // set "aAbsoluteFlag".
   double GetSecsAfterHour(unsigned int& aAbsoluteFlag) const;

   // Get the time for a timestamp accounting for wrapping around the hour
   double GetAbsoluteTime() const;

   // Get a time that has been adjusted
   // for latency in the network (if using relative time) and
   // account for PDU timestamp wrapping through 3600 secs.
   // "aDisEntityID" is the DisEntityId that best
   // represents the owner of this PDU.  The Transfer Ownership
   // PDU muddies the water for this function, but can be accounted
   // for by calling "AddToAltOwnerList" and "RemoveFromAltOwnerList"
   // when an entity is transfered to another owner and then removed from
   // the active simulation.  The last argument controls whether the
   // network latency deltas should be saved for future calls to
   // this function.  Be sure to call "DisTime::Reset()" when appropriate.
   double GetBestAvailableTime(double aSimTime, unsigned int aAdjustDelta = 1) const;

   // Set the time stamp with the SimTime
   void SetTime(double aTime);

   // Operators

   // Assignment operator
   const DisPdu& operator=(const DisPdu& aRhs);

   // This virtual base method is provided as a convenience for debug
   // purposes, but not all implementations may support it fully.
   virtual void Stream(std::ostream& aStream) const;

   //! Sends the contents of the PDU to a string form
   virtual std::string ToString() const;
   // NOTE:  The following enumerations are deprecated, and may be removed
   // in a future DIS library release.  Please transition to enumerations
   // placed in their own namespace, as identified with each deprecated enum.

   // Deprecated.  See file DisPduEnums.hpp for replacement enum DisEnum::Pdu::Version.
   enum
   {
      Version_Other             = 0,
      Version_1_0               = 1,
      Version_IEEE_1278_1993    = 2,
      Version_2_0_Third_Draft   = 3,
      Version_2_0_Fourth_Draft  = 4,
      Version_IEEE_1278_1_1995  = 5,
      Version_IEEE_1278_1A_1998 = 6
   };

   // Deprecated.  See file DisPduEnums.hpp for replacement enum DisEnum::Pdu::Type.
   enum
   {
      TypeOther              = 0,
      TypeEntityState        = 1,
      TypeFire               = 2,
      TypeDetonation         = 3,
      TypeServiceRequest     = 5,
      TypeResupplyOffer      = 6,
      TypeResupplyReceived   = 7,
      TypeResupplyCancel     = 8,
      TypeCreateEntity       = 11,
      TypeStartResume        = 13,
      TypeStopFreeze         = 14,
      TypeAcknowledge        = 15,
      TypeSetData            = 19,
      TypeData               = 20,
      TypeComment            = 22,
      TypeEmission           = 23,
      TypeDesignator         = 24,
      TypeTransmitter        = 25,
      TypeSignal             = 26,
      TypeReceiver           = 27,
      TypeIffAtcNavaids      = 28,
      TypeUnderwaterAcoustic = 29,
      TypeTransferOwnership  = 35,
      TypeCreateEntityR      = 51,
      TypeRemoveEntityR      = 52,
      TypeStartResumeR       = 53,
      TypeStopFreezeR        = 54,
      TypeAcknowledgeR       = 55,
      TypeActionRequestR     = 56,
      TypeActionResponseR    = 57,
      TypeDataQueryR         = 58,
      TypeSetDataR           = 59,
      TypeDataR              = 60,
      TypeEventReportR       = 61,
      TypeCommentR           = 62,
      TypeRecordR            = 63,
      TypeSetRecordR         = 64,
      TypeRecordQueryR       = 65,
      TypeDirectedEnergyFire = 68,
      TypeEntityDamageStatus = 69
   };

   // Deprecated.  See file DisPduEnums.hpp for replacement enum DisEnum::Pdu::Family.
   enum
   {
      FamilyOther                               = 0,
      FamilyEntityInformationInteraction        = 1,
      FamilyWarfare                             = 2,
      FamilyLogistics                           = 3,
      FamilyRadioCommunication                  = 4,
      FamilySimulationManagement                = 5,
      FamilyDistributedEmissionRegeneration     = 6,
      FamilyEntityManagement                    = 7,
      FamilyMinefield                           = 8,
      FamilySyntheticEnvironment                = 9,
      FamilySimulationManagementWithReliability = 10,
      FamilyLiveEntity                          = 11,
      FamilyNonRealTime                         = 12
   };

protected:
   // The constructor and several of the mutators are protected since
   // only a derived class should be constructing the base class or
   // modifying certain key pieces of information.


   void SetPduType(DisEnum8 mPduType);
   void SetProtocolFamily(DisEnum8 aProtocolFamily);
   void SetLength(DisUint16 aLength);
   void SetPduStatus(DisEnum8 aStatus);

   // The following method is called by the specific PDU reading
   // routines to read any 'extra' data.  This occurs when the
   // length declared in the header is greater that actually
   // required for the PDU.
   void ReadExtraData(GenI& aGenI, DisUint16 aLengthToRead);

public:
   // This is a special version of GetLength() that isn't virtual.
   // It returns the length as recorded in the header and not the
   // potentially 'computed' length.  It is also 'const' because
   // it has no potential to modify the PDU while the virtual one
   // does.  It must be called prior to any call to 'GetLength()'
   // as it modifies the header.
   DisUint16 GetLengthToRead() const;

   /**
    * Returns the string id of the PDU
    */
   virtual std::string GetStringId() const { return ""; };

   std::string GetEntityPartCountStringValue(int enumValue);
   // END LOGGING

   //! Gets the process class for this PDU
   DisPduProcessI& GetPduProcess() const;
   //! Sets the process class for this PDU
   void SetPduProcess(DisPduProcessI* aNewPduProcess);

protected:
   struct node
   {
      std::string              value;
      std::vector<struct node> children;
   };

   // Processing object member variable(s)
   std::unique_ptr<DisPduProcessI> mPduProcess;

private:
   //! Seperate Get method specifically for child classes meant to contain all "Get" information relative to that
   //! particular child PDU implementation.
   virtual void GetMemberData(GenI& aGenI){};

   static DisPduFactory* GetLocalDisPduFactory();

   /*!
    * Each published version of the DIS standard is assigned a protocol version value and
    * is included in the protocol version field of the PDU header. This denotes the standard that
    * this particular PDU follows.
    *
    * The protocol version is defined here as "mProtocolVesion".
    */
   DisEnum8 mProtocolVersion{0};
   /*!
    * Each exercise is assigned a unique ID that occurs simultaneously on the same communication medium
    */
   DisUint8 mExerciseIdentifier{0};
   /*!
    * The type of PDU that follows the header.
    */
   DisEnum8 mPduType{0};
   /*!
    * Indicates the family of protocols to which this PDU belongs.
    */
   DisEnum8 mProtocolFamily{0};
   /*!
    * Specifies the time that the data in the PDU was generated
    */
   DisUint32 mTimestamp{0};
   /*!
    * Specifies the length of the PDU including the header in octets.
    */
   DisUint16 mLength{0};
   /*!
    * Represents either the PDU Status for PDU headers or Subprotocol Number for
    * LE Headers in the LE Information/Interaction protocol family.
    */
   DisUint8 mPduStatus{0};
   /*!
    * Padding
    */
   DisUint8 mPaddingEightBits{0};
   //! Pointer to an Attribute PDU that is coupled with this PDU.
   /*!
    *  The coupled attribute PDU is required to be bundled with this
    *  PDU. The total size of this PDU plus the coupled attribute PDU
    *  may not exceed MAX_SIZE_PDU_OCTETS.
    */
   DisAttribute* mCoupledExtension = nullptr;

   // found in IEEE 1278.1-2012 section 6.3.3
   //! Maximum allowed size of the PDU in octets
   static constexpr int MAX_PDU_SIZE_OCTETS = 8192;
   //! Maximum allowed size of the PDU in bits
   static constexpr int MAX_PDU_SIZE_BITS = 65536;
};

// Accessors
inline DisEnum8 DisPdu::GetProtocolVersion() const
{
   return mProtocolVersion;
}
inline DisUint8 DisPdu::GetExerciseIdentifier() const
{
   return mExerciseIdentifier;
}
inline DisEnum8 DisPdu::GetPduType() const
{
   return mPduType;
}
inline DisEnum8 DisPdu::GetProtocolFamily() const
{
   return mProtocolFamily;
}
inline DisUint32 DisPdu::GetTimestamp() const
{
   return mTimestamp;
}
inline DisUint8 DisPdu::GetPduStatus() const
{
   return mPduStatus;
}
inline DisUint8 DisPdu::GetTransferredEntityIndicator() const
{
   return mPduStatus & 0x01;
}
inline DisUint8 DisPdu::GetLvcIndicator() const
{
   return (mPduStatus >> 1) & 0x03;
}
inline DisEnum8 DisPdu::GetSubprotocolNumber() const
{
   return mPduStatus;
}
inline DisUint8 DisPdu::GetCoupledExtensionIndicator() const
{
   return (mPduStatus >> 3) & 0x01;
}
inline const DisAttribute* DisPdu::GetCoupledExtension() const
{
   return mCoupledExtension;
}
inline const int DisPdu::GetMaxPduSizeOctets() const
{
   return MAX_PDU_SIZE_OCTETS;
}
inline const int DisPdu::GetMaxPduSizeBits() const
{
   return MAX_PDU_SIZE_BITS;
}
inline DisUint16 DisPdu::GetLengthToRead() const
{
   return mLength;
}

// Mutators

inline void DisPdu::SetProtocolVersion(DisEnum8 aProtocolVersion)
{
   mProtocolVersion = aProtocolVersion;
}
inline void DisPdu::SetExerciseIdentifier(DisUint8 aExerciseId)
{
   mExerciseIdentifier = aExerciseId;
}
inline void DisPdu::SetPduType(DisEnum8 aPduType)
{
   mPduType = aPduType;
}
inline void DisPdu::SetProtocolFamily(DisEnum8 aProtocolFamily)
{
   mProtocolFamily = aProtocolFamily;
}
inline void DisPdu::SetTimestamp(DisUint32 aTimestamp)
{
   mTimestamp = aTimestamp;
}
inline void DisPdu::SetTransferredEntityIndicator(bool isTransferred)
{
   mPduStatus |= isTransferred ? 0x01 : 0x00;
}
inline void DisPdu::SetLvcIndicator(DisEnum8 aLvcIndicator)
{
   mPduStatus |= ((aLvcIndicator & 0x03) << 1);
}
inline void DisPdu::SetLength(DisUint16 aLength)
{
   mLength = aLength;
}

inline void DisPdu::SetPduStatus(DisEnum8 aStatus)
{
   mPduStatus = aStatus;
}

inline void DisPdu::SetSubprotocolNumber(DisEnum8 aNumber)
{
   mPduStatus = aNumber;
}

#endif
