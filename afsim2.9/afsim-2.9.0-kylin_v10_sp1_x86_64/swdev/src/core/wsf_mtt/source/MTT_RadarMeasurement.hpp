// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MTT_RADARMEASUREMENT_HPP
#define MTT_RADARMEASUREMENT_HPP

#include <array>
#include <fstream>
#include <string>

#include "MTT_Measurement.hpp"

class MTT_RadarMeasurement : public MTT_Measurement
{
public:
   class Data : public MTT_Measurement::Data
   {
   public:
      Data() = default;

      MTT_Measurement::Data* Clone() const override { return new Data(*this); }

      // Suppressor offset
      double                mRange{0.0};                         // 0
      double                mAzimuth{0.0};                       // 1
      double                mElevation{0.0};                     // 2
      double                mRangeRate{0.0};                     // 3
      double                mRangeStDev{0.0};                    // 4
      double                mAzimuthStDev{0.0};                  // 5
      double                mElevationStDev{0.0};                // 6
      double                mRangeRateStDev{0.0};                // 7
      std::array<double, 3> mSensorLocation = {{0.0, 0.0, 0.0}}; // 8-10
      double                mSignalToNoise{0.0};                 // 11
      double                mTrueRange{0.0};                     // 12
      double                mTrueAzimuth{0.0};                   // 13
      double                mTrueElevation{0.0};                 // 14
      double                mTrueRangeRate{0.0};                 // 15
      std::array<double, 3> mSensorVelocity = {{0.0, 0.0, 0.0}}; // 16-18

      virtual void ConvertFrom(const SupBlock& aBlock)
      {
         mRange             = aBlock.mFloatBlock[0];
         mAzimuth           = aBlock.mFloatBlock[1]; // 1
         mElevation         = aBlock.mFloatBlock[2]; // 2
         mRangeRate         = aBlock.mFloatBlock[3]; // 3
         mRangeStDev        = aBlock.mFloatBlock[4]; // 4
         mAzimuthStDev      = aBlock.mFloatBlock[5]; // 5
         mElevationStDev    = aBlock.mFloatBlock[6]; // 6
         mRangeRateStDev    = aBlock.mFloatBlock[7]; // 7
         mSensorLocation[0] = aBlock.mFloatBlock[8]; // 8-10
         mSensorLocation[1] = aBlock.mFloatBlock[9];
         mSensorLocation[2] = aBlock.mFloatBlock[10];
         mSignalToNoise     = aBlock.mFloatBlock[11]; // 11
         mTrueRange         = aBlock.mFloatBlock[12]; // 12
         mTrueAzimuth       = aBlock.mFloatBlock[13]; // 13
         mTrueElevation     = aBlock.mFloatBlock[14]; // 14
         mTrueRangeRate     = aBlock.mFloatBlock[15]; // 15
         mSensorVelocity[0] = aBlock.mFloatBlock[16]; // 16-18
         mSensorVelocity[1] = aBlock.mFloatBlock[17];
         mSensorVelocity[2] = aBlock.mFloatBlock[18];
      }
      /*
               void Log(std::fstream& aFS)
               {
                  aFS << mRange << std::endl;
                  aFS << mAzimuth         << std::endl;
                  aFS << mElevation   << std::endl;
                  aFS << mRangeRate        << std::endl;
                  aFS << mRangeStDev << std::endl;
                  aFS << mAzimuthStDev  << std::endl;
                  aFS << mElevationStDev << std::endl;
                  aFS << mRangeRateStDev << std::endl;
               }
      */
   };

public:
   MTT_RadarMeasurement()
      : MTT_Measurement()
   {
      mDataPtr = new MTT_RadarMeasurement::Data();
   }

   MTT_RadarMeasurement(const MTT_RadarMeasurement& aSrc)
      : MTT_Measurement(aSrc)
   {
   }

   ~MTT_RadarMeasurement() override {}

   MTT_Measurement* Clone() const override { return new MTT_RadarMeasurement(*this); }

   MTT_RadarMeasurement::Data& GetData() const { return *(static_cast<MTT_RadarMeasurement::Data*>(mDataPtr)); }

   void SetData(const MTT_RadarMeasurement::Data& aData)
   {
      if (mDataPtr != nullptr)
      {
         delete mDataPtr;
      }
      mDataPtr = new MTT_RadarMeasurement::Data(aData);
   }
};


#endif
