// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaFactory.hpp"

#include "VaAttachment.hpp"
#include "VaAttachmentEllipsoid.hpp"
#include "VaAttachmentLabel.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentTexture.hpp"
#include "VaAttachmentVectors.hpp"
#include "VaAttachmentZoneCircle.hpp"
#include "VaAttachmentZoneEllipse.hpp"
#include "VaAttachmentZonePolygon.hpp"
#include "VaAttachmentZoneSphere.hpp"
#include "VaEntity.hpp"
#include "VaFileChooser.hpp"
#include "VaOverlay.hpp"
#include "VaOverlayCompass.hpp"
#include "VaOverlayElevationLines.hpp"
#include "VaOverlayGl.hpp"
#include "VaOverlaySelectionBox.hpp"
#include "VaOverlayShapeFile.hpp"
#include "VaOverlayText.hpp"
#include "VaOverlayTextBox.hpp"
#include "VaOverlayWdb.hpp"
#include "VaOverlayZoomBox.hpp"
#include "VaScenario.hpp"
#include "VaUtils.hpp"

namespace vespa
{

VaFactory::VaFactory() {}

VaFactory::~VaFactory() {}

VaEntity* VaFactory::CreateEntity(const std::string& aEntityType) const
{
   // Give the application layer a chance to create the entity.
   VaEntity* entityPtr = UserCreateEntity(aEntityType);

   // If the application layer didn't create an entity, see if the API can create it.
   if (entityPtr == nullptr)
   {
      if (aEntityType == "player")
      {
         entityPtr = new VaEntity("");
      }
   }
   return entityPtr;
}

//! Builds a new VaAttachment and adds it to the passed parent entity.
//!
//! @param aAttachmentType - A string describing the type of attachment to create
//! @param aEntity - The parent entity.
//! @param aViewerPtr - The viewer in which this attachment will be visible. If NULL is passed
//!                     then the attachment will be visible in all viewers.
//! @param aLoad - If true, then Initialize() and Load() will be called on the newly created attachment
VaAttachment* VaFactory::CreateAttachment(const std::string& aAttachmentType,
                                          VaEntity&          aEntity,
                                          VaViewer*          aViewerPtr,
                                          bool               aLoad) const
{
   // Give the application layer a chance to create the attachment.
   VaAttachment* attachmentPtr = UserCreateAttachment(aAttachmentType, aEntity, aViewerPtr);

   // If the application layer didn't create an attachment, see if the API can create it.
   if (attachmentPtr == nullptr)
   {
      if (aAttachmentType == "label")
      {
         attachmentPtr = make_attachment<VaAttachmentLabel>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "model")
      {
         attachmentPtr = make_attachment<VaAttachmentModel>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "circle_zone")
      {
         attachmentPtr = make_attachment<VaAttachmentZoneCircle>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "ellipse_zone")
      {
         attachmentPtr = make_attachment<VaAttachmentZoneEllipse>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "sphere_zone")
      {
         attachmentPtr = make_attachment<VaAttachmentZoneSphere>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "polygon_zone")
      {
         attachmentPtr = make_attachment<VaAttachmentZonePolygon>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "ellipsoid")
      {
         attachmentPtr = make_attachment<VaAttachmentEllipsoid>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "vectors")
      {
         attachmentPtr = make_attachment<VaAttachmentVectors>(aEntity, aViewerPtr);
      }
      else if (aAttachmentType == "textureQuad")
      {
         attachmentPtr = make_attachment<VaAttachmentTexture>(aEntity, aViewerPtr);
      }
      else
      {
         throw UnknownAttachmentTypeError(aAttachmentType);
      }
   }

   if (aLoad)
   {
      VaAttachment::LoadAttachment(*attachmentPtr);
   }
   return attachmentPtr;
}

VaScenario* VaFactory::CreateScenario(const std::string& aScenarioType) const
{
   VaScenario* scenarioPtr = UserCreateScenario(aScenarioType);
   if (scenarioPtr == nullptr)
   {
      if (aScenarioType == "vespa")
      {
         scenarioPtr = new VaScenario("vespa");
      }
   }
   return scenarioPtr;
}

VaOverlay* VaFactory::CreateOverlay(const std::string& aOverlayType, const std::string& aOverlayName) const
{
   VaOverlay* overlayPtr = UserCreateOverlay(aOverlayType, aOverlayName);

   if (overlayPtr == nullptr)
   {
      if (aOverlayType == "compass")
      {
         if (aOverlayName != "")
         {
            overlayPtr = new VaOverlayCompass(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayCompass();
         }
      }
      else if (aOverlayType == "selection_box")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlaySelectionBox(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlaySelectionBox();
         }
      }
      else if (aOverlayType == "text")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayText(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayText();
         }
      }
      else if (aOverlayType == "textbox")
      {
         if (aOverlayName == "")
         {
            overlayPtr = new VaOverlayTextBox();
         }
         else
         {
            overlayPtr = new VaOverlayTextBox(aOverlayName);
         }
      }
      else if (aOverlayType == "gl")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayGl(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayGl();
         }
      }
      else if (aOverlayType == "zoom_box")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayZoomBox(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayZoomBox();
         }
      }
      else if (aOverlayType == "elevationlines")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayElevationLines(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayElevationLines();
         }
      }
      else if (aOverlayType == "shapefile")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayShapeFile(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayShapeFile();
         }
      }
      else if (aOverlayType == "wdb")
      {
         if (!aOverlayName.empty())
         {
            overlayPtr = new VaOverlayWdb(aOverlayName);
         }
         else
         {
            overlayPtr = new VaOverlayWdb();
         }
      }
   }
   return overlayPtr;
}

VaFileChooser* VaFactory::CreateFileChooser(const std::string& aFileChooserType,
                                            const std::string& aMessage,
                                            const std::string& aPattern,
                                            const std::string& aFileName,
                                            int                aRelative,
                                            const std::string& aNS,
                                            int                aType) const
{
   VaFileChooser* fileChooserPtr =
      UserCreateFileChooser(aFileChooserType, aMessage, aPattern, aFileName, aRelative, aNS, aType);
   return fileChooserPtr;
}

} // end namespace vespa
