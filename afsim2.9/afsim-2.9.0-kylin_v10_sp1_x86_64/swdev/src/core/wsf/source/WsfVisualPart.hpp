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

#ifndef WSFVISUALPART_HPP
#define WSFVISUALPART_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
#include "WsfArticulatedPart.hpp"

//! A base class for articulations that are related to visual components.
//!
//! This could include flaps, folding wings, etc.
class WSF_EXPORT WsfVisualPart : public WsfArticulatedPart
{
public:
   WsfVisualPart(const WsfScenario& aScenario);
   WsfVisualPart& operator=(const WsfVisualPart&) = delete;
   ~WsfVisualPart() override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_VISUAL_PART; }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Common infrastructure methods.
   //@{
   const char* GetScriptClassName() const override;
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfVisualPart* Clone() const override { return new WsfVisualPart(*this); }
   bool           Initialize(double aSimTime) override;
   bool           ProcessInput(UtInput& aInput) override;
   void           Update(double aSimTime) override;
   //@}

   //! Get the update interval.
   double GetUpdateInterval() const override { return mUpdateInterval; }

   //! Set the displayed icon
   void SetIcon(const std::string& aIcon) { mIcon = aIcon; }
   //! Get the display icon
   const std::string& GetIcon() const { return mIcon; }

   //! Set the time interval between the periodic calls to Update().
   //! A value of less than or equal to zero indicates periodic updates are not to be performed.
   //! @param aUpdateInterval The new update interval.
   void SetUpdateInterval(double aUpdateInterval) override { mUpdateInterval = aUpdateInterval; }

   //! @name Status methods.
   //@{
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void PartBroken(double aSimTime) override;

protected:
   WsfVisualPart(const WsfVisualPart& aSrc);

private:
   //! The update interval is time between the periodic calls to Update() by the simulation executive.
   //! If less than or equal to zero then the simulation executive should not make the periodic calls.
   double mUpdateInterval;

   //! The next unique ID for requests.
   unsigned int mRequestId;

   //! The request IDs for all active requests.
   std::vector<unsigned int> mActiveRequestList;

   std::string mIcon;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfVisualPart, cWSF_COMPONENT_VISUAL)

#endif
