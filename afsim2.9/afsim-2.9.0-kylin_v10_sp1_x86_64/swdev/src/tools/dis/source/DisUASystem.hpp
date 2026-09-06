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


#ifndef DISUASYSTEM_HPP
#define DISUASYSTEM_HPP

#include "dis_export.h"

#include "DisPtrContainer.hpp"
#include "DisTypes.hpp"
#include "DisUABeam.hpp"

class GenI;
class GenO;

// DIS Underwater Acoustic System

class DIS_EXPORT DisUASystem
{
public:
   DisUASystem();
   ~DisUASystem();

   // Allow compiler to build copy constructor and operator=
   // DisUASystem(const DisUASystem& aSrc);
   // DisUASystem& operator=(const DisUASystem& aRhs);

   // Accessors

   DisUint8  GetDataLength();
   DisUint8  GetBeamCount() const;
   DisEnum16 GetAcousticName() const;
   DisUint8  GetFunction() const;
   DisUint8  GetAcousticId() const;
   void      GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const;

   // Mutators

   void SetAcousticName(DisEnum16 aSystemName);
   void SetFunction(DisUint8 aSystemFunction);
   void SetAcousticId(DisUint8 aSystemId);
   void SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);

   // Input/output

   DisUint16 GetLength();
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO);

   //! Data Validation
   bool IsValid() const;

   // The following gets the length as read from the header, not the
   // length computed by adding the length of the base data plus the
   // length required for the specified number of Track/Jam entries.
   DisUint16 GetLengthRead() const;

   // Beam list maintenance...

   // WARNING: AddBeam takes ownership of the passed object.
   void AddBeam(DisUABeam* aBeam);

   // Return DisUABeam ptr if available, if not return zero.
   // While looping with these two below functions, "AddBeam(DisUABeam*)"
   // may only be called if the last "DisUABeam*" pointer returned from
   // "GetFirstBeam()" or "GetNextBeam()" was removed via a call to
   // "RemoveBeam(DisUABeam*)" or "RemoveBeamWithNoDelete(DisUABeam*)".
   // DisUABeam pointers can be removed, however, with
   // "RemoveBeam(DisUABeam*)" or "RemoveBeamWithNoDelete(DisUABeam*)"
   // at anytime while using these looping routines.
   // WARNING: These 2 looping functions use an internal iterator and only
   // one routine can call them at a time.

   // Return iterators to UABeam vector.
   std::vector<DisUABeam*>::iterator       GetUABeamBegin();
   std::vector<DisUABeam*>::const_iterator GetUABeamBegin() const;
   std::vector<DisUABeam*>::iterator       GetUABeamEnd();
   std::vector<DisUABeam*>::const_iterator GetUABeamEnd() const;

   // Remove and delete a DisUABeam object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Beam on the list.
   unsigned int RemoveBeam(DisUABeam* aBeam);

   // Remove and delete all DisUABeam objects from the list.
   void RemoveAllBeams();

   // Remove DisUABeam object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Beam on the list.
   unsigned int RemoveBeamWithNoDelete(DisUABeam* aBeam);

   // Remove all DisUABeam objects from the list, but DO NOT DELETE them.
   void RemoveAllBeamsWithNoDelete();

private:
   void UpdateBeamCount() const;

   DisUint8                   mDataLength;
   mutable DisUint8           mBeamCount;
   DisEnum16                  mAcousticName;
   DisEnum8                   mFunction;
   DisUint8                   mAcousticId;
   DisFloat32                 mLocation[3];
   DisPtrContainer<DisUABeam> mBeamList;

   DisUint16 mLengthRead;
};

inline void DisUASystem::UpdateBeamCount() const
{
   mBeamCount = static_cast<DisUint8>(mBeamList.GetSize());
}

// Accessors

inline DisUint8 DisUASystem::GetBeamCount() const
{
   UpdateBeamCount();
   return mBeamCount;
}
inline DisEnum16 DisUASystem::GetAcousticName() const
{
   return mAcousticName;
}
inline DisUint8 DisUASystem::GetFunction() const
{
   return mFunction;
}
inline DisUint8 DisUASystem::GetAcousticId() const
{
   return mAcousticId;
}
inline void DisUASystem::GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}
inline DisUint16 DisUASystem::GetLengthRead() const
{
   return mLengthRead;
}

// Mutators

inline void DisUASystem::SetAcousticName(DisEnum16 aAcousticName)
{
   mAcousticName = aAcousticName;
}
inline void DisUASystem::SetFunction(DisUint8 aFunction)
{
   mFunction = aFunction;
}
inline void DisUASystem::SetAcousticId(DisUint8 aAcousticId)
{
   mAcousticId = aAcousticId;
}
inline void DisUASystem::SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisUASystem& aSystem)
{
   aSystem.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisUASystem& aSystem)
{
   aSystem.Get(aGenI);
   return aGenI;
}


// Beam list methods

inline void DisUASystem::AddBeam(DisUABeam* aBeam)
{
   mBeamList.Add(aBeam);
}

inline std::vector<DisUABeam*>::iterator DisUASystem::GetUABeamBegin()
{
   return mBeamList.GetBegin();
}

inline std::vector<DisUABeam*>::const_iterator DisUASystem::GetUABeamBegin() const
{
   return mBeamList.GetBegin();
}

inline std::vector<DisUABeam*>::iterator DisUASystem::GetUABeamEnd()
{
   return mBeamList.GetEnd();
}

inline std::vector<DisUABeam*>::const_iterator DisUASystem::GetUABeamEnd() const
{
   return mBeamList.GetEnd();
}
inline unsigned int DisUASystem::RemoveBeam(DisUABeam* aBeam)
{
   return mBeamList.Remove(aBeam);
}

inline void DisUASystem::RemoveAllBeams()
{
   mBeamList.RemoveAll();
}

inline unsigned int DisUASystem::RemoveBeamWithNoDelete(DisUABeam* aBeam)
{
   return mBeamList.RemoveWithNoDelete(aBeam);
}

inline void DisUASystem::RemoveAllBeamsWithNoDelete()
{
   mBeamList.RemoveAllWithNoDelete();
}

#endif
