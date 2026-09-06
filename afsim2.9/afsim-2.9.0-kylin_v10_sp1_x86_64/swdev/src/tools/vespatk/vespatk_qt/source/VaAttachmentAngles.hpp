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


#ifndef VAATTACHMENTANGLES_HPP
#define VAATTACHMENTANGLES_HPP

#include "vespatk_qt_export.h"

#include <map>
#include <string>

#include "AngleEntry.hpp"
#include "UtVec3.hpp"
#include "VaAttachment.hpp"

namespace vespa
{
//! The primary purpose of this class is to facilitate
//! the ability to attach a set of angles to a particular entity
//! Angle attachment consist of a circle shape the serves to visualize
//! the angle and an additional overlay of the measurement of the
//! angle.
class VESPATK_QT_EXPORT VaAttachmentAngles : public VaAttachment
{
public:
   VaAttachmentAngles(VaEntity& aParentEntity, VaViewer* aViwererPtr);
   ~VaAttachmentAngles() override = default;
   //! Hook into renderer update cycle, delegates to entry updates.
   void Update(double aTime) override;
   //! Adds a new angle to track.
   void AddEntry(AngleEntry& aEntry, const std::string& aUniqueId);
   //! Adds a new angle to track.
   void AddEntry(const UtVec3f&     aFirstAngle,
                 const UtVec3f&     aSecondAngle,
                 const QColor&      aColor,
                 const std::string& aUniqueId,
                 const std::string& aName,
                 float              aRadius);
   //! Updates all the entries that contains a vector that has been updated.
   //! It must be done this way because the same vector can be used for
   //! multiple angles.
   void UpdatePositionContainingVector(int aVectorId, UtVec3f aNewPosition);
   //! Retrieve the governed entries for read only iteration.
   const std::map<const std::string, AngleEntry>& GetEntries() const;
   //! Toggles the visibility of a particular entry.
   void ToggleEntryVisibility(const std::string& aEntryId);
   //! Toggles wire frame rendering of particular entry.
   void ToggleEntryFill(const std::string& aEntryId);
   //! Get a particular entry for evaluation.
   const AngleEntry* GetEntry(const std::string& aUniqueId) const;

private:
   //! Matches id consisting of unique ids for each vector to its
   //! entry.
   //! Format: vector1id_vector2id
   std::map<const std::string, AngleEntry> mAngleEntries{};
};
} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentAngles)

#endif
