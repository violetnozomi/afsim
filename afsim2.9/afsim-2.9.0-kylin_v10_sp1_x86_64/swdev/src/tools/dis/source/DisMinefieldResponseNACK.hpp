//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef DISMINEFIELDRESPONSENACK_HPP
#define DISMINEFIELDRESPONSENACK_HPP

#include "dis_export.h"

#include <utility>
#include <vector>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"
#include "UtVec3.hpp"

class GenI;
class GenO;

/**!
 * The Minefield Response NACK PDU shall provide the means by which a simulation
 * shall request a minefield simulation to retransmit Minefield Data PDUs not received
 * in response to a query when operating in QRP mode.
 */
class DIS_EXPORT DisMinefieldResponseNACK : public DisPdu
{
public:
   DisMinefieldResponseNACK();
   DisMinefieldResponseNACK(const DisMinefieldResponseNACK& aSrc);
   DisMinefieldResponseNACK(const DisPdu& aPdu, GenI& aGenI);
   explicit DisMinefieldResponseNACK(GenI& aGenI);

   DisMinefieldResponseNACK* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Accessors
   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetMineFieldId() const;
   const DisEntityId& GetRequestingSimulationId() const;
   DisUint32          GetRequestId() = delete;
   DisUint8           GetMinefieldRequestID() const;
   DisUint8           GetMissingPDUNumber() const;

   // Mutators
   void SetMinefieldId(const DisEntityId& aMinefieldId);
   void SetMinefieldRequestID(const DisUint8 aMinefieldRequestID);
   void SetRequestingSimulationId(const DisEntityId& aSimId);
   void AddMissingPDUSequence(DisUint8 aSequence);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Return iterators to MineType vector.
   std::vector<DisUint8>::iterator       GetMissingPDUSequencesBegin();
   std::vector<DisUint8>::const_iterator GetMissingPDUSequencesBegin() const;
   std::vector<DisUint8>::iterator       GetMissingPDUSequencesEnd();
   std::vector<DisUint8>::const_iterator GetMissingPDUSequencesEnd() const;

private:
   void GetMemberData(GenI& aGenI);
   //! The minefield to which this PDU is addressed.
   DisEntityId mMinefieldId = DisEntityId::ENTITY_ID_UNKNOWN;

   /**!
    * The simulation that generated the query and is requesting retransmission
    * of information from the minefield simulation.
    */
   DisEntityId mRequestingSimulationId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! The minefield query request
   DisUint8 mMinefieldRequestID = 0;

   /**!
    * The number of PDUs that were not received by the requesting simulation
    * in response to a minefield query request.
    */
   DisUint8 mMissingPDUNumber = 0;

   //! The sequence numbers of the missing PDUs.
   std::vector<DisUint8> mMissingPDUSequences;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 14;
};

inline int DisMinefieldResponseNACK::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::MinefieldResponseNACK);
}

inline int DisMinefieldResponseNACK::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::Minefield);
}

inline const DisEntityId& DisMinefieldResponseNACK::GetOriginatingEntity() const
{
   return mRequestingSimulationId;
}

// Accessors
inline const DisEntityId& DisMinefieldResponseNACK::GetMineFieldId() const
{
   return mMinefieldId;
}

inline const DisEntityId& DisMinefieldResponseNACK::GetRequestingSimulationId() const
{
   return mRequestingSimulationId;
}

inline DisUint8 DisMinefieldResponseNACK::GetMinefieldRequestID() const
{
   return mMinefieldRequestID;
}

inline DisUint8 DisMinefieldResponseNACK::GetMissingPDUNumber() const
{
   return mMissingPDUNumber;
}

// Mutators

inline void DisMinefieldResponseNACK::SetMinefieldId(const DisEntityId& aMinefieldId)
{
   mMinefieldId = aMinefieldId;
}

inline void DisMinefieldResponseNACK::SetMinefieldRequestID(const DisUint8 aMinefieldRequestID)
{
   mMinefieldRequestID = aMinefieldRequestID;
}

inline void DisMinefieldResponseNACK::SetRequestingSimulationId(const DisEntityId& aSimId)
{
   mRequestingSimulationId = aSimId;
}

inline void DisMinefieldResponseNACK::AddMissingPDUSequence(DisUint8 aSequence)
{
   mMissingPDUSequences.push_back(aSequence);
   mMissingPDUNumber++;
}

inline std::string DisMinefieldResponseNACK::GetStringId() const
{
   return "DisMinefieldQuery";
}

inline std::vector<DisEnum8>::iterator DisMinefieldResponseNACK::GetMissingPDUSequencesBegin()
{
   return mMissingPDUSequences.begin();
}

inline std::vector<DisEnum8>::const_iterator DisMinefieldResponseNACK::GetMissingPDUSequencesBegin() const
{
   return mMissingPDUSequences.begin();
}

inline std::vector<DisEnum8>::iterator DisMinefieldResponseNACK::GetMissingPDUSequencesEnd()
{
   return mMissingPDUSequences.end();
}

inline std::vector<DisEnum8>::const_iterator DisMinefieldResponseNACK::GetMissingPDUSequencesEnd() const
{
   return mMissingPDUSequences.end();
}
#endif
