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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFMESSAGE_HPP
#define WSFMESSAGE_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <vector>

#include "UtReferenceTracked.hpp"
#include "UtScriptAccessible.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "WsfCommAddress.hpp"
class WsfPlatform;
class WsfSimulation;
#include "WsfStringId.hpp"

//! A base class for messages sent among simulation communication objects, processors and sensors.
class WSF_EXPORT WsfMessage : public UtScriptAccessible, public UtReferenceTracked, public WsfAuxDataEnabled
{
public:
   // Although this isn't guaranteed, it's fixed for simulation purposes.
   static constexpr size_t cBITS_IN_BYTE = 8U;

   WsfMessage(WsfStringId aType); // For XIO Serialization

   WsfMessage(WsfStringId aType, const wsf::comm::Address& aSrcAddr, WsfSimulation& aSimulation);
   WsfMessage(WsfPlatform* aPlatformPtr);
   WsfMessage(WsfStringId aType, WsfPlatform* aPlatformPtr);
   WsfMessage(WsfStringId aType, WsfStringId aSubType, WsfPlatform* aPlatformPtr);
   WsfMessage(WsfSimulation& aSimulation);
   WsfMessage& operator=(const WsfMessage& aRhs) = default;
   ~WsfMessage() override;

   virtual WsfMessage* Clone() const;

   const char* GetScriptClassName() const override;

   static WsfStringId GetTypeId();

   //! Get the simulation that owns the message.
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   //! Set the simulation that owns the message.
   //! @note This should be used ONLY where a message is being created and will be populated later.
   //!       This prevents the need to have a lot of specialized constructors.
   void SetSimulation(WsfSimulation* aSimulationPtr) { mSimulationPtr = aSimulationPtr; }

   //! Get the platform name identifier of the message originator.
   //! @returns The Platform Name ID of the message originator.
   WsfStringId GetOriginator() const { return mOriginator; }

   //! Set the originator of this message.
   void SetOriginator(WsfPlatform* aPlatformPtr);

   //! Get the platform index of the message originator.
   //! @returns The platform index of the message originator.
   size_t GetOriginatorIndex() const { return mOriginatorIndex; }

   //! Get the length of the message in bits .
   //! @returns The length of the message in bits.
   int GetSizeBits() const { return mSizeBits; }

   //! Set the message size in bits.
   //! @param aSizeBits The message size in bits.
   void SetSizeBits(int aSizeBits) { mSizeBits = aSizeBits; }

   //! Get the length of the message in bytes .
   //! @returns The length of the message in bytes.
   int GetSizeBytes() const { return (mSizeBits / cBITS_IN_BYTE); }

   //! Set the message size in bytes.
   //! @param aSizeBytes The message size in bytes.
   void SetSizeBytes(int aSizeBytes) { mSizeBits = aSizeBytes * cBITS_IN_BYTE; }

   //! Get the serial number of this message.
   //! @returns The serial number of the message.
   unsigned int GetSerialNumber() const { return mSerialNumber; }

   //! Set the serial number of this message.
   //! The simulation object should be used to get the next available serial number to ensure uniqueness.
   //! @param aSerialNumber The serial number for this message.
   void SetSerialNumber(unsigned int aSerialNumber) { mSerialNumber = aSerialNumber; }

   //! Get the data tag of the underlying data.
   //! This is an optional value that may be used to identify the contents of the message.
   //! When an object (e.g.: a track or an image) is sent in a message, the data tag from the object is
   //! copied to the message. This allows the data to be tracked through the network, even though it may
   //! happen through several messages, with interruptions.
   //! @returns The data tag attached to the message. This will be zero if it none has been assigned.
   double GetDataTag() const { return mDataTag; }

   //! Set the 'data tag' to be associated with the data in the message.
   //! @param aDataTag The data tag to be attached to the message.
   void SetDataTag(double aDataTag) { mDataTag = aDataTag; }

   //! Get the 'static type' of the message.
   //! Initially the static type is the same as the message type, but the static type may not be changed.
   //! @returns The static message type as a string ID.
   WsfStringId GetStaticType() const { return mStaticType; }

   //! Get the 'type' of the message.
   //! @returns The message type as a string ID.
   WsfStringId GetType() const { return mType; }

   //! Set the 'type' of the message.
   //! @param aType The message type as a string ID.
   void SetType(WsfStringId aType) { mType = aType; }

   //! Get the 'sub-type' of the message.
   //! @returns The message sub-type as a string ID.
   WsfStringId GetSubType() const { return mSubType; }

   //! Set the 'sub-type' of the message.
   //! @param aSubType The message sub-type as a string ID.
   void SetSubType(WsfStringId aSubType) { mSubType = aSubType; }

   //! Get the priority of the message.
   //! @return Returns the message's priority as an integer
   //!         (0 being the lowest priority).
   int GetPriority() const { return mPriority; }

