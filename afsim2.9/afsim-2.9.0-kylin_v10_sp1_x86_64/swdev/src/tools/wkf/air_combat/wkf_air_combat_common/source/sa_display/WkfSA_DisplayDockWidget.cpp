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

#include "sa_display/WkfSA_DisplayDockWidget.hpp"

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QGLWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMimeData>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Matrix>

#include "UtEllipsoidalEarth.hpp"
#include "UtQt.hpp"
#include "UtQtColorButton.hpp"
#include "UtQtMemory.hpp"
#include "VaAttachmentEllipsoid.hpp"
#include "VaAttachmentModel.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaOverlayCompass.hpp"
#include "VaOverlayElevationLines.hpp"
#include "VaOverlayMapGrid.hpp"
#include "VaOverlayTexture.hpp"
#include "VaWidget.hpp"
//#include "VaOverlayEphemeris.hpp"
#include "VaCameraMotionGeocentric.hpp"
#include "VaUtils.hpp" // required for make_attachment
#include "WkfAction.hpp"
#include "WkfAttachmentModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfOverlayUpdater.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"
#include "sa_display/WkfSA_DisplayOverlay.hpp"
#include "selectors/WkfGroupingSelector.hpp"

wkf::SA_Display::DockWidget::DockWidget(const QString&  aObjectName,
                                        const QString&  aPlatformName,
                                        QMainWindow*    aParent,
                                        Qt::WindowFlags aFlags)
   : wkf::DockWidget(aObjectName, aParent, aFlags)
   , mConnectedPlatformName(aPlatformName)
{
   // based on SA_DisplayDockWidget
   wkf::VtkEnvironment::InitializeOSG_Path();

   // Create the widgets for the window.
   CreateWidgets();

   // based on TetherView
   mCallbacks.Add(vespa::VaObserver::ViewerResized.Connect(&DockWidget::ViewerResizedCB, this));

   setVisible(true);
   setAttribute(Qt::WA_DeleteOnClose);
   setFocusPolicy(Qt::StrongFocus);
}

wkf::SA_Display::DockWidget::~DockWidget()
{
   if (mViewerPtr)
   {
      // mViewerPtr is actually owned by the VaWidget, so we only set it to
      // nullptr. The VaWidget should be automatically destroyed because it's
      // a QWidget, and its parent is this, which is also a QWidget.
      mViewerPtr = nullptr;
   }
}

void wkf::SA_Display::DockWidget::ViewerResizedCB(vespa::VaViewer* aViewerPtr)
{
   if (IsMyViewer(aViewerPtr) && mOverlayPtr)
   {
      mOverlayPtr->SetPosition(0, 0);
      mOverlayPtr->SetSize(aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
      mOverlayPtr->Update();
   }
}

void wkf::SA_Display::DockWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->accept();
   }
}

void wkf::SA_Display::DockWidget::dropEvent(QDropEvent* aEvent)
{
   int                 highdpiFactor = QApplication::desktop()->devicePixelRatio();
   vespa::VaOverlayQt* overlayPtr =
      mViewerPtr->PickOverlayFromScreen<vespa::VaOverlayQt>(aEvent->pos().x() * highdpiFactor,
                                                            mViewerPtr->GetHeight() - aEvent->pos().y() * highdpiFactor);
   // If the viewer has a VaOverlayQt (which it probably won't but since the
   // viewer's pointer is available publicly, you never know) then have it deal
   // with it.
   if (overlayPtr && overlayPtr->SupportsDropEvent())
   {
      if (overlayPtr->DropEvent(aEvent))
      {
         return;
      }
   }
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->acceptProposedAction();
      QByteArray   ba = aEvent->mimeData()->data(wkf::Updater::cMIME_IDENTIFIER);
      QDataStream  in(&ba, QIODevice::ReadOnly);
      unsigned int qi;
      size_t       pluginId;
      QString      platformName;
      QString      datum;
      in >> qi;
      in >> platformName;
      in >> datum;
      pluginId = static_cast<size_t>(qi);

      auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginId);
      if (plugin)
      {
         std::unique_ptr<wkf::Updater> updaterPtr = plugin->GetUpdater(platformName, datum);
         if (updaterPtr)
         {
            QPoint               p  = aEvent->pos();
            wkf::OverlayUpdater* ou = new wkf::OverlayUpdater(mViewerPtr,
                                                              updaterPtr,
                                                              platformName,
                                                              p.x() * highdpiFactor,
                                                              (height() - p.y()) * highdpiFactor);
            ou->SetVisible(true);
         }
      }
   }
}

