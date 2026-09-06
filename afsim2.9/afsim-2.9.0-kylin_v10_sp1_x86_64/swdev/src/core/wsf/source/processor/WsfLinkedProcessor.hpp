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

#ifndef WSFLINKEDPROCESSOR_HPP
#define WSFLINKEDPROCESSOR_HPP

#include "wsf_export.h"

#include "WsfExternalLinks.hpp"
#include "WsfProcessor.hpp"

/**
   A linked processor is a processor with both internal links (see WsfInternalLinks) and external links (see WsfExternalLinks).
*/
class WSF_EXPORT WsfLinkedProcessor : public WsfProcessor
{
public:
   WsfLinkedProcessor(WsfScenario& aScenario);
   ~WsfLinkedProcessor() override;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   void SendMessage(double aSimTime, const WsfMessage& aMessage) override;

   wsf::ExternalLinks& GetExternalLinks() { return mExternalLinks; }

protected:
   WsfLinkedProcessor(const WsfLinkedProcessor& aSrc);
   WsfLinkedProcessor& operator=(const WsfLinkedProcessor& aSrc) = delete;

   // Allow Derived classes to access the links objects.
   wsf::ExternalLinks mExternalLinks; //! This processor's external links (see WsfExternalLinks).
};

#endif
