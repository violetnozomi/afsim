// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <memory>

#include <gtest/gtest.h>

#include "UtMemory.hpp"
#include "WsfPProxyBasicTypes.hpp"

// Test to make sure that two values created by the type
// with different initial memory values actually results in
// the same hash. This commonly fails when the hash function takes into account
// uninitialized padding bytes added by the compiler.
template<class T>
bool ConsistentHash()
{
   T    type;
   auto zeroedBuffer  = ut::make_unique<char[]>(type.mDataSize);
   auto nonzeroBuffer = ut::make_unique<char[]>(type.mDataSize);
   std::memset(zeroedBuffer.get(), 0, type.mDataSize);
   std::memset(nonzeroBuffer.get(), 0xff, type.mDataSize);
   type.Construct(zeroedBuffer.get());
   type.Construct(nonzeroBuffer.get());
   return type.Hash(zeroedBuffer.get()) == type.Hash(nonzeroBuffer.get());
}

TEST(ProxyBasicTypeTest, ConsistentHashes)
{
   ASSERT_TRUE(ConsistentHash<WsfPProxyIntType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyRealType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyLatitudeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyLongitudeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyPositionType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyLengthType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyTimeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxySpeedType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyDataSizeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyPowerType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyPowerDBType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxySolidAngleType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyMassType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyForceType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyTorqueType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAreaType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAreaDBType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyVolumeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyTime2Type>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAccelerationType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyFrequencyType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAngleType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAngularRateType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAngularAccelerationType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyDataRateType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyMassDensityType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyMassTransferType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyEnergyType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyFluenceType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyIrradianceType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyRatioType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyNoisePressureType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyPressureType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyTemperatureType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxySpecificRangeType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyAngularInertiaType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyStringType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyLength2Type>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyLength3Type>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyBoolType>());
   ASSERT_TRUE(ConsistentHash<WsfPProxyEnumType>());
}
