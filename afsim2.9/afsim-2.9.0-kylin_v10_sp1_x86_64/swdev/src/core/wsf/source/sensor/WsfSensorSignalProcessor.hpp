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

#ifndef WSFSENSORSIGNALPROCESSOR_HPP
#define WSFSENSORSIGNALPROCESSOR_HPP

#include "wsf_export.h"

#include <list>

#include "UtCloneablePtr.hpp"
#include "WsfObject.hpp"
#include "WsfSensor.hpp"
class WsfSensorMode;

//! An abstract base class that defines a signal processing technique for a sensor.
//!
//! Not all sensors support this feature, but for those that do it provides access to an extendable
//! library that implement signal processing techniques. When a sensor makes use of this interface
//! it typically computes the unprocessed signal and then calls any defined processors to adjust the
//! received power, clutter power, etc., which are then used to calculate the signal-to-interference
//! ratio and subsequently tested for detectability.
//!
//! Additional processors can be added by registering an object factory (AddObjectFactory) that will
//! create instances with the desired functionality.
class WSF_EXPORT WsfSensorSignalProcessor : public WsfObject
{
public:
   using FactoryPtr        = WsfSensorSignalProcessor* (*)(const std::string&);
   using ObjectFactoryList = std::list<FactoryPtr>;

   //! A convenience class to support attaching a list of signal processors to a sensor.
   class WSF_EXPORT List
   {
   public:
      using ListType = std::vector<UtCloneablePtr<WsfSensorSignalProcessor>>;

      List()                 = default;
      List(const List& aSrc) = default;
      List& operator=(const List& aRhs) = default;
      ~List()                           = default;

      bool   Empty() const { return mProcessorPtrs.empty(); }
      size_t Size() const { return mProcessorPtrs.size(); }

      bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0);

      bool ProcessInput(UtInput& aInput);

      void Execute(double aSimTime, WsfSensorResult& aResult);

   private:
      ListType mProcessorPtrs;
   };

   // TODO:STATIC -- remove these
   //! @name Maintenance of object types.
   //@{
   static void                      AddObjectFactory(FactoryPtr aFactoryPtr);
   static void                      RemoveObjectFactory(FactoryPtr aFactoryPtr);
   static WsfSensorSignalProcessor* CreateInstance(const std::string& aTypeName);
   static void                      ClearTypes();
   //@}

   WsfSensorSignalProcessor* Clone() const override = 0;

   virtual bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, size_t aBeamIndex = 0);

   bool ProcessInput(UtInput& aInput) override;

   virtual void Execute(double aSimTime, WsfSensorResult& aResult) = 0;

protected:
   //! If 'true' additional information is written out to aid debugging
   bool mDebug{false};

private:
   static WsfSensorSignalProcessor* PredefinedObjectFactory(const std::string& aTypeName);

   //! The list of object factories
   static ObjectFactoryList sObjectFactoryList;
};

#endif
