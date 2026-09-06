// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_MULTIRESOLUTION_WRAPPER_META_MODEL_HPP
#define WSF_MULTIRESOLUTION_WRAPPER_META_MODEL_HPP

#include <memory>
#include <vector>

#include "FidelityRange.hpp"
#include "UtCloneablePtr.hpp"
#include "UtOptional.hpp"
#include "WsfComponent.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfStringId.hpp"

class WsfScenario;
class UtInputBlock;

//! @brief A class template for defining multiresolution containers for WsfPlatformComponent types.
//!
//! The purpose of this class is to define a multiresolution container for a WsfPlatformComponent. The
//! assumptions are that the container defines a family of models, each assigned non-overlapping fidelity ranges
//! (which are values between 0 and 1 inclusive). These fidelity ranges define the range over which the model should
//! be used when a fidelity value is given, currently only defined on this container, though in the future it may
//! be definable at other parts of a scenario.
//!
//! More specifically, the container can consist of one or more instantiations of its specific WsfPlatformComponent
//! (such as WsfMover) that are chosen from during initialization. The choice is made by assigning each
//! contained WsfPlatformComponent a non-overlapping <em>fidelity range</em>. The container itself then has a
//! <em>fidelity</em> and the model whose range contains the container's fidelity is used as the containing platform's
//! model. There is also an input available for users to forward input commands to all models.
//! @tparam DerivedComponent A type deriving from WsfPlatformComponent
//! @note Future enhancement may enable specifying fidelity at different input scope(s) than directly in the
//!  container. In other words, the fidelity parameter will be specifiable elsewhere so that specifying the model
//!  fidelity to use is simpler.
template<typename DerivedComponent>
class WsfMultiresolutionWrapperMetaModel final : public WsfMultiresolutionPlatformComponent<DerivedComponent>
{
public:
   WsfMultiresolutionWrapperMetaModel()                    = default;
   ~WsfMultiresolutionWrapperMetaModel() noexcept override = default;
   WsfMultiresolutionWrapperMetaModel& operator=(const WsfMultiresolutionWrapperMetaModel& aSrc) = delete;
   WsfMultiresolutionWrapperMetaModel(WsfMultiresolutionWrapperMetaModel&&)                      = delete;
   WsfMultiresolutionWrapperMetaModel& operator=(WsfMultiresolutionWrapperMetaModel&& aSrc) = delete;

   //! @name Framework methods.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfObject*    Clone() const override;
   bool          ProcessInput(UtInput& aInput) override;
   //@}

private:
   struct ModelWithFidelity
   {
      WsfStringId                         mName;
      wsf::multiresolution::FidelityRange mFidelityRange;
      ut::CloneablePtr<DerivedComponent>  mComponent;
   };
   using Index = typename std::vector<ModelWithFidelity>::size_type;

   //! Private copy ctor for Clone and CloneComponent
   WsfMultiresolutionWrapperMetaModel(const WsfMultiresolutionWrapperMetaModel&) = default;

   std::unique_ptr<DerivedComponent> GetModel() const override;

   //! @name Input processing methods.
   //@{
   //! @brief Reads a new model into `mComponentModels` at index @a aModelIndex.
   //! @throws Ut::BadValue Can throws if the component can't read the input.
   //! @pre @a aModelIndex must be in the range of `mComponentModels`. Checked with an assertion.
   bool ProcessModel(UtInputBlock& aInputBlock, Index aModelIndex);
   //! @brief Reads a new WsfPlatformComponent from @a aInput.
   //! @throw Ut::BadValue May throw if reading the new component fails.
   std::unique_ptr<DerivedComponent> ProcessComponent(UtInput&                          aInput,
                                                      std::unique_ptr<DerivedComponent> aExistingComponent);
   //! @brief Reads the model name from input @a aInputBlock. If the name exists, returns the index of the model
   //! in the `mComponentModels` vector. If the name doesn't exist, creates and inserts it. Additionally,
   //! @a aExpectExists is used for error checking.
   //! @throw Ut::BadValue if the model name doesn't exist and @a aExpectExists is `true`, or the model name
   //!  exists and @a aExpectExists is false.
   Index ProcessModelName(UtInputBlock& aInputBlock, ut::optional<bool> aExpectExists = ut::nullopt);
   //! @brief Reads a component type and constructs a new DerivedComponent from the type.
   //! @throw Ut::BadValue if the type isn't recognized.
   std::unique_ptr<DerivedComponent> ProcessComponentType(UtInput& aInput);
   //! @brief Reads all inputs in a `common` input block and forwards them to all currently instantiated models.
   //! @throw Ut::BadValue Throws if there are no currently defined models.
   bool ProcessCommonParameters(UtInput& aInput);
   //! @brief If valid (contains a non-null pointer). If invalid, throws.
   //! @throw Ut::BadValue Throws if @a aModelWithFidelity's WsfPlatformComponent is a `nullptr`. This likely means
   //!  that the component was not present or unreadable in the input stream @a aInput.
   void FinalizeModelProcessing(UtInput& aInput, const ModelWithFidelity& aModelWithFidelity);
   //! @throws If the defined range overlaps with any existing model's range. A range is never considered to be
   //!  overlapping with itself, i.e., a check is made that `aModelWithFidelity.mComponent` is not the same
   //!  as the one being checked in mComponentModels.
   void AffirmDisjointWithFidelityRanges(UtInput& aInput, const ModelWithFidelity& aModelWithFidelity) const;
   //@}

   //! @brief Searches for a DerivedComponent contained in this class whose fidelity range contains @a aFidelity.
   //! @return An index in `mComponentModels` if a match is found or a default is defined (`mDefaultComponentIndex`).
   //!  If no default is defined, returns `nullopt`.
   ut::optional<Index> ModelIndexForFidelity(double aFidelity) const;
   //! @return An index in mComponentModels for the entry with with the type name @a aTypeName.
   //! `nullopt` if no entry is found.
   ut::optional<Index> ModelIndexWithName(const std::string& aTypeName) const;

   std::vector<ModelWithFidelity> mComponentModels;
   //! Index to an existing DerivedComponent in mComponentModels. Used as a default if a match can't be
   //! made when searching fidelity ranges. An index is used (rather than a pointer, reference or iterator) to
   //! facilitate using the default copy ctor in Clone.
   ut::optional<Index> mDefaultComponentIndex;
};

#include "WsfMultiresolutionWrapperMetaModelImpl.hpp"

#endif
