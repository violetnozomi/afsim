// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPlatformHistoryWingRibbonData.hpp"

#include "RvEventPipeUtil.hpp"
#include "RvResultPlatform.hpp"
#include "VaAttachmentWingRibbon.hpp"
#include "VaModelDatabase.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "platform_history/WkfPlatformHistoryPrefWidget.hpp"

/*************************************************************************************************
 * @brief   Constructor. Fetches all the necessary information to create a wing ribbon attachment, and attaches it to
 *the platform.
 * @param   aPlatform   The platform to attach the wing ribbon to.
 * @param   aViewerPtr  The viewer that the attachment will be placed in.
 * @date 7/14/2017
 **************************************************************************************************/
RvPlatformHistory::WingRibbonData::WingRibbonData(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr)
   : mPlatformPtr(&aPlatform)
   , mLastUpdateTime(0)
   , mFullUpdateRequired(true)
   , mWingRibbonMainMaxColor{1.0f, 0.0f, 1.0f, 0.5f} // rgba
   , mWingRibbonMainMinColor{0.0f, 1.0f, 1.0f, 0.5f}
   , mWingRibbonEdgeMaxColor{1.0f, 1.0f, 0.0f, 1.0f}
   , mWingRibbonEdgeMinColor{0.0f, 0.0f, 0.0f, 1.0f}
   , mWingRibbonMainMaxFlippedColor{1.0f, 0.0f, 1.0f, 0.4f} // rgba
   , mWingRibbonMainMinFlippedColor{0.0f, 1.0f, 1.0f, 0.4f}
   , mWingRibbonEdgeMaxFlippedColor{1.0f, 1.0f, 0.0f, 0.8f}
   , mWingRibbonEdgeMinFlippedColor{0.0f, 0.0f, 0.0f, 0.8f}
   , mWingRibbonWidthScale(1.0f)
   , mWingRibbonHardEdgeWidth(1.0f)
   , mWingRibbonSoftEdgeWidth(1.2f)
   , mEdgeStateBeingTracked(wkf::eALTITUDE)
   , mMainStateBeingTracked(wkf::eSPEED)
   , mEdgeLimitType(wkf::eINDEFINITE_INDIVIDUAL_LIMITS)
   , mMainLimitType(wkf::eCURRENT_INDIVIDUAL_LIMITS)
   , mHardMaxMainLimit(1)
   , mHardMinMainLimit(0)
   , mHardMaxEdgeLimit(1)
   , mHardMinEdgeLimit(0)
   , mWidthScale(1)
   , mHardEdgeWidth(0.09f)
   , mSoftEdgeWidth(0.12f)
{
   const auto modelDefinitionPtr =
      vespa::VaEnvironment::Instance().GetModelDatabase()->GetModelDefinition(mPlatformPtr->GetIcon());

   float      VA_ScreenScale = 0;
   osg::Node* platformNode   = nullptr;
   if (modelDefinitionPtr != nullptr)
   {
      platformNode =
         vespa::VaEnvironment::Instance().GetModelDatabase()->RequestSharedModel(mPlatformPtr->GetIcon(), aViewerPtr);

      if (platformNode != nullptr)
      {
         auto ss = platformNode->getStateSet();
         if (ss != nullptr)
         {
            auto uni = ss->getUniform("VA_ScreenScale");
            if (uni != nullptr)
            {
               uni->get(VA_ScreenScale);
            }
         }
      }
   }
   if (VA_ScreenScale == 0)
   {
      VA_ScreenScale = 63.159f; // default of F-18
   }

   mWingTipA = {0, 0, 0};
   if (modelDefinitionPtr != nullptr)
   {
      mWingTipA = modelDefinitionPtr->GetWingTip(
         vespa::VaEnvironment::Instance().GetModelDatabase()->GetViewerModelSet(*aViewerPtr));
   }
   if (mWingTipA == osg::Vec3{0, 0, 0})
   {
      mWingTipA = osg::Vec3f{-26, 35, 0};

      if (platformNode != nullptr && platformNode->getBound().radius() > 0.0001)
      {
         // try to get a rough value for where the wingtip is based on bounding sphere
         if (mWingTipA.y() > platformNode->getBound().radius() * 0.9 ||
             -mWingTipA.x() > platformNode->getBound().radius() * 0.7)
         {
            mWingTipA.normalize();
            mWingTipA *= platformNode->getBound().radius() * 0.9;
         }
      }
   }
   mWingTipB     = mWingTipA;
   mWingTipB.y() = -mWingTipB.y(); // flip other wingtip across xz plane

   auto* wingribbon = vespa::make_attachment<vespa::VaAttachmentWingRibbon>(aPlatform, aViewerPtr);
   vespa::VaAttachment::LoadAttachment(*wingribbon);
   wingribbon->SetVA_ScreenScale(VA_ScreenScale);
   wingribbon->SetMainMaxColor(mWingRibbonMainMaxColor);
   wingribbon->SetMainMinColor(mWingRibbonMainMinColor);
   wingribbon->SetEdgeMaxColor(mWingRibbonEdgeMaxColor);
   wingribbon->SetEdgeMinColor(mWingRibbonEdgeMinColor);
   wingribbon->SetMainMaxFlippedColor(mWingRibbonMainMaxFlippedColor);
   wingribbon->SetMainMinFlippedColor(mWingRibbonMainMinFlippedColor);
   wingribbon->SetEdgeMaxFlippedColor(mWingRibbonEdgeMaxFlippedColor);
   wingribbon->SetEdgeMinFlippedColor(mWingRibbonEdgeMinFlippedColor);
   wingribbon->SetEdgeLimitType(static_cast<vespa::VaAttachmentWingRibbon::InterpolationLimitsMethod>(mEdgeLimitType));
   wingribbon->SetMainLimitType(static_cast<vespa::VaAttachmentWingRibbon::InterpolationLimitsMethod>(mMainLimitType));
   wingribbon->SetWidthScale(mWidthScale);
   wingribbon->SetHardEdgeWidth(mHardEdgeWidth);
   wingribbon->SetSoftEdgeWidth(mSoftEdgeWidth);
   if (mMainLimitType == wkf::eHARD_LIMITS)
   {
      wingribbon->SetHardMaxMainLimit(mHardMaxMainLimit);
      wingribbon->SetHardMinMainLimit(mHardMinMainLimit);
   }
   if (mEdgeLimitType == wkf::eHARD_LIMITS)
   {
      wingribbon->SetHardMaxEdgeLimit(mHardMaxEdgeLimit);
      wingribbon->SetHardMinEdgeLimit(mHardMinEdgeLimit);
   }
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      auto teamColor =
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mPlatformPtr->GetSide());
      teamColor[3] = .5; // apply an alpha to the team color
      wingribbon->SetMainMinColor(teamColor);
      wingribbon->SetMainMaxColor(teamColor);
      for (int x = 0; x < 4; x++)
      {
         teamColor[x] *= 0.8f;
      }
      wingribbon->SetMainMinFlippedColor(teamColor);
      wingribbon->SetMainMaxFlippedColor(teamColor);
   }
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      auto teamColor =
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mPlatformPtr->GetSide());
      teamColor[3] = .5; // apply an alpha to the team color
      wingribbon->SetEdgeMinColor(teamColor);
      wingribbon->SetEdgeMaxColor(teamColor);
      for (int x = 0; x < 4; x++)
      {
         teamColor[x] *= 0.8f;
      }
      wingribbon->SetEdgeMinFlippedColor(teamColor);
      wingribbon->SetEdgeMaxFlippedColor(teamColor);
   }

   mWingRibbonUniqueId = wingribbon->GetUniqueId();
}

