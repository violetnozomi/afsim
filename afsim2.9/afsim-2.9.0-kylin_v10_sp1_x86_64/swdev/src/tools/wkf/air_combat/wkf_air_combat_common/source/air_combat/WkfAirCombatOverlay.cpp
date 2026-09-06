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

#include "WkfAirCombatOverlay.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QObject>
#include <osg/LineWidth>
#include <osg/Stencil>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "VaUtils.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
// Creates and returns 2D Geometry given a list of points, a color, and an OpenGL draw mode.
osg::ref_ptr<osg::Geometry> CreateGeometry(const osg::ref_ptr<osg::Vec2Array> aPoints, const UtColor& aColor, GLenum aDrawMode)
{
   osg::ref_ptr<osg::Geometry> retGeom(new osg::Geometry);

   osg::ref_ptr<osg::DrawArrays> geomArray(new osg::DrawArrays(aDrawMode, 0, aPoints->size()));
   retGeom->setVertexArray(aPoints.get());
   retGeom->addPrimitiveSet(geomArray.get());

   osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
   float                        r, g, b, a;
   aColor.Get(r, g, b, a);
   colorArray->push_back(osg::Vec4(r, g, b, a));
   retGeom->setColorArray(colorArray.get());
   retGeom->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);

   return retGeom;
}

// Creates and returns an osg::Text object.
osg::ref_ptr<osgText::Text> CreateText(const wkf::Point&                   aPosition,
                                       double                              aFontSize,
                                       double                              aScalingFactor,
                                       const osg::ref_ptr<osgText::Font>   aFont,
                                       const osgText::Text::AlignmentType& aAlignment,
                                       const UtColor&                      aColor,
                                       const std::string&                  aText)
{
   osg::ref_ptr<osgText::Text> retText(new osgText::Text);

   retText->setFont(aFont);
   retText->setText(aText);
   retText->setCharacterSize(aFontSize * aScalingFactor);
   retText->setAlignment(aAlignment);

   float r, g, b, a;
   aColor.Get(r, g, b, a);
   retText->setColor(osg::Vec4(r, g, b, a));
   retText->setPosition(osg::Vec3d(aPosition.mX, aPosition.mY, 0));

   return retText;
}

// Alters a given UtColor by the given multiplier.
UtColor AlterColor(const UtColor& aColor, double aMultiplier, bool aUseTransparency = true)
{
   float r, g, b, a;
   aColor.Get(r, g, b, a);

   r = std::min(1.0, r * aMultiplier);
   g = std::min(1.0, g * aMultiplier);
   b = std::min(1.0, b * aMultiplier);

   if (aUseTransparency)
   {
      a = std::min(1.0, a * aMultiplier);
   }

   return UtColor(r, g, b, a);
}
} // namespace

wkf::AirCombatOverlay::AirCombatOverlay(wkf::Platform& aSourcePlatform)
   : VaOverlay("AirCombatOverlay")
   , mShapePtr(nullptr)
   , mGeodePtr(nullptr)
   , mSourcePlatformPtr(&aSourcePlatform)
   , mCurrentPrioritizedThreat(nullptr)
   , mCurrentPrioritizedTarget(nullptr)
   , mCurrentManualTarget(nullptr)
   , mTargetingMode(TargetingMode::cMANUAL)
   , mSourcePlatformDataBlock(new DataBlock())
   , mSourcePlatformEngagementBlock(new DataBlock())
   , mTargetPlatformEngagementBlock(new DataBlock())
   , mTacticalDataBlock(new DataBlock())
   , mMinimumResolution(std::make_pair<int, int>(640, 480))
   , mIsInitialized(false)
{
   SetSelectable(true);

   // Change block colors when team color preferences change.
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           [&]()
           {
              auto* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
              if (teamVisPrefs != nullptr)
              {
                 if (mSourcePlatformPtr != nullptr)
                 {
                    UtColor primary   = teamVisPrefs->GetTeamUtColor(mSourcePlatformPtr->GetSide());
                    UtColor secondary = AlterColor(primary, 0.5);
                    mSourcePlatformDataBlock->Recolor(primary, secondary);
                    mSourcePlatformEngagementBlock->Recolor(primary, secondary);
                 }

                 switch (mTargetingMode)
                 {
                 case wkf::TargetingMode::cMANUAL:
                 {
                    if (mCurrentManualTarget != nullptr)
                    {
                       UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentManualTarget->GetSide());
                       UtColor secondary = AlterColor(primary, 0.5);
                       mTargetPlatformEngagementBlock->Recolor(primary, secondary);
                    }
                    break;
                 }
                 case wkf::TargetingMode::cTARGETS:
                 {
                    if (mCurrentPrioritizedTarget != nullptr)
                    {
                       UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentPrioritizedTarget->GetSide());
                       UtColor secondary = AlterColor(primary, 0.5);
                       mTargetPlatformEngagementBlock->Recolor(primary, secondary);
                    }
                    break;
                 }
                 case wkf::TargetingMode::cTHREATS:
                 {
                    if (mCurrentPrioritizedThreat != nullptr)
                    {
                       UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentPrioritizedThreat->GetSide());
                       UtColor secondary = AlterColor(primary, 0.5);
                       mTargetPlatformEngagementBlock->Recolor(primary, secondary);
                    }
                    break;
                 }
                 default:
                 {
                    break;
                 }
                 }
              }
           });
}

wkf::AirCombatOverlay* wkf::AirCombatOverlay::Clone() const
{
   return new AirCombatOverlay(*mSourcePlatformPtr);
}

bool wkf::AirCombatOverlay::Initialize()
{
   UtoRawShape raw;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape("AirCombatOverlayShape", raw));
   if (mShapePtr != nullptr)
   {
      mIsInitialized = true;
      SetVisible(true);

      mGeodePtr = new osg::Geode();

      mGeodePtr->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
      mGeodePtr->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

      InitializeSourcePlatformDataBlock();
      InitializeSourcePlatformEngagementBlock();
      InitializeTargetPlatformEngagementBlock();
      InitializeTacticalDataBlock();

      Resize();

      mShapePtr->GetTree()->addChild(mGeodePtr);
   }

   return mIsInitialized;
}

// Calls Btn1Click for each data block. If any one of them returns true, this returns true.
bool wkf::AirCombatOverlay::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   bool success = (mSourcePlatformDataBlock->Btn1Click(aMouseX, aMouseY) ||
                   mSourcePlatformEngagementBlock->Btn1Click(aMouseX, aMouseY) ||
                   mTargetPlatformEngagementBlock->Btn1Click(aMouseX, aMouseY));

   return success;
}

// Calls Btn1Down for each data block. If any one of them returns true, this returns true.
bool wkf::AirCombatOverlay::Btn1Down(int aMouseX, int aMouseY, int aState)
{
   bool success = (mSourcePlatformDataBlock->Btn1Down(aMouseX, aMouseY) ||
                   mSourcePlatformEngagementBlock->Btn1Down(aMouseX, aMouseY) ||
                   mTargetPlatformEngagementBlock->Btn1Down(aMouseX, aMouseY));

   return success;
}

// Calls Btn1Click for each data block. If any one of them returns true, this returns true.
bool wkf::AirCombatOverlay::Btn1Up(int aMouseX, int aMouseY, int aState)
{
   bool success =
      (mSourcePlatformDataBlock->Btn1Up(aMouseX, aMouseY) || mSourcePlatformEngagementBlock->Btn1Up(aMouseX, aMouseY) ||
       mTargetPlatformEngagementBlock->Btn1Up(aMouseX, aMouseY));

   return success;
}

// Calls Btn1DblClick for each data block. If any one of them returns true, this returns true.
bool wkf::AirCombatOverlay::Btn1DblClick(int aMouseX, int aMouseY, int aState)
{
   bool success = (mSourcePlatformDataBlock->Btn1DblClick(aMouseX, aMouseY) ||
                   mSourcePlatformEngagementBlock->Btn1DblClick(aMouseX, aMouseY) ||
                   mTargetPlatformEngagementBlock->Btn1DblClick(aMouseX, aMouseY));

   return success;
}

// Calls MouseWheel for each data block. If any one of them returns true, this returns true.
bool wkf::AirCombatOverlay::MouseWheel(int aMouseX, int aMouseY, int aDelta)
{
   bool success = (mSourcePlatformDataBlock->MouseWheel(aMouseX, aMouseY, aDelta) ||
                   mSourcePlatformEngagementBlock->MouseWheel(aMouseX, aMouseY, aDelta) ||
                   mTargetPlatformEngagementBlock->MouseWheel(aMouseX, aMouseY, aDelta));

   return success;
}

// Returns true if the given position is inside any of the data blocks.
bool wkf::AirCombatOverlay::SelectableAt(int aMouseX, int aMouseY) const
{
   bool selectable = (mSourcePlatformDataBlock->SelectableAt(aMouseX, aMouseY) ||
                      mSourcePlatformEngagementBlock->SelectableAt(aMouseX, aMouseY) ||
                      mTargetPlatformEngagementBlock->SelectableAt(aMouseX, aMouseY));

   return selectable;
}

// Clears the geode and redraws each data block.
void wkf::AirCombatOverlay::Update()
{
   if (mIsInitialized)
   {
      mGeodePtr->removeDrawables(0, mGeodePtr->getNumDrawables());

      Resize();

      mSourcePlatformDataBlock->Update();
      mSourcePlatformEngagementBlock->Update();
      mTargetPlatformEngagementBlock->Update();
      mTacticalDataBlock->Update();

      mSourcePlatformDataBlock->Draw(mGeodePtr);
      mSourcePlatformEngagementBlock->Draw(mGeodePtr);
      mTargetPlatformEngagementBlock->Draw(mGeodePtr);
      mTacticalDataBlock->Draw(mGeodePtr);
   }
}

// Repositions the data blocks according to the viewer's current dimensions
void wkf::AirCombatOverlay::Resize()
{
   if (mIsInitialized)
   {
      int pixelRatio = QApplication::desktop()->devicePixelRatio();
      int height     = GetViewer()->GetHeight() / pixelRatio;
      int width      = GetViewer()->GetWidth() / pixelRatio;
      int padding    = 15;

      double widthScale  = static_cast<double>(width) / static_cast<double>(mMinimumResolution.first);
      double heightScale = static_cast<double>(height) / static_cast<double>(mMinimumResolution.second);
      double scalar      = std::min(widthScale, heightScale);

      // Move the source platform data block to the top left corner
      int dataBlockHeight = mSourcePlatformDataBlock->GetHeight() * scalar;
      mSourcePlatformDataBlock->SetPosition(Point(padding, height - dataBlockHeight - padding));
      mSourcePlatformDataBlock->Resize(scalar);

      // Move the source platform engagement block to the bottom left corner
      mSourcePlatformEngagementBlock->SetPosition(Point(padding, padding));
      mSourcePlatformEngagementBlock->Resize(scalar);

      // Move the target platform engagement block to the bottom right corner
      int targetBlockWidth = mTargetPlatformEngagementBlock->GetWidth() * scalar;
      mTargetPlatformEngagementBlock->SetPosition(Point(width - padding - targetBlockWidth, padding));
      mTargetPlatformEngagementBlock->Resize(scalar);

      // Move the tactical data block to the top right corner
      int tacticalDataBlockWidth  = mTargetPlatformEngagementBlock->GetWidth() * scalar;
      int tacticalDataBlockHeight = mTacticalDataBlock->GetHeight() * scalar;
      mTacticalDataBlock->SetPosition(
         Point(width - padding - tacticalDataBlockWidth, height - tacticalDataBlockHeight - padding));
      mTacticalDataBlock->Resize(scalar);
   }
}

// Receives a list of air combat data objects and updates each data block accordingly
void wkf::AirCombatOverlay::UpdateData(const std::vector<wkf::AirCombatDataEngagementSummary>& aDataList)
{
   // If the data list is empty, update each data block with a blank data object
   if (aDataList.empty())
   {
      UpdateSourcePlatformDataBlock(wkf::AirCombatDataEngagementSummary());
      UpdateSourcePlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
      UpdateTargetPlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
      UpdateTacticalDataBlock(wkf::AirCombatDataEngagementSummary());
      mCurrentPrioritizedThreat = nullptr;
      mCurrentPrioritizedTarget = nullptr;
      mCurrentManualTarget      = nullptr;
   }
   else
   {
      for (const auto& dataObj : aDataList)
      {
         if (dataObj.platformData.mPlatformName == mSourcePlatformPtr->GetName())
         {
            // If the targeting mode is set to cTARGET, add each of the source platform's processor-designated targets
            // and remove any targets that were once in the processor's list but are not anymore. If the targeting mode
            // is set to cTHREAT, do the same with the threat list.
            switch (mTargetingMode)
            {
            case TargetingMode::cTARGETS:
            {
               wkf::Scenario* scenario = vaEnv.GetStandardScenario();
               if (scenario != nullptr)
               {
                  // Remove any platforms that are no longer in the processor list.
                  auto notInProcessorList = [&](wkf::Platform* aPlatform)
                  {
                     bool success = true;
                     for (const auto& target : dataObj.engagementData.mPrioritizedTargetNames)
                     {
                        if (aPlatform->GetName() == target)
                        {
                           success = false;
                           break;
                        }
                     }

                     return success;
                  };

                  auto removeTarget =
                     std::find_if(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), notInProcessorList);
                  while (removeTarget != mPrioritizedTargets.end())
                  {
                     if (mCurrentPrioritizedTarget == (*removeTarget))
                     {
                        mCurrentPrioritizedTarget = nullptr;
                        TargetChanged();
                     }

                     mPrioritizedTargets.erase(removeTarget);
                     removeTarget =
                        std::find_if(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), notInProcessorList);
                  }

                  // Add any targets that are in the processor list but not in the prioritized target list.
                  auto notInPrioritizedTargetList = [&](const std::string& aName)
                  {
                     bool success = true;
                     for (const auto& target : mPrioritizedTargets)
                     {
                        if (target->GetName() == aName)
                        {
                           success = false;
                           break;
                        }
                     }

                     return success;
                  };

                  auto addTarget = std::find_if(dataObj.engagementData.mPrioritizedTargetNames.begin(),
                                                dataObj.engagementData.mPrioritizedTargetNames.end(),
                                                notInPrioritizedTargetList);
                  while (addTarget != dataObj.engagementData.mPrioritizedTargetNames.end())
                  {
                     wkf::Platform* target = scenario->FindPlatformByName((*addTarget));
                     if (target != nullptr)
                     {
                        if (mCurrentPrioritizedTarget == nullptr)
                        {
                           mCurrentPrioritizedTarget = target;
                           TargetChanged();
                        }

                        mPrioritizedTargets.emplace_back(target);
                     }

                     addTarget = std::find_if(dataObj.engagementData.mPrioritizedTargetNames.begin(),
                                              dataObj.engagementData.mPrioritizedTargetNames.end(),
                                              notInPrioritizedTargetList);
                  }
               }
               break;
            }
            case TargetingMode::cTHREATS:
            {
               wkf::Scenario* scenario = vaEnv.GetStandardScenario();
               if (scenario != nullptr)
               {
                  // Remove any platforms that are no longer in the processor list.
                  auto notInProcessorList = [&](wkf::Platform* aPlatform)
                  {
                     bool success = true;
                     for (const auto& target : dataObj.engagementData.mPrioritizedThreatNames)
                     {
                        if (aPlatform->GetName() == target)
                        {
                           success = false;
                           break;
                        }
                     }

                     return success;
                  };

                  auto removeThreat =
                     std::find_if(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), notInProcessorList);
                  while (removeThreat != mPrioritizedThreats.end())
                  {
                     if (mCurrentPrioritizedThreat == (*removeThreat))
                     {
                        mCurrentPrioritizedThreat = nullptr;
                        TargetChanged();
                     }

                     mPrioritizedThreats.erase(removeThreat);
                     removeThreat =
                        std::find_if(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), notInProcessorList);
                  }

                  // Add any threats that are in the processor list but not in the prioritized threat list.
                  auto notInPrioritizedThreatList = [&](const std::string& aName)
                  {
                     bool success = true;
                     for (const auto& threat : mPrioritizedThreats)
                     {
                        if (threat->GetName() == aName)
                        {
                           success = false;
                           break;
                        }
                     }

                     return success;
                  };

                  auto addThreat = std::find_if(dataObj.engagementData.mPrioritizedThreatNames.begin(),
                                                dataObj.engagementData.mPrioritizedThreatNames.end(),
                                                notInPrioritizedThreatList);
                  while (addThreat != dataObj.engagementData.mPrioritizedThreatNames.end())
                  {
                     wkf::Platform* threat = scenario->FindPlatformByName((*addThreat));
                     if (threat != nullptr)
                     {
                        if (mCurrentPrioritizedThreat == nullptr)
                        {
                           mCurrentPrioritizedThreat = threat;
                           TargetChanged();
                        }

                        mPrioritizedThreats.emplace_back(threat);
                     }

                     addThreat = std::find_if(dataObj.engagementData.mPrioritizedThreatNames.begin(),
                                              dataObj.engagementData.mPrioritizedThreatNames.end(),
                                              notInPrioritizedThreatList);
                  }
               }
               break;
            }
            case TargetingMode::cMANUAL:
            default:
            {
               break;
            }
            }

            // This data corresponds to our source platform (i.e. the platform we're tethered to)
            UpdateSourcePlatformDataBlock(dataObj);
            UpdateSourcePlatformEngagementBlock(dataObj);
            UpdateTacticalDataBlock(dataObj);

            // If the source platform is the only entry, update the target engagement block with blank data.
            if (aDataList.size() == 1)
            {
               UpdateTargetPlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
            }
         }
         else
         {
            // Check if this data corresponds to the current target.
            switch (mTargetingMode)
            {
            case TargetingMode::cMANUAL:
            {
               if (mCurrentManualTarget != nullptr)
               {
                  if (dataObj.platformData.mPlatformName == mCurrentManualTarget->GetName())
                  {
                     UpdateTargetPlatformEngagementBlock(dataObj);
                  }
               }
               else
               {
                  UpdateTargetPlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
               }
               break;
            }
            case TargetingMode::cTARGETS:
            {
               if (mCurrentPrioritizedTarget != nullptr)
               {
                  if (dataObj.platformData.mPlatformName == mCurrentPrioritizedTarget->GetName())
                  {
                     UpdateTargetPlatformEngagementBlock(dataObj);
                  }
               }
               else
               {
                  UpdateTargetPlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
               }
               break;
            }
            case TargetingMode::cTHREATS:
            {
               if (mCurrentPrioritizedThreat != nullptr)
               {
                  if (dataObj.platformData.mPlatformName == mCurrentPrioritizedThreat->GetName())
                  {
                     UpdateTargetPlatformEngagementBlock(dataObj);
                  }
               }
               else
               {
                  UpdateTargetPlatformEngagementBlock(wkf::AirCombatDataEngagementSummary());
               }
               break;
            }
            }
         }
      }
   }
}

