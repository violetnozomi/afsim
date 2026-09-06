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

#ifndef DISIFF_HPP
#define DISIFF_HPP

// NOTE: This is NOT a complete implementation of the IFF/ATC/NAVAIDS PDU.
//       It only supports layers 1, 2, 3*, and 5. If other layers are received
//       they will be ignored and not sent if the PDU is subsequently sent.
//
//       * Only part of Layer 3 has been implemented.

#include "dis_export.h"

#include "DisAisPositionData.hpp"
#include "DisAisStaticData.hpp"
#include "DisBeamDataRecord.hpp"
#include "DisEntityId.hpp"
#include "DisEventId.hpp"
#include "DisIffAtcNavaidsParams.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSystemId.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisIff : public DisPdu
{
public:
   DisIff();
   DisIff(DisPdu& aPdu, GenI& aGenI);
   DisIff(GenI& aGenI);
   ~DisIff() override;

   DisPdu* Clone() const override;
   DisIff(const DisIff& aSrc);
   const DisIff& operator=(const DisIff& aRhs);

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Layer 1 Accessors
   const DisEntityId& GetEmitterId() const;
   const DisEventId&  GetEventId() const;
   void               GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const;
   const DisSystemId& GetSystemId() const;
   DisEnum8           GetSystemStatus() const;
   DisEnum8           GetAlternateParameter4() const;
   DisUint8           GetInformationLayers() const;
   DisEnum8           GetModifier() const;
   DisEnum16          GetParameter1() const;
   DisEnum16          GetParameter2() const;
   DisEnum16          GetParameter3() const;
   DisEnum16          GetParameter4() const;
   DisEnum16          GetParameter5() const;
   DisEnum16          GetParameter6() const;

   // Layer 2 Accessors
   const DisBeamDataRecord& GetBeamDataRecord() const;
   DisEnum8                 GetLayer2Parameter1() const;
   DisEnum8                 GetLayer2Parameter2() const;

   // Layer 3 Accessors
   DisUint16 GetReportingSite() const;
   DisUint16 GetReportingApplication() const;

   DisEnum16 GetMode5Status() const;
   DisEnum16 GetPIN() const;
   DisEnum32 GetMode5MessageFormatPresent() const;
   DisEnum16 GetEnhancedMode1() const;
   DisEnum8  GetNationOfOrigin() const;
   DisEnum8  GetMissionCode() const;
   DisEnum8  GetSquitterStatus() const;
   DisEnum8  GetNavigationSource() const;
   DisEnum8  GetFigureOfMerit() const;

   DisUint32 GetNumberOfMode5Records() const;

   // Layer 5 Accessors
   const DisAisPositionData* GetAisPositionData() const;
   const DisAisStaticData*   GetAisStaticData() const;

   // Layer 1 Mutators
   void SetEmitterId(const DisEntityId& aEmitterId);
   void SetEventId(const DisEventId& aEventId);
   void SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);
   void SetSystemId(const DisSystemId& aSystemId);
   void SetSystemStatus(DisEnum8 aSystemStatus);
   void SetAlternateParameter4(DisEnum8 aAlternateParameter4);
   void SetInformationLayers(DisUint8 aInformationLayers);
   void SetModifier(DisEnum8 aModifier);
   void SetParameter1(DisEnum16 aParameter1);
   void SetParameter2(DisEnum16 aParameter2);
   void SetParameter3(DisEnum16 aParameter3);
   void SetParameter4(DisEnum16 aParameter4);
   void SetParameter5(DisEnum16 aParameter5);
   void SetParameter6(DisEnum16 aParameter6);

   // Layer 2 Mutators
   void SetBeamDataRecord(const DisBeamDataRecord& aRecord);
   void SetLayer2Parameter1(DisEnum8 aParameter1);
   void SetLayer2Parameter2(DisEnum8 aParameter2);

   // Layer 3 Mutators
   void SetReportingSite(const DisUint16 aSite);
   void SetReportingApplication(const DisUint16 aApp);

   void SetMode5Status(const DisEnum16 aStatus);
   void SetPIN(const DisEnum16 aPIN);
   void SetMode5MessageFormatPresent(const DisUint32 aFlag);
   void SetEnhancedMode1(const DisEnum16 aMode);
   void SetNationOfOrigin(const DisEnum8 aCountry);
   void SetMissionCode(const DisEnum8 aCode);
   void SetSquitterStatus(const DisEnum8 aStatus);
   void SetNavigationSource(const DisEnum8 aSource);
   void SetFigureOfMerit(const DisEnum8 aMerit);

   void SetNumberOfMode5Records(const DisUint32 aNumber);
   // Above should be part of void SetMode5Records(); or something like that

   // Layer 5 Mutators (takes ownership of the passed pointers)
   void SetAisPositionData(DisAisPositionData* aData);
   void SetAisStaticData(DisAisStaticData* aData);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   //! Data Validation
   bool IsValid() const override;

   // Layer 2 Fundamental Parameter Data maintenance.

   // WARNING: AddParameters takes ownership of the passed object.
   void AddParameters(DisIffAtcNavaidsParams* aParametersPtr);

   // Return iterators to DisIffAtcNavaidsParams vector.
   std::vector<DisIffAtcNavaidsParams*>::iterator       GetParamsBegin();
   std::vector<DisIffAtcNavaidsParams*>::const_iterator GetParamsBegin() const;
   std::vector<DisIffAtcNavaidsParams*>::iterator       GetParamsEnd();
   std::vector<DisIffAtcNavaidsParams*>::const_iterator GetParamsEnd() const;

   // Remove and delete an object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first entry on the list.
   unsigned int RemoveParameters(DisIffAtcNavaidsParams* aParametersPtr);

   // Remove and delete all objects from the list.
   void RemoveAllParameters();

   // Remove an object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first entry on the list.
   unsigned int RemoveParametersWithNoDelete(DisIffAtcNavaidsParams* aParametersPtr);

   // Remove all objects from the list, but DO NOT DELETE them.
   void RemoveAllParametersWithNoDelete();

