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

#include "WkfHDD_Dockable.hpp"

#include <QHBoxLayout>
#include <QMenu>
#include <QRect>
#include <QStringList>

#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "UtoScreenOverlayManager.hpp"
#include "UtoViewer.hpp"
#include "VaCamera.hpp"
#include "VaObserver.hpp"
#include "VaOverlayTexture.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"
#include "VaWidget.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfVtkEnvironment.hpp"
#include "hdd/WkfHDD_Overlay.hpp"
#include "hdd/WkfHDD_Page.hpp"
#include "hdd/WkfHDD_PageBlank.hpp"
#include "hdd/WkfHDD_PageEngine.hpp"
#include "hdd/WkfHDD_PageFCS.hpp"
#include "hdd/WkfHDD_PageFUEL.hpp"
#include "hdd/WkfHDD_PageHUD.hpp"
#include "hdd/WkfHDD_PageHeader.hpp"
#include "hdd/WkfHDD_PageHorizon.hpp"
#include "hdd/WkfHDD_PageICAWS.hpp"
#include "hdd/WkfHDD_PageMenu.hpp"
#include "hdd/WkfHDD_PageNav.hpp"
#include "hdd/WkfHDD_PageSMS.hpp"
#include "hdd/WkfHDD_PageTFLIR.hpp"
#include "hdd/WkfHDD_PageTWD.hpp"
#include "hdd/WkfHDD_PageTactical.hpp"
#include "hdd/WkfHDD_TabButton.hpp"
#include "hdd/WkfHDD_Viewer.hpp"

