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

#ifndef WSFDISCOMMENTR_HPP
#define WSFDISCOMMENTR_HPP

#include "wsf_export.h"

#include "DisCommentR.hpp"
#include "WsfStringId.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisCommentR : public DisCommentR
{
public:
   WsfDisCommentR(WsfDisInterface* aInterfacePtr);
   WsfDisCommentR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisCommentR() override;

   void SetCommentId(WsfStringId aStringId);
   int  Process() override;

protected:
   // Disallow assignment
   WsfDisCommentR& operator=(const WsfDisCommentR& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