private:
   void Copy(const DisIff& aRhs);
   void DeleteClassData();
   void GetMemberData(GenI& aGenI);
   bool IsLayerPresent(int aLayer) const;
   void UnsetLayerPresent(int aLayer);

   DisUint16 GetLayer1Length() const;
   DisUint16 GetLayer2Length() const;
   DisUint16 GetLayer3Length() const;
   DisUint16 GetLayer5Length() const;

   // Read layer data if it is present.  We take special precautions to make
   // sure we don't read past the end of the PDU just in case we're receiving
   // bad data. Update 'aLengthToRead' according to the amount read.
   bool ReadLayer1(GenI& aGenI, DisUint16& aLengthToRead);
   bool ReadLayer2(GenI& aGenI, DisUint16& aLengthToRead);
   bool ReadLayer2Params(GenI& aGenI, DisUint16& aLengthToRead, DisUint16 aLayerLength);
   bool ReadLayer3(GenI& aGenI, DisUint16& aLengthToRead);
   bool ReadLayer3Params(GenI& aGenI, DisUint16& aLengthToRead, DisUint16 aLayerLength);
   bool ReadLayer5(GenI& aGenI, DisUint16& aLengthToRead);
   bool ReadLayer5Records(GenI& aGenI, DisUint16& aLengthToRead, DisUint16 aNumRecords);

   void ResetLayer2Data();
   void ResetLayer3Data();
   void ResetLayer5Data();

   // Layer 1 data
   DisEntityId mEmitterId;
   DisEventId  mEventId;
   DisFloat32  mLocationX;
   DisFloat32  mLocationY;
   DisFloat32  mLocationZ;
   DisSystemId mSystemId;
   DisEnum8    mSystemStatus;
   DisEnum8    mAlternateParameter4;
   DisUint8    mInformationLayers;
   DisEnum8    mModifier;
   DisEnum16   mParameter1;
   DisEnum16   mParameter2;
   DisEnum16   mParameter3;
   DisEnum16   mParameter4;
   DisEnum16   mParameter5;
   DisEnum16   mParameter6;

   // Layer 2 data
   DisBeamDataRecord                       mBeamDataRecord;
   DisEnum8                                mLayer2Parameter1;
   DisEnum8                                mLayer2Parameter2;
   mutable DisUint16                       mParametersCount;
   DisPtrContainer<DisIffAtcNavaidsParams> mParametersList;

   // Layer 3 data
   DisUint16 mReportingSite;
   DisUint16 mReportingApplication;
   DisEnum16 mMode5Status;
   DisEnum16 mPIN;
   DisUint32 mMessageFormat;
   DisEnum16 mEnhancedMode1;
   DisEnum8  mNationOfOrigin;
   DisEnum8  mMissionCode;
   DisEnum8  mSquitterStatus;
   DisEnum8  mNavigationSource;
   DisEnum8  mFigureOfMerit;
   DisUint32 mNumMode5Records;

   // Layer 5 data
   DisAisPositionData* mAisPositionData;
   DisAisStaticData*   mAisStaticData;
};

