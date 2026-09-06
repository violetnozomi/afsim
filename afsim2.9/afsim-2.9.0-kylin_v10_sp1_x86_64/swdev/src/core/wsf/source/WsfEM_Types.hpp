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

#ifndef WSFEM_TYPES_HPP
#define WSFEM_TYPES_HPP

//! Define data types used by the electromagnetic classes.

namespace WsfEM_Types
{
//! Polarization of the electromagnetic wave
enum Polarization
{
   cPOL_DEFAULT        = 0, //!< Default
   cPOL_HORIZONTAL     = 1, //!< Horizontal
   cPOL_VERTICAL       = 2, //!< Vertical
   cPOL_SLANT_45       = 3, //<! Slant 45
   cPOL_SLANT_135      = 4, //<! Slant 135
   cPOL_LEFT_CIRCULAR  = 5, //!< Left circular
   cPOL_RIGHT_CIRCULAR = 6, //!< Right circular
   cPOL_COUNT          = 7  //!< Number of polarizations
};

//! Infrared band names
enum InfraredBand
{
   cIRBAND_DEFAULT   = 0, //!< Default
   cIRBAND_SHORT     = 1, //!< Short wave (1-3 microns)
   cIRBAND_MEDIUM    = 2, //!< Mid wave (3-5 microns)
   cIRBAND_LONG      = 3, //!< Long wave (8-14 microns)
   cIRBAND_VERY_LONG = 4, //!< Very long wave (14-30 microns)
   cIRBAND_COUNT     = 5  //!< Number of types
};

//! Optical bands
//! This includes all bands that are in the optical spectrum.
//! The IR bands defined above are part of this and *MUST* have the same values
//! Also, if this is modified then WsfOpticalEnvironment must also be modified.
enum OpticalBand
{
   cOPT_DEFAULT      = 0, //!< Same as IRBAND_DEFAULT
   cOPT_IR_SHORT     = 1, //!< Short wave (1-3 microns)
   cOPT_IR_MEDIUM    = 2, //!< Mid wave (3-5 microns)
   cOPT_IR_LONG      = 3, //!< Long wave (8-14 microns)
   cOPT_IR_VERY_LONG = 4, //!< Very long wave (14-30 microns)
   cOPT_IR_COUNT     = 5, //!< Must be same as IRBAND_COUNT

   // For now, all of the non-IR bands must follow.
   cOPT_VISUAL = 6, //!< Visual, 380-760nm
   cOPT_COUNT  = 7  //!< Number of bands (including the 'default' band)
};
} // namespace WsfEM_Types

#endif