// Adds a target to the appropriate target list if it isn't already there.
// Also sets the current target to aPlatform.
void wkf::AirCombatOverlay::AddTarget(wkf::Platform& aPlatform)
{
   if (std::find(mManualTargets.begin(), mManualTargets.end(), &aPlatform) == mManualTargets.end())
   {
      mCurrentManualTarget = &aPlatform;
      mManualTargets.emplace_back(&aPlatform);
   }

   TargetChanged();
}

// Removes a target from the target list. If the removed target is the current target,
// the current target is set to nullptr.
void wkf::AirCombatOverlay::RemoveTarget(wkf::Platform& aPlatform)
{
   auto foundTarget = std::find(mManualTargets.begin(), mManualTargets.end(), &aPlatform);
   if (foundTarget != mManualTargets.end())
   {
      if (mCurrentManualTarget == &aPlatform)
      {
         // Revert the manual target to whichever target came before it
         // (or wrap around if it's the first target).
         if (foundTarget != mManualTargets.begin())
         {
            mCurrentManualTarget = (*std::prev(foundTarget));
         }
         else
         {
            mCurrentManualTarget = (*std::prev(mManualTargets.end()));
         }
      }
      mManualTargets.erase(foundTarget);

      // If the target list is empty now, set the current target to nullptr.
      if (mManualTargets.empty())
      {
         mCurrentManualTarget = nullptr;
      }
   }

   TargetChanged();
}

// Initializes the source platform data block by creating the necessary data objects.
void wkf::AirCombatOverlay::InitializeSourcePlatformDataBlock()
{
   // Initialize the outer block.
   mSourcePlatformDataBlock->SetWidth(300);
   mSourcePlatformDataBlock->SetHeight(160);

   // Add the platform name box.
   std::unique_ptr<DataBlock> platformNameBox = ut::make_unique<DataBlock>();
   platformNameBox->SetWidth(300);
   platformNameBox->SetHeight(20);
   platformNameBox->SetOffset(Point(0, 140));

   std::unique_ptr<Label> platformNameLabel = ut::make_unique<Label>();
   platformNameLabel->SetOffset(Point(5, platformNameBox->GetHeight() / 2));
   platformNameLabel->SetFontSize(15);
   platformNameLabel->SetText("PLATFORM:");
   platformNameBox->AddObject("platformNameLabel", std::move(platformNameLabel));

   std::unique_ptr<Label> platformName = ut::make_unique<Label>();
   platformName->SetOffset(Point(90, platformNameBox->GetHeight() / 2));
   platformName->SetFontSize(15);
   platformName->SetText(mSourcePlatformPtr->GetName());
   platformNameBox->AddObject("platformName", std::move(platformName));

   mSourcePlatformDataBlock->AddObject("platformNameBox", std::move(platformNameBox));

   // Add the platform side box.
   std::unique_ptr<DataBlock> platformSideBox = ut::make_unique<DataBlock>();
   platformSideBox->SetWidth(130);
   platformSideBox->SetHeight(15);
   platformSideBox->SetOffset(Point(0, 125));

   std::unique_ptr<Label> platformSideLabel = ut::make_unique<Label>();
   platformSideLabel->SetOffset(Point(7, platformSideBox->GetHeight() / 2));
   platformSideLabel->SetFontSize(13);
   platformSideLabel->SetText("SIDE:");
   platformSideBox->AddObject("platformSideLabel", std::move(platformSideLabel));

   std::unique_ptr<Label> platformSide = ut::make_unique<Label>();
   platformSide->SetOffset(Point(50, platformSideBox->GetHeight() / 2));
   platformSide->SetFontSize(13);
   std::string sideStr = mSourcePlatformPtr->GetSide();
   std::transform(sideStr.begin(), sideStr.end(), sideStr.begin(), toupper);
   int len = sideStr.size();
   if (len > 8)
   {
      sideStr.resize(8);
   }
   platformSide->SetText(sideStr);
   platformSideBox->AddObject("platformSide", std::move(platformSide));
   mSourcePlatformDataBlock->AddObject("platformSideBox", std::move(platformSideBox));

   // Add the platform type box.
   std::unique_ptr<DataBlock> platformTypeBox = ut::make_unique<DataBlock>();
   platformTypeBox->SetWidth(170);
   platformTypeBox->SetHeight(15);
   platformTypeBox->SetOffset(Point(130, 125));

   std::unique_ptr<Label> platformTypeLabel = ut::make_unique<Label>();
   platformTypeLabel->SetOffset(Point(7, platformTypeBox->GetHeight() / 2));
   platformTypeLabel->SetFontSize(13);
   platformTypeLabel->SetText("TYPE:");
   platformTypeBox->AddObject("platformTypeLabel", std::move(platformTypeLabel));

   std::unique_ptr<Label> platformType = ut::make_unique<Label>();
   platformType->SetOffset(Point(50, platformTypeBox->GetHeight() / 2));
   platformType->SetFontSize(13);
   platformType->SetText("Unknown");
   platformTypeBox->AddObject("platformType", std::move(platformType));

   mSourcePlatformDataBlock->AddObject("platformTypeBox", std::move(platformTypeBox));

   // Add the kinematic data.
   std::unique_ptr<DataBlock> kinematicDataBox = ut::make_unique<DataBlock>();
   kinematicDataBox->SetWidth(170);
   kinematicDataBox->SetHeight(125);
   kinematicDataBox->SetOffset(Point(0, 0));

   std::unique_ptr<Label> stateLabel = ut::make_unique<Label>();
   stateLabel->SetOffset(Point(5, 110));
   stateLabel->SetFontSize(15);
   stateLabel->SetText("STATE");
   kinematicDataBox->AddObject("stateLabel", std::move(stateLabel));

   std::unique_ptr<Label> KCASLabel = ut::make_unique<Label>();
   KCASLabel->SetOffset(Point(7, 95));
   KCASLabel->SetFontSize(10);
   KCASLabel->SetText("KCAS:");
   kinematicDataBox->AddObject("KCASLabel", std::move(KCASLabel));

   std::unique_ptr<Label> KCAS = ut::make_unique<Label>();
   KCAS->SetOffset(Point(80, 95));
   KCAS->SetFontSize(10);
   KCAS->SetText("XXXX");
   KCAS->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("KCAS", std::move(KCAS));

   std::unique_ptr<Label> KTASLabel = ut::make_unique<Label>();
   KTASLabel->SetOffset(Point(7, 85));
   KTASLabel->SetFontSize(10);
   KTASLabel->SetText("KTAS:");
   kinematicDataBox->AddObject("KTASLabel", std::move(KTASLabel));

   std::unique_ptr<Label> KTAS = ut::make_unique<Label>();
   KTAS->SetOffset(Point(80, 85));
   KTAS->SetFontSize(10);
   KTAS->SetText("XXXX");
   KTAS->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("KTAS", std::move(KTAS));

   std::unique_ptr<Label> machLabel = ut::make_unique<Label>();
   machLabel->SetOffset(Point(7, 75));
   machLabel->SetFontSize(10);
   machLabel->SetText("MACH:");
   kinematicDataBox->AddObject("machLabel", std::move(machLabel));

   std::unique_ptr<Label> mach = ut::make_unique<Label>();
   mach->SetOffset(Point(80, 75));
   mach->SetFontSize(10);
   mach->SetText("XXXX");
   mach->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("mach", std::move(mach));

   std::unique_ptr<Label> alphaLabel = ut::make_unique<Label>();
   alphaLabel->SetOffset(Point(7, 55));
   alphaLabel->SetFontSize(10);
   alphaLabel->SetText("ALPHA:");
   kinematicDataBox->AddObject("alphaLabel", std::move(alphaLabel));

   std::unique_ptr<Label> alpha = ut::make_unique<Label>();
   alpha->SetOffset(Point(80, 55));
   alpha->SetFontSize(10);
   alpha->SetText("XXXX");
   alpha->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("alpha", std::move(alpha));

   std::unique_ptr<Label> gLoadLabel = ut::make_unique<Label>();
   gLoadLabel->SetOffset(Point(7, 45));
   gLoadLabel->SetFontSize(10);
   gLoadLabel->SetText("G:");
   kinematicDataBox->AddObject("gLoadLabel", std::move(gLoadLabel));

   std::unique_ptr<Label> gLoad = ut::make_unique<Label>();
   gLoad->SetOffset(Point(80, 45));
   gLoad->SetFontSize(10);
   gLoad->SetText("XXXX");
   gLoad->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("gLoad", std::move(gLoad));

   std::unique_ptr<Label> availGsLabel = ut::make_unique<Label>();
   availGsLabel->SetOffset(Point(7, 35));
   availGsLabel->SetFontSize(10);
   availGsLabel->SetText("AVAIL G:");
   kinematicDataBox->AddObject("availGsLabel", std::move(availGsLabel));

   std::unique_ptr<Label> availGs = ut::make_unique<Label>();
   availGs->SetOffset(Point(80, 35));
   availGs->SetFontSize(10);
   availGs->SetText("XXXX");
   availGs->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("availGs", std::move(availGs));

   std::unique_ptr<Label> altitudeLabel = ut::make_unique<Label>();
   altitudeLabel->SetOffset(Point(90, 95));
   altitudeLabel->SetFontSize(10);
   altitudeLabel->SetText("ALT:");
   kinematicDataBox->AddObject("altitudeLabel", std::move(altitudeLabel));

   std::unique_ptr<Label> altitude = ut::make_unique<Label>();
   altitude->SetOffset(Point(160, 95));
   altitude->SetFontSize(10);
   altitude->SetText("XXXX");
   altitude->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("altitude", std::move(altitude));

   std::unique_ptr<Label> vertSpeedLabel = ut::make_unique<Label>();
   vertSpeedLabel->SetOffset(Point(90, 85));
   vertSpeedLabel->SetFontSize(10);
   vertSpeedLabel->SetText("VSPD:");
   kinematicDataBox->AddObject("vertSpeedLabel", std::move(vertSpeedLabel));

   std::unique_ptr<Label> vertSpeed = ut::make_unique<Label>();
   vertSpeed->SetOffset(Point(160, 85));
   vertSpeed->SetFontSize(10);
   vertSpeed->SetText("XXXX");
   vertSpeed->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("vertSpeed", std::move(vertSpeed));

   std::unique_ptr<Label> headingLabel = ut::make_unique<Label>();
   headingLabel->SetOffset(Point(90, 55));
   headingLabel->SetFontSize(10);
   headingLabel->SetText("HDG:");
   kinematicDataBox->AddObject("headingLabel", std::move(headingLabel));

   std::unique_ptr<Label> heading = ut::make_unique<Label>();
   heading->SetOffset(Point(160, 55));
   heading->SetFontSize(10);
   heading->SetText("XXXX");
   heading->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("heading", std::move(heading));

   std::unique_ptr<Label> pitchLabel = ut::make_unique<Label>();
   pitchLabel->SetOffset(Point(90, 45));
   pitchLabel->SetFontSize(10);
   pitchLabel->SetText("PIT:");
   kinematicDataBox->AddObject("pitchLabel", std::move(pitchLabel));

   std::unique_ptr<Label> pitch = ut::make_unique<Label>();
   pitch->SetOffset(Point(160, 45));
   pitch->SetFontSize(10);
   pitch->SetText("XXXX");
   pitch->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("pitch", std::move(pitch));

   std::unique_ptr<Label> rollLabel = ut::make_unique<Label>();
   rollLabel->SetOffset(Point(90, 35));
   rollLabel->SetFontSize(10);
   rollLabel->SetText("ROLL:");
   kinematicDataBox->AddObject("rollLabel", std::move(rollLabel));

   std::unique_ptr<Label> roll = ut::make_unique<Label>();
   roll->SetOffset(Point(160, 35));
   roll->SetFontSize(10);
   roll->SetText("XXXX");
   roll->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   kinematicDataBox->AddObject("roll", std::move(roll));

   std::unique_ptr<ProgressBar> gLoadBar = ut::make_unique<ProgressBar>();
   gLoadBar->SetOffset(Point(7, 15));
   gLoadBar->SetHeight(7);
   gLoadBar->SetWidth(156);
   gLoadBar->SetFillColor(UtColor(0.8, 0.8, 0.8, 1.0));
   gLoadBar->SetLineColor(UtColor(0.8, 0.8, 0.8, 1.0));
   gLoadBar->SetValue(0);
   gLoadBar->AddTickMark("0G", 0.5, 1);
   kinematicDataBox->AddObject("gLoadBar", std::move(gLoadBar));

   mSourcePlatformDataBlock->AddObject("kinematicDataBox", std::move(kinematicDataBox));

   // Add the fuel box.
   std::unique_ptr<DataBlock> fuelDataBox = ut::make_unique<DataBlock>();
   fuelDataBox->SetWidth(130);
   fuelDataBox->SetHeight(125);
   fuelDataBox->SetOffset(Point(170, 0));

   std::unique_ptr<Label> fuelLabel = ut::make_unique<Label>();
   fuelLabel->SetOffset(Point(5, 110));
   fuelLabel->SetFontSize(15);
   fuelLabel->SetText("FUEL");
   fuelDataBox->AddObject("fuelLabel", std::move(fuelLabel));

   std::unique_ptr<Label> internalFuelLabel = ut::make_unique<Label>();
   internalFuelLabel->SetOffset(Point(7, 95));
   internalFuelLabel->SetFontSize(10);
   internalFuelLabel->SetText("INTERNAL:");
   fuelDataBox->AddObject("internalFuelLabel", std::move(internalFuelLabel));

   std::unique_ptr<Label> internalFuel = ut::make_unique<Label>();
   internalFuel->SetOffset(Point(120, 95));
   internalFuel->SetFontSize(10);
   internalFuel->SetText("XXXX");
   internalFuel->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   fuelDataBox->AddObject("internalFuel", std::move(internalFuel));

   std::unique_ptr<Label> externalFuelLabel = ut::make_unique<Label>();
   externalFuelLabel->SetOffset(Point(7, 85));
   externalFuelLabel->SetFontSize(10);
   externalFuelLabel->SetText("EXTERNAL:");
   fuelDataBox->AddObject("externalFuelLabel", std::move(externalFuelLabel));

   std::unique_ptr<Label> externalFuel = ut::make_unique<Label>();
   externalFuel->SetOffset(Point(120, 85));
   externalFuel->SetFontSize(10);
   externalFuel->SetText("XXXX");
   externalFuel->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   fuelDataBox->AddObject("externalFuel", std::move(externalFuel));

   std::unique_ptr<Label> totalFuelLabel = ut::make_unique<Label>();
   totalFuelLabel->SetOffset(Point(7, 75));
   totalFuelLabel->SetFontSize(10);
   totalFuelLabel->SetText("TOTAL:");
   fuelDataBox->AddObject("totalFuelLabel", std::move(totalFuelLabel));

   std::unique_ptr<Label> totalFuel = ut::make_unique<Label>();
   totalFuel->SetOffset(Point(120, 75));
   totalFuel->SetFontSize(10);
   totalFuel->SetText("XXXX");
   totalFuel->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   fuelDataBox->AddObject("totalFuel", std::move(totalFuel));

   std::unique_ptr<Label> bingoLabel = ut::make_unique<Label>();
   bingoLabel->SetOffset(Point(7, 55));
   bingoLabel->SetFontSize(10);
   bingoLabel->SetText("BINGO:");
   fuelDataBox->AddObject("bingoLabel", std::move(bingoLabel));

   std::unique_ptr<Label> bingo = ut::make_unique<Label>();
   bingo->SetOffset(Point(120, 55));
   bingo->SetFontSize(10);
   bingo->SetText("XXXX");
   bingo->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   fuelDataBox->AddObject("bingo", std::move(bingo));

   std::unique_ptr<Label> jokerLabel = ut::make_unique<Label>();
   jokerLabel->SetOffset(Point(7, 45));
   jokerLabel->SetFontSize(10);
   jokerLabel->SetText("JOKER:");
   fuelDataBox->AddObject("jokerLabel", std::move(jokerLabel));

   std::unique_ptr<Label> joker = ut::make_unique<Label>();
   joker->SetOffset(Point(120, 45));
   joker->SetFontSize(10);
   joker->SetText("XXXX");
   joker->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   fuelDataBox->AddObject("joker", std::move(joker));

   std::unique_ptr<ProgressBar> fuelBar = ut::make_unique<ProgressBar>();
   fuelBar->SetOffset(Point(7, 25));
   fuelBar->SetHeight(7);
   fuelBar->SetWidth(116);
   fuelBar->SetFillColor(UtColor(0.8, 0.8, 0.8, 1.0));
   fuelBar->SetLineColor(UtColor(0.8, 0.8, 0.8, 1.0));
   fuelBar->SetValue(0);
   fuelBar->AddTickMark("Bingo", 0.0, 1);
   fuelBar->AddTickMark("Joker", 0.0, 2);
   fuelDataBox->AddObject("fuelBar", std::move(fuelBar));

   mSourcePlatformDataBlock->AddObject("fuelDataBox", std::move(fuelDataBox));
   mSourcePlatformDataBlock->Recolor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));

   auto fuelBox = mSourcePlatformDataBlock->GetObject("fuelDataBox");
   fuelBox->SetColor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.0, 0.0, 0.0, 0.0));
   auto kinematicBox = mSourcePlatformDataBlock->GetObject("kinematicDataBox");
   kinematicBox->SetColor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.0, 0.0, 0.0, 0.0));
}