// Accessors

inline const DisEntityId& DisIff::GetEmitterId() const
{
   return mEmitterId;
}
inline const DisEventId& DisIff::GetEventId() const
{
   return mEventId;
}
inline void DisIff::GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const
{
   aLocationX = mLocationX;
   aLocationY = mLocationY;
   aLocationZ = mLocationZ;
}
inline const DisSystemId& DisIff::GetSystemId() const
{
   return mSystemId;
}

inline DisEnum8 DisIff::GetSystemStatus() const
{
   return mSystemStatus;
}
inline DisEnum8 DisIff::GetAlternateParameter4() const
{
   return mAlternateParameter4;
}
inline DisUint8 DisIff::GetInformationLayers() const
{
   return mInformationLayers;
}
inline DisEnum8 DisIff::GetModifier() const
{
   return mModifier;
}
inline DisEnum16 DisIff::GetParameter1() const
{
   return mParameter1;
}
inline DisEnum16 DisIff::GetParameter2() const
{
   return mParameter2;
}
inline DisEnum16 DisIff::GetParameter3() const
{
   return mParameter3;
}
inline DisEnum16 DisIff::GetParameter4() const
{
   return mParameter4;
}
inline DisEnum16 DisIff::GetParameter5() const
{
   return mParameter5;
}
inline DisEnum16 DisIff::GetParameter6() const
{
   return mParameter6;
}
inline const DisBeamDataRecord& DisIff::GetBeamDataRecord() const
{
   return mBeamDataRecord;
}
inline DisEnum8 DisIff::GetLayer2Parameter1() const
{
   return mLayer2Parameter1;
}
inline DisEnum8 DisIff::GetLayer2Parameter2() const
{
   return mLayer2Parameter2;
}
inline DisUint16 DisIff::GetReportingSite() const
{
   return mReportingSite;
}
inline DisUint16 DisIff::GetReportingApplication() const
{
   return mReportingApplication;
}
inline DisEnum16 DisIff::GetMode5Status() const
{
   return mMode5Status;
}
inline DisEnum16 DisIff::GetPIN() const
{
   return mPIN;
}
inline DisEnum32 DisIff::GetMode5MessageFormatPresent() const
{
   return mMessageFormat;
}
inline DisEnum16 DisIff::GetEnhancedMode1() const
{
   return mEnhancedMode1;
}
inline DisEnum8 DisIff::GetNationOfOrigin() const
{
   return mNationOfOrigin;
}
inline DisEnum8 DisIff::GetMissionCode() const
{
   return mMissionCode;
}
inline DisEnum8 DisIff::GetSquitterStatus() const
{
   return mSquitterStatus;
}
inline DisEnum8 DisIff::GetNavigationSource() const
{
   return mNavigationSource;
}
inline DisEnum8 DisIff::GetFigureOfMerit() const
{
   return mFigureOfMerit;
}
inline DisUint32 DisIff::GetNumberOfMode5Records() const
{
   return mNumMode5Records;
}

inline const DisAisPositionData* DisIff::GetAisPositionData() const
{
   return mAisPositionData;
}
inline const DisAisStaticData* DisIff::GetAisStaticData() const
{
   return mAisStaticData;
}

// Mutators

inline void DisIff::SetEmitterId(const DisEntityId& aEmitterId)
{
   mEmitterId = aEmitterId;
}
inline void DisIff::SetEventId(const DisEventId& aEventId)
{
   mEventId = aEventId;
}
inline void DisIff::SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ)
{
   mLocationX = aLocationX;
   mLocationY = aLocationY;
   mLocationZ = aLocationZ;
}
inline void DisIff::SetSystemId(const DisSystemId& aSystemId)
{
   mSystemId = aSystemId;
}

