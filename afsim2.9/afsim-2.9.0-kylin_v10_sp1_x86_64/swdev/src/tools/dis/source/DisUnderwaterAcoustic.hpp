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


#ifndef DISUNDERWATERACOUSTIC_HPP
#define DISUNDERWATERACOUSTIC_HPP

#include "dis_export.h"

#include "DisAPA.hpp"
#include "DisEntityId.hpp"
#include "DisEventId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisShaft.hpp"
#include "DisUASystem.hpp"

class DisUASystem;
class GenI;
class GenO;

class DIS_EXPORT DisUnderwaterAcoustic : public DisPdu
{
public:
   DisUnderwaterAcoustic();
   DisUnderwaterAcoustic(DisPdu& aPdu, GenI& aGenI);
   DisUnderwaterAcoustic(GenI& aGenI);
   DisUnderwaterAcoustic(const DisUnderwaterAcoustic& aSrc);

   ~DisUnderwaterAcoustic() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetOriginatingEntity() const override;

   const DisEntityId& GetEmitterId() const;
   const DisEventId&  GetEventId() const;
   DisEnum8           GetStateUpdate() const;
   DisUint16          GetPassiveParameterIndex() const;
   DisEnum8           GetPropulsionPlantConfiguration() const;
   DisUint8           GetShaftCount() const;
   DisUint8           GetAPACount() const;
   DisUint8           GetSystemCount() const;

   // Mutators

   void SetEmitterId(const DisEntityId& aEmitterId);
   void SetEventId(const DisEventId& aEventId);
   void SetStateUpdate(DisEnum8 aStateUpdate);
   void SetPassiveParameterIndex(DisUint16 aParm);
   void SetPropulsionPlantConfiguration(DisEnum8 aConfig);

   // Input/Output

   DisUint16 GetLength() override;
   void      Get(GenI& aGenI) override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   // Shafts list maintenance...

   // WARNING: AddShaft takes ownership of the passed object.
   void AddShaft(DisShaft* aShaft);

   // Return iterators to Shaft vector.
   std::vector<DisShaft*>::iterator       GetShaftBegin();
   std::vector<DisShaft*>::const_iterator GetShaftBegin() const;
   std::vector<DisShaft*>::iterator       GetShaftEnd();
   std::vector<DisShaft*>::const_iterator GetShaftEnd() const;

   // Remove and delete a DisShaft object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Shaft on the list.
   unsigned int RemoveShaft(DisShaft* aShaft);

   // Remove and delete all DisShaft objects from the list.
   void RemoveAllShafts();

   // Remove DisShaft object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Shaft on the list.
   unsigned int RemoveShaftWithNoDelete(DisShaft* aShaft);

   // Remove all DisShaft objects from the list, but DO NOT DELETE them.
   void RemoveAllShaftsWithNoDelete();

   // APA list maintenance...

   // WARNING: AddAPA takes ownership of the passed object.
   void AddAPA(DisAPA* aAPA);

   // Return iterators to APA vector.
   std::vector<DisAPA*>::iterator       GetAPABegin();
   std::vector<DisAPA*>::const_iterator GetAPABegin() const;
   std::vector<DisAPA*>::iterator       GetAPAEnd();
   std::vector<DisAPA*>::const_iterator GetAPAEnd() const;

   // Remove and delete a DisAPA object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first APA on the list.
   unsigned int RemoveAPA(DisAPA* aAPA);

   // Remove and delete all DisAPA objects from the list.
   void RemoveAllAPAs();

   // Remove DisAPA object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first APA on the list.
   unsigned int RemoveAPAWithNoDelete(DisAPA* aAPA);

   // Remove all DisAPA objects from the list, but DO NOT DELETE them.
   void RemoveAllAPAsWithNoDelete();


   // System list maintenance...

   // WARNING: AddSystem takes ownership of the passed object.
   void AddSystem(DisUASystem* aSystem);

   // Return iterators to APA vector.
   std::vector<DisUASystem*>::iterator       GetSystemBegin();
   std::vector<DisUASystem*>::const_iterator GetSystemBegin() const;
   std::vector<DisUASystem*>::iterator       GetSystemEnd();
   std::vector<DisUASystem*>::const_iterator GetSystemEnd() const;


   // Remove and delete a DisUASystem object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first System on the list.
   unsigned int RemoveSystem(DisUASystem* aSystem);