void wkf::SA_Display::DockWidget::ScenarioRemovedCB(vespa::VaScenario* aScenarioPtr)
{
   vespa::VaScenario* stdScenario = vaEnv.GetStandardScenario();
   if ((!stdScenario) || (aScenarioPtr->GetKey() == stdScenario->GetKey()))
   {
      mViewerPtr = nullptr;
   }
}

void wkf::SA_Display::DockWidget::ViewerShutdownCB(vespa::VaViewer* aViewerPtr)
{
   // if something else destroyed the viewer (not this plug-in) this is probably game-over
   if (mViewerPtr)
   {
      if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
      {
         mViewerPtr = nullptr;
      }
   }
}

void wkf::SA_Display::DockWidget::ViewerInitialized()
{
   if (mViewerPtr)
   {
      mConnectionEstablished = true;
      mInitialized           = true;

      mCallbacks.Add(wkf::Observer::ScenarioRemoved.Connect(&DockWidget::ScenarioRemovedCB, this));
      mCallbacks.Add(wkf::Observer::ViewerDestroyed.Connect(&DockWidget::ViewerShutdownCB, this));

      // Kludgy fix to get the osg/opengl lines blending and drawing at the correct pixel sizing.  If you remove this
      // everything looks awful.
      wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMapProfile(wkf::SharedMapPreferencesData::NAVIGATIONAL,
                                                                                   mViewerPtr);

      emit DisplayInitialized();

      mOverlayPtr = new Overlay("sa_display", mViewerPtr);
      mOverlayPtr->SetTruthWidget(mEntityTruthData);
      mOverlayPtr->SetPerceivedWidget(mEntityPerceivedData);
      mOverlayPtr->SetRangeLabel(mRangeLabel);
      mOverlayPtr->SetCenterPositionRadioButton(mCenterPositionPtr);
   }
}

bool wkf::SA_Display::DockWidget::IsMyViewer(vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
   {
      return true;
   }
   else
   {
      return false;
   }
}

void wkf::SA_Display::DockWidget::AddOverlay(vespa::VaOverlay* aOverlayPtr)
{
   mViewerPtr->AddOverlay(aOverlayPtr);
   aOverlayPtr->SetVisible(false);
}

void wkf::SA_Display::DockWidget::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](mViewerPtr);
}

bool wkf::SA_Display::DockWidget::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   BuildContextMenuPrivate(aMenu, aPos);

   wkfEnv.BuildViewerContextMenu(&aMenu, mViewerPtr);

#if defined(_DEBUG) || defined(SEE_DEBUG_OPTIONS_IN_RELEASE)
   QAction* action = new QAction("Show Scene Graph", &aMenu);
   connect(action, &QAction::triggered, this, &DockWidget::SceneGraphAction);
   aMenu.addAction(action);
#endif
   return true;
}

void wkf::SA_Display::DockWidget::ActivateKillFrame(bool aState)
{
   mConnectionEstablished = !aState;
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetConnectionDestroyed(aState);
   }
   mLeftWidget->setEnabled(mConnectionEstablished);
   mRightWidget->setEnabled(mConnectionEstablished);
}

void wkf::SA_Display::DockWidget::ConnectionDestroyed(vespa::VaEntity* aEntityPtr)
{
   if (mConnectionEstablished)
   {
      if (mPlatformPtr)
      {
         if (aEntityPtr->GetUniqueId() == mPlatformPtr->GetUniqueId())
         {
            mConnectionEstablished = false;
         }
      }
   }
}