   //! Sets the priority of the message.
   //! @param aPriority The message's priority (0 being the lowest priority).
   void SetPriority(int aPriority) { mPriority = aPriority; }

   //! Get the address of the source interface that sent this message.
   //! @returns The address of the source interface.
   const wsf::comm::Address& GetSrcAddr() const { return mSrcAddr; }

   //! Set the address of the source interface that sent this message.
   //! @param aSrcAddr The address of the source interface.
   /*void SetSrcAddr(const WsfCommAddress& aSrcAddr);*/
   void SetSrcAddr(const wsf::comm::Address& aSrcAddr);

   //! Get the address of the final destination interface for which this message in destined.
   //! @returns The address of the destination interface.
   const wsf::comm::Address& GetDstAddr() const { return mDstAddr; }

   //! Set the address of the final destination interface for which this message is destined.
   //! @param aDstAddr The address of the destination interface.
   void SetDstAddr(const wsf::comm::Address& aDstAddr) { mDstAddr = aDstAddr; }

   //! Get the address of the next-hop interface for which this message in destined.
   //! @returns The address of the next-hop interface.
   const wsf::comm::Address& GetNextHopAddr() const { return mNextHopAddr; }

   //! Set the address of the next-hop interface for which this message is destined.
   //! @param aNextHopAddr The address of the next-hop interface.
   /*void SetNextHopAddr(const WsfCommAddress& aNextHopAddr);*/
   void SetNextHopAddr(const wsf::comm::Address& aNextHopAddr);

   virtual bool CanBeReplacedBy(const WsfMessage& aMessage) const;

   //! @name Component Info Interface
   //! These optional methods define an interface that can be used by subclasses
   //! to provide additional information on the component that either generated
   //! the message or the data contained in the message. They can be used to
   //! filter messages based on the information returned. The methods in the base
   //! class will return a null (0) WsfStringId.
   //@{
   //! @return The string ID of the related component's name
   virtual WsfStringId GetComponentNameId() const { return WsfStringId(); }
   //! @return The string ID of the related component's type
   virtual WsfStringId GetComponentTypeId() const { return WsfStringId(); }
   //! @return The string ID of the related component's mode
   virtual WsfStringId GetComponentModeId() const { return WsfStringId(); }
   //@}

   //! @name Optional Group Specification Accessor/Mutator
   //! Provides the ability to set the group identification for a message.
   //! Used by the communications framework for identifying where a message is supposed
   //! to go without using addressing for legacy support.
   //! This is only for internal usage, and must be placed on the WsfMessage object
   //! as legacy usage of comms still utilizes the WsfMessage object instead of WsfCommMessage.
   //! @note Because many of the use cases for this message is a const WsfMessage,
   //! a const mutator is being provided until more widespread modification are acceptable.
   //@{
   WsfStringId GetGroup() const { return mGroup; }

   //! @note Removal for AFSIM 3.0
   void SetGroup(WsfStringId aGroup) const { mGroup = aGroup; }
   //@}

   friend WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const WsfMessage& aMessage);

   //! Print function to output a data tag at the provided decimal precision
   //! @param aStream    The output stream to which to print the data tag
   //! @param aPrecision The desired print precision
   void PrintDataTagPrecise(std::ostream& aStream, std::streamsize aPrecision) const;

   //! (De)serializer for XIO
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mStaticType& mType& mSubType& mDataTag& mSerialNumber& mSizeBits& mPriority;
      aBuff& mOriginator&                                                      mOriginatorIndex;
      aBuff& mSrcAddr& mDstAddr&                                               mNextHopAddr;
      aBuff&                                                                   mGroup;
      WsfAuxDataEnabled::Serialize(aBuff);
   }

protected:
   WsfMessage(const WsfMessage& aSrc) = default;

private:
   WsfSimulation* mSimulationPtr; //!< The simulation that created this message
   WsfStringId    mStaticType;    //!< The type of the most derived c++ class

   WsfStringId  mType;         //!< The type of the message
   WsfStringId  mSubType;      //!< The sub-type of the message
   double       mDataTag;      //!< The data tag from the message contents.
   unsigned int mSerialNumber; //!< The serial number of the message
   int          mSizeBits;     //!< The logical size of the message (bits)
   int          mPriority;     //!< The message priority (0->10, lowest->highest).

   WsfStringId mOriginator;      //!< The platform name ID of the originator
   size_t      mOriginatorIndex; //!< The platform index of the originator

   wsf::comm::Address mSrcAddr;     //!< The source address of the message
   wsf::comm::Address mDstAddr;     //!< The final destination address of the message
   wsf::comm::Address mNextHopAddr; //!< The next hop address of the message

   // Provided for legacy support of communications - expected removal for AFSIM 3.0.
   mutable WsfStringId mGroup; //!< Optional group message specification.
};

#endif