// Initializes the source platform engagement block by creating the necessary data objects.
void wkf::AirCombatOverlay::InitializeSourcePlatformEngagementBlock()
{
   UtColor primaryColor(0.3, 0.6, 1.0, 1.0);
   UtColor secondaryColor(0.0, 0.125, 0.25, 0.5);
   SetSourceColors(primaryColor, secondaryColor);

   // Initialize the outer block.
   mSourcePlatformEngagementBlock->SetWidth(300);
   mSourcePlatformEngagementBlock->SetHeight(150);

   // Add the platform name box.
   std::unique_ptr<DataBlock> platformNameBox = ut::make_unique<DataBlock>();
   platformNameBox->SetWidth(300);
   platformNameBox->SetHeight(20);
   platformNameBox->SetOffset(Point(0, 130));

   std::unique_ptr<Label> platformNameLabel = ut::make_unique<Label>();
   platformNameLabel->SetOffset(Point(5, platformNameBox->GetHeight() / 2));
   platformNameLabel->SetFontSize(15);
   platformNameLabel->SetText("PLATFORM:");
   platformNameBox->AddObject("platformNameLabel", std::move(platformNameLabel));

   std::unique_ptr<Label> platformName = ut::make_unique<Label>();
   platformName->SetOffset(Point(90, platformNameBox->GetHeight() / 2));
   platformName->SetFontSize(15);
   platformName->SetText(mSourcePlatformPtr->GetName());
   platformNameBox->AddObject("platformName", std::move(platformName));

   mSourcePlatformEngagementBlock->AddObject("platformNameBox", std::move(platformNameBox));

   // Add the target name box.
   std::unique_ptr<DataBlock> targetNameBox = ut::make_unique<DataBlock>();
   targetNameBox->SetWidth(300);
   targetNameBox->SetHeight(20);
   targetNameBox->SetOffset(Point(0, 110));

   std::unique_ptr<Label> targetNameLabel = ut::make_unique<Label>();
   targetNameLabel->SetOffset(Point(5, targetNameBox->GetHeight() / 2));
   targetNameLabel->SetFontSize(15);
   targetNameLabel->SetText("TARGET:");
   targetNameBox->AddObject("targetNameLabel", std::move(targetNameLabel));

   std::unique_ptr<Label> targetName = ut::make_unique<Label>();
   targetName->SetOffset(Point(90, targetNameBox->GetHeight() / 2));
   targetName->SetFontSize(15);
   targetName->SetText("N/A");
   targetNameBox->AddObject("targetName", std::move(targetName));

   mSourcePlatformEngagementBlock->AddObject("targetNameBox", std::move(targetNameBox));

   // Add various engagement data.
   std::unique_ptr<Label> angleOffLabel = ut::make_unique<Label>();
   angleOffLabel->SetOffset(Point(5, 100));
   angleOffLabel->SetFontSize(10);
   angleOffLabel->SetText("ANGLE OFF:");
   mSourcePlatformEngagementBlock->AddObject("angleOffLabel", std::move(angleOffLabel));

   std::unique_ptr<Label> angleOff = ut::make_unique<Label>();
   angleOff->SetOffset(Point(135, 100));
   angleOff->SetFontSize(10);
   angleOff->SetText("XXXX");
   angleOff->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("angleOff", std::move(angleOff));

   std::unique_ptr<Label> aspectLabel = ut::make_unique<Label>();
   aspectLabel->SetOffset(Point(5, 90));
   aspectLabel->SetFontSize(10);
   aspectLabel->SetText("ASPECT:");
   mSourcePlatformEngagementBlock->AddObject("aspectLabel", std::move(aspectLabel));

   std::unique_ptr<Label> aspect = ut::make_unique<Label>();
   aspect->SetOffset(Point(135, 90));
   aspect->SetFontSize(10);
   aspect->SetText("XXXX");
   aspect->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("aspect", std::move(aspect));

   std::unique_ptr<Label> trackQualityLabel = ut::make_unique<Label>();
   trackQualityLabel->SetOffset(Point(5, 70));
   trackQualityLabel->SetFontSize(10);
   trackQualityLabel->SetText("TRK QUAL:");
   mSourcePlatformEngagementBlock->AddObject("trackQualityLabel", std::move(trackQualityLabel));

   std::unique_ptr<Label> trackQuality = ut::make_unique<Label>();
   trackQuality->SetOffset(Point(135, 70));
   trackQuality->SetFontSize(10);
   trackQuality->SetText("XXXX");
   trackQuality->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("trackQuality", std::move(trackQuality));

   std::unique_ptr<Label> rangeLabel = ut::make_unique<Label>();
   rangeLabel->SetOffset(Point(150, 100));
   rangeLabel->SetFontSize(10);
   rangeLabel->SetText("RANGE:");
   mSourcePlatformEngagementBlock->AddObject("rangeLabel", std::move(rangeLabel));

   std::unique_ptr<Label> range = ut::make_unique<Label>();
   range->SetOffset(Point(290, 100));
   range->SetFontSize(10);
   range->SetText("XXXX");
   range->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("range", std::move(range));

   std::unique_ptr<Label> altDiffLabel = ut::make_unique<Label>();
   altDiffLabel->SetOffset(Point(150, 90));
   altDiffLabel->SetFontSize(10);
   altDiffLabel->SetText("DEL-ALT:");
   mSourcePlatformEngagementBlock->AddObject("altDiffLabel", std::move(altDiffLabel));

   std::unique_ptr<Label> altDiff = ut::make_unique<Label>();
   altDiff->SetOffset(Point(290, 90));
   altDiff->SetFontSize(10);
   altDiff->SetText("XXXX");
   altDiff->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("altDiff", std::move(altDiff));

   std::unique_ptr<Label> selectedWeaponLabel = ut::make_unique<Label>();
   selectedWeaponLabel->SetOffset(Point(150, 70));
   selectedWeaponLabel->SetFontSize(10);
   selectedWeaponLabel->SetText("WPN SEL:");
   mSourcePlatformEngagementBlock->AddObject("selectedWeaponLabel", std::move(selectedWeaponLabel));

   std::unique_ptr<Label> selectedWeapon = ut::make_unique<Label>();
   selectedWeapon->SetOffset(Point(290, 70));
   selectedWeapon->SetFontSize(10);
   selectedWeapon->SetText("XXXX");
   selectedWeapon->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("selectedWeapon", std::move(selectedWeapon));

   // Add the detection label/progress bar.
   std::unique_ptr<ProgressBar> detectionBar = ut::make_unique<ProgressBar>();
   detectionBar->SetOffset(Point(120, 50));
   detectionBar->SetHeight(7);
   detectionBar->SetWidth(170);
   detectionBar->SetFillColor(primaryColor);
   detectionBar->SetLineColor(primaryColor);
   detectionBar->SetValue(0);
   int detectionBarYCenter = detectionBar->GetOffset().mY + detectionBar->GetHeight() / 2;
   mSourcePlatformEngagementBlock->AddObject("detectionBar", std::move(detectionBar));

   std::unique_ptr<Label> detectionLabel = ut::make_unique<Label>();
   detectionLabel->SetOffset(Point(5, detectionBarYCenter));
   detectionLabel->SetFontSize(10);
   detectionLabel->SetText("DETECT:");
   mSourcePlatformEngagementBlock->AddObject("detectionLabel", std::move(detectionLabel));

   std::unique_ptr<Label> detection = ut::make_unique<Label>();
   detection->SetOffset(Point(110, detectionBarYCenter));
   detection->SetFontSize(10);
   detection->SetText("XXXX");
   detection->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("detection", std::move(detection));

   // Add the WEZ labels/progress bar.
   std::unique_ptr<ProgressBar> WEZBar = ut::make_unique<ProgressBar>();
   WEZBar->SetOffset(Point(120, 30));
   WEZBar->SetHeight(7);
   WEZBar->SetWidth(170);
   WEZBar->SetFillColor(primaryColor);
   WEZBar->SetLineColor(primaryColor);
   WEZBar->SetValue(0);
   WEZBar->AddTickMark("Rmin", 0.0, 1);
   WEZBar->EnableTickMark("Rmin", false);
   WEZBar->AddTickMark("Rmax", 0.0, 3);
   WEZBar->EnableTickMark("Rmax", false);
   WEZBar->AddTickMark("Rne", 0.0, 2);
   WEZBar->EnableTickMark("Rne", false);
   int wezBarYCenter = WEZBar->GetOffset().mY + WEZBar->GetHeight() / 2;
   mSourcePlatformEngagementBlock->AddObject("WEZBar", std::move(WEZBar));

   std::unique_ptr<Label> WEZMaxLabel = ut::make_unique<Label>();
   WEZMaxLabel->SetOffset(Point(5, wezBarYCenter));
   WEZMaxLabel->SetFontSize(10);
   WEZMaxLabel->SetText("WEZ-RMAX:");
   mSourcePlatformEngagementBlock->AddObject("WEZMaxLabel", std::move(WEZMaxLabel));

   std::unique_ptr<Label> WEZMax = ut::make_unique<Label>();
   WEZMax->SetOffset(Point(110, wezBarYCenter));
   WEZMax->SetFontSize(10);
   WEZMax->SetText("XXXX");
   WEZMax->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("WEZMax", std::move(WEZMax));

   std::unique_ptr<Label> WEZNELabel = ut::make_unique<Label>();
   WEZNELabel->SetOffset(Point(5, wezBarYCenter - 10));
   WEZNELabel->SetFontSize(10);
   WEZNELabel->SetText("WEZ-RNE:");
   mSourcePlatformEngagementBlock->AddObject("WEZNELabel", std::move(WEZNELabel));

   std::unique_ptr<Label> WEZNE = ut::make_unique<Label>();
   WEZNE->SetOffset(Point(110, wezBarYCenter - 10));
   WEZNE->SetFontSize(10);
   WEZNE->SetText("XXXX");
   WEZNE->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("WEZNE", std::move(WEZNE));

   std::unique_ptr<Label> WEZMinLabel = ut::make_unique<Label>();
   WEZMinLabel->SetOffset(Point(5, wezBarYCenter - 20));
   WEZMinLabel->SetFontSize(10);
   WEZMinLabel->SetText("WEZ-RMIN:");
   mSourcePlatformEngagementBlock->AddObject("WEZMinLabel", std::move(WEZMinLabel));

   std::unique_ptr<Label> WEZMin = ut::make_unique<Label>();
   WEZMin->SetOffset(Point(110, wezBarYCenter - 20));
   WEZMin->SetFontSize(10);
   WEZMin->SetText("XXXX");
   WEZMin->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mSourcePlatformEngagementBlock->AddObject("WEZMin", std::move(WEZMin));

   // Set colors based on team colors
   mSourcePlatformEngagementBlock->Recolor(primaryColor, secondaryColor);
}

