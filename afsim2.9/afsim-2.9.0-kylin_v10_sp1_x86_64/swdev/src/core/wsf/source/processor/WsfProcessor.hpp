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

#ifndef WSFPROCESSOR_HPP
#define WSFPROCESSOR_HPP

#include "wsf_export.h"

class UtInput;
class WsfCallback;
#include "WsfComponentList.hpp"
class WsfMessage;
class WsfPlatform;
#include "WsfPlatformPart.hpp"
class WsfProcessor;
#include "WsfProcessorComponent.hpp"
#include "WsfVariable.hpp"

//! A base class for processing systems.
//! A processor is an platform part implements the 'behavior' of a platform.  It
//! can represent either a human or a machine.
//!
//! Processors are invoked by one of two methods:
//!
//! \li By receiving a message from communications sensor systems (ReceiveMessage).
//! \li By periodic call (Update) (the interval is defined by SetUpdateInterval).
//!
//! A process can do just about anything it wants, i.e.: invoke actions, update
//! platform objects, send messages (to other processors or platforms).
class WSF_EXPORT WsfProcessor : public WsfPlatformPart
{
public:
   WsfProcessor(const WsfScenario& aScenario);
   ~WsfProcessor() override = default;

   //! @name Component list support.
   //! A processor contains a list of components.
   //@{
   using ComponentList = WsfComponentListT<WsfProcessorComponent>;
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   //! @name Component infrastructure methods not provided by WsfPlatformPart.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_PROCESSOR; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   const char* GetScriptClassName() const override;
   //! Create a clone of this object.
   //! @return A pointer to the new copy.
   WsfProcessor* Clone() const override = 0;
   bool          PreInitialize(double aSimTime) override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   void          PreInput() override;
   bool          ProcessInput(UtInput& aInput) override;
   void          Update(double aSimTime) override;
   //@}

   //! @name Status methods.
   //@{
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void PartBroken(double aSimTime) override;
   //@}

   //! @name Update interval
   //@{
   double GetUpdateInterval() const override;
   void   SetUpdateInterval(double aUpdateInterval) override;
   //@}

   //! @name Miscellaneous methods.
   //!@{
   virtual void ProcessCallback(double aSimTime, const WsfCallback& aCallback);
   //@}

protected:
   WsfProcessor(const WsfProcessor& aSrc);
   WsfProcessor& operator=(const WsfProcessor& aRhs) = delete;

private:
   ComponentList mComponents;

   //! The update interval is time between the periodic calls to Update() by the simulation executive.
   //! If less than or equal to zero then the simulation executive should not make the periodic calls.
   WsfVariable<double> mUpdateInterval;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfProcessor, cWSF_COMPONENT_PROCESSOR)

#endif
