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
#ifndef WKFATTACHMENTINTERACTION_HPP
#define WKFATTACHMENTINTERACTION_HPP

class QColor;
#include <QObject>
class UtoIconBoardShape;
class UtoInteractionShape;
class UtoMapProjection;
#include "wkf_common_export.h"

#include "UtoTypes.hpp"
#include "VaAttachment.hpp"
#include "WkfInteractionPrefObject.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentInteraction : public vespa::VaAttachment, public QObject
{
public:
   AttachmentInteraction(vespa::VaEntity&             aEntity,
                         vespa::VaViewer*             aViewerPtr,
                         const std::string&           aName,
                         const InteractionPrefObject* aPrefObjectPtr = nullptr);

   void ShowInteractionOfType(const std::pair<std::string, int>& aType, bool aState);
   bool InteractionsOfTypeAreShown(const std::pair<std::string, int>& aType) const;

   void UpdateFrame(double aTime) override;

   void AddInteraction(const std::pair<std::string, int>& aType,
                       vespa::VaEntity*                   aTarget,
                       const std::string&                 aDisplayText = "",
                       unsigned int                       aId          = 0);
   void RemoveInteraction(const std::pair<std::string, int>& aType, vespa::VaEntity* aTarget, unsigned int aId = 0);

   void UpdateVisibility() override;

   enum
   {
      eINCOMING = 0x1000,
      eOUTGOING = 0x2000
   };

   void SetColor(const std::string& aType, const UtoColor& aColor);
   void SetColor(const std::string& aType, const QColor& aColor);
   void SetArchSegments(bool aState) { mArchSegments = aState; }

   void SetTimeout(double aTimeout);
   void SetDescriptionMode(int aMode);
   void SetLineWidth(unsigned int aWidth);

   void HitSubparts(UtoHitInfoList& aHits, vespa::VaHitList& aSubHits) override;

   unsigned int GetHoverMode() const override { return vespa::VaAttachment::cHOVER_ATTACHMENT; }

   std::string GetDescription(unsigned int aSubId) override;
   void        SetStackingAllowed(bool aState);

   void AddCard(const QString& aName, const QString& aIcon);
   void AddInteractionType(const QString& aName, const QString& aHoverText);

private:
   struct Interaction
   {
      Interaction()
         : mType(std::make_pair("", eINCOMING))
         , mTarget(nullptr)
      {
      }
      Interaction(const std::pair<std::string, int>& aType, vespa::VaEntity* aEntityPtr)
         : mType(aType)
         , mTarget(aEntityPtr)
      {
      }

      bool                        operator<(const Interaction& aRHS) const;
      std::pair<std::string, int> mType;
      vespa::VaEntity*            mTarget;
   };

   void MapProjectionChangedCB(unsigned int aTargetId, const UtoMapProjection* aProjection);

   void        CreateShape();
   UtoColor    LookupColor(const std::string& aType);
   std::string LookupText(const std::string& aType);

   bool DrawLine(const std::string& aType) const;
   bool DrawIcon(const std::string& aType) const;

   void PrivateInitializeCallbacks() override;

   void EntityDeleted(vespa::VaEntity* aEntityPtr);

   std::map<std::pair<std::string, int>, bool> mInteractionStateMap;
   std::string                                 mInteractionShapeName;
   std::string                                 mIconShapeName;
   UtoInteractionShape*                        mInteractionShapePtr;
   UtoIconBoardShape*                          mIconShapePtr;
   bool                                        mChanged;
   bool                                        mDescriptionDetailed;
   bool                                        mDescriptionEnabled;

   using TextMap = std::multimap<unsigned int, std::string>; // maps an identifier to a text string for a given
                                                             // interaction type and target
   std::map<Interaction, TextMap>  mActiveInteractions;      // keeps track of how many interactions are active
   std::map<std::string, UtoColor> mColorMap;

   std::map<unsigned int, std::pair<Interaction, std::string>> mActiveLines;
   unsigned int                                                mLineWidth;
   bool                                                        mStackingAllowed{false};
   bool                                                        mArchSegments{false};
   std::map<std::string, QString>                              mCards;
   std::map<std::string, std::string>                          mAddedInteractionTypes;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentInteraction)

#endif