// Initializes the target platform engagement block by creating the necessary data objects.
void wkf::AirCombatOverlay::InitializeTargetPlatformEngagementBlock()
{
   UtColor primaryColor(1.0, 1.0, 1.0, 1.0);
   UtColor secondaryColor(0.1, 0.1, 0.1, 0.5);
   SetTargetColors(primaryColor, secondaryColor);

   // Initialize the outer block.
   mTargetPlatformEngagementBlock->SetWidth(300);
   mTargetPlatformEngagementBlock->SetHeight(150);

   // Add the manual target mode button.
   std::unique_ptr<Button> manualModeButton = ut::make_unique<Button>();
   manualModeButton->SetOffset(Point(265, 155));
   manualModeButton->SetWidth(35);
   manualModeButton->SetHeight(15);
   manualModeButton->SetFontSize(8);
   manualModeButton->SetSelectable(true);
   manualModeButton->SetText("MANUAL");

   // Add the target mode button.
   std::unique_ptr<Button> targetModeButton = ut::make_unique<Button>();
   targetModeButton->SetOffset(Point(225, 155));
   targetModeButton->SetWidth(35);
   targetModeButton->SetHeight(15);
   targetModeButton->SetFontSize(8);
   targetModeButton->SetSelectable(true);
   targetModeButton->SetText("TARGETS");

   // Add the threat mode button.
   std::unique_ptr<Button> threatModeButton = ut::make_unique<Button>();
   threatModeButton->SetOffset(Point(185, 155));
   threatModeButton->SetWidth(35);
   threatModeButton->SetHeight(15);
   threatModeButton->SetFontSize(8);
   threatModeButton->SetSelectable(true);
   threatModeButton->SetText("THREATS");

   switch (mTargetingMode)
   {
   case TargetingMode::cMANUAL:
   {
      manualModeButton->SetSelected(true);
      targetModeButton->SetSelected(false);
      threatModeButton->SetSelected(false);
      break;
   }
   case TargetingMode::cTARGETS:
   {
      targetModeButton->SetSelected(true);
      manualModeButton->SetSelected(false);
      threatModeButton->SetSelected(false);
      break;
   }
   case TargetingMode::cTHREATS:
   {
      threatModeButton->SetSelected(true);
      manualModeButton->SetSelected(false);
      targetModeButton->SetSelected(false);
      break;
   }
   default:
   {
      manualModeButton->SetSelected(false);
      targetModeButton->SetSelected(false);
      threatModeButton->SetSelected(false);
      break;
   }
   }

   mTargetPlatformEngagementBlock->AddObject("manualModeButton", std::move(manualModeButton));
   mTargetPlatformEngagementBlock->AddObject("targetModeButton", std::move(targetModeButton));
   mTargetPlatformEngagementBlock->AddObject("threatModeButton", std::move(threatModeButton));

   connect(dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("manualModeButton")),
           &Button::Clicked,
           [&]()
           {
              mTargetingMode = TargetingMode::cMANUAL;
              Button* targetButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("targetModeButton"));
              if (targetButton != nullptr)
              {
                 targetButton->SetSelected(false);
              }

              Button* threatButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("threatModeButton"));
              if (threatButton != nullptr)
              {
                 threatButton->SetSelected(false);
              }

              TargetChanged();
           });

   connect(dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("targetModeButton")),
           &Button::Clicked,
           [&]()
           {
              mTargetingMode = TargetingMode::cTARGETS;
              Button* manualButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("manualModeButton"));
              if (manualButton != nullptr)
              {
                 manualButton->SetSelected(false);
              }

              Button* threatButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("threatModeButton"));
              if (threatButton != nullptr)
              {
                 threatButton->SetSelected(false);
              }

              TargetChanged();
           });

   connect(dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("threatModeButton")),
           &Button::Clicked,
           [&]()
           {
              mTargetingMode = TargetingMode::cTHREATS;
              Button* manualButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("manualModeButton"));
              if (manualButton != nullptr)
              {
                 manualButton->SetSelected(false);
              }

              Button* targetButton =
                 dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("targetModeButton"));
              if (targetButton != nullptr)
              {
                 targetButton->SetSelected(false);
              }

              TargetChanged();
           });

   // Add the next/prev target buttons.
   std::unique_ptr<Button> nextTargetButton = ut::make_unique<Button>();
   nextTargetButton->SetOffset(Point(145, 155));
   nextTargetButton->SetWidth(35);
   nextTargetButton->SetHeight(15);
   nextTargetButton->SetFontSize(8);
   nextTargetButton->SetText("NEXT");
   mTargetPlatformEngagementBlock->AddObject("nextTargetButton", std::move(nextTargetButton));
   connect(dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("nextTargetButton")),
           &Button::Clicked,
           [&]() { GoToNextTarget(); });

   std::unique_ptr<Button> prevTargetButton = ut::make_unique<Button>();
   prevTargetButton->SetOffset(Point(105, 155));
   prevTargetButton->SetWidth(35);
   prevTargetButton->SetHeight(15);
   prevTargetButton->SetFontSize(8);
   prevTargetButton->SetText("PREV");
   mTargetPlatformEngagementBlock->AddObject("prevTargetButton", std::move(prevTargetButton));
   connect(dynamic_cast<Button*>(mTargetPlatformEngagementBlock->GetObject("prevTargetButton")),
           &Button::Clicked,
           [&]() { GoToPrevTarget(); });

   // Add the platform name box.
   std::unique_ptr<DataBlock> platformNameBox = ut::make_unique<DataBlock>();
   platformNameBox->SetWidth(300);
   platformNameBox->SetHeight(20);
   platformNameBox->SetOffset(Point(0, 130));

   std::unique_ptr<Label> platformNameLabel = ut::make_unique<Label>();
   platformNameLabel->SetOffset(Point(5, platformNameBox->GetHeight() / 2));
   platformNameLabel->SetFontSize(15);
   platformNameLabel->SetText("PLATFORM:");
   platformNameBox->AddObject("platformNameLabel", std::move(platformNameLabel));

   std::unique_ptr<Label> platformName = ut::make_unique<Label>();
   platformName->SetOffset(Point(90, platformNameBox->GetHeight() / 2));
   platformName->SetFontSize(15);
   platformName->SetText("N/A");
   platformNameBox->AddObject("platformName", std::move(platformName));

   mTargetPlatformEngagementBlock->AddObject("platformNameBox", std::move(platformNameBox));

   // Add the target name box.
   std::unique_ptr<DataBlock> targetNameBox = ut::make_unique<DataBlock>();
   targetNameBox->SetWidth(300);
   targetNameBox->SetHeight(20);
   targetNameBox->SetOffset(Point(0, 110));

   std::unique_ptr<Label> targetNameLabel = ut::make_unique<Label>();
   targetNameLabel->SetOffset(Point(5, targetNameBox->GetHeight() / 2));
   targetNameLabel->SetFontSize(15);
   targetNameLabel->SetText("TARGET:");
   targetNameBox->AddObject("targetNameLabel", std::move(targetNameLabel));

   std::unique_ptr<Label> targetName = ut::make_unique<Label>();
   targetName->SetOffset(Point(90, targetNameBox->GetHeight() / 2));
   targetName->SetFontSize(15);
   targetName->SetText(mSourcePlatformPtr->GetName());
   targetNameBox->AddObject("targetName", std::move(targetName));

   mTargetPlatformEngagementBlock->AddObject("targetNameBox", std::move(targetNameBox));

   // Add various engagement data.
   std::unique_ptr<Label> angleOffLabel = ut::make_unique<Label>();
   angleOffLabel->SetOffset(Point(5, 100));
   angleOffLabel->SetFontSize(10);
   angleOffLabel->SetText("ANGLE OFF:");
   mTargetPlatformEngagementBlock->AddObject("angleOffLabel", std::move(angleOffLabel));

   std::unique_ptr<Label> angleOff = ut::make_unique<Label>();
   angleOff->SetOffset(Point(135, 100));
   angleOff->SetFontSize(10);
   angleOff->SetText("XXXX");
   angleOff->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("angleOff", std::move(angleOff));

   std::unique_ptr<Label> aspectLabel = ut::make_unique<Label>();
   aspectLabel->SetOffset(Point(5, 90));
   aspectLabel->SetFontSize(10);
   aspectLabel->SetText("ASPECT:");
   mTargetPlatformEngagementBlock->AddObject("aspectLabel", std::move(aspectLabel));

   std::unique_ptr<Label> aspect = ut::make_unique<Label>();
   aspect->SetOffset(Point(135, 90));
   aspect->SetFontSize(10);
   aspect->SetText("XXXX");
   aspect->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("aspect", std::move(aspect));

   std::unique_ptr<Label> trackQualityLabel = ut::make_unique<Label>();
   trackQualityLabel->SetOffset(Point(5, 70));
   trackQualityLabel->SetFontSize(10);
   trackQualityLabel->SetText("TRK QUAL:");
   mTargetPlatformEngagementBlock->AddObject("trackQualityLabel", std::move(trackQualityLabel));

   std::unique_ptr<Label> trackQuality = ut::make_unique<Label>();
   trackQuality->SetOffset(Point(135, 70));
   trackQuality->SetFontSize(10);
   trackQuality->SetText("XXXX");
   trackQuality->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("trackQuality", std::move(trackQuality));

   std::unique_ptr<Label> rangeLabel = ut::make_unique<Label>();
   rangeLabel->SetOffset(Point(150, 100));
   rangeLabel->SetFontSize(10);
   rangeLabel->SetText("RANGE:");
   mTargetPlatformEngagementBlock->AddObject("rangeLabel", std::move(rangeLabel));

   std::unique_ptr<Label> range = ut::make_unique<Label>();
   range->SetOffset(Point(290, 100));
   range->SetFontSize(10);
   range->SetText("XXXX");
   range->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("range", std::move(range));

   std::unique_ptr<Label> altDiffLabel = ut::make_unique<Label>();
   altDiffLabel->SetOffset(Point(150, 90));
   altDiffLabel->SetFontSize(10);
   altDiffLabel->SetText("DEL-ALT:");
   mTargetPlatformEngagementBlock->AddObject("altDiffLabel", std::move(altDiffLabel));

   std::unique_ptr<Label> altDiff = ut::make_unique<Label>();
   altDiff->SetOffset(Point(290, 90));
   altDiff->SetFontSize(10);
   altDiff->SetText("XXXX");
   altDiff->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("altDiff", std::move(altDiff));

   std::unique_ptr<Label> selectedWeaponLabel = ut::make_unique<Label>();
   selectedWeaponLabel->SetOffset(Point(150, 70));
   selectedWeaponLabel->SetFontSize(10);
   selectedWeaponLabel->SetText("WPN SEL:");
   mTargetPlatformEngagementBlock->AddObject("selectedWeaponLabel", std::move(selectedWeaponLabel));

   std::unique_ptr<Label> selectedWeapon = ut::make_unique<Label>();
   selectedWeapon->SetOffset(Point(290, 70));
   selectedWeapon->SetFontSize(10);
   selectedWeapon->SetText("XXXX");
   selectedWeapon->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("selectedWeapon", std::move(selectedWeapon));

   // Add the detection label/progress bar.
   std::unique_ptr<ProgressBar> detectionBar = ut::make_unique<ProgressBar>();
   detectionBar->SetOffset(Point(120, 50));
   detectionBar->SetHeight(7);
   detectionBar->SetWidth(170);
   detectionBar->SetFillColor(primaryColor);
   detectionBar->SetLineColor(primaryColor);
   detectionBar->SetValue(0);
   int detectionBarYCenter = detectionBar->GetOffset().mY + detectionBar->GetHeight() / 2;
   mTargetPlatformEngagementBlock->AddObject("detectionBar", std::move(detectionBar));

   std::unique_ptr<Label> detectionLabel = ut::make_unique<Label>();
   detectionLabel->SetOffset(Point(5, detectionBarYCenter));
   detectionLabel->SetFontSize(10);
   detectionLabel->SetText("DETECT:");
   mTargetPlatformEngagementBlock->AddObject("detectionLabel", std::move(detectionLabel));

   std::unique_ptr<Label> detection = ut::make_unique<Label>();
   detection->SetOffset(Point(110, detectionBarYCenter));
   detection->SetFontSize(10);
   detection->SetText("XXXX");
   detection->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("detection", std::move(detection));

   // Add the WEZ labels/progress bar.
   std::unique_ptr<ProgressBar> WEZBar = ut::make_unique<ProgressBar>();
   WEZBar->SetOffset(Point(120, 30));
   WEZBar->SetHeight(7);
   WEZBar->SetWidth(170);
   WEZBar->SetFillColor(primaryColor);
   WEZBar->SetLineColor(primaryColor);
   WEZBar->SetValue(0);
   WEZBar->AddTickMark("Rmin", 0.0, 1);
   WEZBar->EnableTickMark("Rmin", false);
   WEZBar->AddTickMark("Rmax", 0.0, 3);
   WEZBar->EnableTickMark("Rmax", false);
   WEZBar->AddTickMark("Rne", 0.0, 2);
   WEZBar->EnableTickMark("Rne", false);
   int wezBarYCenter = WEZBar->GetOffset().mY + WEZBar->GetHeight() / 2;
   mTargetPlatformEngagementBlock->AddObject("WEZBar", std::move(WEZBar));

   std::unique_ptr<Label> WEZMaxLabel = ut::make_unique<Label>();
   WEZMaxLabel->SetOffset(Point(5, wezBarYCenter));
   WEZMaxLabel->SetFontSize(10);
   WEZMaxLabel->SetText("WEZ-RMAX:");
   mTargetPlatformEngagementBlock->AddObject("WEZMaxLabel", std::move(WEZMaxLabel));

   std::unique_ptr<Label> WEZMax = ut::make_unique<Label>();
   WEZMax->SetOffset(Point(110, wezBarYCenter));
   WEZMax->SetFontSize(10);
   WEZMax->SetText("XXXX");
   WEZMax->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("WEZMax", std::move(WEZMax));

   std::unique_ptr<Label> WEZNELabel = ut::make_unique<Label>();
   WEZNELabel->SetOffset(Point(5, wezBarYCenter - 10));
   WEZNELabel->SetFontSize(10);
   WEZNELabel->SetText("WEZ-RNE:");
   mTargetPlatformEngagementBlock->AddObject("WEZNELabel", std::move(WEZNELabel));

   std::unique_ptr<Label> WEZNE = ut::make_unique<Label>();
   WEZNE->SetOffset(Point(110, wezBarYCenter - 10));
   WEZNE->SetFontSize(10);
   WEZNE->SetText("XXXX");
   WEZNE->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("WEZNE", std::move(WEZNE));

   std::unique_ptr<Label> WEZMinLabel = ut::make_unique<Label>();
   WEZMinLabel->SetOffset(Point(5, wezBarYCenter - 20));
   WEZMinLabel->SetFontSize(10);
   WEZMinLabel->SetText("WEZ-RMIN:");
   mTargetPlatformEngagementBlock->AddObject("WEZMinLabel", std::move(WEZMinLabel));

   std::unique_ptr<Label> WEZMin = ut::make_unique<Label>();
   WEZMin->SetOffset(Point(110, wezBarYCenter - 20));
   WEZMin->SetFontSize(10);
   WEZMin->SetText("XXXX");
   WEZMin->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   mTargetPlatformEngagementBlock->AddObject("WEZMin", std::move(WEZMin));

   // Set colors based on team colors
   mTargetPlatformEngagementBlock->Recolor(primaryColor, secondaryColor);
}