   // Remove and delete all DisUASystem objects from the list.
   void RemoveAllSystems();

   // Remove DisUASystem object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first System on the list.
   unsigned int RemoveSystemWithNoDelete(DisUASystem* aSystem);

   // Remove all DisUASystem objects from the list, but DO NOT DELETE them.
   void RemoveAllSystemsWithNoDelete();

   // Enumerations

   // State/Change Update Indicator
   enum
   {
      StateUpdate       = 0,
      ChangedDataUpdate = 1
   };
   // Acoustic System Name
   enum
   {
      NameOther  = 0,
      AN_BQQ_5   = 1,
      AN_SSQ_62  = 2,
      AN_SQS_23  = 3,
      AN_SQS_26  = 4,
      AN_SQS_53  = 5,
      ALFS       = 6,
      LFA        = 7,
      AN_AQS_901 = 8,
      AN_AQS_902 = 9
   };
   // Function
   enum
   {
      FunctionOther             = 0,
      PlatformSearchDetectTrack = 1,
      Navigation                = 2,
      MineHunting               = 3,
      WeaponSearchDetectTrack   = 4
   };
   // Scan Pattern
   enum
   {
      ScanPatternNotUsed = 0,
      Conical            = 1,
      Helical            = 2,
      Raster             = 3,
      SectorSearch       = 4,
      ContinuousSearch   = 5
   };
   // Propulsion Plant Configuration
   enum
   {
      PPC_Other        = 0,
      DieselElectric   = 1,
      Diesel           = 2,
      Battery          = 3,
      TurbineReduction = 4,
      Steam            = 6,
      GasTurbine       = 7

      // bit 7 = Hull Mounted Masker status
      //         0 = Off; 1 = On
   };

private:
   void GetMemberData(GenI& aGenI);

   void UpdateShaftCount() const;
   void UpdateAPACount() const;
   void UpdateSystemCount() const;

   DisEntityId      mEmitterId;
   DisEventId       mEventId;
   DisEnum8         mStateUpdate;
   DisUint16        mIndex;
   DisEnum8         mPropulsionPlantConfig;
   mutable DisUint8 mShaftCount;
   mutable DisUint8 mAPACount;
   mutable DisUint8 mSystemCount;

   DisPtrContainer<DisShaft>    mShaftList;
   DisPtrContainer<DisAPA>      mAPAList;
   DisPtrContainer<DisUASystem> mSystemList;
};

inline void DisUnderwaterAcoustic::UpdateShaftCount() const
{
   mShaftCount = static_cast<DisUint8>(mShaftList.GetSize());
}
inline void DisUnderwaterAcoustic::UpdateAPACount() const
{
   mAPACount = static_cast<DisUint8>(mAPAList.GetSize());
}
inline void DisUnderwaterAcoustic::UpdateSystemCount() const
{
   mSystemCount = static_cast<DisUint8>(mSystemList.GetSize());
}

// Accessors

inline const DisEntityId& DisUnderwaterAcoustic::GetEmitterId() const
{
   return mEmitterId;
}
inline const DisEventId& DisUnderwaterAcoustic::GetEventId() const
{
   return mEventId;
}
inline DisEnum8 DisUnderwaterAcoustic::GetStateUpdate() const
{
   return mStateUpdate;
}
inline DisUint8 DisUnderwaterAcoustic::GetShaftCount() const
{
   UpdateShaftCount();
   return mShaftCount;
}
inline DisUint8 DisUnderwaterAcoustic::GetAPACount() const
{
   UpdateAPACount();
   return mAPACount;
}
inline DisUint8 DisUnderwaterAcoustic::GetSystemCount() const
{
   UpdateSystemCount();
   return mSystemCount;
}

// Mutators
inline void DisUnderwaterAcoustic::SetEmitterId(const DisEntityId& aEmitterId)
{
   mEmitterId = aEmitterId;
}
inline void DisUnderwaterAcoustic::SetEventId(const DisEventId& aEventId)
{
   mEventId = aEventId;
}
inline void DisUnderwaterAcoustic::SetStateUpdate(DisEnum8 aStateUpdate)
{
   mStateUpdate = aStateUpdate;
}

inline void DisUnderwaterAcoustic::SetPassiveParameterIndex(DisUint16 aIndex)
{
   mIndex = aIndex;
}
inline void DisUnderwaterAcoustic::SetPropulsionPlantConfiguration(DisEnum8 aConfig)
{
   mPropulsionPlantConfig = aConfig;
}
// Shaft list methods

