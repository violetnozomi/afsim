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

#ifndef WSFALARM_ANTENNAPATTERN_HPP
#define WSFALARM_ANTENNAPATTERN_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "WsfAntennaPattern.hpp"
#include "WsfEM_Types.hpp"
class WsfScenario;

//! A class that represents an ALARM or SUPPRESSOR ANTGR external antenna pattern.
//! This class provide the capability to read and interrogate an ALARM antenna
//! patterns or SUPPRESSOR ANTGR patterns.
//!
//! @note At the current time this class supports only the processing of 2D files.
//! It does not support the 3D or illumination formats.
//!
//! @note SUPPRESSOR supports the concept of an 'aperture type' or 'shape' that
//! controls how interpolation is performed. This class will support the 'shape'
//! if it is present.
//!
//! @note The routines in this class were primarily adapted from SUPPRESSOR 6.0.
//! The ALARM routine antenna_pattern::gain_lookup appeared to be very similar
//! to the SUPPRESSOR elliptical case, but was different. For now we are taking
//! the SUPPRESSOR approach because that is what we want to match...
//!
//! @note This class is used as a base class by WsfGENAP_AntennaPattern. It
//! generates the pattern rather than reading it in from a file. Other than that
//! it uses the same methods for determine off-axis gains from the data.
class WsfALARM_AntennaPattern : public WsfAntennaPattern
{
public:
   //! Aperture shape.
   //! @note: The values MUST be as defined because they correspond to the values passed to
   //!        the SUPPRESSOR routine GENAP.
   enum ApertureShape
   {
      cAS_UNKNOWN     = 0,
      cAS_CIRCULAR    = 1,
      cAS_ELLIPTICAL  = 2,
      cAS_RECTANGULAR = 3,
      cAS_SQUARE      = 4
   };

   //! Data that is shared amongst all instances of a given ALARM antenna pattern
   class ALARM_Data : public WsfAntennaPattern::BaseData
   {
   public:
      struct PatternData
      {
         PatternData()
            : mPeakGain()
            , mInputAzBeamwidth(0.0)
            , mInputElBeamwidth(0.0)
            , mAzBeamwidth(0.0)
            , mElBeamwidth(0.0)
            , mAzAngles()
            , mAzGains()
            , mAzMin(0.0)
            , mAzMax(0.0)
            , mElAngles()
            , mElGains()
            , mElMin(0.0)
            , mElMax(0.0)
         {
         }


         double mPeakGain;
         double mInputAzBeamwidth;
         double mInputElBeamwidth;
         double mAzBeamwidth;
         double mElBeamwidth;

         std::vector<double> mAzAngles;
         std::vector<double> mAzGains;
         double              mAzMin; //!< The value of mAzAngles[0]
         double              mAzMax; //!< The value of mAzAngles[last]

         std::vector<double> mElAngles;
         std::vector<double> mElGains;
         double              mElMin; //!< The value of mElAngles[0]
         double              mElMax; //!< The value of mElAngles[last]
      };
      using PatternMap = std::map<WsfEM_Types::Polarization, PatternData>;

      ALARM_Data();
      ~ALARM_Data() override = default;
      ;

      bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput) override;
      bool Initialize(WsfAntennaPattern& aAntennaPattern) override;

      double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El) override;

      virtual void ReadPattern(WsfScenario& aScenario, const std::string& aFileName);

      bool InitializeBase();

      std::string mFileName;

      ApertureShape mApertureShape;

      double mMinGain;

      WsfEM_Types::Polarization              mSetPolarization; //! Set polarization for this pattern set by user
      std::vector<WsfEM_Types::Polarization> mPolarizations;   //! vector of ordered polarizations read in

      PatternMap mPatternMap;

      double mOneMinusE_2; //! For elliptical aperture, the value (1 -e^2)

      double mGainCorrection;

      std::string mTitle;
      std::string mClassification;
      double      mWavelength;

      bool mInputIs2D;
      bool mInputIsDB;

   protected:
      std::vector<double>::size_type GetIndex(const std::vector<double>& aTable, double aValue) const;

      double GetCircularApertureGain(double aAzLook, double aElLook);
      double GetEllipticalApertureGain(double aAzLook, double aElLook);
      double GetRectangularApertureGain(double aAzLook, double aElLook);

   private:
      void ConvertValue(const std::string& aString, double& aValue) const;
      void ConvertValue(const std::string& aString, int& aValue) const;

      void ParseLine(const std::string& aLine, std::vector<std::string>& aWords) const;
      void Read2D_File(std::istream& aIn, WsfEM_Types::Polarization aPolarization);
      void ReadLine(std::istream& aIn, std::string& aLine);
   };

   WsfALARM_AntennaPattern(ALARM_Data* aBaseDataPtr);

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   WsfAntennaPattern* Clone() const override;

   double GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetPeakGain(double aFrequency) const override;

   using Table      = std::vector<double>;
   using TableIndex = std::vector<double>::size_type;

   //! @name Private data access.
   //! These methods return data that should not be considered part of the public interface.
   //! They were provided to support External Services.
   //! Use at your own risk!
   //@{
   virtual const std::vector<double>& GetAzAngles() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mAzAngles;
   }
   virtual const std::vector<double>& GetAzGains() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mAzGains;
   }
   virtual const std::vector<double>& GetElAngles() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mElAngles;
   }
   virtual const std::vector<double>& GetElGains() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mElGains;
   }
   virtual ApertureShape GetApertureShape() const { return AlarmData()->mApertureShape; }
   virtual double        GetMinGain() const { return AlarmData()->mMinGain; }
   virtual double        GetGainCorrection() const { return AlarmData()->mGainCorrection; }
   virtual double        GetInputAzimuthBeamwidth() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mInputAzBeamwidth;
   }
   virtual double GetInputElevationBeamwidth() const
   {
      return AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mInputElBeamwidth;
   }

   //@}

protected:
   ALARM_Data* AlarmData() const { return dynamic_cast<ALARM_Data*>(mSharedDataPtr); }

   WsfALARM_AntennaPattern(const WsfALARM_AntennaPattern& aSrc) = default;
   WsfALARM_AntennaPattern operator=(const WsfALARM_AntennaPattern& aRhs) const = delete;
};

#endif
