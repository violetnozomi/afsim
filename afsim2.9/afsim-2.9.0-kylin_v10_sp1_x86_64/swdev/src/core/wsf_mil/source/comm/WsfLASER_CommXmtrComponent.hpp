// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFLASER_COMMXMTRCOMPONENT_HPP
#define WSFLASER_COMMXMTRCOMPONENT_HPP

#include <memory>

#include "WsfLASER_XmtrComponent.hpp"
#include "WsfMilComponentRoles.hpp"

//! A LASER transmitter component specialized for comm devices.
//! In addition to the functionality provided by WsfLASER_XmtrComponent,
//! this component also provides comm modulation and pointing transmission loss.
class WsfLASER_CommXmtrComponent : public WsfLASER_XmtrComponent
{
public:
   WsfLASER_CommXmtrComponent();
   WsfLASER_CommXmtrComponent(const WsfLASER_CommXmtrComponent& aSrc);
   WsfLASER_CommXmtrComponent& operator=(const WsfLASER_CommXmtrComponent&) = delete;

   //! @name WsfComponent required methods.
   //@{
   WsfComponent* CloneComponent() const override;
   //@}

   //! @name Framework method overrides.
   //@{
   bool PreInitialize(double aSimTime) override;
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   //! Get the pointing transmission factor (0-1) associated with slight pointing offset of the
   //! transmitter to receiver.
   double GetPointingTransmission() const { return mPointingTransmissionFactor; }

   //! Return the data rate of this comm device (pass-through from Modulation).
   double GetDataRate() const { return mModulationPtr->GetDataRate(); }

   //! Return the duty cycle (time the laser is on; pass-through from Modulation).
   double GetDutyCycle() const { return mModulationPtr->GetDutyCycle(); }

   class Modulation
   {
   public:
      enum Type
      {
         cUNDEFINED,
         cOOK,
         cPPM,
         cDPSK,
      };

      Modulation();
      ~Modulation() = default;

      bool ProcessInput(UtInput& aInput);
      bool Initialize(WsfEM_Xmtr& aXmtr);

      //! Return the data rate associated with the modulation parameters.
      double GetDataRate() const { return mDataRate; }

      //! Return the duty cycle (time the laser is on).
      double GetDutyCycle() const { return mDutyCycle; }

      //! Return the slot duration (time between bit intervals).
      double GetSlotDuration() const { return mSlotDuration; }

      //! Set type modulation type.
      void SetType(Type aType) { mType = aType; }

      //! Set the PPM order (2, 4, 8, etc.) if the selected type was cPPM.
      void SetPPM_Order(unsigned aPPM_Order) { mPPM_Order = aPPM_Order; }

      //! Get the name associated with the modulation type.
      const std::string& GetTypeName() const { return mTypeName; }

   private:
      Type        mType;
      std::string mTypeName;
      double      mSlotDuration;
      double      mDataRate;
      double      mDutyCycle;
      unsigned    mPPM_Order;

      using TypesMap = std::map<std::string, Type>;
      static const TypesMap sTypesMap;
   };

   Modulation& GetModulation() { return *mModulationPtr; }

private:
   double                      mPointingTransmissionFactor;
   double                      mAveragePower;
   std::unique_ptr<Modulation> mModulationPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfLASER_CommXmtrComponent, cWSF_COMPONENT_LASER_XMTR)

#endif
