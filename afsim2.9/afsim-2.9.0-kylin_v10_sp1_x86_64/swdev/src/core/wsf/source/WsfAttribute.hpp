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

#ifndef WSFATTRIBUTE_HPP
#define WSFATTRIBUTE_HPP

#include "wsf_export.h"

#include "UtAttribute.hpp"
#include "UtCompilerAttributes.hpp"

//! @deprecated These types have been deprecated and will be removed in a future release.
//! @{
template<typename T>
using WsfAttribute CPP14_DEPRECATED_REASON("Use UtAttribute<T> instead") = UtAttribute<T>;

using WsfIntAttribute    CPP14_DEPRECATED_REASON("Use UtIntAttribute instead")    = UtIntAttribute;
using WsfBoolAttribute   CPP14_DEPRECATED_REASON("Use UtBoolAttribute instead")   = UtBoolAttribute;
using WsfDoubleAttribute CPP14_DEPRECATED_REASON("Use UtDoubleAttribute instead") = UtDoubleAttribute;
using WsfStringAttribute CPP14_DEPRECATED_REASON("Use UtStringAttribute instead") = UtStringAttribute;
//! @}

#endif
