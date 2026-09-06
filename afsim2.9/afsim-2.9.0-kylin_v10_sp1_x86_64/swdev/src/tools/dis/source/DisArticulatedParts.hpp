//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISARTICULATEDPARTS_HPP
#define DISARTICULATEDPARTS_HPP

#include "dis_export.h"

#include <memory>
#include <vector>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisLiveEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;
class DisVariableParameterRecord;

/**!
 * The Articulated Parts PDU shall communicate information about an entity’s articulated and attached parts.
 * This PDU includes state information that is necessary for the receiving simulation applications to represent
 * the issuing entity’s articulated and attached parts appearance in its own simulation.
 */
class DIS_EXPORT DisArticulatedParts : public DisPdu
{
public:
   DisArticulatedParts();
   DisArticulatedParts(const DisArticulatedParts&);
   DisArticulatedParts(const DisPdu& aPdu, GenI& aGenI);
   explicit DisArticulatedParts(GenI& aGenI);
   ~DisArticulatedParts() override = default;

   DisArticulatedParts* Clone() const override;
   int                  GetClass() const override;

   // Accessors
   const DisLiveEntityId& GetLiveEntityId() const;
   DisUint8               GetVariableParameterRecordCount() const;

   // Mutators
   void SetLiveEntityId(const DisLiveEntityId& aLiveEntityId);

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   //! Sends the contents of the PDU to a string form
   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

   // Data Validation
   bool        IsValid() const override;
   std::string GetStringId() const override;

   // Variable Parameter Record vector modifiers
   void AddRecord(DisVariableParameterRecord& aPartPtr);
   void RemoveAllRecords();
   void RemoveRecord(DisVariableParameterRecord* aPartPtr);

   // Return iterators to DisEntityPart vector.
   std::vector<std::unique_ptr<DisVariableParameterRecord>>::iterator       GetVariableParameterRecordtBegin();
   std::vector<std::unique_ptr<DisVariableParameterRecord>>::const_iterator GetVariableParameterRecordBegin() const;
   std::vector<std::unique_ptr<DisVariableParameterRecord>>::iterator       GetVariableParameterRecordEnd();
   std::vector<std::unique_ptr<DisVariableParameterRecord>>::const_iterator GetVariableParameterRecordEnd() const;
   DisArticulatedParts& operator=(const DisArticulatedParts& aRhs) = delete;

private:
   void GetMemberData(GenI& aGenI);
   void GetRecords(GenI& aGenI);
   void PutRecords(GenO& aGenO);

   //! The entity issuing the PDU
   DisLiveEntityId mLiveEntityId;

   //! The number of Variable Parameter records present.
   DisUint8 mVariableParameterRecordCount = 0;

   //! The parameter values for each Variable Parameter record that is included
   std::vector<std::unique_ptr<DisVariableParameterRecord>> mVariableParameterRecords;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 5;
};

// Define GenIO insertion and extraction operators for LiveEntityId
inline GenO& operator<<(GenO& aGenO, DisArticulatedParts& aPart)
{
   aPart.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisArticulatedParts& aPart)
{
   aPart.Get(aGenI);
   return aGenI;
}

inline int DisArticulatedParts::GetClass() const
{
   return DisEnum::Pdu::Type::ArticulatedParts;
}

inline const DisLiveEntityId& DisArticulatedParts::GetLiveEntityId() const
{
   return mLiveEntityId;
}

inline DisUint8 DisArticulatedParts::GetVariableParameterRecordCount() const
{
   return mVariableParameterRecordCount;
}

inline void DisArticulatedParts::SetLiveEntityId(const DisLiveEntityId& aLiveEntityId)
{
   mLiveEntityId = aLiveEntityId;
}

inline std::vector<std::unique_ptr<DisVariableParameterRecord>>::iterator DisArticulatedParts::GetVariableParameterRecordtBegin()
{
   return mVariableParameterRecords.begin();
}

inline std::vector<std::unique_ptr<DisVariableParameterRecord>>::const_iterator
DisArticulatedParts::GetVariableParameterRecordBegin() const
{
   return mVariableParameterRecords.begin();
}

inline std::vector<std::unique_ptr<DisVariableParameterRecord>>::iterator DisArticulatedParts::GetVariableParameterRecordEnd()
{
   return mVariableParameterRecords.end();
}

inline std::vector<std::unique_ptr<DisVariableParameterRecord>>::const_iterator
DisArticulatedParts::GetVariableParameterRecordEnd() const
{
   return mVariableParameterRecords.end();
}
#endif
