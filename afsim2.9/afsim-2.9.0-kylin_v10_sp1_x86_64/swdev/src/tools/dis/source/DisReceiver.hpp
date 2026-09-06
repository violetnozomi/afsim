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

#ifndef DISRECEIVER_HPP
#define DISRECEIVER_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

//! Defines information for communicating the receiver state
class DIS_EXPORT DisReceiver : public DisPdu
{
public:
   DisReceiver();
   DisReceiver(const DisReceiver& aSrc);
   DisReceiver(DisPdu& aPdu, GenI& aGenI);
   DisReceiver(GenI& aGenI);
   ~DisReceiver() override = default;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   enum // Receiver state
   {
      RsOff               = 0,
      RsOnAndNotReceiving = 1,
      RsOnAndReceiving    = 2
   };

   // Accessors

   const DisEntityId& GetEntityId() const;
   DisUint16          GetRadioId() const;
   DisUint16          GetReceiverState() const;
   DisFloat32         GetReceivedPower() const;
   const DisEntityId& GetTransmitterEntityId() const;
   DisUint16          GetTransmitterRadioId() const;

   // Mutators

   void SetEntityId(const DisEntityId& aEntityId);
   void SetRadioId(DisUint16 aRadioId);
   void SetReceiverState(DisUint16 aReceiverState);
   void SetReceivedPower(DisFloat32 aReceivedPower);
   void SetTransmitterEntityId(const DisEntityId& aTransmitterEntityId);
   void SetTransmitterRadioId(DisUint16 aTransmitterRadioId);

   // Input/Output

   void        Get(GenI& aGenI) override;
   DisUint16   GetLength() override;
   void        Put(GenO& aGenO) override;
   bool        IsValid() const override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

private:
   // Disallow assignment
   DisReceiver& operator=(const DisReceiver& aRhs);

   void GetMemberData(GenI& aGenI);

   /*!
    * Identifies the entity or object to which the radio is attached. For unattached radios
    * this variable should contain an unattached identifier record.
    */
   DisEntityId mEntityId{DisEntityId::ENTITY_ID_UNKNOWN};
   /*!
    * Identifies a particular radio that is either associated with an entity, object
    * or is an unattached radio.
    */
   DisUint16 mRadioId{0};
   /*!
    * Indicates the state of the receiver, either idle or active.
    */
   DisEnum16 mReceiverState{0};
   /*!
    * Indicates the radio frequency power received after applying any propagation loss
    * and antenna gain.
    */
   DisFloat32 mReceivedPower{0.0f};
   /*!
    * Identifies the radio that is the source of the transmission that is currently being received.
    * The selection of the received transmitter depends on the characteristics and state of the simulation
    * reciever.
    */
   DisEntityId mTransmitterEntityId{};
   /*!
    * Identifies the particular radio within the radio transmitter.
    */
   DisUint16 mTransmitterRadioId{0};
};

// Accessors

inline const DisEntityId& DisReceiver::GetEntityId() const
{
   return mEntityId;
}
inline DisUint16 DisReceiver::GetRadioId() const
{
   return mRadioId;
}
inline DisEnum16 DisReceiver::GetReceiverState() const
{
   return mReceiverState;
}
inline DisFloat32 DisReceiver::GetReceivedPower() const
{
   return mReceivedPower;
}
inline const DisEntityId& DisReceiver::GetTransmitterEntityId() const
{
   return mTransmitterEntityId;
}
inline DisUint16 DisReceiver::GetTransmitterRadioId() const
{
   return mTransmitterRadioId;
}

// Mutators

inline void DisReceiver::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}
inline void DisReceiver::SetRadioId(DisUint16 aRadioId)
{
   mRadioId = aRadioId;
}
inline void DisReceiver::SetReceiverState(DisEnum16 aReceiverState)
{
   mReceiverState = aReceiverState;
}
inline void DisReceiver::SetReceivedPower(DisFloat32 aReceivedPower)
{
   mReceivedPower = aReceivedPower;
}
inline void DisReceiver::SetTransmitterEntityId(const DisEntityId& aTransmitterEntityId)
{
   mTransmitterEntityId = aTransmitterEntityId;
}
inline void DisReceiver::SetTransmitterRadioId(DisUint16 aTransmitterRadioId)
{
   mTransmitterRadioId = aTransmitterRadioId;
}

#endif
