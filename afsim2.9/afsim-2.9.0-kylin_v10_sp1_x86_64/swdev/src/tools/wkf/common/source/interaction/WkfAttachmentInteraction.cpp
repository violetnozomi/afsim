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

#include "WkfAttachmentInteraction.hpp"

#include <sstream>

#include <QGLWidget>
#include <QImage>

#include "UtoHitInfo.hpp"
#include "UtoIconBoardShape.hpp"
#include "UtoInteractionShape.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfInteractionPrefObject.hpp"
#include "WkfObserver.hpp"
#include "WkfUtils.hpp"

namespace
{
bool TypeIsUnpaired(const std::string& aType)
{
   return ((aType == "Kill") || (aType == "Message"));
}
} // namespace

wkf::AttachmentInteraction::AttachmentInteraction(vespa::VaEntity&             aEntity,
                                                  vespa::VaViewer*             aViewerPtr,
                                                  const std::string&           aName,
                                                  const InteractionPrefObject* aPrefObjectPtr)
   : vespa::VaAttachment(aEntity, aViewerPtr, aName)
   , mInteractionShapePtr(nullptr)
   , mIconShapePtr(nullptr)
   , mChanged(false)
   , mDescriptionDetailed(true)
   , mDescriptionEnabled(true)
   , mLineWidth(3)
{
   SetType<AttachmentInteraction>();
   if (aPrefObjectPtr != nullptr)
   {
      // Initialize defaults from pref object.
      SetColor(InteractionPrefData::cDETECT, aPrefObjectPtr->GetColor(InteractionPrefData::cDETECT));
      SetColor(InteractionPrefData::cTRACK, aPrefObjectPtr->GetColor(InteractionPrefData::cTRACK));
      SetColor(InteractionPrefData::cLOCALTRACK, aPrefObjectPtr->GetColor(InteractionPrefData::cLOCALTRACK));
      SetColor(InteractionPrefData::cFIRE, aPrefObjectPtr->GetColor(InteractionPrefData::cFIRE));
      SetColor(InteractionPrefData::cKILL, aPrefObjectPtr->GetColor(InteractionPrefData::cKILL));
      SetColor(InteractionPrefData::cJAM, aPrefObjectPtr->GetColor(InteractionPrefData::cJAM));
      SetColor(InteractionPrefData::cMESSAGE, aPrefObjectPtr->GetColor(InteractionPrefData::cMESSAGE));
      SetColor(InteractionPrefData::cTASK, aPrefObjectPtr->GetColor(InteractionPrefData::cTASK));
      SetArchSegments(aPrefObjectPtr->GetArchSegments());

      SetTimeout(aPrefObjectPtr->GetTimeout());
      SetDescriptionMode(aPrefObjectPtr->GetTooltipMode());
      SetLineWidth(aPrefObjectPtr->GetLineWidth());
      mStackingAllowed = aPrefObjectPtr->GetStackingAllowed();

      // Connect signals from pref object.
      QObject::connect(aPrefObjectPtr,
                       &InteractionPrefObject::ColorChanged,
                       this,
                       [this](const QString& aKey, const QColor& c) { SetColor(aKey.toStdString(), c); });
      QObject::connect(aPrefObjectPtr,
                       &InteractionPrefObject::TimeoutChanged,
                       this,
                       [this](double aTimeout) { SetTimeout(aTimeout); });
      QObject::connect(aPrefObjectPtr,
                       &InteractionPrefObject::TooltipModeChanged,
                       this,
                       [this](int mode) { SetDescriptionMode(mode); });
      QObject::connect(aPrefObjectPtr,
                       &InteractionPrefObject::LineWidthChanged,
                       this,
                       [this](unsigned int width) { SetLineWidth(width); });
      mCallbacks.Add(wkf::Observer::MapProjectionChanged.Connect(&wkf::AttachmentInteraction::MapProjectionChangedCB, this));
   }
}

void wkf::AttachmentInteraction::ShowInteractionOfType(const std::pair<std::string, int>& aType, bool aState)
{
   mChanged                    = true;
   mInteractionStateMap[aType] = aState;
}

bool wkf::AttachmentInteraction::InteractionsOfTypeAreShown(const std::pair<std::string, int>& aType) const
{
   auto it = mInteractionStateMap.find(aType);
   if (it != mInteractionStateMap.end())
   {
      return it->second;
   }
   return false;
}

void wkf::AttachmentInteraction::UpdateVisibility()
{
   if (GetParent().IsVisible(GetViewer()))
   {
      mChanged = true;
   }
   else
   {
      if (nullptr != mInteractionShapePtr)
      {
         mActiveLines.clear();
         mInteractionShapePtr->Clear();
         mIconShapePtr->Clear();
      }
   }
}

