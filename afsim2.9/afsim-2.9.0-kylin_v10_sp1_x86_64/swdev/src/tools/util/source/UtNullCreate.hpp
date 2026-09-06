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
#ifndef UTNULLCREATE_HPP
#define UTNULLCREATE_HPP

//! Unfortunately, c++ does not have true named constructors!  This empty class is a
//! parameter to a constructor when we need to implement a constructor which
//! minimally creates the class instance -- possibly in an uninitialized state.
//!
//! This is commonly used for serialization.
//!
//! For use with UtSerialArchive, a class should either define a default constructor
//!   or define a constructor which takes as a single parameter (const UtNullCreate&)
class UtNullCreate
{
public:
};

#endif
