// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtBlackBody.hpp"

// Expected results calculated with https://www.spectralcalc.com/blackbody_calculator/blackbody.php
// Temperature: 300K, Wavelength band: 8um - 12um
TEST(UtBlackBody, BandRadiance)
{
   EXPECT_NEAR(ut::BlackBody::BandRadiance(300.0, 8.0e-6, 12.0e-6), 38.5017, 0.0011); // W/m^2/sr
}

// Temperature: 300K, Wavelength: 10um
TEST(UtBlackBody, SpectralRadiance)
{
   EXPECT_NEAR(ut::BlackBody::SpectralRadiance(300.0, 10.0e-6), 9.92435e6, 261); // W/m^2/sr/m
}

TEST(UtBlackBody, TotalRadiance)
{
   EXPECT_NEAR(ut::BlackBody::TotalRadiance(300.0), 146.204, 0.005); // W/m^2/sr
}

// Test that temperature can be backed out of radiance with same result as that provided to TotalRadiance
TEST(UtBlackBody, Temperature)
{
   auto radiance = ut::BlackBody::TotalRadiance(300.0);
   EXPECT_DOUBLE_EQ(ut::BlackBody::Temperature(radiance), 300.0);
}
