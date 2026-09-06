// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ORBITPREVIEWENTITY_HPP
#define ORBITPREVIEWENTITY_HPP

class UtCalendar;
#include "wsf_spaceg_export.h"

#include "UtColor.hpp"
#include "WkfEntity.hpp"

class UtOrbitalElements;

namespace vespa
{

class VaAttachmentModel;

} // namespace vespa

namespace wkf
{

class AttachmentOrbit;

} // namespace wkf

namespace wsfg
{

namespace spaceg
{

class EventOrbitDetails;

class WSF_SPACEG_EXPORT OrbitPreviewEntity : public wkf::Entity
{
public:
   OrbitPreviewEntity(const std::string& aName, const UtColor& aColor = UtColor{1.0f, 1.0f, 1.0f});
   ~OrbitPreviewEntity() override = default;

   void UpdateEventDetails(const EventOrbitDetails& aDetails);

   void    SetColor(const UtColor& aColor);
   UtColor GetColor() const { return mPreviewColor; }

   void SetVisible(bool aVisible);

private:
   UtColor FadedColor() const;
   void    AssureBothOrbitsExist();
   void    UpdateColors();
   void    UpdatePositionOrientation(UtOrbitalElements aElements, const UtCalendar& aEpoch);

   UtColor                   mPreviewColor;
   vespa::VaAttachmentModel* mModelPtr;
   wkf::AttachmentOrbit*     mOrbitBeforePtr;
   wkf::AttachmentOrbit*     mOrbitAfterPtr;
};

} // namespace spaceg

} // namespace wsfg

VA_DECLARE_OBJECT_TYPE(wsfg::spaceg::OrbitPreviewEntity)
#endif // ORBITPREVIEWENTITY_HPP
