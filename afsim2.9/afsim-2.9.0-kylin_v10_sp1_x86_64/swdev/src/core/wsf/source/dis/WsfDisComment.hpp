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

#ifndef WSFDISCOMMENT_HPP
#define WSFDISCOMMENT_HPP

#include "wsf_export.h"

#include "DisComment.hpp"
class WsfDisInterface;
#include "WsfStringId.hpp"

class WSF_EXPORT WsfDisComment : public DisComment
{
public:
   WsfDisComment(WsfDisInterface* aInterfacePtr);
   WsfDisComment(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisComment() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisComment& operator=(const WsfDisComment& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
