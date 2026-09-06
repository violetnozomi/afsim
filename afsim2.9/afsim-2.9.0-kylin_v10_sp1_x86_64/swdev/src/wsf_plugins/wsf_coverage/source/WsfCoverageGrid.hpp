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

#ifndef WSFCOVERAGEGRID_HPP
#define WSFCOVERAGEGRID_HPP

#include <utility>

#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageGridPoint.hpp"
#include "WsfObject.hpp"
#include "WsfScenario.hpp"
class WsfSimulation;

#include "wsf_coverage_export.h"


namespace wsf
{
namespace coverage
{

class TextOutputWriter;

//! The Grid object defines the region over which coverage can be computed.
class WSF_COVERAGE_EXPORT Grid : public WsfObject
{
public:
   explicit Grid(const WsfScenario& aScenario);
   Grid(const Grid& aOther) = default;
   ~Grid() override         = default;

   // WsfObject overrides
   Grid* Clone() const override = 0;
   bool  ProcessInput(UtInput& aInput) override;

   // Grid Interface

   //! Prepare the platforms for this grid.
   //!
   //! This method will create or modify the platforms associated with this
   //! grid object. In particular, this should mark created platforms as private
   //! so they are not sent over DIS. This should also set the side of created
   //! platforms so they might be easily removed from the visual tools.
   //!
   //! \param aSimulation - The simulation in which this grid's platforms will participate.
   virtual void PreparePlatforms(WsfSimulation& aSimulation) = 0;

   //! Return the Point at the given index if it exists.
   //!
   //! This will return the Point with the given @p aPointId if such
   //! a point exists. This will return nullptr if this Grid has not
   //! been initialized.
   //!
   //! \param aPointId - The ID of the point to retrieve.
   //! \returns        - The Point if it exists; nullptr otherwise.
   virtual const Point* GetPoint(PointId aPointId) const = 0;

   //! Return the number of Points in this Grid.
   //!
   //! This will return the number of Points in this Grid. This will return
   //! 0 if this Grid has not been initialized.
   //!
   //! \returns - The number of Points in this grid.
   virtual std::size_t GetSize() const = 0;

   //! Invoke the given action on every Point in this Grid.
   //!
   //! This method invokes the given action for every Point in this Grid.
   //! If this method is used before this Grid is initialized, this method
   //! will do nothing.
   //!
   //! \param aAction - The action to invoke for every Point in this Grid.
   virtual void Invoke(GridAction& aAction) const = 0;

   //! Return a grid asset for the point with the given ID.
   //!
   //! This returns the grid asset for the point with the given @p aPointId.
   //! If there is no such point, this will return an invalid grid asset.
   //!
   //! \param aPointId - The ID of a point in this grid.
   //! \returns        - The corresponding grid asset if it exists; an invalid
   //!                   grid asset otherwise.
   virtual GridAsset GetAsset(PointId aPointId) const = 0;

   //! Return a grid asset for the given platform.
   //!
   //! This returns the grid asset associated with the platform that has the
   //! given @p aPlatformName. If there is no such asset, this will return an
   //! invalid grid asset.
   //!
   //! \param aPlatformName - The name of one of this grid's asset platform.
   //! \returns             - The corresponding grid asset if it exists; an invalid
   //!                        grid asset otherwise.
   virtual GridAsset GetAsset(const std::string& aPlatformName) const = 0;

   //! Return if this grid's assets have null devices.
   //!
   //! This will determine if this grid has any asset with a null device, and
   //! set @p aAnyNull accordingly. This will also determing if all of this grid's
   //! assets have null devices, and set @p aAllNull accordingly.
   //!
   //! \param aAnyNull [out] - Does any asset have a null device.
   //! \param aAllNull [out] - Do all assets have a null device.
   virtual void HasNullDevice(bool& aAnyNull, bool& aAllNull) const = 0;

   //! Return if the grid creates new platforms during initialization.
   //!
   //! A grid should either create new platforms, or use existing platforms.
   //! This method returns if this grid creates new platforms. The behavior of
   //! some coverage classes will vary depending on the result of this method.
   //!
   //! \returns - true if this grid creates new platforms during initialization; false
   //!            if it uses existing platforms.
   virtual bool CreatesNewPlatforms() const = 0;

   //! Return if the grid is structured.
   //!
   //! Structured grids are those grids that have some positional organization of the
   //! grid points. Most grids are structured. For an example of an unstructured
   //! grid, see the ExistingPlatformGrid. To prevent a subclass from participating
   //! in overlay output from a Coverage object, IsStructured should return false.
   //!
   //! \returns true if the grid has some structure to its points; false otherwise.
   virtual bool IsStructured() const { return false; }

