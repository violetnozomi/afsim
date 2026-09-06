// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoVersionInfo.hpp : header file

#if !defined(_UTOVERSIONINFO_HPP_)
#define _UTOVERSIONINFO_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UTILOSG_EXPORT UtoVersionInfo
{
public:
   typedef unsigned short VerId;
   static const VerId     any_id;

   UtoVersionInfo();
   virtual ~UtoVersionInfo();

   VerId       Major() const;
   VerId       Minor() const;
   VerId       Update() const;
   VerId       Build() const;
   std::string Version() const;
   std::string Comment() const;
   int         Compare(VerId major, VerId minor, VerId update = any_id, VerId build = any_id);

protected:
private:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoVersionInfo(const UtoVersionInfo&);
#endif
   UtoVersionInfo& operator=(const UtoVersionInfo&);
};

#endif // !defined(_UTOVERSIONINFO_HPP_)