// Initializes the tactical data block
void wkf::AirCombatOverlay::InitializeTacticalDataBlock()
{
   // Initialize the outer block
   mTacticalDataBlock->SetWidth(300);
   mTacticalDataBlock->SetHeight(160);

   // Add the title line box
   std::unique_ptr<DataBlock> titleLineBox = ut::make_unique<DataBlock>();
   titleLineBox->SetWidth(300);
   titleLineBox->SetHeight(20);
   titleLineBox->SetOffset(Point(0, 140));

   std::unique_ptr<Label> titleLineLabel = ut::make_unique<Label>();
   titleLineLabel->SetOffset(Point(5, titleLineBox->GetHeight() / 2));
   titleLineLabel->SetFontSize(15);
   titleLineLabel->SetText("TACTICAL DATA SUMMARY");
   titleLineBox->AddObject("titleLineLabel", std::move(titleLineLabel));
   titleLineBox->SetColor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));
   mTacticalDataBlock->AddObject("titleLineBox", std::move(titleLineBox));

   // Add the tactical data region (box)
   std::unique_ptr<DataBlock> tacticalDataBox = ut::make_unique<DataBlock>();
   tacticalDataBox->SetWidth(170);
   tacticalDataBox->SetHeight(140);
   tacticalDataBox->SetOffset(Point(0, 0));

   // Add the emissions label
   std::unique_ptr<Label> emissionsLabel = ut::make_unique<Label>();
   emissionsLabel->SetOffset(Point(5, 125));
   emissionsLabel->SetFontSize(15);
   emissionsLabel->SetText("EMISSIONS");
   tacticalDataBox->AddObject("emissionsLabel", std::move(emissionsLabel));

   // Add the emissions data
   std::unique_ptr<Label> radarLabel = ut::make_unique<Label>();
   radarLabel->SetOffset(Point(7, 115));
   radarLabel->SetFontSize(10);
   radarLabel->SetText("RADAR");
   radarLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("radarLabel", std::move(radarLabel));

   std::unique_ptr<Label> jammerLabel = ut::make_unique<Label>();
   jammerLabel->SetOffset(Point(57, 115));
   jammerLabel->SetFontSize(10);
   jammerLabel->SetText("JAMMER");
   jammerLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("jammerLabel", std::move(jammerLabel));

   std::unique_ptr<Label> otherEmissionsLabel = ut::make_unique<Label>();
   otherEmissionsLabel->SetOffset(Point(107, 115));
   otherEmissionsLabel->SetFontSize(10);
   otherEmissionsLabel->SetText("OTHER");
   otherEmissionsLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("otherEmissionsLabel", std::move(otherEmissionsLabel));

   // Add the signature label
   std::unique_ptr<Label> signatureLabel = ut::make_unique<Label>();
   signatureLabel->SetOffset(Point(5, 100));
   signatureLabel->SetFontSize(15);
   signatureLabel->SetText("SIGNATURE");
   tacticalDataBox->AddObject("signatureLabel", std::move(signatureLabel));

   // Add the signature data
   std::unique_ptr<Label> afterburnerSigLabel = ut::make_unique<Label>();
   afterburnerSigLabel->SetOffset(Point(7, 90));
   afterburnerSigLabel->SetFontSize(10);
   afterburnerSigLabel->SetText("A/B");
   afterburnerSigLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("afterburnerSigLabel", std::move(afterburnerSigLabel));

   std::unique_ptr<Label> contrailSigLabel = ut::make_unique<Label>();
   contrailSigLabel->SetOffset(Point(57, 90));
   contrailSigLabel->SetFontSize(10);
   contrailSigLabel->SetText("CONS");
   contrailSigLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("contrailSigLabel", std::move(contrailSigLabel));

   std::unique_ptr<Label> weaponBaySigLabel = ut::make_unique<Label>();
   weaponBaySigLabel->SetOffset(Point(107, 90));
   weaponBaySigLabel->SetFontSize(10);
   weaponBaySigLabel->SetText("WPN-BAY");
   weaponBaySigLabel->SetFontColor(UtColor(0.6, 0.6, 0.6, 1.0));
   tacticalDataBox->AddObject("weaponBaySigLabel", std::move(weaponBaySigLabel));

   // Add the sa-summary label
   std::unique_ptr<Label> saSummaryLabel = ut::make_unique<Label>();
   saSummaryLabel->SetOffset(Point(5, 75));
   saSummaryLabel->SetFontSize(15);
   saSummaryLabel->SetText("SA SUMMARY");
   tacticalDataBox->AddObject("saSummaryLabel", std::move(saSummaryLabel));

   // Add the mission task label
   std::unique_ptr<Label> missionTaskLabel = ut::make_unique<Label>();
   missionTaskLabel->SetOffset(Point(7, 60));
   missionTaskLabel->SetFontSize(10);
   missionTaskLabel->SetText("TASK:");
   tacticalDataBox->AddObject("missionTaskLabel", std::move(missionTaskLabel));

   std::unique_ptr<Label> missionTask = ut::make_unique<Label>();
   missionTask->SetOffset(Point(163, 60));
   missionTask->SetFontSize(10);
   missionTask->SetText("XXXX");
   missionTask->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   tacticalDataBox->AddObject("missionTask", std::move(missionTask));

   // Add the summary label
   std::unique_ptr<Label> riskCurrentLabel = ut::make_unique<Label>();
   riskCurrentLabel->SetOffset(Point(7, 50));
   riskCurrentLabel->SetFontSize(10);
   riskCurrentLabel->SetText("RISK:");
   tacticalDataBox->AddObject("riskCurrentLabel", std::move(riskCurrentLabel));

   std::unique_ptr<Label> riskCurrent = ut::make_unique<Label>();
   riskCurrent->SetOffset(Point(163, 50));
   riskCurrent->SetFontSize(10);
   riskCurrent->SetText("XXXX");
   riskCurrent->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   tacticalDataBox->AddObject("riskCurrent", std::move(riskCurrent));

   // Add the summary label
   std::unique_ptr<Label> acceptedRiskLabel = ut::make_unique<Label>();
   acceptedRiskLabel->SetOffset(Point(7, 40));
   acceptedRiskLabel->SetFontSize(10);
   acceptedRiskLabel->SetText("ALR:");
   tacticalDataBox->AddObject("acceptedRiskLabel", std::move(acceptedRiskLabel));

   std::unique_ptr<Label> acceptedRisk = ut::make_unique<Label>();
   acceptedRisk->SetOffset(Point(163, 40));
   acceptedRisk->SetFontSize(10);
   acceptedRisk->SetText("XXXX");
   acceptedRisk->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   tacticalDataBox->AddObject("acceptedRisk", std::move(acceptedRisk));

   // Add the summary label
   std::unique_ptr<Label> defensivenessLabel = ut::make_unique<Label>();
   defensivenessLabel->SetOffset(Point(7, 30));
   defensivenessLabel->SetFontSize(10);
   defensivenessLabel->SetText("DEFENSIVENESS:");
   tacticalDataBox->AddObject("defensivenessLabel", std::move(defensivenessLabel));

   std::unique_ptr<Label> defensiveness = ut::make_unique<Label>();
   defensiveness->SetOffset(Point(163, 30));
   defensiveness->SetFontSize(10);
   defensiveness->SetText("XXXX");
   defensiveness->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   tacticalDataBox->AddObject("defensiveness", std::move(defensiveness));

   // Add the summary label
   std::unique_ptr<Label> disengageLabel = ut::make_unique<Label>();
   disengageLabel->SetOffset(Point(7, 20));
   disengageLabel->SetFontSize(10);
   disengageLabel->SetText("URGENCY:");
   tacticalDataBox->AddObject("disengageLabel", std::move(disengageLabel));

   std::unique_ptr<Label> disengageUrgency = ut::make_unique<Label>();
   disengageUrgency->SetOffset(Point(163, 20));
   disengageUrgency->SetFontSize(10);
   disengageUrgency->SetText("XXXX");
   disengageUrgency->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   tacticalDataBox->AddObject("disengageUrgency", std::move(disengageUrgency));

   tacticalDataBox->Recolor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.0, 0.0, 0.0, 0.0));
   mTacticalDataBlock->AddObject("tacticalDataBox", std::move(tacticalDataBox));

   // EMISSIONS:
   //   * RADAR   *JAMMER   *OTHER
   // SIGNATURE:
   //   * A/B     * CONS    *WPN BAY
   // SA SUMMARY:
   //   * MISSION TASK            HVAA-P   (OCA, DCA, etc)
   //   * ACCEPTABLE LEVEL OF RISK  HIGH
   //   * RISK ASSESSMENT          +#.##
   //   * DEFENSIVENESS            +#.##
   //   * URGENCY                  +#.##

   // Add the weapons data box
   std::unique_ptr<DataBlock> weaponsDataBox = ut::make_unique<DataBlock>();
   weaponsDataBox->SetWidth(130);
   weaponsDataBox->SetHeight(140);
   weaponsDataBox->SetOffset(Point(170, 0));
   weaponsDataBox->SetColor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.1, 0.1, 0.1, 0.0));

   std::unique_ptr<Label> weaponsLabel = ut::make_unique<Label>();
   weaponsLabel->SetOffset(Point(5, 125));
   weaponsLabel->SetFontSize(15);
   weaponsLabel->SetText("WEAPONS:");
   weaponsDataBox->AddObject("weaponsLabel", std::move(weaponsLabel));

   // Add the selected weapon label.
   std::unique_ptr<Label> selectedWeaponLabel = ut::make_unique<Label>();
   selectedWeaponLabel->SetOffset(Point(7, 110));
   selectedWeaponLabel->SetFontSize(10);
   selectedWeaponLabel->SetText("WPN SEL:");
   weaponsDataBox->AddObject("selectedWeaponLabel", std::move(selectedWeaponLabel));

   std::unique_ptr<Label> selectedWeapon = ut::make_unique<Label>();
   selectedWeapon->SetOffset(Point(123, 110));
   selectedWeapon->SetFontSize(10);
   selectedWeapon->SetText("None");
   selectedWeapon->SetAlignment(osgText::Text::AlignmentType::RIGHT_CENTER);
   weaponsDataBox->AddObject("selectedWeapon", std::move(selectedWeapon));

   mTacticalDataBlock->AddObject("weaponsDataBox", std::move(weaponsDataBox));

   // Set the color of the region
   mTacticalDataBlock->SetColor(UtColor(0.8, 0.8, 0.8, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));
}

