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
// UtoViewFilter.hpp : header file

#if !defined(_UTOVIEWFILTER_HPP_)
#define _UTOVIEWFILTER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

class UtoEntity;
// class UtoSymbol;

class UtoViewFilter
{
public:
   UtoViewFilter() {}
   virtual ~UtoViewFilter(){};

   virtual void OnAdd(UtoEntity&) {}
   virtual void OnRemove(UtoEntity&) {}

   virtual void OnAddSymbol(UtoShape&, UtoViewer&) {}
   virtual void OnRemoveSymbol(UtoShape&, UtoViewer&) {}

   // {secret}
   void Dump(std::ostream&) {}

protected:
private:
   UtoViewFilter(const UtoViewFilter&);
   UtoViewFilter& operator=(const UtoViewFilter&);
};

#endif // !defined(_UTOVIEWFILTER_HPP_)
