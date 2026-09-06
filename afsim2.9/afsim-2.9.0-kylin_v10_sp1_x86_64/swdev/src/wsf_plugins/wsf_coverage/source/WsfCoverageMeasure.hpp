// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOVERAGEMEASURE_HPP
#define WSFCOVERAGEMEASURE_HPP

#include <string>

#include "UtCallbackHolder.hpp"
#include "UtCloneablePtr.hpp"
#include "WsfObject.hpp"
class UtInput;

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasureOutput.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace coverage
{

class Coverage;
class GridAsset;

//! Measure represents a MOE for a coverage computation.
//!
//! Each MOE is owned by a coverage object, and is responsible for connecting
//! to the various interval events on that coverage objects. In this way, the
//! Measure is updated as the simulation progresses with data as it becomes
//! available. Measures also own instances of output objects that will write the
//! resulting measured values out in various formats.
class WSF_COVERAGE_EXPORT Measure : public WsfObject
{
public:
   explicit Measure(const WsfScenario& aScenario)
      : mScenario(aScenario)
   {
   }
   Measure(const Measure& aOther) = default;
   ~Measure() override            = default;

   // WsfObject overrides
   Measure* Clone() const override = 0;
   bool     ProcessInput(UtInput& aInput) override;

   //! Return the measured value for a given @p aGridAsset.
   virtual double GetMeasuredValue(const GridAsset& aGridAsset) const = 0;

   //! Return a string giving the header for this value in output files.
   //!
   //! This header should be descriptive enough to be interpreted by a human reader.
   //! The return value will appear as a field in a CSV file, so there should not be
   //! any commas in the returned value.
   virtual std::string GetValueHeader() const = 0;

   //! Return the default value of the measure.
   virtual double GetDefaultValue() const = 0;

   bool Initialize(Coverage& aCoverage);

   //! Perform any needed actions when the coverage interval begins.
   //!
   //! \param aCoverage - The coverage owning this object.
   //! \param aSimTime  - The sim time at which the coverate interval starts.
   virtual void CollectionStarting(Coverage& aCoverage, double aSimTime) {}

   void CollectionCompleting(Coverage& aCoverage, double aSimTime);

protected:
   //! Return the number of output objects attached to this Measure.
   std::size_t GetNumOutput() const { return mOutputs.size(); }

   //! Return the output object at the given index.
   const MeasureOutput* GetOutput(std::size_t aIndex) const { return mOutputs[aIndex].get(); }

   //! Holder for callbacks created when this object connects to events on it's coverage.
   UtCallbackHolder mCallbacks{};

private:
   //! Perform any subclass-specific intialization.
   //!
   //! Subclasses should use this method to connect to the interval events on the
   //! given @p aCoverage. If the subclass can fail to initialize, it should
   //! return false for this method.
   //!
   //! \param aCoverage - The coverage owning this object.
   //! \returns         - true if initialization was successful; false otherwise.
   virtual bool InitializeP(Coverage& aCoverage) { return true; }

   //! Perform any subclass-specific actions when the coverage interval ends.
   //!
   //! \param aCoverage - The coverage owning this object.
   //! \param aSimTime  - The sim time at which the coverage interval ends.
   virtual void CollectionCompletingP(Coverage& aCoverage, double aSimTime) {}

   const WsfScenario&                           mScenario;  //! The scenario to which this measure belongs.
   std::vector<ut::CloneablePtr<MeasureOutput>> mOutputs{}; //! The output objects owned by this Measure.
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURE_HPP
