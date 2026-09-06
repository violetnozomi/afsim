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

#ifndef UTQTMEMORYUSAGE_HPP
#define UTQTMEMORYUSAGE_HPP

#include <QString>

//! UtQtMemoryUsage provides an interface for querying how much physical
//! memory the current application is consuming.

class UtQtMemoryUsage
{
public:
   UtQtMemoryUsage() = default;

   double GetMemoryUsageInBytes() const;
   double GetMemoryUsageInKBytes() const;
   double GetMemoryUsageInMBytes() const;
   double GetMemoryUsageInGBytes() const;

   QString GetMemoryUsageString() const;

   static QString GetMemoryUsageString(double aMemoryInBytes);

private:
   double GetMemoryUsageInBytesPrivate() const;
};

#endif