void wkf::SA_Display::DockWidget::SetSA_Data(const SA_Data& aData)
{
   // Set the Kinematic state widget data here.
   mAltitude->setText(1, QString::number(aData.platformData.altBaro_ft));
   mKCAS->setText(1, QString::number(aData.platformData.kcas));
   mKTAS->setText(1, QString::number(aData.platformData.ktas));
   mMach->setText(1, QString::number(aData.platformData.mach));
   mGs->setText(1, QString::number(aData.platformData.gLoad));

   // Set the Fuel state widget data here.
   mInternalFuel->setText(1, QString::number(aData.fuelData.fuelInternal_lbs));
   mExternalFuel->setText(1, QString::number(aData.fuelData.fuelExternal_lbs));
   mTotalFuel->setText(1, QString::number(aData.fuelData.fuelInternal_lbs + aData.fuelData.fuelExternal_lbs));
   mJoker->setText(1, QString::number(aData.fuelData.joker_lbs));
   mBingo->setText(1, QString::number(aData.fuelData.bingo_lbs));

   // Set the Weapons state widget data here.
   for (auto widget : mWeapons)
   {
      delete widget;
   }
   mWeapons.clear();
   for (auto weapon : aData.weaponData.weaponList)
   {
      QTreeWidgetItem* widget = new QTreeWidgetItem(mWeaponStateTree.get());
      widget->setText(0, QString::fromStdString(weapon.weaponName));
      widget->setText(1, QString::number(weapon.quantity));
      widget->setTextAlignment(1, Qt::AlignRight);
      mWeapons.push_back(widget);
   }

   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetSA_Data(aData);
   }
}

void wkf::SA_Display::DockWidget::SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aData)
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetSA_TruthPlatforms(aData);
   }
}

void wkf::SA_Display::DockWidget::ResetCenterPoint()
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->ResetCenterPoint();
   }
}

void wkf::SA_Display::DockWidget::SetCenterPoint()
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetCenterPoint();
   }
}

void wkf::SA_Display::DockWidget::BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos)
{
   auto action = new QAction(QIcon::fromTheme("aim"), QString("Set Center Point"), &aMenu);
   connect(action, &QAction::triggered, this, &DockWidget::SetCenterPoint);
   aMenu.addAction(action);

   auto action2 = new QAction(QIcon::fromTheme("undo"), QString("Reset Center Point"), &aMenu);
   connect(action2, &QAction::triggered, this, &DockWidget::ResetCenterPoint);
   aMenu.addAction(action2);
}

void wkf::SA_Display::DockWidget::closeEvent(QCloseEvent* aEvent)
{
   // Tether views should be deleted when they are closed, and recreated on demand
   deleteLater();
}