// Updates the data block according to the given platform data
void wkf::AirCombatOverlay::UpdateSourcePlatformDataBlock(const wkf::AirCombatDataEngagementSummary& aData)
{
   auto UpdateLabel = [&](DataBlock& aDataBlock, const std::string& aLabelID, const std::string& aText)
   {
      Label* label = dynamic_cast<Label*>(aDataBlock.GetObject(aLabelID));
      if (label != nullptr)
      {
         label->SetText(aText);
      }
   };

   // Update the platform name block.
   DataBlock* platformTypeBlock = dynamic_cast<DataBlock*>(mSourcePlatformDataBlock->GetObject("platformTypeBox"));
   if (platformTypeBlock != nullptr)
   {
      UpdateLabel(*platformTypeBlock,
                  "platformType",
                  aData.platformData.mPlatformType.empty() ? "Unknown" : aData.platformData.mPlatformType);
   }

   // Update the kinematics.
   DataBlock* kinematicBlock = dynamic_cast<DataBlock*>(mSourcePlatformDataBlock->GetObject("kinematicDataBox"));
   if (kinematicBlock != nullptr)
   {
      std::stringstream stream;
      stream << std::fixed << std::setprecision(0) << aData.kinematicData.mKCAS;
      UpdateLabel(*kinematicBlock, "KCAS", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.kinematicData.mKTAS;
      UpdateLabel(*kinematicBlock, "KTAS", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(2) << aData.kinematicData.mMach;
      UpdateLabel(*kinematicBlock, "mach", stream.str());
      stream.str("");

      if (aData.kinematicData.mAlpha_deg >= 0)
      {
         stream << "+";
      }
      stream << std::fixed << std::setprecision(0) << aData.kinematicData.mAlpha_deg;
      UpdateLabel(*kinematicBlock, "alpha", stream.str());
      stream.str("");

      if (aData.kinematicData.mGLoad >= 0)
      {
         stream << "+";
      }
      stream << std::fixed << std::setprecision(1) << aData.kinematicData.mGLoad;
      UpdateLabel(*kinematicBlock, "gLoad", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(1) << aData.kinematicData.mAvailG;
      UpdateLabel(*kinematicBlock, "availGs", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.kinematicData.mAltitude_ft;
      UpdateLabel(*kinematicBlock, "altitude", stream.str());
      stream.str("");

      if (aData.kinematicData.mVerticalSpeed_fpm >= 0)
      {
         stream << "+";
      }
      stream << std::fixed << std::setprecision(0) << aData.kinematicData.mVerticalSpeed_fpm;
      UpdateLabel(*kinematicBlock, "vertSpeed", stream.str());
      stream.str("");

      double hdg = aData.kinematicData.mHeading_deg;
      if (hdg < 0)
      {
         hdg += 360.0;
      }
      stream << std::fixed << std::setprecision(1) << hdg;
      UpdateLabel(*kinematicBlock, "heading", stream.str());
      stream.str("");

      if (aData.kinematicData.mPitch_deg >= 0)
      {
         stream << "+";
      }
      double pitch = aData.kinematicData.mPitch_deg;
      stream << std::fixed << std::setprecision(1) << pitch;
      UpdateLabel(*kinematicBlock, "pitch", stream.str());
      stream.str("");

      if (aData.kinematicData.mRoll_deg >= 0)
      {
         stream << "+";
      }
      double roll = aData.kinematicData.mRoll_deg;
      stream << std::fixed << std::setprecision(1) << roll;
      UpdateLabel(*kinematicBlock, "roll", stream.str());
      stream.str("");

      // Update the GLoad bar.
      ProgressBar* gLoadBar = dynamic_cast<ProgressBar*>(kinematicBlock->GetObject("gLoadBar"));
      if (gLoadBar != nullptr)
      {
         double fillValue = std::fabs(aData.kinematicData.mGLoad) / 20.0;
         if (aData.kinematicData.mGLoad > 0)
         {
            fillValue += 0.5;
         }
         fillValue = std::max(0.0, fillValue);
         fillValue = std::min(1.0, fillValue);

         gLoadBar->SetValue(fillValue);
      }
   }

   // Update the fuel data.
   DataBlock* fuelBlock = dynamic_cast<DataBlock*>(mSourcePlatformDataBlock->GetObject("fuelDataBox"));
   if (fuelBlock != nullptr)
   {
      std::stringstream stream;
      stream << std::fixed << std::setprecision(0) << aData.fuelData.mCurrentInternalFuel_lbs << " LBS";
      UpdateLabel(*fuelBlock, "internalFuel", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.fuelData.mCurrentExternalFuel_lbs << " LBS";
      UpdateLabel(*fuelBlock, "externalFuel", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.fuelData.mTotalFuel_lbs << " LBS";
      UpdateLabel(*fuelBlock, "totalFuel", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.fuelData.mBingoFuel_lbs << " LBS";
      UpdateLabel(*fuelBlock, "bingo", stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0) << aData.fuelData.mJokerFuel_lbs << " LBS";
      UpdateLabel(*fuelBlock, "joker", stream.str());
      stream.str("");

      auto         fuelBarObject = fuelBlock->GetObject("fuelBar");
      ProgressBar* fuelBar       = dynamic_cast<ProgressBar*>(fuelBarObject);
      if (fuelBar != nullptr)
      {
         double fuelCapacity = aData.fuelData.mInternalFuelCapacity_lbs + aData.fuelData.mExternalFuelCapacity_lbs;
         double currentFuel  = aData.fuelData.mCurrentInternalFuel_lbs + aData.fuelData.mExternalFuelCapacity_lbs;

         double fillValue = 0, bingoValue = 0, jokerValue = 0;
         if (fuelCapacity > 0)
         {
            fillValue  = currentFuel / fuelCapacity;
            bingoValue = aData.fuelData.mBingoFuel_lbs / fuelCapacity;
            jokerValue = aData.fuelData.mJokerFuel_lbs / fuelCapacity;
         }

         fuelBar->SetValue(fillValue);
         fuelBar->UpdateTickMark("Bingo", bingoValue);
         fuelBar->UpdateTickMark("Joker", jokerValue);
      }
   }
}

void wkf::AirCombatOverlay::UpdateSourcePlatformEngagementBlock(const wkf::AirCombatDataEngagementSummary& aData)
{
   wkf::Platform* target = nullptr;
   switch (mTargetingMode)
   {
   case TargetingMode::cMANUAL:
   {
      target = mCurrentManualTarget;
      break;
   }
   case TargetingMode::cTARGETS:
   {
      target = mCurrentPrioritizedTarget;
      break;
   }
   case TargetingMode::cTHREATS:
   {
      target = mCurrentPrioritizedThreat;
      break;
   }
   }

   DataBlock*   targetNameBox    = dynamic_cast<DataBlock*>(mSourcePlatformEngagementBlock->GetObject("targetNameBox"));
   Label*       targetNameLabel  = dynamic_cast<Label*>(targetNameBox->GetObject("targetName"));
   Label*       angleOffLabel    = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("angleOff"));
   Label*       aspectAngleLabel = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("aspect"));
   Label*       rangeLabel       = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("range"));
   Label*       altDiffLabel     = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("altDiff"));
   Label*       trackLabel       = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("trackQuality"));
   Label*       detectionLabel   = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("detection"));
   ProgressBar* detectionBar = dynamic_cast<ProgressBar*>(mSourcePlatformEngagementBlock->GetObject("detectionBar"));
   Label*       WEZMaxLabel  = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("WEZMax"));
   Label*       WEZNELabel   = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("WEZNE"));
   Label*       WEZMinLabel  = dynamic_cast<Label*>(mSourcePlatformEngagementBlock->GetObject("WEZMin"));
   ProgressBar* WEZBar       = dynamic_cast<ProgressBar*>(mSourcePlatformEngagementBlock->GetObject("WEZBar"));

   if (target != nullptr)
   {
      targetNameLabel->SetText(target->GetName());

      std::stringstream stream;
      stream << std::fixed << std::setprecision(0);
      auto angleOffData_deg = aData.engagementData.mAngleOffMap_deg.find(target->GetName());
      if (angleOffData_deg != aData.engagementData.mAngleOffMap_deg.end())
      {
         if (angleOffData_deg->second > 0)
         {
            stream << "+";
         }
         stream << angleOffData_deg->second;
      }
      else
      {
         stream << 0;
      }
      angleOffLabel->SetText(stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0);
      auto aspectAngleData_deg = aData.engagementData.mAspectAngleMap_deg.find(target->GetName());
      if (aspectAngleData_deg != aData.engagementData.mAspectAngleMap_deg.end())
      {
         if (aspectAngleData_deg->second > 0)
         {
            stream << "+";
         }
         stream << aspectAngleData_deg->second;
      }
      else
      {
         stream << 0;
      }
      aspectAngleLabel->SetText(stream.str());
      stream.str("");

      double currentRange_m = -1.0;
      stream << std::fixed << std::setprecision(1);
      auto rangeData_nm = aData.engagementData.mCurrentRangeMap_nm.find(target->GetName());
      if (rangeData_nm != aData.engagementData.mCurrentRangeMap_nm.end())
      {
         currentRange_m = rangeData_nm->second * UtMath::cM_PER_NM;
         stream << rangeData_nm->second << " NM";
      }
      else
      {
         stream << 0 << " NM";
      }
      rangeLabel->SetText(stream.str());
      stream.str("");

      stream << std::fixed << std::setprecision(0);
      auto altDiffData_ft = aData.engagementData.mCurrentDeltaAltMap_ft.find(target->GetName());
      if (altDiffData_ft != aData.engagementData.mCurrentDeltaAltMap_ft.end())
      {
         stream << altDiffData_ft->second << " FT";
      }
      else
      {
         stream << 0 << " FT";
      }
      altDiffLabel->SetText(stream.str());
      stream.str("");

      auto tracked = std::find(aData.engagementData.mTrackedPlatformNames.begin(),
                               aData.engagementData.mTrackedPlatformNames.end(),
                               target->GetName());
      if (tracked != aData.engagementData.mTrackedPlatformNames.end())
      {
         stream << "HIGH";
      }
      else
      {
         stream << "N/A";
      }
      trackLabel->SetText(stream.str());
      stream.str("");

      double detectionRange_m = -1.0;
      stream << std::fixed << std::setprecision(1);
      auto rawDetection_nm = aData.engagementData.mRawDetectionAgainstTargetMap_nm.find(target->GetName());
      if (rawDetection_nm != aData.engagementData.mRawDetectionAgainstTargetMap_nm.end())
      {
         detectionRange_m = rawDetection_nm->second * UtMath::cM_PER_NM;
         stream << (rawDetection_nm->second) << " NM";
      }
      else
      {
         stream << 0;
      }
      detectionLabel->SetText(stream.str());
      stream.str("");

      // Calc normalized detection range
      if ((currentRange_m < 0) || (detectionRange_m < 0))
      {
         detectionBar->SetValue(0.0);
      }
      else
      {
         double normalizedRange = detectionRange_m / currentRange_m;
         if (normalizedRange > 1.0)
         {
            normalizedRange = 1.0;
         }
         detectionBar->SetValue(normalizedRange);
      }

      double wezRange_m = -1.0;
      auto   rawWEZ_nm  = aData.engagementData.mRawWEZAgainstTargetMap_nm.find(target->GetName());
      if (rawWEZ_nm != aData.engagementData.mRawWEZAgainstTargetMap_nm.end())
      {
         wezRange_m = rawWEZ_nm->second * UtMath::cM_PER_NM;
      }

      // Calc normalized wez range
      if ((currentRange_m < 0) || (wezRange_m < 0))
      {
         WEZBar->SetValue(0.0);
      }
      else
      {
         double normalizedWezRange = wezRange_m / currentRange_m;
         if (normalizedWezRange > 1.0)
         {
            normalizedWezRange = 1.0;
         }
         WEZBar->SetValue(normalizedWezRange);
      }

      stream.str("");
      stream << std::fixed << std::setprecision(1);
      auto maximumRange_nm = aData.engagementData.mWEZMaxRangeMap_nm.find(target->GetName());
      if (maximumRange_nm != aData.engagementData.mWEZMaxRangeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rmax", (maximumRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rmax", true);
         stream << maximumRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rmax", 0.0);
         WEZBar->EnableTickMark("Rmax", false);
         stream << 0 << " NM";
      }
      WEZMaxLabel->SetText(stream.str());

      stream.str("");
      stream << std::fixed << std::setprecision(1);
      auto noEscapeRange_nm = aData.engagementData.mWEZNoEscapeMap_nm.find(target->GetName());
      if (noEscapeRange_nm != aData.engagementData.mWEZNoEscapeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rne", (noEscapeRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rne", true);
         stream << noEscapeRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rne", 0.0);
         WEZBar->EnableTickMark("Rne", false);
         stream << 0 << " NM";
      }
      WEZNELabel->SetText(stream.str());

      stream.str("");
      stream << std::fixed << std::setprecision(1);
      auto minimumRange_nm = aData.engagementData.mWEZMinRangeMap_nm.find(target->GetName());
      if (minimumRange_nm != aData.engagementData.mWEZMinRangeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rmin", (minimumRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rmin", true);
         stream << minimumRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rmin", 0.0);
         WEZBar->EnableTickMark("Rmin", false);
         stream << 0 << " NM";
      }
      WEZMinLabel->SetText(stream.str());
   }
   else
   {
      targetNameLabel->SetText("N/A");
      angleOffLabel->SetText("XXXX");
      aspectAngleLabel->SetText("XXXX");
      rangeLabel->SetText("XXXX");
      altDiffLabel->SetText("XXXX");
      trackLabel->SetText("N/A");
      detectionLabel->SetText("XXXX");
      detectionBar->SetValue(0);
      WEZMaxLabel->SetText("XXXX");
      WEZNELabel->SetText("XXXX");
      WEZMinLabel->SetText("XXXX");
      WEZBar->SetValue(0);
      WEZBar->UpdateTickMark("Rmax", 0.0);
      WEZBar->EnableTickMark("Rmax", false);
      WEZBar->UpdateTickMark("Rmin", 0.0);
      WEZBar->EnableTickMark("Rmin", false);
      WEZBar->UpdateTickMark("Rne", 0.0);
      WEZBar->EnableTickMark("Rne", false);
   }
}

// Updates the target engagement block using data associated with the current target.
void wkf::AirCombatOverlay::UpdateTargetPlatformEngagementBlock(const wkf::AirCombatDataEngagementSummary& aData)
{
   DataBlock* platformNameBox = dynamic_cast<DataBlock*>(mTargetPlatformEngagementBlock->GetObject("platformNameBox"));
   Label*     platformNameLabel = dynamic_cast<Label*>(platformNameBox->GetObject("platformName"));

   Label*       angleOffLabel    = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("angleOff"));
   Label*       aspectAngleLabel = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("aspect"));
   Label*       rangeLabel       = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("range"));
   Label*       altDiffLabel     = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("altDiff"));
   Label*       trackLabel       = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("trackQuality"));
   Label*       detectionLabel   = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("detection"));
   ProgressBar* detectionBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
   Label*       WEZMaxLabel  = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("WEZMax"));
   Label*       WEZNELabel   = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("WEZNE"));
   Label*       WEZMinLabel  = dynamic_cast<Label*>(mTargetPlatformEngagementBlock->GetObject("WEZMin"));
   ProgressBar* WEZBar       = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));

   if (mSourcePlatformPtr != nullptr)
   {
      platformNameLabel->SetText(aData.platformData.mPlatformName.empty() ? "N/A" : aData.platformData.mPlatformName);

      std::stringstream stream;
      stream << std::fixed << std::setprecision(0);
      auto angleOffData_deg = aData.engagementData.mAngleOffMap_deg.find(mSourcePlatformPtr->GetName());
      if (angleOffData_deg != aData.engagementData.mAngleOffMap_deg.end())
      {
         if (angleOffData_deg->second > 0)
         {
            stream << "+";
         }
         stream << angleOffData_deg->second;
      }
      else
      {
         stream << 0;
      }
      angleOffLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      stream << std::fixed << std::setprecision(0);
      auto aspectAngleData_deg = aData.engagementData.mAspectAngleMap_deg.find(mSourcePlatformPtr->GetName());
      if (aspectAngleData_deg != aData.engagementData.mAspectAngleMap_deg.end())
      {
         if (aspectAngleData_deg->second > 0)
         {
            stream << "+";
         }
         stream << aspectAngleData_deg->second;
      }
      else
      {
         stream << 0;
      }
      aspectAngleLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      double currentRange_m = -1.0;
      stream << std::fixed << std::setprecision(1);
      auto rangeData_nm = aData.engagementData.mCurrentRangeMap_nm.find(mSourcePlatformPtr->GetName());
      if (rangeData_nm != aData.engagementData.mCurrentRangeMap_nm.end())
      {
         currentRange_m = rangeData_nm->second * UtMath::cM_PER_NM;
         stream << rangeData_nm->second << " NM";
      }
      else
      {
         stream << 0 << " NM";
      }
      rangeLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      stream << std::fixed << std::setprecision(0);
      auto altDiffData_ft = aData.engagementData.mCurrentDeltaAltMap_ft.find(mSourcePlatformPtr->GetName());
      if (altDiffData_ft != aData.engagementData.mCurrentDeltaAltMap_ft.end())
      {
         stream << altDiffData_ft->second << " FT";
      }
      else
      {
         stream << 0 << " FT";
      }
      altDiffLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      auto tracked = std::find(aData.engagementData.mTrackedPlatformNames.begin(),
                               aData.engagementData.mTrackedPlatformNames.end(),
                               mSourcePlatformPtr->GetName());
      if (tracked != aData.engagementData.mTrackedPlatformNames.end())
      {
         stream << "HIGH";
      }
      else
      {
         stream << "N/A";
      }
      trackLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      double detectionRange_m = -1.0;
      stream << std::fixed << std::setprecision(1);
      auto rawDetection_nm = aData.engagementData.mRawDetectionAgainstTargetMap_nm.find(mSourcePlatformPtr->GetName());
      if (rawDetection_nm != aData.engagementData.mRawDetectionAgainstTargetMap_nm.end())
      {
         detectionRange_m = rawDetection_nm->second * UtMath::cM_PER_NM;
         stream << rawDetection_nm->second << " NM";
      }
      else
      {
         stream << 0;
      }
      detectionLabel->SetText(stream.str());

      // Calc normalized detection range
      if ((currentRange_m < 0) || (detectionRange_m < 0))
      {
         detectionBar->SetValue(0.0);
      }
      else
      {
         double normalizedRange = detectionRange_m / currentRange_m;
         if (normalizedRange > 1.0)
         {
            normalizedRange = 1.0;
         }
         detectionBar->SetValue(normalizedRange);
      }

      double wezRange_m = -1.0;
      auto   rawWEZ_nm  = aData.engagementData.mRawWEZAgainstTargetMap_nm.find(mSourcePlatformPtr->GetName());
      if (rawWEZ_nm != aData.engagementData.mRawWEZAgainstTargetMap_nm.end())
      {
         wezRange_m = rawWEZ_nm->second * UtMath::cM_PER_NM;
      }

      // Calc normalized wez range
      if ((currentRange_m < 0) || (wezRange_m < 0))
      {
         WEZBar->SetValue(0.0);
      }
      else
      {
         double normalizedWezRange = wezRange_m / currentRange_m;
         if (normalizedWezRange > 1.0)
         {
            normalizedWezRange = 1.0;
         }
         WEZBar->SetValue(normalizedWezRange);
      }

      // auto normalizedWEZ = aData.mNormalizedWEZAgainstTargetMap.find(mSourcePlatformPtr->GetName());
      // if (normalizedWEZ != aData.mNormalizedWEZAgainstTargetMap.end())
      // {
      //    WEZBar->SetValue(normalizedWEZ->second);
      // }
      // else
      // {
      //    WEZBar->SetValue(0);
      // }

      stream.str(""); // Clear the stream
      stream << std::fixed << std::setprecision(1);
      auto maximumRange_nm = aData.engagementData.mWEZMaxRangeMap_nm.find(mSourcePlatformPtr->GetName());
      if (maximumRange_nm != aData.engagementData.mWEZMaxRangeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rmax", (maximumRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rmax", true);
         stream << maximumRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rmax", 0.0);
         WEZBar->EnableTickMark("Rmax", false);
         stream << 0 << " NM";
      }
      WEZMaxLabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      stream << std::fixed << std::setprecision(1);
      auto noEscapeRange_nm = aData.engagementData.mWEZNoEscapeMap_nm.find(mSourcePlatformPtr->GetName());
      if (noEscapeRange_nm != aData.engagementData.mWEZNoEscapeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rne", (noEscapeRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rne", true);
         stream << noEscapeRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rne", 0.0);
         WEZBar->EnableTickMark("Rne", false);
         stream << 0 << " NM";
      }
      WEZNELabel->SetText(stream.str());

      stream.str(""); // Clear the stream
      stream << std::fixed << std::setprecision(1);
      auto minimumRange_nm = aData.engagementData.mWEZMinRangeMap_nm.find(mSourcePlatformPtr->GetName());
      if (minimumRange_nm != aData.engagementData.mWEZMinRangeMap_nm.end())
      {
         WEZBar->UpdateTickMark("Rmin", (minimumRange_nm->second / rangeData_nm->second));
         WEZBar->EnableTickMark("Rmin", true);
         stream << minimumRange_nm->second << " NM";
      }
      else
      {
         WEZBar->UpdateTickMark("Rmin", 0.0);
         WEZBar->EnableTickMark("Rmin", false);
         stream << 0 << " NM";
      }
      WEZMinLabel->SetText(stream.str());
   }
   else
   {
      platformNameLabel->SetText("N/A");
      angleOffLabel->SetText("XXXX");
      aspectAngleLabel->SetText("XXXX");
      rangeLabel->SetText("XXXX");
      altDiffLabel->SetText("XXXX");
      trackLabel->SetText("N/A");
      detectionLabel->SetText("XXXX");
      detectionBar->SetValue(0);
      WEZMaxLabel->SetText("XXXX");
      WEZNELabel->SetText("XXXX");
      WEZMinLabel->SetText("XXXX");
      WEZBar->SetValue(0);
      WEZBar->UpdateTickMark("Rmax", 0.0);
      WEZBar->EnableTickMark("Rmax", false);
      WEZBar->UpdateTickMark("Rmin", 0.0);
      WEZBar->EnableTickMark("Rmin", false);
      WEZBar->UpdateTickMark("Rne", 0.0);
      WEZBar->EnableTickMark("Rne", false);
   }
}