void wkf::AttachmentInteraction::UpdateFrame(double aTime)
{
   if (mChanged)
   {
      mChanged = false;
      if (nullptr != mInteractionShapePtr)
      {
         mActiveLines.clear();
         mInteractionShapePtr->Clear(); // remove existing data... this isn't terribly efficient
         mIconShapePtr->Clear();
      }
      if (GetParent().IsVisible(GetViewer()))
      {
         std::map<unsigned int, unsigned int> offsetMap;

         for (const auto& interactMap : mActiveInteractions) // for every live interaction
         {
            const Interaction& interact = interactMap.first;
            const TextMap&     textMap  = interactMap.second;
            if ((InteractionsOfTypeAreShown(interact.mType)) && !textMap.empty())
            {
               if (nullptr == mInteractionShapePtr) // if we don't have a shape
               {
                  CreateShape(); // create the shape
               }
               if (nullptr != mInteractionShapePtr) // if we have a shape
               {
                  if (interact.mType.second == eOUTGOING) // if interaction.direction is outgoing
                  {
                     auto* tgtint = interact.mTarget->FindFirstAttachmentOfType<AttachmentInteraction>();

                     if (!tgtint || (!tgtint->InteractionsOfTypeAreShown(std::make_pair(interact.mType.first, eINCOMING))))
                     // if target is NOT showing interactions of interaction.type | incoming
                     {
                        if (interact.mTarget->IsVisible(GetViewer()))
                        {
                           bool first = true;
                           for (const auto& idText : textMap)
                           {
                              if (mStackingAllowed || first || !TypeIsUnpaired(interact.mType.first))
                              {
                                 // if the event is not explicitly terminated, or it is the first one, or we allow stacking
                                 int retval =
                                    mInteractionShapePtr->AddOutboundSegment(interact.mTarget->GetUtoEntity(),
                                                                             offsetMap[interact.mTarget->GetUniqueId()]++,
                                                                             LookupColor(interact.mType.first));
                                 mActiveLines[retval] = std::make_pair(interact, idText.second);
                              }
                              first = false;
                           }
                        }
                        else
                        {
                           mChanged = true; // we could resolve this yet
                        }
                     }
                  }
                  else
                  {
                     if ((interact.mTarget) && (interact.mTarget->IsVisible(GetViewer())))
                     {
                        bool first = true;
                        for (const auto& idText : textMap)
                        {
                           if (DrawLine(interact.mType.first))
                           {
                              if (mStackingAllowed || first || !TypeIsUnpaired(interact.mType.first))
                              {
                                 int retval =
                                    mInteractionShapePtr->AddInboundSegment(interact.mTarget->GetUtoEntity(),
                                                                            offsetMap[interact.mTarget->GetUniqueId()]++,
                                                                            LookupColor(interact.mType.first));
                                 mActiveLines[retval] = std::make_pair(interact, idText.second);
                              }
                           }
                           else if (DrawIcon(interact.mType.first)) // we only draw icons on incoming events
                           {
                              mIconShapePtr->AddCard(interact.mType.first);
                           }
                           first = false;
                        }
                     }
                     else
                     {
                        mChanged = true; // we could resolve this yet
                     }
                  }
               }
            }
         }
      }
   }
}

void wkf::AttachmentInteraction::AddInteraction(const std::pair<std::string, int>& aType,
                                                vespa::VaEntity*                   aTarget,
                                                const std::string&                 aDisplayText,
                                                unsigned int                       aId)
{
   TextMap& textMap = mActiveInteractions[Interaction(aType, aTarget)];
   textMap.insert(std::make_pair(aId, aDisplayText));
   mChanged = true;
}

void wkf::AttachmentInteraction::RemoveInteraction(const std::pair<std::string, int>& aType,
                                                   vespa::VaEntity*                   aTarget,
                                                   unsigned int                       aId)
{
   Interaction interact(aType, aTarget);
   auto        it = mActiveInteractions.find(interact);
   if (it != mActiveInteractions.end())
   {
      auto text = it->second.find(aId);
      if (text != it->second.end())
      {
         it->second.erase(text);
         mChanged = true;
      }
   }
}

void wkf::AttachmentInteraction::MapProjectionChangedCB(unsigned int aTargetId, const UtoMapProjection* aProjection)
{
   if (mInteractionShapePtr != nullptr)
   {
      vespa::VaViewer* viewer = GetViewer();
      mInteractionShapePtr->SetSceneProjection(viewer->GetMapProjection());
   }
}