RvPlatformHistory::WingRibbonData::~WingRibbonData()
{
   mPlatformPtr->RemoveAttachment(mWingRibbonUniqueId);
}

/*************************************************************************************************
 * @brief   Sets main maximum color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mMainStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the mainStateBeingTracked is set to something else.
 * @param   aMainMaxColor  The main maximum color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainMaxColor(const UtColor& aMainMaxColor)
{
   if (mWingRibbonMainMaxColor == aMainMaxColor)
   {
      return;
   }
   mWingRibbonMainMaxColor = {aMainMaxColor};
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetMainMaxColor(mWingRibbonMainMaxColor);
   }
}
/*************************************************************************************************
 * @brief   Sets main minimum color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mMainStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the mainStateBeingTracked is set to something else.
 * @param   aMainMinColor  The main minimum color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainMinColor(const UtColor& aMainMinColor)
{
   if (mWingRibbonMainMinColor == aMainMinColor)
   {
      return;
   }
   mWingRibbonMainMinColor = {aMainMinColor};
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetMainMinColor(mWingRibbonMainMinColor);
   }
}
/*************************************************************************************************
 * @brief   Sets edge maximum color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mEdgeStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the edgeStateBeingTracked is set to something else.
 * @param   aEdgeMaxColor  The edge maximum color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeMaxColor(const UtColor& aEdgeMaxColor)
{
   if (mWingRibbonEdgeMaxColor == aEdgeMaxColor)
   {
      return;
   }
   mWingRibbonEdgeMaxColor = {aEdgeMaxColor};
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetEdgeMaxColor(mWingRibbonEdgeMaxColor);
   }
}
/*************************************************************************************************
 * @brief   Sets edge minimum color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mEdgeStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the edgeStateBeingTracked is set to something else.
 * @param   aEdgeMinColor  The edge minimum color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeMinColor(const UtColor& aEdgeMinColor)
{
   if (mWingRibbonEdgeMinColor == aEdgeMinColor)
   {
      return;
   }
   mWingRibbonEdgeMinColor = {aEdgeMinColor};
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetEdgeMinColor(mWingRibbonEdgeMinColor);
   }
}
/*************************************************************************************************
 * @brief   Sets main maximum flipped color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mMainStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the mainStateBeingTracked is set to something else.
 * @param   aMainMaxFlippedColor The main maximum flipped color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainMaxFlippedColor(const UtColor& aMainMaxFlippedColor)
{
   if (mWingRibbonMainMaxFlippedColor == aMainMaxFlippedColor)
   {
      return;
   }
   mWingRibbonMainMaxFlippedColor = {aMainMaxFlippedColor};
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetMainMaxFlippedColor(mWingRibbonMainMaxFlippedColor);
   }
}
/*************************************************************************************************
 * @brief   Sets main minimum flipped color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mMainStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the mainStateBeingTracked is set to something else.
 * @param   aMainMinFlippedColor The main minimum flipped color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainMinFlippedColor(const UtColor& aMainMinFlippedColor)
{
   if (mWingRibbonMainMinFlippedColor == aMainMinFlippedColor)
   {
      return;
   }
   mWingRibbonMainMinFlippedColor = {aMainMinFlippedColor};
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetMainMinFlippedColor(mWingRibbonMainMinFlippedColor);
   }
}
/*************************************************************************************************
 * @brief   Sets edge maximum flipped color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mEdgeStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the edgeStateBeingTracked is set to something else.
 * @param   aEdgeMaxFlippedColor The edge maximum flipped color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeMaxFlippedColor(const UtColor& aEdgeMaxFlippedColor)
{
   if (mWingRibbonEdgeMaxFlippedColor == aEdgeMaxFlippedColor)
   {
      return;
   }
   mWingRibbonEdgeMaxFlippedColor = {aEdgeMaxFlippedColor};
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetEdgeMaxFlippedColor(mWingRibbonEdgeMaxFlippedColor);
   }
}
/*************************************************************************************************
 * @brief   Sets edge minimum flipped color of the wing ribbon, which is used in modifying how the wing
 *          ribbon looks in response to its recorded state values.
 * @note if mEdgeStateBeingTracked is set to eTEAM_COLOR, this color change will not take effect
 *       until the edgeStateBeingTracked is set to something else.
 * @param   aEdgeMinFlippedColor The edge minimum flipped color.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeMinFlippedColor(const UtColor& aEdgeMinFlippedColor)
{
   if (mWingRibbonEdgeMinFlippedColor == aEdgeMinFlippedColor)
   {
      return;
   }
   mWingRibbonEdgeMinFlippedColor = {aEdgeMinFlippedColor};
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetEdgeMinFlippedColor(mWingRibbonEdgeMinFlippedColor);
   }
}

/*************************************************************************************************
 * @brief   Sets which state value from the platform will be recorded by this wing ribbon for the
 *          purpose of being displayed by the center/main area of the wing ribbon.
 * @param   stateToTrack   The type of state to track.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainStateBeingTracked(wkf::TrackableStateValue stateToTrack)
{
   if (mMainStateBeingTracked == stateToTrack)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         // undo reset of scenario wide min/max values
         mCurrentScenarioMainMax = std::max(mCurrentScenarioMainMax, wingribbon->GetCurrentMainMaxStateVal());
         mCurrentScenarioMainMin = std::min(mCurrentScenarioMainMin, wingribbon->GetCurrentMainMinStateVal());
      }
      return; // nothing changed
   }
   if (mMainStateBeingTracked == wkf::eTEAM_COLOR)
   {
      // changing away from team color setting, reset the min/max colors to what what they should be
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetMainMinColor(mWingRibbonMainMinColor);
         wingribbon->SetMainMaxColor(mWingRibbonMainMaxColor);
         wingribbon->SetMainMinFlippedColor(mWingRibbonMainMinFlippedColor);
         wingribbon->SetMainMaxFlippedColor(mWingRibbonMainMaxFlippedColor);
      }
   }
   mMainStateBeingTracked = stateToTrack;
   if (stateToTrack == wkf::eTEAM_COLOR)
   {
      auto teamColor =
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mPlatformPtr->GetSide());
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         teamColor[3] = .5; // apply an alpha to the team color
         wingribbon->SetMainMinColor(teamColor);
         wingribbon->SetMainMaxColor(teamColor);
         for (unsigned int x = 0; x < 4; x++)
         {
            teamColor[x] *= 0.8f;
         } // make rgba values of team color 20% darker/transparent on flip side
         wingribbon->SetMainMinFlippedColor(teamColor);
         wingribbon->SetMainMaxFlippedColor(teamColor);
      }
   }
}

/*************************************************************************************************
 * @brief   Sets which state value from the platform will be recorded by this wing ribbon for the
 *          purpose of being displayed by the edge area of the wing ribbon.
 * @param   stateToTrack   The type of state to track.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeStateBeingTracked(wkf::TrackableStateValue stateToTrack)
{
   if (mEdgeStateBeingTracked == stateToTrack)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         // undo reset of scenario wide min/max values
         mCurrentScenarioEdgeMax = std::max(mCurrentScenarioEdgeMax, wingribbon->GetCurrentEdgeMaxStateVal());
         mCurrentScenarioEdgeMin = std::min(mCurrentScenarioEdgeMin, wingribbon->GetCurrentEdgeMinStateVal());
      }
      return; // nothing changed
   }
   if (mEdgeStateBeingTracked == wkf::eTEAM_COLOR)
   {
      // changing away from team color setting, reset the min/max colors to what what they should be
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetEdgeMinColor(mWingRibbonEdgeMinColor);
         wingribbon->SetEdgeMaxColor(mWingRibbonEdgeMaxColor);
         wingribbon->SetEdgeMinFlippedColor(mWingRibbonEdgeMinFlippedColor);
         wingribbon->SetEdgeMaxFlippedColor(mWingRibbonEdgeMaxFlippedColor);
      }
   }
   mEdgeStateBeingTracked = stateToTrack;
   if (stateToTrack == wkf::eTEAM_COLOR)
   {
      auto teamColor =
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mPlatformPtr->GetSide());
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         teamColor[3] = .5; // apply an alpha to the team color
         wingribbon->SetEdgeMinColor(teamColor);
         wingribbon->SetEdgeMaxColor(teamColor);
         for (int x = 0; x < 4; ++x)
         {
            teamColor[x] *= 0.8f;
         } // make rgba values of team color 20% darker/transparent on flip side
         wingribbon->SetEdgeMinFlippedColor(teamColor);
         wingribbon->SetEdgeMaxFlippedColor(teamColor);
      }
   }
}

/*************************************************************************************************
 * @brief   Changes how the colors of the wing ribbon's edge area is calculated in response to the
 *          recorded state values.
 * @param   aEdgeLimitType Type of the edge limit that determines how the colors of the wing
 *                         ribbon's main is calculated in response to the recorded state values.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetEdgeLimitType(wkf::InterpolationLimitsMethod aEdgeLimitType)
{
   if (mEdgeLimitType == aEdgeLimitType)
   {
      return; // nothing changed
   }

   mEdgeLimitType = aEdgeLimitType;

   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetEdgeLimitType(static_cast<vespa::VaAttachmentWingRibbon::InterpolationLimitsMethod>(mEdgeLimitType));
      if (mEdgeLimitType == wkf::eHARD_LIMITS)
      {
         wingribbon->SetHardMaxEdgeLimit(mHardMaxEdgeLimit);
         wingribbon->SetHardMinEdgeLimit(mHardMinEdgeLimit);
      }
      else
      {
         if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mEdgeLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
         {
            mCurrentScenarioEdgeMax = std::max(mCurrentScenarioEdgeMax, wingribbon->GetCurrentEdgeMaxStateVal());
            mCurrentScenarioEdgeMin = std::min(mCurrentScenarioEdgeMin, wingribbon->GetCurrentEdgeMinStateVal());
         }
      }
   }
}

/*************************************************************************************************
 * @brief   Changes how the colors of the wing ribbon's main/center area is calculated in response to the
 *          recorded state values.
 * @param   aMainLimitType Type of the main limit that determines how the colors of the wing
 *                         ribbon's main is calculated in response to the recorded state values.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetMainLimitType(wkf::InterpolationLimitsMethod aMainLimitType)
{
   if (mMainLimitType == aMainLimitType)
   {
      return; // nothing changed
   }

   mMainLimitType = aMainLimitType;

   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetMainLimitType(static_cast<vespa::VaAttachmentWingRibbon::InterpolationLimitsMethod>(mMainLimitType));
      if (mMainLimitType == wkf::eHARD_LIMITS)
      {
         wingribbon->SetHardMaxMainLimit(mHardMaxMainLimit);
         wingribbon->SetHardMinMainLimit(mHardMinMainLimit);
      }
      else
      {
         if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mMainLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
         {
            mCurrentScenarioMainMax = std::max(mCurrentScenarioMainMax, wingribbon->GetCurrentMainMaxStateVal());
            mCurrentScenarioMainMin = std::min(mCurrentScenarioMainMin, wingribbon->GetCurrentMainMinStateVal());
         }
      }
   }
}
/// @see RvPluginPlatformHistory::WingRibbonHardMaxMainLimitHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetHardMaxMainLimit(float aHardMaxMainLimit)
{
   mHardMaxMainLimit = aHardMaxMainLimit;
   if (mMainLimitType == wkf::eHARD_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetHardMaxMainLimit(mHardMaxMainLimit);
      }
   }
}
/// @see RvPluginPlatformHistory::WingRibbonHardMinMainLimitHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetHardMinMainLimit(float aHardMinMainLimit)
{
   mHardMinMainLimit = aHardMinMainLimit;
   if (mMainLimitType == wkf::eHARD_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetHardMinMainLimit(aHardMinMainLimit);
      }
   }
}
/// @see RvPluginPlatformHistory::WingRibbonHardMaxEdgeLimitHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetHardMaxEdgeLimit(float aHardMaxEdgeLimit)
{
   mHardMaxEdgeLimit = aHardMaxEdgeLimit;
   if (mEdgeLimitType == wkf::eHARD_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetHardMaxEdgeLimit(aHardMaxEdgeLimit);
      }
   }
}
/// @see RvPluginPlatformHistory::WingRibbonHardMinEdgeLimitHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetHardMinEdgeLimit(float aHardMinEdgeLimit)
{
   mHardMinEdgeLimit = aHardMinEdgeLimit;
   if (mEdgeLimitType == wkf::eHARD_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         wingribbon->SetHardMinEdgeLimit(aHardMinEdgeLimit);
      }
   }
}

/// @see RvPluginPlatformHistory::WingRibbonWidthScaleHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetWidthScale(float aWidthScale)
{
   mWidthScale = aWidthScale;
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetWidthScale(aWidthScale);
   }
}
/// @see RvPluginPlatformHistory::WingRibbonHardEdgeWidthHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetHardEdgeWidth(float aHardEdgeWidth)
{
   mHardEdgeWidth = aHardEdgeWidth;
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetHardEdgeWidth(aHardEdgeWidth);
   }
}
/// @see RvPluginPlatformHistory::WingRibbonSoftEdgeWidthHandler for documented behavior
void RvPlatformHistory::WingRibbonData::SetSoftEdgeWidth(float aSoftEdgeWidth)
{
   mSoftEdgeWidth = aSoftEdgeWidth;
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      wingribbon->SetSoftEdgeWidth(aSoftEdgeWidth);
   }
}

/*************************************************************************************************
 * @brief   Sets the wing ribbon attachment's visibility.
 * @param   aVisible True to show, false to hide.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetVisible(bool aVisible)
{
   vespa::VaAttachment* attachment = mPlatformPtr->FindAttachment(mWingRibbonUniqueId);
   if (attachment != nullptr)
   {
      if (aVisible == false && IsVisible())
      {
         // changing from visible to not visible
         auto wingribbon = dynamic_cast<vespa::VaAttachmentWingRibbon*>(attachment);
         if (wingribbon != nullptr)
         {
            // delete everything in the wingribbon buffer to clear up memory
            wingribbon->PruneBefore(std::numeric_limits<float>::max());
            wingribbon->shrink_to_fit();
            mFullUpdateRequired = true;
         }
      }
      else if (!IsVisible() && aVisible == true)
      {
         mFullUpdateRequired = true;
      }

      attachment->SetStateVisibility(aVisible);
   }
}

/*************************************************************************************************
 * @brief   Query if the wing ribbon attachment is visible.
 * @return  True if visible, false if not.
 * @date 7/14/2017
 **************************************************************************************************/