// Updates the sim data block
void wkf::AirCombatOverlay::UpdateTacticalDataBlock(const wkf::AirCombatDataEngagementSummary& aData)
{
   auto UpdateLabel = [&](DataBlock& aDataBlock, const std::string& aLabelID, const std::string& aText)
   {
      Label* label = dynamic_cast<Label*>(aDataBlock.GetObject(aLabelID));
      if (label != nullptr)
      {
         label->SetText(aText);
      }
   };


   DataBlock* tacticalDataBox = dynamic_cast<DataBlock*>(mTacticalDataBlock->GetObject("tacticalDataBox"));
   if (tacticalDataBox != nullptr)
   {
      UtColor disabled(0.6, 0.6, 0.6, 1.0);
      UtColor enabled(1.0, 1.0, 1.0, 1.0);

      // Update the Emissions section.
      Label* radarLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("radarLabel"));
      if (radarLabel != nullptr)
      {
         radarLabel->SetFontColor(aData.emissionsData.mRadarEmitting ? enabled : disabled);
      }

      Label* jammerLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("jammerLabel"));
      if (jammerLabel != nullptr)
      {
         jammerLabel->SetFontColor(aData.emissionsData.mJammerEmitting ? enabled : disabled);
      }

      Label* otherEmissionsLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("otherEmissionsLabel"));
      if (otherEmissionsLabel != nullptr)
      {
         otherEmissionsLabel->SetFontColor(aData.emissionsData.mOtherSystemsEmitting ? enabled : disabled);
      }

      // Update the Signature section.
      Label* afterburnerSigLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("afterburnerSigLabel"));
      if (afterburnerSigLabel != nullptr)
      {
         afterburnerSigLabel->SetFontColor(aData.signatureData.mAfterburnerOn ? enabled : disabled);
      }

      Label* contrailSigLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("contrailSigLabel"));
      if (contrailSigLabel != nullptr)
      {
         contrailSigLabel->SetFontColor(aData.signatureData.mContrailing ? enabled : disabled);
      }

      Label* weaponBaySigLabel = dynamic_cast<Label*>(tacticalDataBox->GetObject("weaponBaySigLabel"));
      if (weaponBaySigLabel != nullptr)
      {
         weaponBaySigLabel->SetFontColor(aData.signatureData.mWeaponBayDoorsOpen ? enabled : disabled);
      }

      // Update the SA summary.
      UpdateLabel(*tacticalDataBox, "missionTask", aData.saData.mMissionTask);

      std::stringstream ss;

      ss << std::fixed << std::setprecision(1) << aData.saData.mRiskCurrent;
      UpdateLabel(*tacticalDataBox, "riskCurrent", ss.str());
      ss.str("");

      if (aData.saData.mRiskAcceptable == 0)
      {
         ss << "LOW";
      }
      else if (aData.saData.mRiskAcceptable == 1)
      {
         ss << "MEDIUM";
      }
      else if (aData.saData.mRiskAcceptable == 2)
      {
         ss << "HIGH";
      }
      UpdateLabel(*tacticalDataBox, "acceptedRisk", ss.str());
      ss.str("");

      ss << std::fixed << std::setprecision(1) << aData.saData.mDefensiveness;
      UpdateLabel(*tacticalDataBox, "defensiveness", ss.str());
      ss.str("");

      ss << std::fixed << std::setprecision(1) << aData.saData.mUrgency;
      UpdateLabel(*tacticalDataBox, "disengageUrgency", ss.str());
      ss.str("");
   }

   DataBlock* weaponsBlock = dynamic_cast<DataBlock*>(mTacticalDataBlock->GetObject("weaponsDataBox"));
   if (weaponsBlock != nullptr)
   {
      // Update the selected weapon.
      if (!aData.weaponsData.mSelectedWeapon.empty())
      {
         std::string weaponName = aData.weaponsData.mSelectedWeapon;
         for (auto& c : weaponName)
         {
            c = toupper(c);
         }

         UpdateLabel(*weaponsBlock, "selectedWeapon", weaponName);
      }
      else
      {
         UpdateLabel(*weaponsBlock, "selectedWeapon", "None");
      }

      // Add weapons to the weapons block.
      for (const auto& weaponData : aData.weaponsData.mWeaponsQtyMap)
      {
         Label* weaponLabel = dynamic_cast<Label*>(weaponsBlock->GetObject(weaponData.first));
         if (weaponLabel != nullptr)
         {
            std::string uppercaseLabel = weaponData.first;
            for (auto& c : uppercaseLabel)
            {
               c = toupper(c);
            }

            std::stringstream stream;
            stream << std::fixed << std::setprecision(0);
            stream << uppercaseLabel << " " << weaponData.second << "x";
            weaponLabel->SetText(stream.str());
         }
         else
         {
            // Determine the offset for this new weapon label.
            Point               newOffset(7, 100);
            std::vector<Label*> weaponLabels = weaponsBlock->GetObjectsOfType<Label>();
            newOffset.mY =
               newOffset.mY - (10 * (weaponLabels.size() - 2)); // subtract 2 for the WEAPONS label and the WPN SEL label
            if (newOffset.mY > 0)
            {
               std::unique_ptr<Label> newWeaponLabel = ut::make_unique<Label>();
               newWeaponLabel->SetOffset(newOffset);
               newWeaponLabel->SetFontSize(10);

               std::string uppercaseLabel = weaponData.first;
               for (auto& c : uppercaseLabel)
               {
                  c = toupper(c);
               }

               std::stringstream stream;
               stream << std::fixed << std::setprecision(0);
               stream << uppercaseLabel << " " << weaponData.second << "x";
               newWeaponLabel->SetText(stream.str());

               weaponsBlock->AddObject(weaponData.first, std::move(newWeaponLabel));
            }
         }
      }
   }
}

// Advances the current target to the next platform in the list.
// Wraps around to the first target if the current target is at the end of the list.
void wkf::AirCombatOverlay::GoToNextTarget()
{
   switch (mTargetingMode)
   {
   case TargetingMode::cMANUAL:
   {
      auto foundPlatform = std::find(mManualTargets.begin(), mManualTargets.end(), mCurrentManualTarget);
      if (foundPlatform != mManualTargets.end())
      {
         if (foundPlatform != std::prev(mManualTargets.end()))
         {
            mCurrentManualTarget = (*std::next(foundPlatform));
         }
         else
         {
            mCurrentManualTarget = (*mManualTargets.begin());
         }
      }
      break;
   }
   case TargetingMode::cTARGETS:
   {
      auto foundPlatform = std::find(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), mCurrentPrioritizedTarget);
      if (foundPlatform != mPrioritizedTargets.end())
      {
         if (foundPlatform != std::prev(mPrioritizedTargets.end()))
         {
            mCurrentPrioritizedTarget = (*std::next(foundPlatform));
         }
         else
         {
            mCurrentPrioritizedTarget = (*mPrioritizedTargets.begin());
         }
      }
      break;
   }
   case TargetingMode::cTHREATS:
   {
      auto foundPlatform = std::find(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), mCurrentPrioritizedThreat);
      if (foundPlatform != mPrioritizedThreats.end())
      {
         if (foundPlatform != std::prev(mPrioritizedThreats.end()))
         {
            mCurrentPrioritizedThreat = (*std::next(foundPlatform));
         }
         else
         {
            mCurrentPrioritizedThreat = (*mPrioritizedThreats.begin());
         }
      }
      break;
   }
   }

   TargetChanged();
}

// Reverts to the previous target in the list.
// Wraps around to the last target if the current target is at the beginning of the list.
void wkf::AirCombatOverlay::GoToPrevTarget()
{
   switch (mTargetingMode)
   {
   case TargetingMode::cMANUAL:
   {
      auto foundPlatform = std::find(mManualTargets.begin(), mManualTargets.end(), mCurrentManualTarget);
      if (foundPlatform != mManualTargets.end())
      {
         if (foundPlatform != mManualTargets.begin())
         {
            mCurrentManualTarget = (*std::prev(foundPlatform));
         }
         else
         {
            mCurrentManualTarget = (*std::prev(mManualTargets.end()));
         }
      }
      break;
   }
   case TargetingMode::cTARGETS:
   {
      auto foundPlatform = std::find(mPrioritizedTargets.begin(), mPrioritizedTargets.end(), mCurrentPrioritizedTarget);
      if (foundPlatform != mPrioritizedTargets.end())
      {
         if (foundPlatform != mPrioritizedTargets.begin())
         {
            mCurrentPrioritizedTarget = (*std::prev(foundPlatform));
         }
         else
         {
            mCurrentPrioritizedTarget = (*std::prev(mPrioritizedTargets.end()));
         }
      }
      break;
   }
   case TargetingMode::cTHREATS:
   {
      auto foundPlatform = std::find(mPrioritizedThreats.begin(), mPrioritizedThreats.end(), mCurrentPrioritizedThreat);
      if (foundPlatform != mPrioritizedThreats.end())
      {
         if (foundPlatform != mPrioritizedThreats.begin())
         {
            mCurrentPrioritizedThreat = (*std::prev(foundPlatform));
         }
         else
         {
            mCurrentPrioritizedThreat = (*std::prev(mPrioritizedThreats.end()));
         }
      }
      break;
   }
   }

   TargetChanged();
}

// Changes the color of the target engagement block based on the current target.
void wkf::AirCombatOverlay::TargetChanged()
{
   auto* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   if (teamVisPrefs != nullptr)
   {
      switch (mTargetingMode)
      {
      case wkf::TargetingMode::cMANUAL:
      {
         if (mCurrentManualTarget != nullptr)
         {
            UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentManualTarget->GetSide());
            UtColor secondary = AlterColor(primary, 0.5);
            mTargetPlatformEngagementBlock->Recolor(primary, secondary);

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(primary);
            WEZBar->SetLineColor(primary);

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(primary);
            detectionBar->SetLineColor(primary);
         }
         else
         {
            // No target; change to default colors.
            mTargetPlatformEngagementBlock->Recolor(UtColor(1.0, 1.0, 1.0, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            WEZBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            detectionBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));
         }
         break;
      }
      case wkf::TargetingMode::cTARGETS:
      {
         if (mCurrentPrioritizedTarget != nullptr)
         {
            UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentPrioritizedTarget->GetSide());
            UtColor secondary = AlterColor(primary, 0.5);
            mTargetPlatformEngagementBlock->Recolor(primary, secondary);

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(primary);
            WEZBar->SetLineColor(primary);

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(primary);
            detectionBar->SetLineColor(primary);
         }
         else
         {
            // No target; change to default colors.
            mTargetPlatformEngagementBlock->Recolor(UtColor(1.0, 1.0, 1.0, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            WEZBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            detectionBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));
         }
         break;
      }
      case wkf::TargetingMode::cTHREATS:
      {
         if (mCurrentPrioritizedThreat != nullptr)
         {
            UtColor primary   = teamVisPrefs->GetTeamUtColor(mCurrentPrioritizedThreat->GetSide());
            UtColor secondary = AlterColor(primary, 0.5);
            mTargetPlatformEngagementBlock->Recolor(primary, secondary);

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(primary);
            WEZBar->SetLineColor(primary);

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(primary);
            detectionBar->SetLineColor(primary);
         }
         else
         {
            // No target; change to default colors.
            mTargetPlatformEngagementBlock->Recolor(UtColor(1.0, 1.0, 1.0, 1.0), UtColor(0.1, 0.1, 0.1, 0.5));

            ProgressBar* WEZBar = dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("WEZBar"));
            WEZBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            WEZBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));

            ProgressBar* detectionBar =
               dynamic_cast<ProgressBar*>(mTargetPlatformEngagementBlock->GetObject("detectionBar"));
            detectionBar->SetFillColor(UtColor(1.0, 1.0, 1.0, 1.0));
            detectionBar->SetLineColor(UtColor(1.0, 1.0, 1.0, 1.0));
         }
         break;
      }
      default:
      {
         break;
      }
      }
   }
}

void wkf::AirCombatOverlay::SetSourceColors(UtColor& aPrimaryColor, UtColor& aSecondaryColor)
{
   UtColor primaryColor(0.3, 0.6, 1.0, 1.0);
   UtColor secondaryColor(0.0, 0.125, 0.25, 0.5);

   // Grab the source platform's team color, if we can.
   if (mSourcePlatformPtr != nullptr)
   {
      wkf::TeamVisibilityPrefObject* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (teamVisPrefs != nullptr)
      {
         primaryColor   = teamVisPrefs->GetTeamUtColor(mSourcePlatformPtr->GetSide());
         secondaryColor = AlterColor(primaryColor, 0.5);
      }
   }

   aPrimaryColor   = primaryColor;
   aSecondaryColor = secondaryColor;
}

void wkf::AirCombatOverlay::SetTargetColors(UtColor& aPrimaryColor, UtColor& aSecondaryColor)
{
   UtColor primaryColor(1.0, 1.0, 1.0, 1.0);
   UtColor secondaryColor(0.1, 0.1, 0.1, 0.5);

   // Grab the current target's team color, if we can.
   wkf::Platform* currentTarget = nullptr;
   switch (mTargetingMode)
   {
   case wkf::TargetingMode::cMANUAL:
   {
      currentTarget = mCurrentManualTarget;
      break;
   }
   case wkf::TargetingMode::cTARGETS:
   {
      currentTarget = mCurrentPrioritizedTarget;
      break;
   }
   case wkf::TargetingMode::cTHREATS:
   {
      currentTarget = mCurrentPrioritizedThreat;
   }
   default:
   {
      break;
   }
   }

   if (currentTarget != nullptr)
   {
      wkf::TeamVisibilityPrefObject* teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (teamVisPrefs != nullptr)
      {
         primaryColor   = teamVisPrefs->GetTeamUtColor(currentTarget->GetSide());
         secondaryColor = AlterColor(primaryColor, 0.5);
      }
   }

   aPrimaryColor   = primaryColor;
   aSecondaryColor = secondaryColor;
}

wkf::OverlayObject::OverlayObject(wkf::OverlayObject* aParent)
   : QObject()
   , mParent(aParent)
   , mPosition(Point(0, 0))
   , mWidth(0)
   , mHeight(0)
   , mScalar(1)
{
}

// Repositions the object based on its offset if it has a parent.
void wkf::OverlayObject::Resize(double aScalar)
{
   if (mParent != nullptr)
   {
      Point scaledOffset = Point(mOffset.mX * aScalar, mOffset.mY * aScalar);
      Point newPos = Point(mParent->GetPosition().mX + scaledOffset.mX, mParent->GetPosition().mY + scaledOffset.mY);
      SetPosition(newPos);
   }

   mScalar = aScalar;
}

// Returns true if the mouse position is within the bounds of the object.
bool wkf::OverlayObject::SelectableAt(int aMouseX, int aMouseY) const
{
   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   return (xPos <= aMouseX && aMouseX <= (xPos + width) && yPos <= aMouseY && aMouseY <= (yPos + height));
}

wkf::DataBlock::DataBlock(OverlayObject* aParent)
   : OverlayObject(aParent)
{
}

// Calls update for each child object.
void wkf::DataBlock::Update()
{
   for (auto& overlayObj : mObjects)
   {
      overlayObj.second->Update();
   }
}

// Sets own colors plus children
void wkf::DataBlock::Recolor(const UtColor& aPrimary, const UtColor& aSecondary)
{
   mPrimaryColor   = aPrimary;
   mSecondaryColor = aSecondary;

   for (auto& overlayObj : mObjects)
   {
      overlayObj.second->Recolor(aPrimary, aSecondary);
   }
}

// Only sets own colors
void wkf::DataBlock::SetColor(const UtColor& aPrimary, const UtColor& aSecondary)
{
   mPrimaryColor   = aPrimary;
   mSecondaryColor = aSecondary;
}

// Draws the background box of the data block, as well as all contained objects.
void wkf::DataBlock::Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const
{
   // Draw the background box.
   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   osg::ref_ptr<osg::Vec2Array> backgroundPoints(new osg::Vec2Array());
   backgroundPoints->push_back(osg::Vec2d(xPos, yPos));
   backgroundPoints->push_back(osg::Vec2d(xPos, yPos + height));
   backgroundPoints->push_back(osg::Vec2d(xPos + width, yPos + height));
   backgroundPoints->push_back(osg::Vec2d(xPos + width, yPos));

   aGeodePtr->addDrawable(CreateGeometry(backgroundPoints, mPrimaryColor, GL_LINE_LOOP));
   aGeodePtr->addDrawable(CreateGeometry(backgroundPoints, mSecondaryColor, GL_POLYGON));

   // Draw each of the objects.
   for (const auto& overlayObj : mObjects)
   {
      overlayObj.second->Draw(aGeodePtr);
   }
}

