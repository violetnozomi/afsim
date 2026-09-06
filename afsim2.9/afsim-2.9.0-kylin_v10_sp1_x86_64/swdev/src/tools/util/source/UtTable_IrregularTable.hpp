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

#ifndef UTTABLE_IRREGULARTABLE_HPP
#define UTTABLE_IRREGULARTABLE_HPP

//! An implementation of 'irregular tables'.
#include <memory>

#include "UtInput.hpp"
#include "UtTable.hpp"

namespace UtTable
{
namespace IrregularTable
{
UT_EXPORT std::unique_ptr<Table> LoadInstance(UtInput&           aInput,
                                              UtInput::ValueType aDV_UnitType,
                                              const ValueCheck&  aDV_ValueCheck,
                                              const Parameters&  aParameters);
}
} // namespace UtTable

#endif