bool RvPlatformHistory::WingRibbonData::IsVisible() const
{
   vespa::VaAttachment* attachment = mPlatformPtr->FindAttachment(mWingRibbonUniqueId);
   if (attachment != nullptr)
   {
      return attachment->GetStateVisibility();
   }
   return false;
}

/*************************************************************************************************
 * @brief   Updates the wingribbon with new datapoints for where the platform's wing tips were
 *          and what the values of the tracked states were for the platform in the provided ranges.
 *          Also erases old datapoints that fell outside of desired ribbon length.
 * @note in this same call, the static values for min/max scenario value are collected from the
 *       min/max values currently recorded in the wingribbon.
 * @note Datapoints will only be generated if the wing ribbon is visible (for mild performance
 *          reasons), which means all of them will end up being calculated once the wing ribbon
 *          is visible again.
 * @param   aPlatformData  Data describing the recorded history for the platform.
 * @param   aPruneBefore   Timepoint where all recorded records in the wing ribbon before this
 *                         should be removed.
 * @param   aPruneAfter    Timepoint where all recorded records in the wing ribbon after this
 *                         should be removed.
 * @param   aLowRange      Timepoint range that need need datapoints added to the back of the
 *                         wing ribbon.
 * @param   aHighRange     Timepoint range that need need datapoints added to the front of the
 *                         wing ribbon.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::Update(rv::ResultPlatform*            aPlatformData,
                                               float                          aPruneBefore,
                                               float                          aPruneAfter,
                                               const std::pair<float, float>& aLowRange,
                                               const std::pair<float, float>& aHighRange)
{
   if (!IsVisible())
   {
      // don't do all this math if it's not visible, put it off for when the ribbon is visible again
      return;
   }
   vespa::VaAttachmentWingRibbon* wingribbon =
      dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
   if (wingribbon != nullptr)
   {
      aPlatformData->BeginRead();
      if (aPruneAfter > -1.0f)
      {
         wingribbon->PruneAfter(aPruneAfter);
      }
      if (aPruneBefore > -1.0f)
      {
         wingribbon->PruneBefore(aPruneBefore);
      }
      auto states = aPlatformData->GetArray<rv::MsgEntityState>();
      if (states != nullptr)
      {
         if (aHighRange.second > aHighRange.first)
         {
            auto iter = states->FindFirstIteratorAfter(aHighRange.first);
            while (iter != states->end())
            {
               const rv::MsgEntityState& msg = static_cast<rv::MsgEntityState&>(**iter);
               // Problem here, we shouldn't be able to fail this if-test if each state has a distinct time
               // but we are failing.  Skip to avoid a duplicate point
               if ((msg.simTime() > aHighRange.first) && (msg.simTime() < aHighRange.second))
               {
                  rv::Vec3d pos     = msg.state().locationWCS();
                  double    ecef[3] = {pos.x(), pos.y(), pos.z()};

                  const auto& hpr = msg.state().orientationWCS();
                  auto        q =
                     osg::Quat(hpr.z(), osg::Vec3(1, 0, 0), hpr.y(), osg::Vec3(0, 1, 0), hpr.x(), osg::Vec3(0, 0, 1));
                  osg::Matrixf rotation;
                  q.get(rotation);

                  osg::Vec3f wingTipA = mWingTipA;
                  osg::Vec3f wingTipB = mWingTipB;

                  wingTipA = rotation.preMult(wingTipA);
                  wingTipB = rotation.preMult(wingTipB);

                  float stateValEdge = RetrieveStateValFrom(msg, mEdgeStateBeingTracked);
                  float stateValMain = RetrieveStateValFrom(msg, mMainStateBeingTracked);

                  bool isAlive = true;
                  wingribbon->AddPointBack(msg.simTime(), ecef, stateValMain, stateValEdge, wingTipA, wingTipB, isAlive);

                  if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mMainLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
                  {
                     mCurrentScenarioMainMax = std::max(mCurrentScenarioMainMax, wingribbon->GetCurrentMainMaxStateVal());
                     mCurrentScenarioMainMin = std::min(mCurrentScenarioMainMin, wingribbon->GetCurrentMainMinStateVal());
                  }
                  if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mEdgeLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
                  {
                     mCurrentScenarioEdgeMax = std::max(mCurrentScenarioEdgeMax, wingribbon->GetCurrentEdgeMaxStateVal());
                     mCurrentScenarioEdgeMin = std::min(mCurrentScenarioEdgeMin, wingribbon->GetCurrentEdgeMinStateVal());
                  }
               }
               ++iter;
            }
         }

         if (aLowRange.second > aLowRange.first)
         {
            auto iter = states->FindFirstIteratorBefore(aLowRange.second);
            if (iter != states->end())
            {
               while (true)
               {
                  const rv::MsgEntityState& msg = static_cast<rv::MsgEntityState&>(**iter);
                  if (msg.simTime() < aLowRange.first)
                  {
                     break;
                  }
                  rv::Vec3d pos     = msg.state().locationWCS();
                  double    ecef[3] = {pos.x(), pos.y(), pos.z()};

                  const auto& hpr = msg.state().orientationWCS();
                  auto        q =
                     osg::Quat(hpr.z(), osg::Vec3(1, 0, 0), hpr.y(), osg::Vec3(0, 1, 0), hpr.x(), osg::Vec3(0, 0, 1));
                  osg::Matrixf rotation;
                  q.get(rotation);

                  osg::Vec3f wingTipA = mWingTipA;
                  osg::Vec3f wingTipB = mWingTipB;

                  wingTipA = rotation.preMult(wingTipA);
                  wingTipB = rotation.preMult(wingTipB);

                  float stateValEdge = RetrieveStateValFrom(msg, mEdgeStateBeingTracked);
                  float stateValMain = RetrieveStateValFrom(msg, mMainStateBeingTracked);

                  bool isAlive = true;
                  wingribbon->AddPointFront(msg.simTime(), ecef, stateValMain, stateValEdge, wingTipA, wingTipB, isAlive);

                  if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mMainLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
                  {
                     mCurrentScenarioMainMax = std::max(mCurrentScenarioMainMax, wingribbon->GetCurrentMainMaxStateVal());
                     mCurrentScenarioMainMin = std::min(mCurrentScenarioMainMin, wingribbon->GetCurrentMainMinStateVal());
                  }
                  if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mEdgeLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
                  {
                     mCurrentScenarioEdgeMax = std::max(mCurrentScenarioEdgeMax, wingribbon->GetCurrentEdgeMaxStateVal());
                     mCurrentScenarioEdgeMin = std::min(mCurrentScenarioEdgeMin, wingribbon->GetCurrentEdgeMinStateVal());
                  }

                  if (iter == states->begin())
                  {
                     break;
                  }
                  --iter;
               }
            }
         }


         if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mMainLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
         {
            mCurrentScenarioMainMax = std::max(mCurrentScenarioMainMax, wingribbon->GetCurrentMainMaxStateVal());
            mCurrentScenarioMainMin = std::min(mCurrentScenarioMainMin, wingribbon->GetCurrentMainMinStateVal());
         }
         if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mEdgeLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
         {
            mCurrentScenarioEdgeMax = std::max(mCurrentScenarioEdgeMax, wingribbon->GetCurrentEdgeMaxStateVal());
            mCurrentScenarioEdgeMin = std::min(mCurrentScenarioEdgeMin, wingribbon->GetCurrentEdgeMinStateVal());
         }
      }
      aPlatformData->EndRead();
   }
}

float RvPlatformHistory::WingRibbonData::mCurrentScenarioMainMax =
   std::numeric_limits<decltype(mCurrentScenarioMainMax)>::lowest();
float RvPlatformHistory::WingRibbonData::mCurrentScenarioMainMin =
   std::numeric_limits<decltype(mCurrentScenarioMainMin)>::max();
float RvPlatformHistory::WingRibbonData::mCurrentScenarioEdgeMax =
   std::numeric_limits<decltype(mCurrentScenarioEdgeMax)>::lowest();
float RvPlatformHistory::WingRibbonData::mCurrentScenarioEdgeMin =
   std::numeric_limits<decltype(mCurrentScenarioEdgeMin)>::max();

/*************************************************************************************************
 * @brief   Resets the current scenario limits if needed. It is necessary to reset the scenario
 *          limits if the limit type is using current scenario limits instead of indefinite scenario
 *          limits, as not reseting would allow the highest/lowest recorded values to outlive how
 *          long they existed in the scenario's wingribbons.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::ResetCurrentScenarioLimitsIfNeeded()
{
   if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS)
   {
      ResetCurrentScenarioMainLimits();
   }
   if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS)
   {
      ResetCurrentScenarioEdgeLimits();
   }
}

/*************************************************************************************************
 * @brief   Resets the current scenario main limits.
 * @see ResetCurrentScenarioLimitsIfNeeded for further documentation
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::ResetCurrentScenarioMainLimits()
{
   mCurrentScenarioMainMax = std::numeric_limits<decltype(mCurrentScenarioMainMax)>::lowest();
   mCurrentScenarioMainMin = std::numeric_limits<decltype(mCurrentScenarioMainMin)>::max();
}

/*************************************************************************************************
 * @brief   Resets the current scenario edge limits.
 * @see ResetCurrentScenarioLimitsIfNeeded for further documentation
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::ResetCurrentScenarioEdgeLimits()
{
   mCurrentScenarioEdgeMax = std::numeric_limits<decltype(mCurrentScenarioEdgeMax)>::lowest();
   mCurrentScenarioEdgeMin = std::numeric_limits<decltype(mCurrentScenarioEdgeMin)>::max();
}

/*************************************************************************************************
 * @brief   Sets the min/max limits that determine what color to use in response to the
 *          wingribbons recorded values, if using a scenario wide InterpolationLimitsMethod.
 *
 *          This is called on all the wingribbons after all the wingribbons have been updated with
 *          new data points, wherein there may potentially be a newly recorded max/min value which
 *          needs to be propagated to the rest of the wingribbons.
 * @see   WingRibbonData::Update() for further details on how scenario wide values are collected.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::WingRibbonData::SetCurrentScenarioLimitsIfNeeded()
{
   if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mMainLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         if (mMainLimitType == wkf::eCURRENT_SCENARIO_LIMITS)
         {
            wingribbon->SetScenarioMaxMainLimit(mCurrentScenarioMainMax);
            wingribbon->SetScenarioMinMainLimit(mCurrentScenarioMainMin);
         }
         else
         {
            // this if/else isn't necessary, indefinite could setScenario instead of Hard too,
            //  but for efficiency reasons, by setting Hard there won't be as many calls to recalculate min/max in the attachment
            wingribbon->SetHardMaxMainLimit(mCurrentScenarioMainMax);
            wingribbon->SetHardMinMainLimit(mCurrentScenarioMainMin);
         }
      }
   }
   if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS || mEdgeLimitType == wkf::eINDEFINITE_SCENARIO_LIMITS)
   {
      vespa::VaAttachmentWingRibbon* wingribbon =
         dynamic_cast<vespa::VaAttachmentWingRibbon*>(mPlatformPtr->FindAttachment(mWingRibbonUniqueId));
      if (wingribbon != nullptr)
      {
         if (mEdgeLimitType == wkf::eCURRENT_SCENARIO_LIMITS)
         {
            wingribbon->SetScenarioMaxEdgeLimit(mCurrentScenarioEdgeMax);
            wingribbon->SetScenarioMinEdgeLimit(mCurrentScenarioEdgeMin);
         }
         else
         {
            // this if/else isn't necessary, indefinite could setScenario instead of Hard too,
            //  but for efficiency reasons, by setting Hard there won't be as many calls to recalculate min/max in the attachment
            wingribbon->SetHardMaxEdgeLimit(mCurrentScenarioEdgeMax);
            wingribbon->SetHardMinEdgeLimit(mCurrentScenarioEdgeMin);
         }
      }
   }
}

/*************************************************************************************************
 * @brief   Gets yaw, pitch, and roll from the entity state.
 * @param        msg   The Entity state.
 * @param [out]  yaw   The yaw.
 * @param [out]  pitch The pitch.
 * @param [out]  roll  The roll.
 * @date 7/14/2017
 **************************************************************************************************/