   //! Return the number of grid components.
   //!
   //! Grid components are geometrically distinct portions of a grid. Most grids have
   //! a single component. For an example of a grid with multiple components, see
   //! CompositeGrid.
   //!
   //! \returns The number of components of this grid.
   virtual std::size_t GetNumComponents() const { return 1; }

   //! Return the name of the specified grid component.
   virtual std::string GetComponentName(std::size_t aComponentIndex) const { return GetName(); }

   //! Return the number of points in the specified grid component.
   //!
   //! This method returns the size of the structured grid as the N-S and E-W size of the grid in
   //! the first and second components of the returned pair, but only if this grid returns true
   //! from IsStructured. Otherwise, the size of the component is returned in the first component,
   //! and a 1 is returned in the second component.
   //!
   //! \param aComponentIndex  The component whose size is being queried.
   //! \returns                A pair containing the N-S and E-W number of points in the structured
   //!                         grid (respectively), or the size of the component in the first component
   //!                         and 1 in the second component if the grid is not structured.
   virtual std::pair<std::size_t, std::size_t> GetComponentSizes(std::size_t aComponentIndex) const;

   //! Return the structured index of the point with the given @p aPointId.
   //!
   //! This returns the indices of the point with the given @p aPointId in the structure of the
   //! grid. If the grid is not structured, this will return a pair containing two zeros. The
   //! returned indices will each be in the range [0, N) where N is the respective component of
   //! the value returned by GetComponentSizes. If the given @p aPointId does not refer to a point
   //! in this grid, this will return a pair with out of bounds indices (they are larger than
   //! the return values from GetComponentSizes).
   //!
   //! \param aPointId  The id of the point whose structure indices are requested.
   //! \returns         The structure indices of the point for structured grids; a pair with two zeros otherwise.
   virtual std::pair<std::size_t, std::size_t> GetPointIndices(PointId aPointId) const;

   //! Invoke the given @p aAction on the points of the specified grid component.
   virtual void InvokeOnComponent(GridAction& aAction, std::size_t aComponentIndex) const { Invoke(aAction); }

   void SetUsedFlag() { mUsedFlag = true; }
   bool IsUsed() const { return mUsedFlag; }

   bool Initialize();
   void CategorizeAssets(const WsfSimulation& aSimulation, const std::string& aCategory) const;

   //! Return if this Grid has been initialized.
   bool IsInitialized() const { return mInitialized; }

   //! Return the central body to which this grid's point locations refer.
   const ut::CentralBody& GetCentralBody() const { return *mCentralBodyPtr; }

   void SetCentralBody(const ut::CentralBody& aCentralBody);

   //! Return the configured grid data file name.
   const std::string& GetGridDataFileName() const { return mGridDataFileName; }

   //! Return if this grid is suppressing grid platforms.
   bool GetSuppressGridPlatforms() const { return mSuppressGridPlatforms; }

   //! Set if this grid suppresses grid platforms.
   void SetSuppressGridPlatforms(bool aSuppress) { mSuppressGridPlatforms = aSuppress; }


   bool WriteGridDataFile(TextOutputWriter& aWriter) const;
   bool WriteGridDataFile() const;

protected:
   //! Grid type specific initialization.
   //!
   //! Initialization specific to the grid type occurs in this method. In particular,
   //! validation on input quantities should occur here. The set of grid points should be
   //! defined after this method's successful return, and both GetAsset methods should
   //! return valid GridAsset instances for any point in this grid.
   //!
   //! \returns true if initialization is successful; false otherwise.
   virtual bool InitializeP() = 0;

   PointId            GetNextAvailableId();
   const WsfScenario& GetScenario() const { return mScenario; }

private:
   // Private Grid interface
   virtual void WriteHeaderExtension(TextOutputWriter& aWriter) const {}
   virtual void WriteLineExtension(TextOutputWriter& aWriter, PointId aPointId) const {}

   const WsfScenario&                mScenario;
   ut::CloneablePtr<ut::CentralBody> mCentralBodyPtr;
   std::string                       mGridDataFileName{};
   PointId                           mNextPointId{cINVALID_POINT_ID};
   bool                              mInitialized{false};
   bool                              mSuppressGridPlatforms{true};
   bool                              mUsedFlag{false};
   // NOTE: The base class does not impose a certain structure to the storage
   //       of the grid points. That is up to the derived classes to work out
   //       what is most convenient.
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEGRID_HPP
