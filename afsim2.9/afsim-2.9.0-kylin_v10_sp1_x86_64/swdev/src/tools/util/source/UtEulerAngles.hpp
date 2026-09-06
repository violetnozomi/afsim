// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTEULERANGLES_HPP
#define UTEULERANGLES_HPP

#include "ut_export.h"

#include "UtMat3.hpp"

namespace ut
{
namespace EulerAngles
{
//! Compute the classical Euler transform.
//!@param aAlpha A rotation angle about the z axis.
//!@param aBeta  A rotation angle about the intermediate x axis.
//!@param aGamma A rotation angle about the intermediate z axis.
UT_EXPORT UtMat3d ComputeClassicalTransform(double aAlpha, double aBeta, double aGamma);
}; // namespace EulerAngles

} // namespace ut

#endif