void wkf::SA_Display::DockWidget::CreateWidgets()
{
   // This is the frame in which all of the other widgets are placed.
   // When the tethered platform is destroyed, it should have a red border.
   mFrame          = new QFrame(this);
   QHBoxLayout* lo = new QHBoxLayout(mFrame);
   lo->setContentsMargins(0, 0, 0, 0);
   lo->setSpacing(0);
   mFrame->setLayout(lo);
   mFrame->setLineWidth(0);
   mFrame->setObjectName("killFrame");
   setWidget(mFrame);

   // Things left of the SA Display
   mLeftWidget     = new QWidget(this);
   QVBoxLayout* lv = new QVBoxLayout(mFrame);
   lv->setAlignment(Qt::AlignTop);
   mLeftWidget->setLayout(lv);

   // The SA Display.
   // Sets up the viewer, as well as the QWidget that it lives in
   vespa::VaWidget* vawidget =
      new vespa::VaWidget(vespa::VaViewer::SimpleViewer, vespa::VaViewer::SmallFeatureCulling, this);
   connect(vawidget, &vespa::VaWidget::Initialized, this, &DockWidget::ViewerInitialized);

   // Things right of the SA Display.
   mRightWidget = new QWidget(this);
   mRightWidget->setContentsMargins(0, 0, 0, 0);
   QVBoxLayout* ivl = new QVBoxLayout(mRightWidget);
   ivl->setContentsMargins(0, 0, 0, 0);
   mRightWidget->setLayout(ivl);

   // Positioning of the aircraft (Center, Offset, Bottom)
   QPointer<QLabel>       PositionLabel  = new QLabel("Ownship Position", mLeftWidget);
   QPointer<QWidget>      PositionWidget = new QWidget(mLeftWidget);
   QHBoxLayout*           phl            = new QHBoxLayout(PositionWidget);
   QPointer<QButtonGroup> positionGroup  = new QButtonGroup(PositionWidget);
   mCenterPositionPtr                    = new QRadioButton("Center", PositionWidget);
   QPointer<QRadioButton> offsetPosition = new QRadioButton("Offset", PositionWidget);
   QPointer<QRadioButton> bottomPosition = new QRadioButton("Bottom", PositionWidget);
   mCenterPositionPtr->setChecked(true);
   connect(mCenterPositionPtr.data(), &QRadioButton::toggled, this, &DockWidget::CenterPosition);
   connect(offsetPosition.data(), &QRadioButton::toggled, this, &DockWidget::OffsetPosition);
   connect(bottomPosition.data(), &QRadioButton::toggled, this, &DockWidget::BottomPosition);
   positionGroup->addButton(mCenterPositionPtr);
   positionGroup->addButton(offsetPosition);
   positionGroup->addButton(bottomPosition);
   phl->addWidget(mCenterPositionPtr);
   phl->addWidget(offsetPosition);
   phl->addWidget(bottomPosition);

   // QComboBox for range
   QWidget*     hrf = new QWidget(this);
   QHBoxLayout* hrl = new QHBoxLayout(hrf);
   hrl->setSizeConstraint(QLayout::SetFixedSize);
   hrf->setLayout(hrl);
   QPointer<QLabel> RangeLabel = new QLabel("Range (nm):", hrf);
   mRangeLabel                 = new QLabel("160", hrf);
   hrl->addWidget(RangeLabel);
   hrl->addWidget(mRangeLabel);

   // Buttons/Checkboxes
   QPointer<QCheckBox> ShowPerception = new QCheckBox(QString("Show Perception"), this);
   ShowPerception->setChecked(true);
   connect(ShowPerception.data(), &QPushButton::toggled, this, &DockWidget::ShowPerception);

   QPointer<QCheckBox> ShowTruthAir = new QCheckBox(QString("Show Truth (Air)"), this);
   ShowTruthAir->setChecked(false);
   connect(ShowTruthAir.data(), &QPushButton::toggled, this, &DockWidget::ShowTruthAir);

   QPointer<QCheckBox> ShowTruthGround = new QCheckBox(QString("Show Truth (Other, Non-Air)"), this);
   ShowTruthGround->setChecked(false);
   connect(ShowTruthGround.data(), &QPushButton::toggled, this, &DockWidget::ShowTruthGround);

   QPointer<QPushButton> ShowTruthToggleButton = new QPushButton(QString("Show Truth"), this);
   connect(ShowTruthToggleButton.data(),
           &QPushButton::pressed,
           this,
           [ShowTruthToggleButton, this] { this->ShowTruth(true); });
   connect(ShowTruthToggleButton.data(),
           &QPushButton::released,
           this,
           [ShowTruthToggleButton, ShowTruthGround, ShowTruthAir, this]
           {
              if (!ShowTruthGround->isChecked())
              {
                 this->ShowTruthGround(false);
              }
              if (!ShowTruthAir->isChecked())
              {
                 this->ShowTruthAir(false);
              }
           });

   QPointer<QCheckBox> ShowFriends = new QCheckBox(QString("Show Friendlies"), this);
   ShowFriends->setChecked(true);
   connect(ShowFriends.data(), &QPushButton::toggled, this, &DockWidget::ShowFriends);

   QPointer<QCheckBox> ShowFlight = new QCheckBox(QString("Show Flight Members"), this);
   ShowFlight->setChecked(true);
   connect(ShowFlight.data(), &QPushButton::toggled, this, &DockWidget::ShowFlight);

   QPointer<QCheckBox> ShowPriorityThreats = new QCheckBox(QString("Show Priority Threats"), this);
   ShowPriorityThreats->setChecked(true);
   connect(ShowPriorityThreats.data(), &QPushButton::toggled, this, &DockWidget::ShowPriorityThreats);

   QPointer<QCheckBox> ShowPriorityTargets = new QCheckBox(QString("Show Priority Targets"), this);
   ShowPriorityTargets->setChecked(true);
   connect(ShowPriorityTargets.data(), &QPushButton::toggled, this, &DockWidget::ShowPriorityTargets);

   QPointer<QCheckBox> ShowAltitude = new QCheckBox(QString("Show Altitude"), this);
   ShowAltitude->setChecked(true);
   connect(ShowAltitude.data(), &QPushButton::toggled, this, &DockWidget::ShowAltitude);

   QPointer<QCheckBox> ShowESM_RWR = new QCheckBox(QString("Show ESM/RWR Tracks"), this);
   ShowESM_RWR->setChecked(true);
   connect(ShowESM_RWR.data(), &QPushButton::toggled, this, &DockWidget::ShowESM_RWR);

   QPointer<QCheckBox> ShowAngleOnly = new QCheckBox(QString("Show Angle Only Tracks"), this);
   ShowAngleOnly->setChecked(true);
   connect(ShowAngleOnly.data(), &QPushButton::toggled, this, &DockWidget::ShowAngleOnly);

   QPointer<QCheckBox> ShowOwnEngagements = new QCheckBox(QString("Show Ownship Engagements"), this);
   // ShowOwnEngagements->setChecked(true);
   ShowOwnEngagements->setDisabled(true);
   connect(ShowOwnEngagements.data(), &QPushButton::toggled, this, &DockWidget::ShowOwnEngagements);

   QPointer<QCheckBox> ShowFlightEngagements = new QCheckBox(QString("Show Flight Engagements"), this);
   // ShowFlightEngagements->setChecked(true);
   ShowFlightEngagements->setDisabled(true);
   connect(ShowFlightEngagements.data(), &QPushButton::toggled, this, &DockWidget::ShowFlightEngagements);

   QPointer<QCheckBox> ShowThreatEngagements = new QCheckBox(QString("Show Threat Engagements"), this);
   // ShowThreatEngagements->setChecked(true);
   ShowThreatEngagements->setDisabled(true);
   connect(ShowThreatEngagements.data(), &QPushButton::toggled, this, &DockWidget::ShowThreatEngagements);

   QPointer<QCheckBox> ShowRoute = new QCheckBox(QString("Show Route"), this);
   ShowRoute->setChecked(true);
   connect(ShowRoute.data(), &QPushButton::toggled, this, &DockWidget::ShowRoute);

   QPointer<QCheckBox> ShowFOV = new QCheckBox(QString("Show FOV"), this);
   ShowFOV->setChecked(true);
   connect(ShowFOV.data(), &QPushButton::toggled, this, &DockWidget::ShowFOV);

   QPointer<QCheckBox> ShowGroups = new QCheckBox(QString("Show Groups"), this);
   ShowGroups->setChecked(true);
   connect(ShowGroups.data(), &QPushButton::toggled, this, &DockWidget::ShowGroups);

   // Select Mode for entity data (click/hover)
   QPointer<QLabel>       SelectModeLabel = new QLabel("Entity Select Mode", this);
   QPointer<QWidget>      SelectMode      = new QWidget(this);
   QHBoxLayout*           shl             = new QHBoxLayout(SelectMode);
   QPointer<QButtonGroup> selectModeGroup = new QButtonGroup(SelectMode);
   QPointer<QRadioButton> hoverMode       = new QRadioButton("Hover", SelectMode);
   hoverMode->setChecked(true);
   connect(hoverMode.data(), &QRadioButton::toggled, this, &DockWidget::HoverMode);
   QPointer<QRadioButton> clickMode = new QRadioButton("Click", SelectMode);
   selectModeGroup->addButton(hoverMode);
   selectModeGroup->addButton(clickMode);
   shl->addWidget(hoverMode);
   shl->addWidget(clickMode);

   // Kinematic state widgets
   QPointer<QTreeWidget> kinematicStateTree = new QTreeWidget(mRightWidget);
   kinematicStateTree->setRootIsDecorated(false);
   kinematicStateTree->setColumnCount(2);
   kinematicStateTree->setHeaderLabels(QStringList({"Kinematic State", ""}));
   kinematicStateTree->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
   mAltitude = new QTreeWidgetItem(kinematicStateTree);
   mAltitude->setText(0, "Altitude (ft)");
   mAltitude->setTextAlignment(1, Qt::AlignRight);
   mKCAS = new QTreeWidgetItem(kinematicStateTree);
   mKCAS->setText(0, "KCAS");
   mKCAS->setTextAlignment(1, Qt::AlignRight);
   mKTAS = new QTreeWidgetItem(kinematicStateTree);
   mKTAS->setText(0, "KTAS");
   mKTAS->setTextAlignment(1, Qt::AlignRight);
   mMach = new QTreeWidgetItem(kinematicStateTree);
   mMach->setText(0, "Mach");
   mMach->setTextAlignment(1, Qt::AlignRight);
   mGs = new QTreeWidgetItem(kinematicStateTree);
   mGs->setText(0, "G");
   mGs->setTextAlignment(1, Qt::AlignRight);

   mWeaponStateTree = ut::make_unique<QTreeWidget>(mRightWidget);
   mWeaponStateTree->setRootIsDecorated(false);
   mWeaponStateTree->setColumnCount(2);
   mWeaponStateTree->setHeaderLabels(QStringList({"Weapons", "Quantity"}));

   // Fuel state widgets
   QPointer<QTreeWidget> fuelStateTree = new QTreeWidget(mRightWidget);
   fuelStateTree->setRootIsDecorated(false);
   fuelStateTree->setColumnCount(2);
   fuelStateTree->setHeaderLabels(QStringList({"Fuel State", ""}));
   mInternalFuel = new QTreeWidgetItem(fuelStateTree);
   mInternalFuel->setText(0, "Internal Fuel Remaining (lbs)");
   mInternalFuel->setTextAlignment(1, Qt::AlignRight);
   mExternalFuel = new QTreeWidgetItem(fuelStateTree);
   mExternalFuel->setText(0, "External Fuel Remaining (lbs)");
   mExternalFuel->setTextAlignment(1, Qt::AlignRight);
   mTotalFuel = new QTreeWidgetItem(fuelStateTree);
   mTotalFuel->setText(0, "Total Fuel Remaining (lbs)");
   mTotalFuel->setTextAlignment(1, Qt::AlignRight);
   mJoker = new QTreeWidgetItem(fuelStateTree);
   mJoker->setText(0, "Joker");
   mJoker->setTextAlignment(1, Qt::AlignRight);
   mBingo = new QTreeWidgetItem(fuelStateTree);
   mBingo->setText(0, "Bingo");
   mBingo->setTextAlignment(1, Qt::AlignRight);

   QPointer<QTreeWidget> entityDetailsTree = new QTreeWidget(mRightWidget);
   entityDetailsTree->setRootIsDecorated(false);
   entityDetailsTree->setColumnCount(2);
   entityDetailsTree->setHeaderLabels(QStringList({"Entity Details", ""}));
   mEntityTruthData = new QTreeWidgetItem(entityDetailsTree);
   mEntityTruthData->setText(0, "Truth Data");
   mEntityTruthData->setExpanded(true);
   mEntityTruthName = new QTreeWidgetItem(mEntityTruthData);
   mEntityTruthName->setText(0, "Name");
   mEntityTruthName->setTextAlignment(1, Qt::AlignRight);
   mEntityTruthType = new QTreeWidgetItem(mEntityTruthData);
   mEntityTruthType->setText(0, "Type");
   mEntityTruthType->setTextAlignment(1, Qt::AlignRight);
   mEntityTruthRange = new QTreeWidgetItem(mEntityTruthData);
   mEntityTruthRange->setText(0, "Range (nm)");
   mEntityTruthRange->setTextAlignment(1, Qt::AlignRight);

   mEntityPerceivedData = new QTreeWidgetItem(entityDetailsTree);
   mEntityPerceivedData->setText(0, "Perceived Data");
   mEntityPerceivedData->setExpanded(true);
   mEntityPerceivedType = new QTreeWidgetItem(mEntityPerceivedData);
   mEntityPerceivedType->setText(0, "Type");
   mEntityPerceivedType->setTextAlignment(1, Qt::AlignRight);
   mEntityPerceivedRange = new QTreeWidgetItem(mEntityPerceivedData);
   mEntityPerceivedRange->setText(0, "Range (nm)");
   mEntityPerceivedRange->setTextAlignment(1, Qt::AlignRight);

   // Add our trees to the right vertical layout
   ivl->addWidget(kinematicStateTree);
   ivl->addWidget(mWeaponStateTree.get());
   ivl->addWidget(fuelStateTree);
   ivl->addWidget(entityDetailsTree);

   // Add the widgets in the left vertical layout
   lv->addWidget(PositionLabel);
   lv->addWidget(PositionWidget);
   lv->addWidget(hrf);
   lv->addWidget(ShowPerception);
   lv->addWidget(ShowTruthAir);
   lv->addWidget(ShowTruthGround);
   lv->addWidget(ShowTruthToggleButton);
   lv->addWidget(ShowFriends);
   lv->addWidget(ShowFlight);
   lv->addWidget(ShowPriorityThreats);
   lv->addWidget(ShowPriorityTargets);
   lv->addWidget(ShowAltitude);
   lv->addWidget(ShowESM_RWR);
   lv->addWidget(ShowAngleOnly);
   lv->addWidget(ShowOwnEngagements);
   lv->addWidget(ShowFlightEngagements);
   lv->addWidget(ShowThreatEngagements);
   lv->addWidget(ShowRoute);
   lv->addWidget(ShowFOV);
   lv->addWidget(ShowGroups);
   lv->addSpacing(10);
   lv->addWidget(SelectModeLabel);
   lv->addWidget(SelectMode);

   // Add all widgets to the layout.
   lo->addWidget(mLeftWidget);
   lo->addWidget(vawidget, 1);
   lo->addWidget(mRightWidget);

   mViewerPtr = vawidget->GetViewer();
   mViewerPtr->SetData("type", UtVariant(std::string("perception")));
   mViewerPtr->GetCamera()->SetCustomCameraMotion(nullptr);
   setAcceptDrops(true);
}