inline void DisIff::SetSystemStatus(DisEnum8 aSystemStatus)
{
   mSystemStatus = aSystemStatus;
}
inline void DisIff::SetAlternateParameter4(DisEnum8 aAlternateParameter4)
{
   mAlternateParameter4 = aAlternateParameter4;
}
inline void DisIff::SetInformationLayers(DisUint8 aInformationLayers)
{
   mInformationLayers = aInformationLayers;
}
inline void DisIff::SetModifier(DisEnum8 aModifier)
{
   mModifier = aModifier;
}
inline void DisIff::SetParameter1(DisEnum16 aParameter1)
{
   mParameter1 = aParameter1;
}
inline void DisIff::SetParameter2(DisEnum16 aParameter2)
{
   mParameter2 = aParameter2;
}
inline void DisIff::SetParameter3(DisEnum16 aParameter3)
{
   mParameter3 = aParameter3;
}
inline void DisIff::SetParameter4(DisEnum16 aParameter4)
{
   mParameter4 = aParameter4;
}
inline void DisIff::SetParameter5(DisEnum16 aParameter5)
{
   mParameter5 = aParameter5;
}
inline void DisIff::SetParameter6(DisEnum16 aParameter6)
{
   mParameter6 = aParameter6;
}

inline void DisIff::SetBeamDataRecord(const DisBeamDataRecord& aRecord)
{
   mBeamDataRecord = aRecord;
}
inline void DisIff::SetLayer2Parameter1(DisEnum8 aLayer2Parameter1)
{
   mLayer2Parameter1 = aLayer2Parameter1;
}
inline void DisIff::SetLayer2Parameter2(DisEnum8 aLayer2Parameter2)
{
   mLayer2Parameter2 = aLayer2Parameter2;
}

// System list methods

inline void DisIff::AddParameters(DisIffAtcNavaidsParams* aParametersPtr)
{
   mParametersList.Add(aParametersPtr);
}

inline std::vector<DisIffAtcNavaidsParams*>::iterator DisIff::GetParamsBegin()
{
   return mParametersList.GetBegin();
}

inline std::vector<DisIffAtcNavaidsParams*>::const_iterator DisIff::GetParamsBegin() const
{
   return mParametersList.GetBegin();
}

inline std::vector<DisIffAtcNavaidsParams*>::iterator DisIff::GetParamsEnd()
{
   return mParametersList.GetEnd();
}

inline std::vector<DisIffAtcNavaidsParams*>::const_iterator DisIff::GetParamsEnd() const
{
   return mParametersList.GetEnd();
}

inline unsigned int DisIff::RemoveParameters(DisIffAtcNavaidsParams* aParametersPtr)
{
   return mParametersList.Remove(aParametersPtr);
}

inline void DisIff::RemoveAllParameters()
{
   mParametersList.RemoveAll();
}

inline unsigned int DisIff::RemoveParametersWithNoDelete(DisIffAtcNavaidsParams* aParametersPtr)
{
   return mParametersList.RemoveWithNoDelete(aParametersPtr);
}

inline void DisIff::RemoveAllParametersWithNoDelete()
{
   mParametersList.RemoveAllWithNoDelete();
}
inline void DisIff::SetReportingSite(const DisUint16 aSite)
{
   mReportingSite = aSite;
}
inline void DisIff::SetReportingApplication(const DisUint16 aApp)
{
   mReportingApplication = aApp;
}

inline void DisIff::SetMode5Status(const DisEnum16 aStatus)
{
   mMode5Status = aStatus;
}
inline void DisIff::SetPIN(const DisEnum16 aPIN)
{
   mPIN = aPIN;
}
inline void DisIff::SetMode5MessageFormatPresent(const DisUint32 aFlag)
{
   mMessageFormat = aFlag;
}
inline void DisIff::SetEnhancedMode1(const DisEnum16 aMode)
{
   mEnhancedMode1 = aMode;
}
inline void DisIff::SetNationOfOrigin(const DisEnum8 aCountry)
{
   mNationOfOrigin = aCountry;
}
inline void DisIff::SetMissionCode(const DisEnum8 aCode)
{
   mMissionCode = aCode;
}
inline void DisIff::SetSquitterStatus(const DisEnum8 aStatus)
{
   mSquitterStatus = aStatus;
}
inline void DisIff::SetNavigationSource(const DisEnum8 aSource)
{
   mNavigationSource = aSource;
}
inline void DisIff::SetFigureOfMerit(const DisEnum8 aMerit)
{
   mFigureOfMerit = aMerit;
}
inline void DisIff::SetNumberOfMode5Records(const DisUint32 aNumber)
{
   mNumMode5Records = aNumber;
}

#endif