void wkf::AttachmentInteraction::CreateShape()
{
   std::ostringstream oss;
   oss << "WkfAttachmentInteractionLine_" << GetUniqueId();

   RemoveShapeFromParent(oss.str().c_str());
   mInteractionShapeName = oss.str().c_str();
   mInteractionShapePtr  = nullptr;

   // Load the shape.
   UtoInteractionShape intrDraw;
   intrDraw.SetAggregation(UtoPolyLineShape::LINES);
   intrDraw.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);

   unsigned int   tmapSize   = 8;
   unsigned char* textureMap = new unsigned char[8];
   textureMap[0]             = 255;
   textureMap[1]             = 230;
   textureMap[2]             = 205;
   textureMap[3]             = 180;
   textureMap[4]             = 155;
   textureMap[5]             = 130;
   textureMap[6]             = 105;
   textureMap[7]             = 80;

   mInteractionShapePtr = dynamic_cast<UtoInteractionShape*>(AddShapeToParent(oss.str(), intrDraw));
   mInteractionShapePtr->EnableAntiAliasLines(true);
   mInteractionShapePtr->ArchSegments(mArchSegments);

   if (mInteractionShapePtr != nullptr)
   {
      vespa::VaViewer* viewer = GetViewer();
      mInteractionShapePtr->SetSceneProjection(viewer->GetMapProjection());
      mInteractionShapePtr->SetTexturePattern(textureMap, tmapSize);
      mInteractionShapePtr->SetWidth(mLineWidth);
      mInteractionShapePtr->OverrideRenderBin(100, false);
   }
   if (nullptr != textureMap)
   {
      delete[] textureMap;
   }

   // lets also create our icon shape
   std::ostringstream oss2;
   oss2 << "WkfAttachmentInteractionIcon_" << GetUniqueId();

   RemoveShapeFromParent(oss2.str().c_str());
   mIconShapeName = oss2.str().c_str();
   mIconShapePtr  = nullptr;

   // Load the shape.
   UtoIconBoardShape iconDraw;
   iconDraw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   mIconShapePtr = dynamic_cast<UtoIconBoardShape*>(AddShapeToParent(oss2.str(), iconDraw));

   if (mIconShapePtr != nullptr)
   {
      mIconShapePtr->OverrideRenderBin(100, false);
      for (const auto& c : mCards)
      {
         QImage image(c.second);
         QImage GL_formatted = QGLWidget::convertToGLFormat(image);
         mIconShapePtr->AddTexture(c.first, GL_formatted.width(), GL_formatted.height(), GL_formatted.bits());
      }
   }
}

void wkf::AttachmentInteraction::AddCard(const QString& aName, const QString& aIcon)
{
   mCards[aName.toStdString()] = aIcon;
   if (mIconShapePtr != nullptr)
   {
      QImage image(aIcon);
      QImage GL_formatted = QGLWidget::convertToGLFormat(image);
      mIconShapePtr->AddTexture(aName.toStdString(), GL_formatted.width(), GL_formatted.height(), GL_formatted.bits());
   }
}

void wkf::AttachmentInteraction::AddInteractionType(const QString& aName, const QString& aHoverText)
{
   mAddedInteractionTypes[aName.toStdString()] = aHoverText.toStdString();
   SetColor(aName.toStdString(), wkfEnv.GetPreferenceObject<wkf::InteractionPrefObject>()->GetColor(aName));
}

UtoColor wkf::AttachmentInteraction::LookupColor(const std::string& aType)
{
   const auto it = mColorMap.find(aType);
   if (it != mColorMap.end())
   {
      return it->second;
   }
   else
   {
      return UtoColor(255, 0, 0, 255);
   }
}

std::string wkf::AttachmentInteraction::LookupText(const std::string& aType)
{
   if (aType == InteractionPrefData::cDETECT)
   {
      return "detects";
   }
   else if (aType == InteractionPrefData::cTRACK)
   {
      return "has a sensor track on";
   }
   else if (aType == InteractionPrefData::cFIRE)
   {
      return "engages";
   }
   else if (aType == InteractionPrefData::cKILL)
   {
      return "kills";
   }
   else if (aType == InteractionPrefData::cJAM)
   {
      return "jams";
   }
   else if (aType == InteractionPrefData::cMESSAGE)
   {
      return "communicates with";
   }
   else if (aType == InteractionPrefData::cTASK)
   {
      return "tasks";
   }
   else if (aType == InteractionPrefData::cLOCALTRACK)
   {
      return "has a local track on";
   }
   else
   {
      const auto& inttype = mAddedInteractionTypes.find(aType);
      if (inttype != mAddedInteractionTypes.end())
      {
         return inttype->second;
      }
      return aType + " at";
   }
}

