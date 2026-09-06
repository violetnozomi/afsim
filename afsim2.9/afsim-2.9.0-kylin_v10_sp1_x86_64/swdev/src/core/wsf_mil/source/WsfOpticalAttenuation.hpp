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

#ifndef WSFOPTICALATTENUATION_HPP
#define WSFOPTICALATTENUATION_HPP

#include "wsf_mil_export.h"

#include <functional>
#include <iosfwd>
#include <memory>

#include "UtAtmosphere.hpp"
#include "WsfEM_Attenuation.hpp"

//! An attenuation model for optical systems (visual and infrared).
class WSF_MIL_EXPORT WsfOpticalAttenuation : public WsfEM_Attenuation
{
public:
   class Segment
   {
   public:
      Segment()
         : mRangeLimit(0.0F)
         , mA(0.0F)
         , mB(0.0F)
      {
      }
      Segment(float aRangeLimit, float aA, float aB)
         : mRangeLimit(aRangeLimit)
         , mA(aA)
         , mB(aB)
      {
      }

      float mRangeLimit;
      float mA;
      float mB;
   };

   class SegmentRef
   {
   public:
      SegmentRef()
         : mAltitude(0.0F)
         , mIndex(0)
      {
      }
      SegmentRef(float aAltitude, unsigned int aIndex)
         : mAltitude(aAltitude)
         , mIndex(aIndex)
      {
      }

      float        mAltitude;
      unsigned int mIndex;
   };

   class Table
   {
   public:
      Table(const SegmentRef* aSegmentRefs, const Segment* aSegments)
         : mSegmentRefs(aSegmentRefs)
         , mSegments(aSegments)
      {
      }
      virtual ~Table() = default;

      const SegmentRef* mSegmentRefs;
      const Segment*    mSegments;
   };

   class TableX : public Table
   {
   public:
      TableX(const SegmentRef* aSegmentRefs, const Segment* aSegments)
         : Table(aSegmentRefs, aSegments)
      {
      }

      ~TableX() override
      {
         delete[] mSegmentRefs;
         delete[] mSegments;
      }
   };

   class TableRef
   {
   public:
      TableRef(const char* aTableNamePtr, const Table* aTablePtr)
         : mTableNamePtr(aTableNamePtr)
         , mTablePtr(aTablePtr)
      {
      }

      const char*  mTableNamePtr;
      const Table* mTablePtr;
   };

   WsfOpticalAttenuation(const WsfScenario& aScenario);
   WsfOpticalAttenuation(const WsfScenario&           aScenario,
                         double                       aAtmosphericAttenuation,
                         UtAtmosphere::AtmosphereType aAtmosphereType);
   ~WsfOpticalAttenuation() override = default;

   static std::function<WsfEM_Attenuation*(const std::string&)> GetObjectFactory(WsfScenario& aScenario);

   WsfEM_Attenuation* Clone() const override;

   bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   double ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                   WsfEnvironment&             aEnvironment,
                                   WsfEM_Interaction::Geometry aGeometry) override;

private:
   double ComputeSimpleAttenuation(double aAltitude1, double aAltitude2, double aGroundRange);

   double ComputeCompactAttenuation(double       aAltitude,
                                    double       aElevationAngle,
                                    double       aSlantRange,
                                    double       aEffectiveEarthRadius,
                                    const Table* aTablePtr);

   void ComputePathAttenuation(double&      aSumAlphaR,
                               double       aBegAltitude,
                               double       aElevationAngle,
                               double       aSlantRange,
                               double       aEndAltitude,
                               double       aAirSlantRange,
                               double       aEffectiveEarthRadius,
                               const Table* aTablePtr);

   const Table* FindInternalTable(const std::string& aTableName) const;

   bool SelectInternalTable(WsfEM_XmtrRcvr* aXmtrRcvrPtr);

   //! @name Methods to process the embedded 'query' command.
   //@{
   void ProcessQueryCommand(UtInput& aInput);
   //@}

   void LoadExternalTable(UtInput& aInput, const std::string& aTableFileName);

   void OpticalPathConversion(UtInput& aInput);

   //! @name Methods to convert raw spectral data to compact tables usable by this model.
   //@{
   void SpectralDataConversion(UtInput& aInput);

   void ProcessAltitude(std::ofstream&             aCodeOfs,
                        std::ofstream&             aTableOfs,
                        std::ofstream&             aPlotOfs,
                        double                     aAltitude,
                        const std::vector<double>& aRangeValues,
                        const std::vector<double>& aTransmittanceValues,
                        bool                       aMonochromatic,
                        std::vector<SegmentRef>&   aSegmentRefs,
                        std::vector<Segment>&      aSegments,
                        double                     aMaximumAbsoluteError,
                        double                     aMaximumRelativeError,
                        int                        aMaximumSegmentCount) const;

   void GenerateSegments(const std::vector<double>& aRangeValues,
                         const std::vector<double>& aTransmittanceValues,
                         const std::vector<double>& aAlphaValues,
                         const std::vector<double>& aLnRangeValues,
                         const std::vector<double>& aLnAlphaValues,
                         double                     aMaximumAbsoluteError,
                         double                     aMaximumRelativeError,
                         int                        aMaximumSegmentCount,
                         bool                       aOnlyUseConstantAlpha,
                         std::vector<Segment>&      aSegments) const;

   size_t TestStraightLineFit(const std::vector<double>& aRangeValues,
                              const std::vector<double>& aTransmittanceValues,
                              size_t                     aBeg,
                              size_t                     aEnd,
                              double                     aA,
                              double                     aB,
                              double                     aMaximumAbsoluteError,
                              double                     aMaximumRelativeError) const;

   void StraightLineFit(const std::vector<double>& aX,
                        const std::vector<double>& aY,
                        size_t                     aStart,
                        size_t                     aCount,
                        double&                    aA,
                        double&                    aB,
                        double&                    aSigA,
                        double&                    aSigB,
                        double&                    aChi2) const;
   //@}
   enum
   {
      cAT_COMPACT,
      cAT_SIMPLE,
      cAT_NONE
   } mAlgorithm;

   //! @name Parameters for automatic table selection
   //@{
   int mAtmosphereModel;
   int mHazeModel;
   int mCloudModel;
   //@}

   //! An adjustment factor that can be applied to adjust the returned value.
   double mAdjustmentFactor;

   //! Pointer to the selected internal compact table or the external compact table.
   const Table* mTablePtr;

   //! Pointer to the external compact table.
   std::shared_ptr<TableX> mExternalTablePtr;

   //! @name Parameters for the simple legacy model from WSF_EOIR_SENSOR and WSF_IRST_SENSOR.
   //@{
   //! The specific attenuation using the 'old' form
   //! This is amount of signal that remains per meter.
   double       mSimpleAttenuation;
   UtAtmosphere mAtmosphere;
   //@}
};

#endif