void wkf::SA_Display::DockWidget::ClearCallback(int& aId)
{
   if (aId)
   {
      mCallbacks.Disconnect(aId);
      aId = 0;
   }
}

void wkf::SA_Display::DockWidget::ShowPerception(bool aShow)
{
   mOverlayPtr->ShowPerception(aShow);
}

void wkf::SA_Display::DockWidget::ShowTruth(bool aShow)
{
   mOverlayPtr->ShowTruthAir(aShow);
   mOverlayPtr->ShowTruthGround(aShow);
}

void wkf::SA_Display::DockWidget::ShowTruthGround(bool aShow)
{
   mOverlayPtr->ShowTruthGround(aShow);
}

void wkf::SA_Display::DockWidget::ShowTruthAir(bool aShow)
{
   mOverlayPtr->ShowTruthAir(aShow);
}

void wkf::SA_Display::DockWidget::ShowFriends(bool aShow)
{
   mOverlayPtr->ShowFriends(aShow);
}

void wkf::SA_Display::DockWidget::ShowFlight(bool aShow)
{
   mOverlayPtr->ShowFlight(aShow);
}

void wkf::SA_Display::DockWidget::ShowPriorityThreats(bool aShow)
{
   mOverlayPtr->ShowPriorityThreats(aShow);
}