bool wkf::AttachmentInteraction::DrawLine(const std::string& aType) const
{
   return (mCards.find(aType) == mCards.end());
}

bool wkf::AttachmentInteraction::DrawIcon(const std::string& aType) const
{
   return !DrawLine(aType);
}

void wkf::AttachmentInteraction::PrivateInitializeCallbacks()
{
   mCallbacks.Add(vespa::VaObserver::EntityDeleted.Connect(&AttachmentInteraction::EntityDeleted, this));
}

void wkf::AttachmentInteraction::EntityDeleted(vespa::VaEntity* aEntityPtr)
{
   auto it = mActiveInteractions.begin();
   while (it != mActiveInteractions.end())
   {
      it->first.mTarget->GetUniqueId() == aEntityPtr->GetUniqueId() ? it = mActiveInteractions.erase(it) : ++it;
   }
}

void wkf::AttachmentInteraction::SetColor(const std::string& aType, const UtoColor& aColor)
{
   mChanged         = true;
   mColorMap[aType] = aColor;
}

void wkf::AttachmentInteraction::SetColor(const std::string& aType, const QColor& aColor)
{
   SetColor(aType, UtoColor(aColor.red(), aColor.green(), aColor.blue(), aColor.alpha()));
}

void wkf::AttachmentInteraction::SetTimeout(double aTimeout)
{
   // we are really at the mercy of whatever is feeding us active state to update this, but we can take note that a
   // redraw is necessary
   mChanged = true;
}

void wkf::AttachmentInteraction::SetDescriptionMode(int aMode)
{
   mDescriptionDetailed = (aMode == InteractionPrefData::cTT_DETAILED);
   mDescriptionEnabled  = (aMode != InteractionPrefData::cTT_OFF);
}

void wkf::AttachmentInteraction::SetLineWidth(unsigned int aWidth)
{
   mLineWidth = aWidth;
   if (mInteractionShapePtr)
   {
      mInteractionShapePtr->SetWidth(mLineWidth);
   }
}

void wkf::AttachmentInteraction::HitSubparts(UtoHitInfoList& aHits, vespa::VaHitList& aSubHits)
{
   if (aHits.mShapePtr)
   {
      int                entityUID(0), attachmentUID(0), additionalInfo(0);
      std::string        entityUIDStr = aHits.mShapePtr->AuxData();
      std::istringstream iss(entityUIDStr.c_str());
      iss >> entityUID;
      iss >> attachmentUID;
      additionalInfo = aHits._hits.begin()->getPrimitiveIndex() + 1;
      //      iss >> additionalInfo;

      aSubHits.push_back(vespa::VaHitEntry::FromAttachment(*this, additionalInfo));
   }
}


std::string wkf::AttachmentInteraction::GetDescription(unsigned int aSubId)
{
   if (mDescriptionEnabled)
   {
      int  actualId = 1 + (aSubId - 1) / 20; // account for arch-segmentation
      auto it       = mActiveLines.find(actualId);
      if (it != mActiveLines.end())
      {
         Interaction& interact = it->second.first;
         std::string  text     = it->second.second;

         if (interact.mType.second == eINCOMING)
         {
            std::string retval = interact.mTarget->GetName();
            retval += " ";
            retval += LookupText(interact.mType.first);
            retval += " ";
            retval += GetParent().GetName();
            // retval += ".  ";
            if (mDescriptionDetailed)
            {
               retval += text;
            }

            return retval;
         }
         else if (interact.mType.second == eOUTGOING)
         {
            std::string retval = GetParent().GetName();
            retval += " ";
            retval += LookupText(interact.mType.first);
            retval += " ";
            retval += interact.mTarget->GetName();
            // retval += ".  ";
            if (mDescriptionDetailed)
            {
               retval += text;
            }

            return retval;
         }
      }
   }

   return std::string();
}

void wkf::AttachmentInteraction::SetStackingAllowed(bool aState)
{
   mStackingAllowed = aState;
   mChanged         = true;
}

bool wkf::AttachmentInteraction::Interaction::operator<(const Interaction& aRHS) const
{
   if (mType > aRHS.mType)
   {
      return false;
   }
   else if (mType < aRHS.mType)
   {
      return true;
   }
   else if (mTarget->GetUniqueId() >= aRHS.mTarget->GetUniqueId())
   {
      return false;
   }
   else // (mTarget->GetUniqueId() < aRHS.mTarget->GetUniqueId())
   {
      return true;
   }
}