static void GetYawPitchRoll(const rv::MsgEntityState& msg, double& yaw, double& pitch, double& roll)
{
   UtEntity    tmpEntity;
   const auto& vec3 = msg.state().orientationWCS();
   tmpEntity.SetOrientationWCS(vec3.x(), vec3.y(), vec3.z());
   tmpEntity.GetOrientationNED(yaw, pitch, roll);
}

static float GetHeading(const rv::MsgEntityState& msg)
{
   double yaw, pitch, roll;
   GetYawPitchRoll(msg, yaw, pitch, roll);
   return yaw;
}
static float GetPitch(const rv::MsgEntityState& msg)
{
   double yaw, pitch, roll;
   GetYawPitchRoll(msg, yaw, pitch, roll);
   return pitch;
}
static float GetRoll(const rv::MsgEntityState& msg)
{
   double yaw, pitch, roll;
   GetYawPitchRoll(msg, yaw, pitch, roll);
   return roll;
}
static float GetAltitude(const rv::MsgEntityState& msg)
{
   UtEntity tmpEntity;
   tmpEntity.SetLocationWCS(toVec3(msg.state().locationWCS()).GetData());
   double lat, lon, alt;
   tmpEntity.GetLocationLLA(lat, lon, alt);
   return alt;
}

/*************************************************************************************************
 * @brief   Retrieves that tracked state value from the entity's state.
 * @param   msg            Pointer to the entity state at that timepoint we want to retrieve the value from.
 * @param   aStateTracked  The type of state being tracked that we want to retrieve from the
 *                         platform.
 * @return  The current value of the platform's state thats being tracked.
 * @date 7/14/2017
 **************************************************************************************************/
float RvPlatformHistory::WingRibbonData::RetrieveStateValFrom(const rv::MsgEntityState& msg,
                                                              wkf::TrackableStateValue  aStateTracked)
{
   float value = 0.5f;

   switch (aStateTracked)
   {
   case wkf::eHEADING:
      value = std::abs(sin(GetHeading(msg)));
      break;
   case wkf::ePITCH:
      value = GetPitch(msg);
      break;
   case wkf::eROLL:
      value = GetRoll(msg);
      break;
   case wkf::eSPEED:
      value =
         osg::Vec3(msg.state().velocityWCS().x(), msg.state().velocityWCS().y(), msg.state().velocityWCS().z()).length();
      break;
   case wkf::eALTITUDE:
      value = GetAltitude(msg);
      break;
   case wkf::eTEAM_COLOR:
      value = 0.5f; // team color is handled through setting min/max color to the team color, any state val will do
      break;
   case wkf::eACCELERATION:
      value =
         osg::Vec3(msg.state().accelerationWCS().x(), msg.state().accelerationWCS().y(), msg.state().accelerationWCS().z())
            .length();
      break;
   case wkf::eTRACK_NONE:
      value = 0.5f; // handled through setting min/max color to the same color, any state val will do
      break;
   }
   return value;
}
