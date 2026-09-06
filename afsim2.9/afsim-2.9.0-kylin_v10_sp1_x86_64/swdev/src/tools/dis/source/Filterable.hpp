//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef FILTERABLE_HPP
#define FILTERABLE_HPP

//! Enables PDU Viewer to filter DIS objects which extend this class
class Filterable
{
public:
   // Filterable();
   virtual ~Filterable() = default;

   virtual bool Equals(const Filterable* aOtherFilterable) const = 0;
};
#endif