HDD::Dockable::Dockable(vespa::VaEntity* aEntityPtr,
                        int              aWidth,
                        int              aHeight,
                        QStringList      aPages,
                        QMainWindow*     aParent /*= 0*/,
                        Qt::WindowFlags  aFlags /*= 0*/)
   : wkf::DockWidget("WkfHDD", aParent, aFlags)
{
   mConnectedPlatformName = aEntityPtr->GetName();

   setVisible(true);
   setFixedSize(aWidth, aHeight);
   setAttribute(Qt::WA_DeleteOnClose);

   setFocusPolicy(Qt::StrongFocus);

   mViewerPtr                = new Viewer(vespa::VaViewer::SimpleViewer, vespa::VaViewer::SmallFeatureCulling);
   vespa::VaWidget* vaWidget = new vespa::VaWidget(mViewerPtr, this);
   setWidget(vaWidget);

   mEntityPtr = aEntityPtr;

   // Header page
   mHeader = ut::make_unique<PageHeader>(mViewerPtr, 0, this);

   // Get the pages to load from the prefs
   for (size_t i = 1; i <= 12; i++)
   {
      mPageMap[i].reset(NewPage(aPages[i - 1].toStdString(), i));
   }

   // Declare these to save space
   int w = mViewerPtr->GetWidth();
   int h = mViewerPtr->GetHeight();

   // Create the arrow buttons that will control the expanding and shrinking of pages.
   mButtons.emplace(0, ArrowButton("hdd_button0", ArrowButton::Down, mViewerPtr, QRectF(0.0f, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(
      1,
      ArrowButton("hdd_button1", ArrowButton::Right, mViewerPtr, QRectF((0.25f - 0.03f) * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(2,
                    ArrowButton("hdd_button2", ArrowButton::Left, mViewerPtr, QRectF(0.25f * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(
      3,
      ArrowButton("hdd_button3", ArrowButton::Down, mViewerPtr, QRectF((0.5f - 0.03f) * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(4,
                    ArrowButton("hdd_button4", ArrowButton::Down, mViewerPtr, QRectF(0.5f * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(
      5,
      ArrowButton("hdd_button5", ArrowButton::Right, mViewerPtr, QRectF((0.75f - 0.03f) * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(6,
                    ArrowButton("hdd_button6", ArrowButton::Left, mViewerPtr, QRectF(0.75f * w, 0.0f, 0.03f * w, 0.03f * w)));
   mButtons.emplace(
      7,
      ArrowButton("hdd_button7", ArrowButton::Down, mViewerPtr, QRectF((1.0f - 0.03f) * w, 0.0f, 0.03f * w, 0.03f * w)));
   // Set the button click action for each button
   for (auto& button : mButtons)
   {
      button.second.SetDrawFunction(std::bind(&ArrowButton::Draw, std::ref(button.second)));
      button.second.SetButton1ClickAction(
         [this, button](int x, int y, int s) -> bool
         {
            if (button.second.IsVisible())
            {
               this->PushButton(button.first);
            }
            return false;
         });
   }

   // Menu Buttons
   mMenuButtons.emplace(1,
                        MenuButton("menu_button1",
                                   mPageMap[1].get(),
                                   mViewerPtr,
                                   new PageMenu(mViewerPtr, 1, this),
                                   QRectF(0.00f * w, (0.85f * h) - (0.05f * w), 0.05f * w, 0.05f * w)));
   mMenuButtons.emplace(2,
                        MenuButton("menu_button2",
                                   mPageMap[2].get(),
                                   mViewerPtr,
                                   new PageMenu(mViewerPtr, 2, this),
                                   QRectF(0.25f * w, (0.85f * h) - (0.05f * w), 0.05f * w, 0.05f * w)));
   mMenuButtons.emplace(3,
                        MenuButton("menu_button3",
                                   mPageMap[3].get(),
                                   mViewerPtr,
                                   new PageMenu(mViewerPtr, 3, this),
                                   QRectF(0.50f * w, (0.85f * h) - (0.05f * w), 0.05f * w, 0.05f * w)));
   mMenuButtons.emplace(4,
                        MenuButton("menu_button4",
                                   mPageMap[4].get(),
                                   mViewerPtr,
                                   new PageMenu(mViewerPtr, 4, this),
                                   QRectF(0.75f * w, (0.85f * h) - (0.05f * w), 0.05f * w, 0.05f * w)));

   // Set the button click action for each menu button
   for (auto& menuButton : mMenuButtons)
   {
      menuButton.second.SetDrawFunction(std::bind(&MenuButton::Draw, std::ref(menuButton.second)));
      menuButton.second.SetButton1ClickAction(
         [this, menuButton](int x, int y, int s) -> bool
         {
            if (menuButton.second.IsVisible())
            {
               this->PushMenuButton(menuButton.first);
            }
            return false;
         });
   }

   // Section 1 tabs
   mTabs[1] =
      std::make_shared<TabButton>("tab1", mPageMap[1].get(), mViewerPtr, QRectF(0.05f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[5] =
      std::make_shared<TabButton>("tab5", mPageMap[5].get(), mViewerPtr, QRectF(0.10f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[6] =
      std::make_shared<TabButton>("tab6", mPageMap[6].get(), mViewerPtr, QRectF(0.15f * w, 0.0f, 0.05f * w, 0.04f * h));
   // Section 2 tabs
   mTabs[2] =
      std::make_shared<TabButton>("tab2", mPageMap[2].get(), mViewerPtr, QRectF(0.30f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[7] =
      std::make_shared<TabButton>("tab7", mPageMap[7].get(), mViewerPtr, QRectF(0.35f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[8] =
      std::make_shared<TabButton>("tab8", mPageMap[8].get(), mViewerPtr, QRectF(0.40f * w, 0.0f, 0.05f * w, 0.04f * h));
   // Section 3 tabs
   mTabs[3] =
      std::make_shared<TabButton>("tab3", mPageMap[3].get(), mViewerPtr, QRectF(0.55f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[9] =
      std::make_shared<TabButton>("tab9", mPageMap[9].get(), mViewerPtr, QRectF(0.60f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[10] =
      std::make_shared<TabButton>("tab10", mPageMap[10].get(), mViewerPtr, QRectF(0.65f * w, 0.0f, 0.05f * w, 0.04f * h));
   // Section 4 tabs
   mTabs[4] =
      std::make_shared<TabButton>("tab4", mPageMap[4].get(), mViewerPtr, QRectF(0.80f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[11] =
      std::make_shared<TabButton>("tab11", mPageMap[11].get(), mViewerPtr, QRectF(0.85f * w, 0.0f, 0.05f * w, 0.04f * h));
   mTabs[12] =
      std::make_shared<TabButton>("tab12", mPageMap[12].get(), mViewerPtr, QRectF(0.90f * w, 0.0f, 0.05f * w, 0.04f * h));

   // Set the button click action for each tab
   for (auto&& tab : mTabs)
   {
      auto tPtr = tab.second;
      tab.second->SetVisible(false);
      tab.second->SetButton1ClickAction(
         [this, tPtr](int x, int y, int s) -> bool
         {
            if (tPtr->IsVisible())
            {
               this->TabClicked(tPtr);
            }
            return false;
         });
   }

   // Set the active tab defaults.
   mActiveTab[1] = mTabs[1];
   mActiveTab[2] = mTabs[2];
   mActiveTab[3] = mTabs[3];
   mActiveTab[4] = mTabs[4];

   // If the first page is a TSD, expand it to full.
   if (mPageMap[1]->GetPageName().compare("TSD-1") == 0)
   {
      PushButton(0);
      PushButton(1);
   }

   mScreenOverlayPtr = new Overlay("hdd_screen", mViewerPtr->GetWidth(), mViewerPtr->GetHeight());
   if (mScreenOverlayPtr != nullptr)
   {
      mViewerPtr->AddOverlay(mScreenOverlayPtr);
      mScreenOverlayPtr->SetPosition(0, 0);
      mScreenOverlayPtr->SetSize(mViewerPtr->GetWidth(), mViewerPtr->GetHeight());
      mScreenOverlayPtr->SetVisible(true);
   }

   mCallbacks.Add(vespa::VaObserver::ViewerDestroyed.Connect(&Dockable::ViewerDestroyedCB, this));
}

HDD::Dockable::~Dockable() = default;

bool HDD::Dockable::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
#if defined(_DEBUG) || defined(SEE_DEBUG_OPTIONS_IN_RELEASE)
   QAction* action = new QAction(QIcon::fromTheme("graph"), "Show Scene Graph", &aMenu);
   connect(action, &QAction::triggered, this, &Dockable::SceneGraphAction);
   aMenu.addAction(action);
#endif
   return true;
}

QString HDD::Dockable::GetName() const
{
   return QString::fromStdString(mConnectedPlatformName);
}

void HDD::Dockable::SetConnectionDestroyed(const bool aState)
{
   if (mScreenOverlayPtr != nullptr)
   {
      mScreenOverlayPtr->SetConnectionDestroyed(aState);
   }
}

void HDD::Dockable::MoveButtonsToFront()
{
   // Bring all buttons, menu buttons, and tabs to the front
   // of the screen.
   for (auto& it : mButtons)
   {
      it.second.BringOverlayToFront();
   }

   for (auto& it : mMenuButtons)
   {
      it.second.BringOverlayToFront();
   }

   for (auto&& it : mTabs)
   {
      it.second->BringOverlayToFront();
   }
}

void HDD::Dockable::AddPage(const size_t aPosition, Page* aPage)
{
   aPage->SetPosition(aPosition);
   aPage->SetVisible(true);
}

HDD::Page* HDD::Dockable::NewPage(const std::string& aPageName, const size_t aPosition)
{
   if (aPageName.compare("ASR") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   if (aPageName.compare("BLANK") == 0)
   {
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("CKLST") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("CNI") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("DAS") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("DIM") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("EFI") == 0)
   {
      mHorizonDisplay = new PageHorizon(mViewerPtr, aPosition, this);
      connect(mHorizonDisplay, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mHorizonDisplay;
   }
   else if (aPageName.compare("ENG") == 0)
   {
      mENG = new PageEngine(mViewerPtr, aPosition, this);
      connect(mENG, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mENG;
   }
   else if (aPageName.compare("FCS") == 0)
   {
      PageFCS* fcs = new PageFCS(mViewerPtr, aPosition, this);
      connect(fcs, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return fcs;
   }
   else if (aPageName.compare("FTA") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("FTI") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("FUEL") == 0)
   {
      mFUEL = new PageFUEL(mViewerPtr, aPosition, this);
      connect(mFUEL, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mFUEL;
   }
   else if (aPageName.compare("HUD") == 0)
   {
      mHUD = new PageHUD(mViewerPtr, aPosition, this);
      connect(mHUD, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mHUD;
   }
   else if (aPageName.compare("ICAWS") == 0)
   {
      PageICAWS* icaws = new PageICAWS(mViewerPtr, aPosition, this);
      connect(icaws, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return icaws;
   }
   else if (aPageName.compare("NAV") == 0)
   {
      PageNav* nav = new PageNav(mViewerPtr, aPosition, this, mEntityPtr);
      connect(nav, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return nav;
   }
   else if (aPageName.compare("PWM") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("SMS") == 0)
   {
      mSMS = new PageSMS(mViewerPtr, aPosition, this);
      connect(mSMS, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mSMS;
   }
   else if (aPageName.compare("SACH") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("TFLIR") == 0)
   {
      PageTFLIR* tflir = new PageTFLIR(mViewerPtr, aPosition, this);
      connect(tflir, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return tflir;
   }
   else if (aPageName.compare("TSD-1") == 0)
   {
      mTSDs[1] = new PageTactical(mViewerPtr, aPosition, this, 1);
      connect(mTSDs[1], &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mTSDs[1];
   }
   else if (aPageName.compare("TSD-2") == 0)
   {
      mTSDs[2] = new PageTactical(mViewerPtr, aPosition, this, 2);
      connect(mTSDs[2], &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mTSDs[2];
   }
   else if (aPageName.compare("TSD-3") == 0)
   {
      mTSDs[3] = new PageTactical(mViewerPtr, aPosition, this, 3);
      connect(mTSDs[3], &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mTSDs[3];
   }
   else if (aPageName.compare("TWD") == 0)
   {
      mTWD = new PageTWD(mViewerPtr, aPosition, this);
      connect(mTWD, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return mTWD;
   }
   else if (aPageName.compare("WPN-A") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   else if (aPageName.compare("WPN-S") == 0)
   {
      // NEEDS IMPLEMENTED
      PageBlank* blank = new PageBlank(mViewerPtr, aPosition, this);
      connect(blank, &HDD::Page::PageClicked, this, &Dockable::PageClicked);
      return blank;
   }
   return nullptr;
}

void HDD::Dockable::DeletePage(const std::string& aPageName)
{
   if (aPageName.compare("ASR") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("CKLST") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("CNI") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("DAS") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("DIM") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("EFI") == 0)
   {
      delete mHorizonDisplay;
      mHorizonDisplay = nullptr;
   }
   else if (aPageName.compare("ENG") == 0)
   {
      delete mENG;
      mENG = nullptr;
   }
   else if (aPageName.compare("FCS") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("FTA") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("FTI") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("FUEL") == 0)
   {
      delete mFUEL;
      mFUEL = nullptr;
   }
   else if (aPageName.compare("HUD") == 0)
   {
      delete mHUD;
      mHUD = nullptr;
   }
   else if (aPageName.compare("ICAWS") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("NAV") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("PWM") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("SMS") == 0)
   {
      delete mSMS;
      mSMS = nullptr;
   }
   else if (aPageName.compare("SACH") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("TFLIR") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("TSD-1") == 0)
   {
      delete mTSDs[1];
      mTSDs[1] = nullptr;
   }
   else if (aPageName.compare("TSD-2") == 0)
   {
      delete mTSDs[2];
      mTSDs[2] = nullptr;
   }
   else if (aPageName.compare("TSD-3") == 0)
   {
      delete mTSDs[3];
      mTSDs[3] = nullptr;
   }
   else if (aPageName.compare("TWD") == 0)
   {
      delete mTWD;
      mTWD = nullptr;
   }
   else if (aPageName.compare("WPN-A") == 0)
   {
      // NEEDS IMPLEMENTED
   }
   else if (aPageName.compare("WPN-S") == 0)
   {
      // NEEDS IMPLEMENTED
   }
}

size_t HDD::Dockable::FindPageWithName(const std::string& aName)
{
   // Check each page to see if its name matches.
   for (auto&& page : mPageMap)
   {
      // If the name matches, return the index of the page.
      if (page.second->GetPageName().toStdString().compare(aName) == 0)
      {
         return page.first;
      }
   }
   return ut::npos;
}

void HDD::Dockable::ShowPage(int aId, bool aState)
{
   if (mPageMap.find(aId) != mPageMap.end())
   {
      mPageMap[aId]->SetVisible(aState);
   }
}

bool HDD::Dockable::PushButton(int aButton)
{
   if (aButton == 0)
   {
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         Retract(17); // Slot 1 Large -> Small
         mButtons.at(0).SetDirection(ArrowButton::Down);
         mButtons.at(1).SetDirection(ArrowButton::Right);
         mButtons.at(2).SetDirection(ArrowButton::Left);
         mButtons.at(3).SetDirection(ArrowButton::Down);
         mButtons.at(1).SetVisible(true);
         mButtons.at(2).SetVisible(true);
      }
      else if (mActiveTab[1]->GetPagePosition() == 13)
      {
         Retract(13); // Slot 1 Medium -> Small
         mButtons.at(0).SetDirection(ArrowButton::Down);
         mButtons.at(1).SetDirection(ArrowButton::Right);
      }
      else
      {
         if (mMenuButtons.at(1).IsMenuOpen())
         {
            PushMenuButton(1);
         }

         Expand(1); // Slot 1 Small -> Medium
         mButtons.at(0).SetDirection(ArrowButton::Up);
         mButtons.at(1).SetDirection(ArrowButton::Right);
      }
   }
   else if (aButton == 1)
   {
      // If the menus in this section are open, close them
      if (mMenuButtons.at(1).IsMenuOpen())
      {
         PushMenuButton(1);
      }
      if (mMenuButtons.at(2).IsMenuOpen())
      {
         PushMenuButton(2);
      }

      if (mActiveTab[1]->GetPagePosition() == 13)
      {
         Expand(13); // Slot 1 Medium -> Large
         mButtons.at(1).SetVisible(false);
         mButtons.at(2).SetVisible(false);
         mButtons.at(3).SetDirection(ArrowButton::Up);
      }
      else
      {
         Expand(17); // Slot 1 Small -> Large
         mButtons.at(0).SetDirection(ArrowButton::Up);
         mButtons.at(1).SetVisible(false);
         mButtons.at(2).SetVisible(false);
         mButtons.at(3).SetDirection(ArrowButton::Up);
      }
   }
   else if (aButton == 2)
   {
      // If the menus in this section are open, close them
      if (mMenuButtons.at(1).IsMenuOpen())
      {
         PushMenuButton(1);
      }
      if (mMenuButtons.at(2).IsMenuOpen())
      {
         PushMenuButton(2);
      }

      if (mActiveTab[2]->GetPagePosition() == 14)
      {
         Expand(14); // Slot 2 Medium -> Large
         mButtons.at(0).SetDirection(ArrowButton::Up);
         mButtons.at(1).SetVisible(false);
         mButtons.at(2).SetVisible(false);
      }
      else
      {
         Expand(18); // Slot 2 Small -> Large
         mButtons.at(0).SetDirection(ArrowButton::Up);
         mButtons.at(1).SetVisible(false);
         mButtons.at(2).SetVisible(false);
         mButtons.at(3).SetDirection(ArrowButton::Up);
      }
   }
   else if (aButton == 3)
   {
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         Retract(17); // Slot 2 Large -> Small
         mButtons.at(0).SetDirection(ArrowButton::Down);
         mButtons.at(1).SetDirection(ArrowButton::Right);
         mButtons.at(2).SetDirection(ArrowButton::Left);
         mButtons.at(3).SetDirection(ArrowButton::Down);
         mButtons.at(1).SetVisible(true);
         mButtons.at(2).SetVisible(true);
      }
      else if (mActiveTab[2]->GetPagePosition() == 14)
      {
         Retract(14); // Slot 2 Medium -> Small
         mButtons.at(2).SetDirection(ArrowButton::Left);
         mButtons.at(3).SetDirection(ArrowButton::Down);
      }
      else
      {
         if (mMenuButtons.at(2).IsMenuOpen())
         {
            PushMenuButton(2);
         }

         Expand(2); // Slot 2 Small -> Medium
         mButtons.at(2).SetDirection(ArrowButton::Left);
         mButtons.at(3).SetDirection(ArrowButton::Up);
      }
   }
   else if (aButton == 4)
   {
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         Retract(18); // Slot 3 Large -> Small
         mButtons.at(4).SetDirection(ArrowButton::Down);
         mButtons.at(5).SetDirection(ArrowButton::Right);
         mButtons.at(6).SetDirection(ArrowButton::Left);
         mButtons.at(7).SetDirection(ArrowButton::Down);
         mButtons.at(5).SetVisible(true);
         mButtons.at(6).SetVisible(true);
      }
      else if (mActiveTab[3]->GetPagePosition() == 15)
      {
         Retract(15); // Slot 3 Medium -> Small
         mButtons.at(4).SetDirection(ArrowButton::Down);
         mButtons.at(5).SetDirection(ArrowButton::Right);
      }
      else
      {
         if (mMenuButtons.at(3).IsMenuOpen())
         {
            PushMenuButton(3);
         }

         Expand(3); // Slot 3 Small -> Medium
         mButtons.at(4).SetDirection(ArrowButton::Up);
         mButtons.at(5).SetDirection(ArrowButton::Right);
      }
   }
   else if (aButton == 5)
   {
      // If the menus in this section are open, close them
      if (mMenuButtons.at(3).IsMenuOpen())
      {
         PushMenuButton(3);
      }
      if (mMenuButtons.at(4).IsMenuOpen())
      {
         PushMenuButton(4);
      }

      if (mActiveTab[3]->GetPagePosition() == 15)
      {
         Expand(15); // Slot 3 Medium -> Large
         mButtons.at(5).SetVisible(false);
         mButtons.at(6).SetVisible(false);
         mButtons.at(7).SetDirection(ArrowButton::Up);
      }
      else
      {
         Expand(19); // Slot 3 Small -> Large
         mButtons.at(4).SetDirection(ArrowButton::Up);
         mButtons.at(5).SetVisible(false);
         mButtons.at(6).SetVisible(false);
         mButtons.at(7).SetDirection(ArrowButton::Up);
      }
   }
   else if (aButton == 6)
   {
      // If the menus in this section are open, close them
      if (mMenuButtons.at(3).IsMenuOpen())
      {
         PushMenuButton(3);
      }
      if (mMenuButtons.at(4).IsMenuOpen())
      {
         PushMenuButton(4);
      }

      if (mActiveTab[4]->GetPagePosition() == 16)
      {
         Expand(16); // Slot 4 Medium -> Large
         mButtons.at(4).SetDirection(ArrowButton::Up);
         mButtons.at(5).SetVisible(false);
         mButtons.at(6).SetVisible(false);
      }
      else
      {
         Expand(20); // Slot 4 Small -> Large
         mButtons.at(4).SetDirection(ArrowButton::Up);
         mButtons.at(5).SetVisible(false);
         mButtons.at(6).SetVisible(false);
         mButtons.at(7).SetDirection(ArrowButton::Up);
      }
   }
   else if (aButton == 7)
   {
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         Retract(18); // Slot 4 Large -> Small
         mButtons.at(4).SetDirection(ArrowButton::Down);
         mButtons.at(5).SetDirection(ArrowButton::Right);
         mButtons.at(6).SetDirection(ArrowButton::Left);
         mButtons.at(7).SetDirection(ArrowButton::Down);
         mButtons.at(5).SetVisible(true);
         mButtons.at(6).SetVisible(true);
      }
      else if (mActiveTab[4]->GetPagePosition() == 16)
      {
         Retract(16); // Slot 4 Medium -> Small
         mButtons.at(6).SetDirection(ArrowButton::Left);
         mButtons.at(7).SetDirection(ArrowButton::Down);
      }
      else
      {
         if (mMenuButtons.at(4).IsMenuOpen())
         {
            PushMenuButton(4);
         }
         Expand(4); // Slot 4 Small -> Medium
         mButtons.at(6).SetDirection(ArrowButton::Left);
         mButtons.at(7).SetDirection(ArrowButton::Up);
      }
   }
   MoveButtonsToFront();
   return false;
}

void HDD::Dockable::PushMenuButton(int aButton)
{
   bool menuOpen = mMenuButtons.at(aButton).ToggleMenu();

   // Handle hiding button/tabs and resizing the menu.
   // We'll need to resize the menu overlay to match whatever layout may be active.
   switch (aButton)
   {
   case 1:
      // If our page is in the Full position.
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         mMenuButtons.at(aButton).SetMenuPosition(17);
         SetSectionButtonVisibility(17, !menuOpen);
      }
      // If our page is in the Large position.
      else if (mActiveTab[1]->GetPagePosition() == 13)
      {
         mMenuButtons.at(aButton).SetMenuPosition(13);
         SetSectionButtonVisibility(13, !menuOpen);
      }
      // If our page is in the Small/Medium position.
      else
      {
         mMenuButtons.at(aButton).SetMenuPosition(aButton);
      }
      break;
   case 2:
      // If our page is in the Full position.
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         mMenuButtons.at(aButton).SetMenuPosition(17);
         SetSectionButtonVisibility(17, !menuOpen);
      }
      // If our page is in the Large position.
      else if (mActiveTab[2]->GetPagePosition() == 14)
      {
         mMenuButtons.at(aButton).SetMenuPosition(14);
         SetSectionButtonVisibility(14, !menuOpen);
      }
      // If our page is in the Small/Medium position.
      else
      {
         mMenuButtons.at(aButton).SetMenuPosition(aButton);
      }
      break;
   case 3:
      // If our page is in the Full position.
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         mMenuButtons.at(aButton).SetMenuPosition(18);
         SetSectionButtonVisibility(18, !menuOpen);
      }
      // If our page is in the Large position.
      else if (mActiveTab[3]->GetPagePosition() == 15)
      {
         mMenuButtons.at(aButton).SetMenuPosition(15);
         SetSectionButtonVisibility(15, !menuOpen);
      }
      // If our page is in the Small/Medium position.
      else
      {
         mMenuButtons.at(aButton).SetMenuPosition(aButton);
      }
      break;
   case 4:
      // If our page is in the Full position.
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         mMenuButtons.at(aButton).SetMenuPosition(18);
         SetSectionButtonVisibility(18, !menuOpen);
      }
      // If our page is in the Large position.
      else if (mActiveTab[4]->GetPagePosition() == 16)
      {
         mMenuButtons.at(aButton).SetMenuPosition(16);
         SetSectionButtonVisibility(16, !menuOpen);
      }
      // If our page is in the Small/Medium position.
      else
      {
         mMenuButtons.at(aButton).SetMenuPosition(aButton);
      }
      break;
   }

   if (menuOpen)
   {
      mMenuButtons.at(aButton).SetPageVisible(false);
      mMenuButtons.at(aButton).SetMenuVisible(true);
   }
   else
   {
      mMenuButtons.at(aButton).SetPageVisible(true);
      mMenuButtons.at(aButton).SetMenuVisible(false);
      mMenuButtons.at(aButton).SetMenuPosition(aButton);
   }
}

void HDD::Dockable::MenuPageSelected(const size_t aPosition, const std::string& aPageName)
{
   // Attempt to find an existing page with aPageName.
   size_t                pageIndex = FindPageWithName(aPageName);
   std::unique_ptr<Page> page      = nullptr;

   // Get the button/active tab section
   size_t buttonPos     = GetPageSection(aPosition);
   size_t menuButtonPos = GetMenuButton(aPosition);


   // If the user picks the page they're already viewing,
   // do nothing.
   if (pageIndex == aPosition)
   {
      // Close the menu
      PushMenuButton(menuButtonPos);

      // Move the button overlays back to the front.
      MoveButtonsToFront();
      return;
   }

   // Remove the page underneath
   std::string tmpName = mPageMap[aPosition]->GetPageName().toStdString();
   mPageMap[aPosition]->RemoveOverlay();
   mPageMap[aPosition].release();
   DeletePage(tmpName);

   // If an existing page was found, replace it's slot with
   // a blank page and move it to the menu spot chosen (aPosition).
   if (pageIndex != ut::npos)
   {
      page           = std::move(mPageMap[pageIndex]);
      size_t pagePos = page->GetPosition();
      Page*  blank(NewPage("BLANK", pagePos));
      mPageMap[pageIndex].reset(blank);
      mTabs[pageIndex]->SetPage(blank);
      size_t blankMenuPos = GetPageSection(pagePos);
      if (blankMenuPos != 0)
      {
         mMenuButtons.at(blankMenuPos).SetPage(blank);
      }
      else
      {
         SetPageVisibility(pageIndex);
      }
   }
   // If the page wasn't found, create a new one.
   else
   {
      page.reset(NewPage(aPageName, aPosition));
   }

   // If we created/retrieved a valid page.
   if (page != nullptr)
   {
      // Position the new page in the map and on screen.
      mPageMap[buttonPos] = std::move(page);
      mPageMap[buttonPos]->SetPosition(aPosition);

      mActiveTab[buttonPos]->SetPage(mPageMap[buttonPos].get());
      mMenuButtons.at(menuButtonPos).SetPage(mPageMap[buttonPos].get());
      mPageMap[buttonPos]->BringOverlayToFront();

      // Close the menu
      PushMenuButton(menuButtonPos);
   }

   // Move the button overlays back to the front.
   MoveButtonsToFront();
}

void HDD::Dockable::Expand(int aItem)
{
   switch (aItem)
   {
   case 1: // Slot 1 Small -> Medium
      // Hide all section 1 pages, and show their tabs
      mTabs[1]->SetVisible(true);
      mTabs[1]->SetPageVisible(false);
      mTabs[5]->SetVisible(true);
      mTabs[5]->SetPageVisible(false);
      mTabs[6]->SetVisible(true);
      mTabs[6]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[1]->SetVisible(false);
      mActiveTab[1]->SetPageVisible(true);
      mActiveTab[1]->SetPagePosition(13);
      break;
   case 2: // Slot 2 Small -> Medium
      // Hide all section 2 pages, and show their tabs
      mTabs[2]->SetVisible(true);
      mTabs[2]->SetPageVisible(false);
      mTabs[7]->SetVisible(true);
      mTabs[7]->SetPageVisible(false);
      mTabs[8]->SetVisible(true);
      mTabs[8]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[2]->SetVisible(false);
      mActiveTab[2]->SetPageVisible(true);
      mActiveTab[2]->SetPagePosition(14);
      break;
   case 3: // Slot 3 Small -> Medium
      // Hide all section 3 pages, and show their tabs
      mTabs[3]->SetVisible(true);
      mTabs[3]->SetPageVisible(false);
      mTabs[9]->SetVisible(true);
      mTabs[9]->SetPageVisible(false);
      mTabs[10]->SetVisible(true);
      mTabs[10]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[3]->SetVisible(false);
      mActiveTab[3]->SetPageVisible(true);
      mActiveTab[3]->SetPagePosition(15);
      break;
   case 4: // Slot 4 Small -> Medium
      // Hide all section 4 pages, and show their tabs
      mTabs[4]->SetVisible(true);
      mTabs[4]->SetPageVisible(false);
      mTabs[11]->SetVisible(true);
      mTabs[11]->SetPageVisible(false);
      mTabs[12]->SetVisible(true);
      mTabs[12]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[4]->SetVisible(false);
      mActiveTab[4]->SetPageVisible(true);
      mActiveTab[4]->SetPagePosition(16);
      break;
   case 13: // Slot 1 Medium -> Large
   case 17: // Slot 1 Small -> Large
      // Hide all section 1 and 2 pages, and show their tabs
      mTabs[1]->SetVisible(true);
      mTabs[1]->SetPageVisible(false);
      mTabs[5]->SetVisible(true);
      mTabs[5]->SetPageVisible(false);
      mTabs[6]->SetVisible(true);
      mTabs[6]->SetPageVisible(false);
      mTabs[2]->SetVisible(true);
      mTabs[2]->SetPageVisible(false);
      mTabs[7]->SetVisible(true);
      mTabs[7]->SetPageVisible(false);
      mTabs[8]->SetVisible(true);
      mTabs[8]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[1]->SetVisible(false);
      mActiveTab[1]->SetPageVisible(true);
      mActiveTab[1]->SetPagePosition(17);
      // Set the menu button page
      mMenuButtons.at(1).SetPage(mActiveTab[1]->GetPage());
      // Hide the menu button for section 2.
      mMenuButtons.at(2).SetVisible(false);
      break;
   case 14: // Slot 2 Medium -> Large
   case 18: // Slot 2 Small -> Large
      // Hide all section 1 and 2 pages, and show their tabs
      mTabs[1]->SetVisible(true);
      mTabs[1]->SetPageVisible(false);
      mTabs[5]->SetVisible(true);
      mTabs[5]->SetPageVisible(false);
      mTabs[6]->SetVisible(true);
      mTabs[6]->SetPageVisible(false);
      mTabs[2]->SetVisible(true);
      mTabs[2]->SetPageVisible(false);
      mTabs[7]->SetVisible(true);
      mTabs[7]->SetPageVisible(false);
      mTabs[8]->SetVisible(true);
      mTabs[8]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[2]->SetVisible(false);
      mActiveTab[2]->SetPageVisible(true);
      mActiveTab[2]->SetPagePosition(17);
      // Set the menu button page
      mMenuButtons.at(1).SetPage(mActiveTab[2]->GetPage());
      // Hide the menu button for section 2.
      mMenuButtons.at(2).SetVisible(false);
      break;
   case 15: // Slot 3 Medium -> Large
   case 19: // Slot 3 Small -> Large
      // Hide all section 3 and 4 pages, and show their tabs
      mTabs[3]->SetVisible(true);
      mTabs[3]->SetPageVisible(false);
      mTabs[9]->SetVisible(true);
      mTabs[9]->SetPageVisible(false);
      mTabs[10]->SetVisible(true);
      mTabs[10]->SetPageVisible(false);
      mTabs[4]->SetVisible(true);
      mTabs[4]->SetPageVisible(false);
      mTabs[11]->SetVisible(true);
      mTabs[11]->SetPageVisible(false);
      mTabs[12]->SetVisible(true);
      mTabs[12]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[3]->SetVisible(false);
      mActiveTab[3]->SetPageVisible(true);
      mActiveTab[3]->SetPagePosition(18);
      // Set the menu button page
      mMenuButtons.at(3).SetPage(mActiveTab[3]->GetPage());
      // Hide the menu button for section 4.
      mMenuButtons.at(4).SetVisible(false);
      break;
   case 16: // Slot 4 Medium -> Large
   case 20: // Slot 4 Small -> Large
      // Hide all section 3 and 4 pages, and show their tabs
      mTabs[3]->SetVisible(true);
      mTabs[3]->SetPageVisible(false);
      mTabs[9]->SetVisible(true);
      mTabs[9]->SetPageVisible(false);
      mTabs[10]->SetVisible(true);
      mTabs[10]->SetPageVisible(false);
      mTabs[4]->SetVisible(true);
      mTabs[4]->SetPageVisible(false);
      mTabs[11]->SetVisible(true);
      mTabs[11]->SetPageVisible(false);
      mTabs[12]->SetVisible(true);
      mTabs[12]->SetPageVisible(false);
      // Hide the active display tab and show the page.
      mActiveTab[4]->SetVisible(false);
      mActiveTab[4]->SetPageVisible(true);
      mActiveTab[4]->SetPagePosition(18);
      // Set the menu button page
      mMenuButtons.at(3).SetPage(mActiveTab[4]->GetPage());
      // Hide the menu button for section 4.
      mMenuButtons.at(4).SetVisible(false);
      break;
   default:
      break;
   }
   MoveButtonsToFront();
}

void HDD::Dockable::Retract(int aItem)
{
   switch (aItem)
   {
   case (13): // Slot 1 Medium -> Small
      // Show all section 1 pages, and hide their tabs
      mTabs[1]->SetVisible(false);
      mTabs[1]->SetPageVisible(true);
      mTabs[5]->SetVisible(false);
      mTabs[5]->SetPageVisible(true);
      mTabs[6]->SetVisible(false);
      mTabs[6]->SetPageVisible(true);
      // Set the active display position back to the small position.
      mActiveTab[1]->SetPagePosition(1);
      break;
   case (14): // Slot 2 Medium -> Small
      // Show all section 2 pages, and hide their tabs
      mTabs[2]->SetVisible(false);
      mTabs[2]->SetPageVisible(true);
      mTabs[7]->SetVisible(false);
      mTabs[7]->SetPageVisible(true);
      mTabs[8]->SetVisible(false);
      mTabs[8]->SetPageVisible(true);
      // Set the active display position back to the small position.
      mActiveTab[2]->SetPagePosition(2);
      break;
   case (15): // Slot 3 Medium -> Small
      // Show all section 3 pages, and hide their tabs
      mTabs[3]->SetVisible(false);
      mTabs[3]->SetPageVisible(true);
      mTabs[9]->SetVisible(false);
      mTabs[9]->SetPageVisible(true);
      mTabs[10]->SetVisible(false);
      mTabs[10]->SetPageVisible(true);
      // Set the active display position back to the small position.
      mActiveTab[3]->SetPagePosition(3);
      break;
   case (16): // Slot 4 Medium -> Small
      // Show all section 4 pages, and hide their tabs
      mTabs[4]->SetVisible(false);
      mTabs[4]->SetPageVisible(true);
      mTabs[11]->SetVisible(false);
      mTabs[11]->SetPageVisible(true);
      mTabs[12]->SetVisible(false);
      mTabs[12]->SetPageVisible(true);
      // Set the active display position back to the small position.
      mActiveTab[4]->SetPagePosition(4);
      break;
   case (17): // Slot 1/2 Large -> Small
      // Show all section 1 and 2 pages, and hide their tabs
      mTabs[1]->SetVisible(false);
      mTabs[1]->SetPageVisible(true);
      mTabs[5]->SetVisible(false);
      mTabs[5]->SetPageVisible(true);
      mTabs[6]->SetVisible(false);
      mTabs[6]->SetPageVisible(true);
      mTabs[2]->SetVisible(false);
      mTabs[2]->SetPageVisible(true);
      mTabs[7]->SetVisible(false);
      mTabs[7]->SetPageVisible(true);
      mTabs[8]->SetVisible(false);
      mTabs[8]->SetPageVisible(true);
      // Set the active display positions back to the small positions.
      mActiveTab[1]->SetPagePosition(1);
      mActiveTab[2]->SetPagePosition(2);
      // Reset the menu button pages and reset visibility of the menu
      // button at section 2
      mMenuButtons.at(1).SetPage(mActiveTab[1]->GetPage());
      mMenuButtons.at(2).SetPage(mActiveTab[2]->GetPage());
      mMenuButtons.at(2).SetVisible(true);
      break;
   case (18): // Slot 3/4 Large -> Small
      // Show all section 3 pages, and hide their tabs
      mTabs[3]->SetVisible(false);
      mTabs[3]->SetPageVisible(true);
      mTabs[9]->SetVisible(false);
      mTabs[9]->SetPageVisible(true);
      mTabs[10]->SetVisible(false);
      mTabs[10]->SetPageVisible(true);
      mTabs[4]->SetVisible(false);
      mTabs[4]->SetPageVisible(true);
      mTabs[11]->SetVisible(false);
      mTabs[11]->SetPageVisible(true);
      mTabs[12]->SetVisible(false);
      mTabs[12]->SetPageVisible(true);
      // Set the active display positions back to the small positions.
      mActiveTab[3]->SetPagePosition(3);
      mActiveTab[4]->SetPagePosition(4);
      // Reset the menu button pages and reset visibility of the menu
      // button at section 4
      mMenuButtons.at(3).SetPage(mActiveTab[3]->GetPage());
      mMenuButtons.at(4).SetPage(mActiveTab[4]->GetPage());
      mMenuButtons.at(4).SetVisible(true);
      break;
   default:
      break;
   }
   MoveButtonsToFront();
}

void HDD::Dockable::PageClicked(Page* aPage)
{
   // Only handle the button click if the page is visible.
   if (aPage->IsVisible())
   {
      size_t clickedPosition = aPage->GetPosition();
      switch (clickedPosition)
      {
      // Pages 5/6 will swap with page 1.
      case 5:
      case 6:
         if (mMenuButtons.at(1).IsMenuOpen())
         {
            PushMenuButton(1);
         }
         SwapPages(clickedPosition, 1);
         mTabs[1]->SetPage(mPageMap[1].get());
         mTabs[clickedPosition]->SetPage(mPageMap[clickedPosition].get());
         mActiveTab[1] = mTabs[1];
         mMenuButtons.at(1).SetPage(mPageMap[1].get());
         break;
      // Pages 7/8 will swap with page 2.
      case 7:
      case 8:
         if (mMenuButtons.at(2).IsMenuOpen())
         {
            PushMenuButton(2);
         }
         SwapPages(clickedPosition, 2);
         mTabs[2]->SetPage(mPageMap[2].get());
         mTabs[clickedPosition]->SetPage(mPageMap[clickedPosition].get());
         mActiveTab[2] = mTabs[2];
         mMenuButtons.at(2).SetPage(mPageMap[2].get());
         break;
      // Pages 9/10 will swap with page 3.
      case 9:
      case 10:
         if (mMenuButtons.at(3).IsMenuOpen())
         {
            PushMenuButton(3);
         }
         SwapPages(clickedPosition, 3);
         mTabs[3]->SetPage(mPageMap[3].get());
         mTabs[clickedPosition]->SetPage(mPageMap[clickedPosition].get());
         mActiveTab[3] = mTabs[3];
         mMenuButtons.at(3).SetPage(mPageMap[3].get());
         break;
      // Pages 11/12 will swap with page 4.
      case 11:
      case 12:
         if (mMenuButtons.at(4).IsMenuOpen())
         {
            PushMenuButton(4);
         }
         SwapPages(clickedPosition, 4);
         mTabs[4]->SetPage(mPageMap[4].get());
         mTabs[clickedPosition]->SetPage(mPageMap[clickedPosition].get());
         mActiveTab[4] = mTabs[4];
         mMenuButtons.at(4).SetPage(mPageMap[4].get());
         break;
      default:
         break;
      }
      MoveButtonsToFront();
   }
}

void HDD::Dockable::SwapPages(const size_t aPosition1, const size_t aPosition2)
{
   if (aPosition1 != aPosition2)
   {
      std::unique_ptr<Page> p1 = std::move(mPageMap.at(aPosition1));
      std::unique_ptr<Page> p2 = std::move(mPageMap.at(aPosition2));
      // Move and position p1.
      p1->SetPosition(aPosition2);
      mPageMap[aPosition2] = std::move(p1);
      // Move and position p2.
      p2->SetPosition(aPosition1);
      mPageMap[aPosition1] = std::move(p2);
   }
}

void HDD::Dockable::TabClicked(std::shared_ptr<TabButton> aTab)
{
   switch (aTab->GetPagePosition())
   {
   // Section 1 - Pages 1, 5, 6
   case 1:
   case 5:
   case 6:
      // If the active page (full) is in the same section, swap as normal.
      if (mActiveTab[1]->GetPagePosition() == 17) // Large
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[1];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 1);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(17);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[1] = mTabs[1]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 1.
         mMenuButtons.at(1).SetPage(mActiveTab[1]->GetPage());
      }
      // If the active page (full) is in the adjacent section, position
      // that page before making the swap.
      else if (mActiveTab[2]->GetPagePosition() == 17)
      {
         // Position the section 2 page and set visibility.
         mActiveTab[2]->SetPagePosition(2);
         mActiveTab[2]->SetPageVisible(false);
         mActiveTab[2]->SetVisible(true);

         // If we need to swap the active tab for section 1 do so
         // and then set the position and visibility.
         if (aTab != mActiveTab[1])
         {
            std::shared_ptr<TabButton> tmpTab = mActiveTab[1];

            // Swap the pages.
            SwapPages(aTab->GetPagePosition(), 1);

            // Position the clicked tabs page and set visibility.
            aTab->SetPagePosition(17);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);

            // Position the old tabs page and set visibility.
            tmpTab->SetPageVisible(false);
            tmpTab->SetVisible(true);

            // Set the active tab and swap the tabs positions in the map.
            mActiveTab[1] = mTabs[1]         = aTab;
            mTabs[tmpTab->GetPagePosition()] = tmpTab;
         }
         // If our active tab for the section doesn't need changed, set its
         // position and visibility.
         else
         {
            aTab->SetPagePosition(17);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);
         }

         // Set the menu button page for section 1.
         mMenuButtons.at(1).SetPage(mActiveTab[1]->GetPage());
      }
      // If the active page (large) is in the same section, swap as normal.
      else if (mActiveTab[1]->GetPagePosition() == 13)
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[1];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 1);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(13);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[1] = mTabs[1]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 1.
         mMenuButtons.at(1).SetPage(mActiveTab[1]->GetPage());
      }
      break;
   // Section 2 - Pages 2, 7, 8
   case 2:
   case 7:
   case 8:
      // If the active page (full) is in the same section, swap as normal.
      if (mActiveTab[2]->GetPagePosition() == 17) // Large
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[2];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 2);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(17);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[2] = mTabs[2]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 1.
         mMenuButtons.at(1).SetPage(mActiveTab[2]->GetPage());
      }
      // If the active page (full) is in the adjacent section, position
      // that page before making the swap.
      else if (mActiveTab[1]->GetPagePosition() == 17)
      {
         // Position the section 1 page and set visibility.
         mActiveTab[1]->SetPagePosition(1);
         mActiveTab[1]->SetPageVisible(false);
         mActiveTab[1]->SetVisible(true);

         // If we need to swap the active tab for section 2 do so
         // and then set the position and visibility.
         if (aTab != mActiveTab[2])
         {
            std::shared_ptr<TabButton> tmpTab = mActiveTab[2];

            // Swap the pages.
            SwapPages(aTab->GetPagePosition(), 2);

            // Position the clicked tabs page and set visibility.
            aTab->SetPagePosition(17);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);

            // Position the old tabs page and set visibility.
            tmpTab->SetPageVisible(false);
            tmpTab->SetVisible(true);

            // Set the active tab and swap the tabs positions in the map.
            mActiveTab[2] = mTabs[2]         = aTab;
            mTabs[tmpTab->GetPagePosition()] = tmpTab;
         }
         // If our active tab for the section doesn't need changed, set its
         // position and visibility.
         else
         {
            aTab->SetPagePosition(17);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);
         }

         // Set the menu button page for section 1.
         mMenuButtons.at(1).SetPage(mActiveTab[2]->GetPage());
      }
      // If the active page (large) is in the same section, swap as normal.
      else if (mActiveTab[2]->GetPagePosition() == 14)
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[2];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 2);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(14);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[2] = mTabs[2]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 2.
         mMenuButtons.at(2).SetPage(mActiveTab[2]->GetPage());
      }
      break;
   // Section 3 - Pages 3, 9, 10
   case 3:
   case 9:
   case 10:
      // If the active page (full) is in the same section, swap as normal.
      if (mActiveTab[3]->GetPagePosition() == 18) // Large
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[3];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 3);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(18);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[3] = mTabs[3]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 3.
         mMenuButtons.at(3).SetPage(mActiveTab[3]->GetPage());
      }
      // If the active page (full) is in the adjacent section, position
      // that page before making the swap.
      else if (mActiveTab[4]->GetPagePosition() == 18)
      {
         // Position the section 4 page and set visibility.
         mActiveTab[4]->SetPagePosition(4);
         mActiveTab[4]->SetPageVisible(false);
         mActiveTab[4]->SetVisible(true);

         // If we need to swap the active tab for section 3 do so
         // and then set the position and visibility.
         if (aTab != mActiveTab[3])
         {
            std::shared_ptr<TabButton> tmpTab = mActiveTab[3];

            // Swap the pages.
            SwapPages(aTab->GetPagePosition(), 3);

            // Position the clicked tabs page and set visibility.
            aTab->SetPagePosition(18);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);

            // Position the old tabs page and set visibility.
            tmpTab->SetPageVisible(false);
            tmpTab->SetVisible(true);

            // Set the active tab and swap the tabs positions in the map.
            mActiveTab[3] = mTabs[3]         = aTab;
            mTabs[tmpTab->GetPagePosition()] = tmpTab;
         }
         // If our active tab for the section doesn't need changed, set its
         // position and visiblity.
         else
         {
            aTab->SetPagePosition(18);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);
         }

         // Set the page for the menu button at section 3.
         mMenuButtons.at(3).SetPage(mActiveTab[3]->GetPage());
      }
      // If the active page (large) is in the same section, swap as normal.
      else if (mActiveTab[3]->GetPagePosition() == 15)
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[3];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 3);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(15);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[3] = mTabs[3]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 3.
         mMenuButtons.at(3).SetPage(mActiveTab[3]->GetPage());
      }
      break;
   // Section 4 - Pages 4, 11, 12
   case 4:
   case 11:
   case 12:
      // If the active page (full) is in the same section, swap as normal.
      if (mActiveTab[4]->GetPagePosition() == 18) // Large
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[4];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 4);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(18);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[4] = mTabs[4]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 3.
         mMenuButtons.at(3).SetPage(mActiveTab[4]->GetPage());
      }
      // If the active page (full) is in the adjacent section, position
      // that page before making the swap.
      else if (mActiveTab[3]->GetPagePosition() == 18)
      {
         // Position the section 3 page and set visibility.
         mActiveTab[3]->SetPagePosition(3);
         mActiveTab[3]->SetPageVisible(false);
         mActiveTab[3]->SetVisible(true);

         // If we need to swap the active tab for section 4 do so
         // and then set the position and visibility.
         if (aTab != mActiveTab[4])
         {
            std::shared_ptr<TabButton> tmpTab = mActiveTab[4];

            // Swap the pages.
            SwapPages(aTab->GetPagePosition(), 4);

            // Position the clicked tabs page and set visibility.
            aTab->SetPagePosition(18);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);

            // Position the old tabs page and set visibility.
            tmpTab->SetPageVisible(false);
            tmpTab->SetVisible(true);

            // Set the active tab and swap the tabs positions in the map.
            mActiveTab[4] = mTabs[4]         = aTab;
            mTabs[tmpTab->GetPagePosition()] = tmpTab;
         }
         // If our active tab for the section doesn't need changed, set its
         // position and visiblity.
         else
         {
            aTab->SetPagePosition(18);
            aTab->SetPageVisible(true);
            aTab->SetVisible(false);
         }

         // Set the page for the menu button at section 3.
         mMenuButtons.at(3).SetPage(mActiveTab[4]->GetPage());
      }
      // If the active page (large) is in the same section, swap as normal.
      else if (mActiveTab[4]->GetPagePosition() == 16)
      {
         std::shared_ptr<TabButton> tmpTab = mActiveTab[4];

         // Swap the pages.
         SwapPages(aTab->GetPagePosition(), 4);

         // Position the clicked tabs page and set visibility.
         aTab->SetPagePosition(16);
         aTab->SetPageVisible(true);
         aTab->SetVisible(false);

         // Position the old tabs page and set visibility.
         tmpTab->SetPageVisible(false);
         tmpTab->SetVisible(true);

         // Set the active tab and swap the tabs positions in the map.
         mActiveTab[4] = mTabs[4]         = aTab;
         mTabs[tmpTab->GetPagePosition()] = tmpTab;

         // Set the page for the menu button at section 4.
         mMenuButtons.at(4).SetPage(mActiveTab[4]->GetPage());
      }
      break;
   default:
      break;
   }
}

size_t HDD::Dockable::GetMenuButton(const size_t aPosition)
{
   switch (aPosition)
   {
   case 1:
   case 5:
   case 6:
   case 13:
   case 17:
      return 1;
   case 2:
   case 7:
   case 8:
   case 14:
      return 2;
   case 3:
   case 9:
   case 10:
   case 15:
   case 18:
      return 3;
   case 4:
   case 11:
   case 12:
   case 16:
      return 4;
   default:
      return 0;
   }
}

size_t HDD::Dockable::GetPageSection(const size_t aPosition)
{
   switch (aPosition)
   {
   case 1:
   case 13:
      return 1;
   case 2:
   case 14:
      return 2;
   case 3:
   case 15:
      return 3;
   case 4:
   case 16:
      return 4;
   case 17:
      if (mActiveTab[1]->GetPagePosition() == 17)
      {
         return 1;
      }
      else
      {
         return 2;
      }
   case 18:
      if (mActiveTab[3]->GetPagePosition() == 18)
      {
         return 3;
      }
      else
      {
         return 4;
      }
   default:
      return 0;
   }
}

void HDD::Dockable::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](mViewerPtr);
}

void HDD::Dockable::SetTrackProcessorData(const std::vector<SimData::TrackProcessor>& aData,
                                          const std::string                           aESM_TrackProcessor)
{
   if (mTWD)
   {
      mTWD->Set_ESM_RWR_TrackData(aData, aESM_TrackProcessor);
   }
}

void HDD::Dockable::SetFlightControlsData(const SimData::FlightControlsData& aData)
{
   if (mHeader)
   {
      mHeader->SetLandingGear(aData.landingGear);
      mHeader->SetThrustThrottle(aData.thrustNormalized, aData.throttleValid, aData.throttleInput);
   }
   if (mENG)
   {
      mENG->SetThrustThrottle(aData.thrustNormalized, aData.throttleValid, aData.throttleInput);
   }
}

void HDD::Dockable::SetNavData(const SimData::NavData& aData)
{
   for (auto&& tsd : mTSDs)
   {
      if (tsd.second)
      {
         tsd.second->SetNavData(aData);
      }
   }
}

void HDD::Dockable::SetWeaponData(const SimData::WeaponDataSummary& aData)
{
   if (mSMS)
   {
      mSMS->SetWeaponData(aData);
   }
   if (mHeader)
   {
      mHeader->SetWeaponData(aData);
   }
   if (mTWD)
   {
      mTWD->SetCountermeasures(aData.numChaff, aData.numFlares, aData.numDecoys);
   }
   for (auto&& tsd : mTSDs)
   {
      if (tsd.second)
      {
         tsd.second->SetWeaponSelectedAndArmed(aData.selectedWeapon, aData.numSelectedWeapon, aData.masterArmActive);
      }
   }
}

void HDD::Dockable::SetFuelData(const SimData::FuelData& aData)
{
   if (mHeader)
   {
      mHeader->SetFuelData(aData);
   }
   if (mFUEL)
   {
      mFUEL->SetFuelData(aData);
   }
   if (mSMS)
   {
      mSMS->SetExternalFuelTankCount(aData.numExtTanks);
   }
   if (mENG)
   {
      mENG->SetFuelFlow(aData.fuelFlow_pph);
   }
}

void HDD::Dockable::SetPlatformData(const HDD::SimData::PlatformData& aData)
{
   if (mHeader)
   {
      // Flight Data Box
      mHeader->SetAltitude(aData.altBaro_ft);
      mHeader->SetHeading(aData.heading_deg);
      mHeader->SetKTAS(aData.ktas);

      // Time Box
      mHeader->SetWallTime(aData.simTime);

      // ICAWS Box
      mHeader->SetStalling(aData.stallWarning);
   }
   if (mFUEL)
   {
      mFUEL->SetKTAS(aData.ktas);
   }
   if (mHUD)
   {
      mHUD->SetHeading_deg(aData.heading_deg);
      mHUD->SetKTAS(aData.ktas);
      mHUD->SetAltitude_ft(aData.altBaro_ft);
   }
   if (mTWD)
   {
      mTWD->SetSimTime(aData.simTime);
      mTWD->SetOwnshipHeading_deg(aData.heading_deg);
   }
   for (auto&& tsd : mTSDs)
   {
      if (tsd.second)
      {
         tsd.second->SetLatLon(aData.lat_deg, aData.lon_deg);
         tsd.second->SetOwnshipHeading_deg(aData.heading_deg);
         tsd.second->SetFlightId(aData.flightId);
      }
   }
   if (mHorizonDisplay)
   {
      mHorizonDisplay->SetOwnshipPitchAndRoll_deg(aData.pitch_deg, aData.roll_deg);
   }
}

void HDD::Dockable::SetTrackData(const HDD::SimData::TargetTrackItemList& aData)
{
   for (auto&& tsd : mTSDs)
   {
      if (tsd.second)
      {
         tsd.second->SetTrackData(aData);
      }
   }
}

void HDD::Dockable::SetAssetsListData(const SimData::AssetsList& aData)
{
   for (auto&& tsd : mTSDs)
   {
      if (tsd.second)
      {
         tsd.second->SetAssetsListData(aData);
      }
   }
}

void HDD::Dockable::SetPageVisibility(const size_t aPosition)
{
   switch (aPosition)
   {
   case 1:
   case 5:
   case 6:
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      else if (mActiveTab[1]->GetPagePosition() == 13)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      break;
   case 2:
   case 7:
   case 8:
      if (mActiveTab[1]->GetPagePosition() == 17 || mActiveTab[2]->GetPagePosition() == 17)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      else if (mActiveTab[2]->GetPagePosition() == 14)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      break;
   case 3:
   case 9:
   case 10:
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      else if (mActiveTab[3]->GetPagePosition() == 15)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      break;
   case 4:
   case 11:
   case 12:
      if (mActiveTab[3]->GetPagePosition() == 18 || mActiveTab[4]->GetPagePosition() == 18)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      else if (mActiveTab[4]->GetPagePosition() == 16)
      {
         mPageMap[aPosition]->SetVisible(false);
      }
      break;
   default:
      break;
   }
}

void HDD::Dockable::SetSectionButtonVisibility(const size_t aPosition, const bool aState)
{
   switch (aPosition)
   {
   // Position 13 is section 1
   case 13:
      mTabs[1]->SetVisible(aState);
      mTabs[5]->SetVisible(aState);
      mTabs[6]->SetVisible(aState);
      if (aState)
      {
         mActiveTab[1]->SetVisible(false);
      }
      mButtons.at(0).SetVisible(aState);
      mButtons.at(1).SetVisible(aState);
      break;
   // Position 14 is section 2
   case 14:
      mTabs[2]->SetVisible(aState);
      mTabs[7]->SetVisible(aState);
      mTabs[8]->SetVisible(aState);
      if (aState)
      {
         mActiveTab[2]->SetVisible(false);
      }
      mButtons.at(2).SetVisible(aState);
      mButtons.at(3).SetVisible(aState);
      break;
   // Position 15 is section 3
   case 15:
      mTabs[3]->SetVisible(aState);
      mTabs[9]->SetVisible(aState);
      mTabs[10]->SetVisible(aState);
      if (aState)
      {
         mActiveTab[3]->SetVisible(false);
      }
      mButtons.at(4).SetVisible(aState);
      mButtons.at(5).SetVisible(aState);
      break;
   // Position 16 is section 4
   case 16:
      mTabs[4]->SetVisible(aState);
      mTabs[11]->SetVisible(aState);
      mTabs[12]->SetVisible(aState);
      if (aState)
      {
         mActiveTab[4]->SetVisible(false);
      }
      mButtons.at(6).SetVisible(aState);
      mButtons.at(7).SetVisible(aState);
      break;
   // Position 17 is sections 1 and 2
   case 17:
      mTabs[1]->SetVisible(aState);
      mTabs[5]->SetVisible(aState);
      mTabs[6]->SetVisible(aState);
      mTabs[2]->SetVisible(aState);
      mTabs[7]->SetVisible(aState);
      mTabs[8]->SetVisible(aState);
      if (aState)
      {
         (mActiveTab[1]->GetPagePosition() == 17) ? mActiveTab[1]->SetVisible(false) : mActiveTab[2]->SetVisible(false);
      }
      mButtons.at(0).SetVisible(aState);
      mButtons.at(3).SetVisible(aState);
      break;
   // Position 18 is sections 3 and 4
   case 18:
      mTabs[3]->SetVisible(aState);
      mTabs[9]->SetVisible(aState);
      mTabs[10]->SetVisible(aState);
      mTabs[4]->SetVisible(aState);
      mTabs[11]->SetVisible(aState);
      mTabs[12]->SetVisible(aState);
      if (aState)
      {
         (mActiveTab[3]->GetPagePosition() == 18) ? mActiveTab[3]->SetVisible(false) : mActiveTab[4]->SetVisible(false);
      }
      mButtons.at(4).SetVisible(aState);
      mButtons.at(7).SetVisible(aState);
      break;
   default:
      break;
   }
}

// probably at shutdown the viewer might get cleaned up by VTK before Qt gets around to widget destruction
void HDD::Dockable::ViewerDestroyedCB(vespa::VaViewer* aViewerPtr)
{
   if (mViewerPtr)
   {
      if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
      {
         mViewerPtr = nullptr;
      }
   }
}
