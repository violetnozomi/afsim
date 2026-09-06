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

#ifndef GEODATACONSTANTS_H
#define GEODATACONSTANTS_H

//-----------------------------------------------------------------------------
//
//
//   constants.h - Define physical constants.
//
//-----------------------------------------------------------------------------


static const double pi         = 3.14159265358979;
static const double twoPi      = 6.28318530718;
static const double ftpnm      = 6076.115;
static const double nmpft      = 1.6457885e-4;
static const double fpspkt     = 1.68781;
static const double ktpfps     = 0.5924838;
static const double meterpspkt = 0.5144454;
static const double ktpmeterps = 1.943841;
static const float  g          = 32.1741F;
static const float  dtor       = 0.0174532925199433F;
static const float  rtod       = 57.2957795130823F;

static const double ftpmeter = 39.37 / 12.0;
static const double meterpft = 12.0 / 39.37;
static const double nmpmeter = 5.3995577e-4;

static const double ddtor = 1.74532925199433e-02;
static const double drtod = 5.72957795130823e+01;

static const double dEarthRadiusm  = 6371221.3;  // meters
static const double dEarthRadiusft = 20902915.2; // feet

static const float growLargerFast  = 1.3F;
static const float growLargerSlow  = 1.1F;
static const float growSmallerFast = 1.0F / growLargerFast;
static const float growSmallerSlow = 1.0F / growLargerSlow;

static const double ooEarthRadiusm  = 1.56955779e-7; // 1.0 / radius (feet)
static const double ooEarthRadiusft = 4.78402170e-8; // 1.0 / radius (feet)

static const int iTrue  = 1;
static const int iFalse = 0;

#endif
