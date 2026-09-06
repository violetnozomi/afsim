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

#ifndef DISSPOTTED_HPP
#define DISSPOTTED_HPP

#include <ostream>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

//! The Spotted PDU (Digital Spot Report) is transmitted each time a Blue vehicle spots a Red vehicle.
//! It is also transmitted once per minute as long as the Red vehicle is spotted.
//! If three Red vehicles are spotted, three Spotted PDUs are transmitted.
class DisSpotted : public DisPdu
{
public:
   static const DisInt8 sType;   // 185 "Spotted"
   static const DisInt8 sFamily; // 150 "Experimental"

   DisSpotted();
   DisSpotted(const DisSpotted& aSrc);
   DisSpotted(DisPdu& aPdu, GenI& aGenI);
   DisSpotted(GenI& aGenI);
   ~DisSpotted() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors (TBD)

   // Mutators (TBD)

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   // Operators

   DisUint16 GetLength() override;

protected:
   DisSpotted& operator=(const DisSpotted& aRhs);

   // private:
public: // (temp)
   void Copy(const DisSpotted& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId   mEntityId;       //! Identifier of spotting entity
   DisEntityId   mTargetId;       //! Identifier of spotted entity. Can be 0-0-0
   DisEnum8      mSensorName[24]; //! Name of Sensor
   DisFloat64    mLocation[3];    //! Location of spotted entity
   DisFloat32    mVelocity[3];    //! Velocity of spotted entity
   DisEntityType mGuise;          //! Perceived Type of spotted entity.
                                  //! Type should take acquisition level into account.
                                  //! For example, a T72 that is Detected should be reported as a vehicle_Other_Vehicle.
   DisInt8 mAlive;                //! Whether the spotted entity is known to be alive.
                                  //! Sensors that can not perform BDA should use 0.
   DisInt8 mMobilityDead;         //! Whether the spotted entity is known to be mobility-killed.
                                  //! Sensors that can not perform BDA should use 0.
   DisInt8 mFirepowerDead;        //! Whether the spotted entity is known to be firepower-killed.
                                  //! Sensors that can not perform BDA should use 0.
   DisInt8 mCatastropicDead;      //! Whether the spotted entity is known to be catastrophic-killed.
                                  //! Sensors that can not perform BDA should use 0.
   DisUint32 mAppearance;
   DisUint32 mSensorTypes; //! Or'd enumeration of the sensor(s) that contributed to the report.
   DisUint8  mCount;       //! Number of spotted entities ( =1)
   DisUint8  mNewLevel;    //! Acquisition Level
   DisInt8   mForce;       //! Perceived Force of Spotted Entity
   DisUint8  mLocationAccuracy;
   DisUint8  mAgeInSecs;
   DisUint8  mNumComments;
   DisEnum8  mComments[2];
};

#endif