void wkf::SA_Display::DockWidget::ShowPriorityTargets(bool aShow)
{
   mOverlayPtr->ShowPriorityTargets(aShow);
}

void wkf::SA_Display::DockWidget::ShowAltitude(bool aShow)
{
   mOverlayPtr->ShowAltitude(aShow);
}

void wkf::SA_Display::DockWidget::ShowESM_RWR(bool aShow)
{
   mOverlayPtr->ShowESM_RWR(aShow);
}

void wkf::SA_Display::DockWidget::ShowAngleOnly(bool aShow)
{
   mOverlayPtr->ShowAngleOnly(aShow);
}

void wkf::SA_Display::DockWidget::ShowOwnEngagements(bool aShow)
{
   mOverlayPtr->ShowOwnEngagements(aShow);
}

void wkf::SA_Display::DockWidget::ShowFlightEngagements(bool aShow)
{
   mOverlayPtr->ShowFlightEngagements(aShow);
}

void wkf::SA_Display::DockWidget::ShowThreatEngagements(bool aShow)
{
   mOverlayPtr->ShowThreatEngagements(aShow);
}

void wkf::SA_Display::DockWidget::ShowSAMs(bool aShow)
{
   mOverlayPtr->ShowSAMs(aShow);
}

void wkf::SA_Display::DockWidget::ShowRoute(bool aShow)
{
   mOverlayPtr->ShowRoute(aShow);
}

void wkf::SA_Display::DockWidget::ShowBullseye(bool aShow)
{
   mOverlayPtr->ShowBullseye(aShow);
}

void wkf::SA_Display::DockWidget::ShowFOV(bool aShow)
{
   mOverlayPtr->ShowFOV(aShow);
}

void wkf::SA_Display::DockWidget::ShowGroups(bool aShow)
{
   mOverlayPtr->ShowGroups(aShow);
}

void wkf::SA_Display::DockWidget::HoverMode(bool aState)
{
   mOverlayPtr->HoverMode(aState);
}

void wkf::SA_Display::DockWidget::CenterPosition(bool aState)
{
   if (aState)
   {
      mOverlayPtr->SetCenterPosition();
   }
}

void wkf::SA_Display::DockWidget::OffsetPosition(bool aState)
{
   if (aState)
   {
      mOverlayPtr->SetOffsetPosition();
   }
}

void wkf::SA_Display::DockWidget::BottomPosition(bool aState)
{
   if (aState)
   {
      mOverlayPtr->SetBottomPosition();
   }
}