// Calls resize for each child object.
void wkf::DataBlock::Resize(double aScalar)
{
   for (auto& obj : mObjects)
   {
      obj.second->Resize(aScalar);
   }

   OverlayObject::Resize(aScalar);
}

// Returns true if any of the OverlayObjects are selectable at the given position.
bool wkf::DataBlock::SelectableAt(int aMouseX, int aMouseY) const
{
   bool success = false;
   for (const auto& overlayObj : mObjects)
   {
      if (overlayObj.second->SelectableAt(aMouseX, aMouseY))
      {
         success = true;
      }
   }

   return success;
}

// Returns true if any of the OverlayObjects utilized the click.
bool wkf::DataBlock::Btn1Click(int aMouseX, int aMouseY)
{
   bool success = false;
   for (auto& overlayObj : mObjects)
   {
      if (overlayObj.second->Btn1Click(aMouseX, aMouseY))
      {
         success = true;
      }
   }

   return success;
}

// Returns true if any of the OverlayObjects utilized the left mouse button being pressed.
bool wkf::DataBlock::Btn1Down(int aMouseX, int aMouseY)
{
   bool success = false;
   for (auto& overlayObj : mObjects)
   {
      if (overlayObj.second->Btn1Down(aMouseX, aMouseY))
      {
         success = true;
      }
   }

   return success;
}

// Returns true if any of the OverlayObjects utilized the left mouse button being released.
bool wkf::DataBlock::Btn1Up(int aMouseX, int aMouseY)
{
   bool success = false;
   for (auto& overlayObj : mObjects)
   {
      if (overlayObj.second->Btn1Up(aMouseX, aMouseY))
      {
         success = true;
      }
   }

   return success;
}

// Returns true if any of the OverlayObjects utilized the double click.
bool wkf::DataBlock::Btn1DblClick(int aMouseX, int aMouseY)
{
   bool success = false;
   for (auto& overlayObj : mObjects)
   {
      if (overlayObj.second->Btn1DblClick(aMouseX, aMouseY))
      {
         success = true;
      }
   }

   return success;
}

// Returns true if any of the OverlayObjects utilized the scrolling.
bool wkf::DataBlock::MouseWheel(int aMouseX, int aMouseY, int aDelta)
{
   bool success = false;
   for (auto& overlayObj : mObjects)
   {
      if (overlayObj.second->MouseWheel(aMouseX, aMouseY, aDelta))
      {
         success = true;
      }
   }

   return success;
}

// Removes all OverlayObjects from the DataBlock.
void wkf::DataBlock::Clear()
{
   mObjects.clear();
}

// Adds an OverlayObject to the DataBlock.
void wkf::DataBlock::AddObject(const std::string& aObjectID, std::unique_ptr<OverlayObject> aObject)
{
   aObject->SetParent(this);
   mObjects.emplace(aObjectID, std::move(aObject));
}

// Removes the OverlayObject that corresponds with aObjectID.
void wkf::DataBlock::RemoveObject(const std::string& aObjectID)
{
   auto object = mObjects.find(aObjectID);
   if (object != mObjects.end())
   {
      mObjects.erase(object);
   }
}

// Returns an OverlayObject given an ID.
wkf::OverlayObject* wkf::DataBlock::GetObject(const std::string& aObjectID)
{
   auto foundObject = mObjects.find(aObjectID);
   if (foundObject != mObjects.end())
   {
      return foundObject->second.get();
   }
   else
   {
      return nullptr;
   }
}

// Returns a vector of objects that correspond to a certain type.
template<class T>
std::vector<T*> wkf::DataBlock::GetObjectsOfType()
{
   std::vector<T*> retVec;

   for (auto& obj : mObjects)
   {
      if (dynamic_cast<T*>(obj.second.get()) != nullptr)
      {
         retVec.emplace_back(dynamic_cast<T*>(obj.second.get()));
      }
   }

   return retVec;
}

wkf::Label::Label(OverlayObject* aParent)
   : OverlayObject(aParent)
   , mAlignment(osgText::Text::AlignmentType::LEFT_CENTER)
   , mFontColor(1.0, 1.0, 1.0, 1.0)
   , mFontSize(10)
{
   std::string fontPath;
   vespa::VaUtils::GetPathPrioritizedFilename("fonts/DejaVuSansMono.ttf", fontPath);
   mFontPtr = osgText::readFontFile(fontPath);
}

// Draws the label and the value.
void wkf::Label::Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const
{
   osg::ref_ptr<osgText::Text> label =
      CreateText(GetPosition(), mFontSize, mFontResolution, mFontPtr, mAlignment, mFontColor, mText);
   aGeodePtr->addDrawable(label);
}

void wkf::Label::Resize(double aScalar)
{
   mFontResolution = aScalar;
   OverlayObject::Resize(aScalar);
}

wkf::ProgressBar::ProgressBar(OverlayObject* aParent)
   : OverlayObject(aParent)
   , mLineColor(UtColor(0.0, 1.0, 0.0, 1.0))
   , mFillColor(UtColor(0.0, 1.0, 0.0, 1.0))
   , mDangerColor(UtColor(1.0, 0.0, 0.0, 1.0))
   , mTargetValue(0)
   , mCurrentValue(0)
   , mSpeed(0.3)
   , mLevel(0)
   , mOrientation(Orientation::cHORIZONTAL)
{
}

// Updates the progressbar value and any tick mark labels.
void wkf::ProgressBar::Update()
{
   UpdateValue();

   for (auto& tickmark : mTickMarks)
   {
      tickmark.second.mLabel->Update();
   }
}

// Resizes the progressbar and any tick mark labels.
void wkf::ProgressBar::Resize(double aScalar)
{
   for (auto& tickmark : mTickMarks)
   {
      tickmark.second.mLabel->Resize(aScalar);
   }

   OverlayObject::Resize(aScalar);
}

// Changes the line color based on the primary color given.
void wkf::ProgressBar::Recolor(const UtColor& aPrimaryColor, const UtColor& aSecondaryColor)
{
   mLineColor = aPrimaryColor;
}

// Draws the progress bar on the given geode based on the current value, not the target value.
void wkf::ProgressBar::Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const
{
   osg::ref_ptr<osg::Vec2Array> barPoints(new osg::Vec2Array());
   osg::ref_ptr<osg::Vec2Array> fillPoints(new osg::Vec2Array());

   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   // Draw the filled portion of the progressbar.
   switch (mOrientation)
   {
   case wkf::ProgressBar::Orientation::cVERTICAL:
   {
      fillPoints->push_back(osg::Vec2d(xPos, yPos));
      fillPoints->push_back(osg::Vec2d(xPos, yPos + (height * std::abs(mCurrentValue))));
      fillPoints->push_back(osg::Vec2d(xPos + width, yPos + (height * std::abs(mCurrentValue))));
      fillPoints->push_back(osg::Vec2d(xPos + width, yPos));
      break;
   }
   case wkf::ProgressBar::Orientation::cHORIZONTAL:
   {
      fillPoints->push_back(osg::Vec2d(xPos, yPos));
      fillPoints->push_back(osg::Vec2d(xPos, yPos + height));
      fillPoints->push_back(osg::Vec2d(xPos + (width * std::abs(mCurrentValue)), yPos + height));
      fillPoints->push_back(osg::Vec2d(xPos + (width * std::abs(mCurrentValue)), yPos));
      break;
   }
   default:
   {
      break;
   }
   }

   aGeodePtr->addDrawable(CreateGeometry(fillPoints, mCurrentValue >= 0 ? mFillColor : mDangerColor, GL_POLYGON));

   // Draw the outline of the progressbar.
   barPoints->push_back(osg::Vec2d(xPos, yPos));
   barPoints->push_back(osg::Vec2d(xPos, yPos + height));
   barPoints->push_back(osg::Vec2d(xPos + width, yPos + height));
   barPoints->push_back(osg::Vec2d(xPos + width, yPos));

   aGeodePtr->addDrawable(CreateGeometry(barPoints, mLineColor, GL_LINE_LOOP));

   // Draw any tick marks.
   for (const auto& tickMark : mTickMarks)
   {
      if (tickMark.second.mEnabled)
      {
         osg::ref_ptr<osg::Vec2Array> tickMarkPoints(new osg::Vec2Array());

         double value = tickMark.second.mValue;
         double x     = xPos + (width * value);

         if (value > 1.0)
         {
            value = 1.0;
            x     = xPos + (width * value);
            tickMarkPoints->push_back(osg::Vec2d(x, yPos + height));
            tickMarkPoints->push_back(osg::Vec2d(x + height / 2, yPos + height / 2));
            tickMarkPoints->push_back(osg::Vec2d(x, yPos));
            tickMarkPoints->push_back(osg::Vec2d(x, yPos - 8 * (tickMark.second.mLevel - 1)));
         }
         else
         {
            tickMarkPoints->push_back(osg::Vec2d(x, yPos + height));
            tickMarkPoints->push_back(osg::Vec2d(x, yPos - 8 * (tickMark.second.mLevel - 1)));
         }

         aGeodePtr->addDrawable(CreateGeometry(tickMarkPoints, UtColor(1.0, 1.0, 1.0, 0.4), GL_LINE_LOOP));
         tickMark.second.mLabel->SetPosition(Point(x, yPos - 8 * tickMark.second.mLevel));

         if (value < 0.5)
         {
            tickMark.second.mLabel->SetAlignment(osgText::Text::AlignmentType::LEFT_TOP);
         }
         else
         {
            tickMark.second.mLabel->SetAlignment(osgText::Text::AlignmentType::RIGHT_TOP);
         }

         tickMark.second.mLabel->Draw(aGeodePtr);
      }
   }
}

// Interpolates the current values towards the target value based on mSpeed.
void wkf::ProgressBar::UpdateValue()
{
   if (mCurrentValue != mTargetValue)
   {
      mCurrentValue = mCurrentValue + (mTargetValue - mCurrentValue) * mSpeed;
   }
}

// Adds a tick mark, using the given name as an identifier.
void wkf::ProgressBar::AddTickMark(const std::string& aName, double aValue, int aLevel)
{
   auto tickMark = mTickMarks.find(aName);
   if (tickMark == mTickMarks.end())
   {
      TickMark t;
      t.mValue   = aValue;
      t.mLevel   = aLevel;
      t.mEnabled = true;
      t.mLabel.reset(new Label(this));
      t.mLabel->SetText(aName);
      t.mLabel->SetFontSize(7);
      mTickMarks.emplace(aName, std::move(t));
   }
   else
   {
      UpdateTickMark(aName, aValue);
   }
}

// Removes the tick mark corresponding with the given name.
void wkf::ProgressBar::RemoveTickMark(const std::string& aName)
{
   auto tickMark = mTickMarks.find(aName);
   if (tickMark != mTickMarks.end())
   {
      mTickMarks.erase(tickMark);
   }
}

// Updates the tick mark corresponding with the given name.
void wkf::ProgressBar::UpdateTickMark(const std::string& aName, double aValue)
{
   auto tickMark = mTickMarks.find(aName);
   if (tickMark != mTickMarks.end())
   {
      mTickMarks.at(aName).mValue = aValue;
   }
}

// Enables or disables the tick mark corresponding with the given name.
void wkf::ProgressBar::EnableTickMark(const std::string& aName, bool aEnabled)
{
   auto tickMark = mTickMarks.find(aName);
   if (tickMark != mTickMarks.end())
   {
      mTickMarks.at(aName).mEnabled = aEnabled;
   }
}

// Removes all tick marks.
void wkf::ProgressBar::ClearTickMarks()
{
   mTickMarks.clear();
}

wkf::Button::Button(OverlayObject* aParent)
   : OverlayObject(aParent)
   , mLineColor(0.0, 0.0, 0.0, 1.0)
   , mFillColor(0.0, 0.0, 0.0, 1.0)
   , mFontColor(1.0, 1.0, 1.0, 1.0)
   , mFontSize(10)
   , mSelected(false)
   , mSelectable(false)
{
   std::string fontPath;
   vespa::VaUtils::GetPathPrioritizedFilename("fonts/DejaVuSansMono.ttf", fontPath);
   mFontPtr = osgText::readFontFile(fontPath);
}

// Changes the line, fill and font colors based on the ones given.
void wkf::Button::Recolor(const UtColor& aPrimaryColor, const UtColor& aSecondaryColor)
{
   mLineColor = aPrimaryColor;
   // mFontColor = aPrimaryColor;
   mFillColor = aSecondaryColor;
}

// Draws the button on the given geode.
void wkf::Button::Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const
{
   int    xPos   = GetPosition().mX;
   int    yPos   = GetPosition().mY;
   double width  = GetWidth() * GetScalar();
   double height = GetHeight() * GetScalar();

   // Draw the button outline.
   osg::ref_ptr<osg::Vec2Array> buttonPoints(new osg::Vec2Array());
   buttonPoints->push_back(osg::Vec2d(xPos, yPos));
   buttonPoints->push_back(osg::Vec2d(xPos, yPos + height));
   buttonPoints->push_back(osg::Vec2d(xPos + width, yPos + height));
   buttonPoints->push_back(osg::Vec2d(xPos + width, yPos));

   aGeodePtr->addDrawable(
      CreateGeometry(buttonPoints, mSelected ? AlterColor(mFillColor, 0.7, false) : mFillColor, GL_POLYGON));
   aGeodePtr->addDrawable(
      CreateGeometry(buttonPoints, mSelected ? AlterColor(mLineColor, 0.7, false) : mLineColor, GL_LINE_LOOP));

   // Set the text location -- offset to the center of the button
   Point textPt(xPos + ut::cast_to_int(width * 0.5), yPos + ut::cast_to_int(height * 0.5));

   // Draw the button text.
   osg::ref_ptr<osgText::Text> text = CreateText(textPt,
                                                 mFontSize,
                                                 mFontResolution,
                                                 mFontPtr,
                                                 osgText::Text::AlignmentType::CENTER_CENTER,
                                                 mSelected ? AlterColor(mFontColor, 0.7, false) : mFontColor,
                                                 mText);

   aGeodePtr->addDrawable(text);
}

void wkf::Button::Resize(double aScalar)
{
   mFontResolution = aScalar;
   OverlayObject::Resize(aScalar);
}

// Toggles the selection state if the given position is within the button's boundaries.
bool wkf::Button::Btn1Click(int aMouseX, int aMouseY)
{
   bool success = false;

   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   if (xPos <= aMouseX && aMouseX <= xPos + width && yPos <= aMouseY && aMouseY <= yPos + height)
   {
      success = true;
      emit Clicked();
   }

   return success;
}

// Mutes the colors when the left mouse button is pressed.
bool wkf::Button::Btn1Down(int aMouseX, int aMouseY)
{
   bool success = false;

   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   if (xPos <= aMouseX && aMouseX <= xPos + width && yPos <= aMouseY && aMouseY <= yPos + height)
   {
      success = true;

      if (mSelectable)
      {
         mSelected = !mSelected;
      }
      else
      {
         mSelected = true;
      }
   }

   return success;
}

// Restores the colors when the left mouse button is released.
bool wkf::Button::Btn1Up(int aMouseX, int aMouseY)
{
   bool success = false;

   int xPos   = GetPosition().mX;
   int yPos   = GetPosition().mY;
   int width  = GetWidth() * GetScalar();
   int height = GetHeight() * GetScalar();

   if (xPos <= aMouseX && aMouseX <= xPos + width && yPos <= aMouseY && aMouseY <= yPos + height)
   {
      success = true;

      if (!mSelectable)
      {
         mSelected = false;
      }
   }

   return success;
}
