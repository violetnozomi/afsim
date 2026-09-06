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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DISIOEFFECTRECORD_HPP
#define DISIOEFFECTRECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisIOBaseRecord.hpp"
#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

//! Class for identification of IO effects on an entity when calculated by an IO simulation
class DIS_EXPORT DisIOEffectRecord : public DisIOBaseRecord
{
public:
   DisIOEffectRecord();
   DisIOEffectRecord(const DisIOEffectRecord& aSrc);
   DisIOEffectRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI);
   explicit DisIOEffectRecord(GenI& aGenI);
   ~DisIOEffectRecord() override = default;
   DisIOEffectRecord*  Clone() const override;
   DisIORecordTypeEnum GetIORecordType() const override;

   //----------------------- Script accessibility methods -----------------------
   const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

   // Input / Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   void SetIOStatus(const DisEnum8 aIOStatus);
   void SetIOLinkType(const DisEnum8 aIOLinkType);
   void SetIOEffect(const DisEnum8 aIOEffect);
   void SetIOEffectDutyCycle(const DisUint8 aIOEffectDutyCycle);
   void SetIOEffectDuration(const DisUint16 aIOEffectDuration);
   void SetIOProcess(const DisEnum16 aIOProcess);

   DisEnum8  GetIOStatus() const;
   DisEnum8  GetIOLinkType() const;
   DisEnum8  GetIOEffect() const;
   DisUint8  GetIOEffectDutyCycle() const;
   DisUint16 GetIOEffectDuration() const;
   DisEnum16 GetIOProcess() const;

   bool        operator==(const DisRecordHeader& other) const override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;
   /*!
    * Record type can be found in the SISO-REF-010-2019.pdf section 19: Variable Record Types.
    * NOTE: Not all values are present and the document assumes additional family
    * values are incremented in accordance to the notation described in
    * previous family values.
    */
   static const DisUint32 cRECORD_TYPE = 5500;
   //! Fixed record size in octets
   static const DisUint32 cRECORD_LENGTH = 16;

   //! indicate whether the IO effect has an effect on the sender, receiver,
   //! message(s), or some combination of them.
   DisEnum8 mIOStatus = 0;
   //! The IO link type as a logical or physical link or node.
   DisEnum8 mIOLinkType = 0;
   //! The IO effect associated with this IO attack.
   DisEnum8 mIOEffect = 0;
   //! the IO effect duty cycle represented as a percentage in
   //! the range of 0% to 100% in 1% increments.
   DisUint8 mIOEffectDutyCycle = 0;
   //! the duration of the IO effect in seconds, from 1 s to
   //! 65 534 s. Set to IO_UNTIL_FURTHER_NOTICE (65 535) if the duration has no
   //! fixed simulation time interval, 0 only if the IO Effect field is set to Terminate Effect (255).
   DisUint16 mIOEffectDuration = 65535;
   //! The IO process being performed
   DisEnum16 mIOProcess = 0;
};

#endif
