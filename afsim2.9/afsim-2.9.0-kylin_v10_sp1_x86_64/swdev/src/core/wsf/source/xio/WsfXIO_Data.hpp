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

#ifndef WSFXIO_DATA_HPP
#define WSFXIO_DATA_HPP

#include "PakDataBuffer.hpp"

//! A buffer of data which can be 'serialized' into and out of using the '&' operator.
//! Prior to using the '&' operator, xio/WsfXIO_Defines.hpp should be included
typedef PakDataBuffer WsfXIO_Data;

#endif