inline void DisUnderwaterAcoustic::AddShaft(DisShaft* aShaft)
{
   mShaftList.Add(aShaft);
   UpdateShaftCount();
}

inline std::vector<DisShaft*>::iterator DisUnderwaterAcoustic::GetShaftBegin()
{
   return mShaftList.GetBegin();
}

inline std::vector<DisShaft*>::const_iterator DisUnderwaterAcoustic::GetShaftBegin() const
{
   return mShaftList.GetBegin();
}

inline std::vector<DisShaft*>::iterator DisUnderwaterAcoustic::GetShaftEnd()
{
   return mShaftList.GetEnd();
}

inline std::vector<DisShaft*>::const_iterator DisUnderwaterAcoustic::GetShaftEnd() const
{
   return mShaftList.GetEnd();
}

inline unsigned int DisUnderwaterAcoustic::RemoveShaft(DisShaft* aShaft)
{
   return mShaftList.Remove(aShaft);
}

inline void DisUnderwaterAcoustic::RemoveAllShafts()
{
   mShaftList.RemoveAll();
}

inline unsigned int DisUnderwaterAcoustic::RemoveShaftWithNoDelete(DisShaft* aShaft)
{
   return mShaftList.RemoveWithNoDelete(aShaft);
}

inline void DisUnderwaterAcoustic::RemoveAllShaftsWithNoDelete()
{
   mShaftList.RemoveAllWithNoDelete();
}

// APA list methods

inline void DisUnderwaterAcoustic::AddAPA(DisAPA* aAPA)
{
   mAPAList.Add(aAPA);
}

inline std::vector<DisAPA*>::iterator DisUnderwaterAcoustic::GetAPABegin()
{
   return mAPAList.GetBegin();
}

inline std::vector<DisAPA*>::const_iterator DisUnderwaterAcoustic::GetAPABegin() const
{
   return mAPAList.GetBegin();
}

inline std::vector<DisAPA*>::iterator DisUnderwaterAcoustic::GetAPAEnd()
{
   return mAPAList.GetEnd();
}

inline std::vector<DisAPA*>::const_iterator DisUnderwaterAcoustic::GetAPAEnd() const
{
   return mAPAList.GetEnd();
}

inline unsigned int DisUnderwaterAcoustic::RemoveAPA(DisAPA* aAPA)
{
   return mAPAList.Remove(aAPA);
}

inline void DisUnderwaterAcoustic::RemoveAllAPAs()
{
   mAPAList.RemoveAll();
}

inline unsigned int DisUnderwaterAcoustic::RemoveAPAWithNoDelete(DisAPA* aAPA)
{
   return mAPAList.RemoveWithNoDelete(aAPA);
}

inline void DisUnderwaterAcoustic::RemoveAllAPAsWithNoDelete()
{
   mAPAList.RemoveAllWithNoDelete();
}

// System list methods

inline void DisUnderwaterAcoustic::AddSystem(DisUASystem* aSystem)
{
   mSystemList.Add(aSystem);
}

inline std::vector<DisUASystem*>::iterator DisUnderwaterAcoustic::GetSystemBegin()
{
   return mSystemList.GetBegin();
}

inline std::vector<DisUASystem*>::const_iterator DisUnderwaterAcoustic::GetSystemBegin() const
{
   return mSystemList.GetBegin();
}

inline std::vector<DisUASystem*>::iterator DisUnderwaterAcoustic::GetSystemEnd()
{
   return mSystemList.GetEnd();
}

inline std::vector<DisUASystem*>::const_iterator DisUnderwaterAcoustic::GetSystemEnd() const
{
   return mSystemList.GetEnd();
}

inline unsigned int DisUnderwaterAcoustic::RemoveSystem(DisUASystem* aSystem)
{
   return mSystemList.Remove(aSystem);
}

inline void DisUnderwaterAcoustic::RemoveAllSystems()
{
   mSystemList.RemoveAll();
}

inline unsigned int DisUnderwaterAcoustic::RemoveSystemWithNoDelete(DisUASystem* aSystem)
{
   return mSystemList.RemoveWithNoDelete(aSystem);
}

inline void DisUnderwaterAcoustic::RemoveAllSystemsWithNoDelete()
{
   mSystemList.RemoveAllWithNoDelete();
}
#endif
