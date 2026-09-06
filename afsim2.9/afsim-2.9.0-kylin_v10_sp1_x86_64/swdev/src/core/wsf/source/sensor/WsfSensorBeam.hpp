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

#ifndef WSFSENSORBEAM_HPP
#define WSFSENSORBEAM_HPP

#include "wsf_export.h"

#include <cstddef> // for size_t

class UtInput;
class WsfEM_Clutter;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfSensor;
class WsfSensorMode;
#include "WsfSensorSignalProcessor.hpp"


//! An optional base class for sensor beams.
//!
//! Use of this class is totally optional. If your sensor implementation implements multiple beams
//! AND it desired to being accessible to some optional derived versions of WsfSensorComponent, then
//! you MUST implement this interface (as well as the GetBeamCount and GetBeamEntry methods) defined
//! in WsfSensorMode. This allows components to have access to the transmitter and receiver defined
//! in the beam.
//!
//! If you use this class then you *MUST* adhere to the following rules:
//!
//! - When creating the first beam in a mode (typically in the constructor), you must set the beam owner
//!   This typically looks something like:
//!   \code
//!      mBeamList.push_back(new MyBeam());      // Add the beam to the beam list
//!      mBeamList.back()->SetBeamOwner(this);   // Set its owning mode
//!   \endcode
//!
//! - When adding beams to the beam list, you must set the beam index (index into the list).
//!   \code
//!      unsigned int beamIndex = static_cast<unsigned int>(mBeamList.size());
//!      mBeamList.push_back(new MyBeam(mBeamList[0]));   // Add the beam (clone of the first beam)
//!      mBeamList.back()->SetBeamIndex(beamIndex);       // Set the index for the new beam
//!   \endcode
//!
//! - You must implement the GetEM_Rcvr and GetEM_Xmtr methods in your beam class.
//!
//! (Examples of this code may be found in WsfPassiveSensor and WsfRadarSensor.)
//!
//! From that point forward the beam owner and beam index will be maintained by the sensor framework.
class WSF_EXPORT WsfSensorBeam
{
   friend class WsfSensor;
   friend class WsfSensorMode;
   friend class WsfSensorModeList;

public:
   WsfSensorBeam();
   WsfSensorBeam(const WsfSensorBeam& aSrc) = default;
   WsfSensorBeam& operator                  =(const WsfSensorBeam& aRhs);
   virtual ~WsfSensorBeam()                 = default;

   //! @name Transmitter/receiver access.
   //! Derived classes should implement these methods to return the pointer to the underlying
   //! transmitter and receiver.
   //! @note The returned values may be if the beam doesn't have the respective object.
   //@{
   virtual WsfEM_Rcvr* GetEM_Rcvr() { return nullptr; }
   virtual WsfEM_Xmtr* GetEM_Xmtr() { return nullptr; }
   //@}

   virtual bool ProcessInput(UtInput& aInput);
   virtual bool Initialize(double aSimTime);

   //! Set the beam owner (the parent mode).
   void SetBeamOwner(WsfSensorMode* aSensorModePtr) { mSensorModePtr = aSensorModePtr; }

   //! Set the beam index for the second an subsequent beams.
   //! The application is responsible for calling this when a beam is first added to the beam list.
   void SetBeamIndex(size_t aBeamIndex) { mBeamIndex = aBeamIndex; }

   // NOTE: Do not add a GetSensor method as it would require sensor modes that derive
   //       from both WsfSensorMode and WsfSensorBeam to have a 'using' directive to
   //       resolve the ambiguity.

   WsfSensorMode* GetSensorMode() const { return mSensorModePtr; }
   size_t         GetBeamIndex() const { return mBeamIndex; }

   WsfSensorSignalProcessor::List& GetSignalProcessors() { return mSignalProcessors; }

   //! @name Casting elimination accessors/mutators.
   //! Derived classes should implement these methods to properly set/get data.
   //@{
   virtual int            GetNumberOfPulsesIntegrated() const;
   virtual double         GetIntegrationGain() const;
   virtual void           SetIntegrationGain(double aIntegrationGain);
   virtual WsfEM_Clutter* GetClutter() const { return nullptr; };
   //@}

private:
   WsfSensorMode* mSensorModePtr;
   size_t         mBeamIndex;

   //! Signal processors list.
   //! Used to isolate common functionality across different sensors
   WsfSensorSignalProcessor::List mSignalProcessors;
};

#endif
