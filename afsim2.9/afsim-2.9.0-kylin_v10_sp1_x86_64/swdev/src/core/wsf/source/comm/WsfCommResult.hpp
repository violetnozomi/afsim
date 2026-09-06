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

#ifndef WSFCOMMRESULT_HPP
#define WSFCOMMRESULT_HPP

#include "wsf_export.h"

#include "WsfEM_Interaction.hpp"

namespace wsf
{
namespace comm
{

//! A class that provides detailed information about message reception.
//! An instance of this class is supplied to simulation observers by a comm
//! receiver to provide more detailed information about the physics of the
//! reception.
//!
//! On output it contains detailed data about the reception attempt.
//! Not every comm device will update every member. Each member documents under
//! what conditions it is valid. It is the responsibility of the caller to
//! ensure that the member they are using contains valid data.
class Result : public WsfEM_Interaction
{
public:
   Result()           = default;
   ~Result() override = default;

   //! Get the name of the script class associated with this class.
   //! This is necessary for proper downcasts in the scripting language.
   const char* GetScriptClassName() const override { return "WsfCommInteraction"; }

   //! The reported data rate in bit/s.
   double mDataRate{-1.0};

   //! The reported energy per bit to the spectral noise density (Eb/No) ratio in dB.
   double mEnergyBitToNoiseDensity{-1.0};

   //! The reported bit error rate (BER).
   double mBitErrorRate{-1.0};
};

} // namespace comm
} // namespace wsf

#endif
