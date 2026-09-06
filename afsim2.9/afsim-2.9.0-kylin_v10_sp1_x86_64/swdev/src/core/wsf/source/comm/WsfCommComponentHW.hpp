// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMCOMPONENTHW_HPP
#define WSFCOMMCOMPONENTHW_HPP

#include "wsf_export.h"

#include <memory>

#include "WsfCommComponent.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEvent.hpp"

namespace wsf
{
namespace comm
{

//! The hardware component that can be attached to all comm systems.
class WSF_EXPORT ComponentHW : public Component
{
public:
   static ComponentHW* Find(const Comm& aParent);
   static ComponentHW* FindOrCreate(Comm& aParent);

   ComponentHW();
   ~ComponentHW() override = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void TurnOn(double aSimTime) override;
   void TurnOff(double aSimTime) override;
   bool CanInteractWith(const Comm* aXmtrCommPtr) override;
   //@}

   virtual bool CanSendTo(double aSimTime, Comm* aRcvrPtr, Result& aResult);

   virtual bool CanReceiveFrom(double aSimTime, Comm* aXmtrPtr, Result& aResult) { return true; }

   //! @name WsfCommHW_Component methods.
   //! These methods support accessing the underlying transmitter and receiver components.
   //@{
   virtual unsigned int GetEM_RcvrCount() const { return 1; }
   virtual WsfEM_Rcvr&  GetEM_Rcvr(unsigned int aIndex) { return *mRcvrPtr; }
   virtual unsigned int GetEM_XmtrCount() const { return 1; }
   virtual WsfEM_Xmtr&  GetEM_Xmtr(unsigned int aIndex) { return *mXmtrPtr; }
   //@}

   //! @name Alternate Frequency methods.
   //@{
   bool           IsAltFreqChangeScheduled() const { return mAltFreqChangeScheduled; }
   double         GetAltFreqSelectDelay() const { return mAltFreqSelectDelay; }
   virtual double GetLastAltFreqSelectTime() const { return mLastAltFreqSelectTime; }
   virtual void   ScheduleAltFreqChange(double aSimTime, int aAltFreqId = -1);
   void           SelectAlternateFrequency(double aSimTime, int aAltFreqId = -1);
   //@}

   //! Indicates if the device transmits continuously.
   //! @returns if the device transmits continuously or false if it transmits only when a message is being sent.
   bool TransmitsContinuously() const { return mTransmitsContinuously; }

   //! The nested class 'FrequencyChangeEvent' is necessary so that
   //! the data necessary for this event is separate from WsfCommEvent.
   class AltFrequencyChangeEvent : public WsfEvent
   {
   public:
      AltFrequencyChangeEvent() = default;
      AltFrequencyChangeEvent(double aSimTime, Comm* aCommPtr, int aAltFrequencyId = -1);

      EventDisposition Execute() override;

   private:
      size_t mPlatformIndex;  //!< The platform index of the platform.
      Comm*  mCommPtr;        //!< The pointer to the comm.
      int    mAltFrequencyId; //!< The ID of the alternate frequency to select.
   };

protected:
   ComponentHW(const ComponentHW& aSrc);
   ComponentHW& operator=(const ComponentHW& aRhs) = delete;

   std::unique_ptr<WsfEM_Antenna> mAntennaPtr;
   std::unique_ptr<WsfEM_Xmtr>    mXmtrPtr;
   std::unique_ptr<WsfEM_Rcvr>    mRcvrPtr;

   bool   mAltFreqChangeScheduled;
   double mAltFreqSelectDelay;
   double mLastAltFreqSelectTime;

   bool mTransmitsContinuously; //!< True if this interface transmits continuously

   //! If non-zero, the name of ID of the category to be used for zone-based attenuation.
   WsfStringId mZoneAttenuationModifier;
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ComponentHW, cWSF_COMPONENT_COMM_HW)

#endif
