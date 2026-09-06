// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef TARGETSELECTIONCONFIGWIDGET_HPP
#define TARGETSELECTIONCONFIGWIDGET_HPP

#include "ConfigWidget.hpp"
#include "UtQtUnitValueEdit.hpp"

class QComboBox;
class QLineEdit;
class QStackedWidget;

namespace wkf
{

class Platform;
class PlatformSelector;
class Track;
class TrackSelector;

} // namespace wkf

namespace wsfg
{

namespace spaceg
{

class PlatformSelectionWidget : public QWidget
{
public:
   explicit PlatformSelectionWidget(QWidget* aParentPtr = nullptr);
   ~PlatformSelectionWidget() override = default;

   QString GetPlatformName() const;
   void    SetPlatformName(const QString& aPlatformName);
   void    Clear();

private:
   void HandlePlatformSelection(wkf::Platform* aPlatformPtr);

   QLineEdit*             mValuePtr;
   wkf::PlatformSelector* mSelectorPtr;
};

class TrackSelectionWidget : public QWidget
{
public:
   explicit TrackSelectionWidget(QWidget* aParentPtr = nullptr);
   ~TrackSelectionWidget() override = default;

   QString GetTrackString() const;
   void    SetTrackString(const QString& aTrackString);
   void    Clear();

private:
   void HandleTrackSelection(wkf::Track* aTrackPtr);

   QLineEdit*          mValuePtr;
   wkf::TrackSelector* mSelectorPtr;
};

class LibrationPointSelectionWidget : public QWidget
{
public:
   explicit LibrationPointSelectionWidget(QWidget* aParentPtr = nullptr);
   ~LibrationPointSelectionWidget() override = default;

   QString GetSystem() const;
   QString GetPoint() const;

   void SetSystem(const QString& aSystem);
   void SetPoint(const QString& aPoint);

   void Clear();

private:
   void SetupSystemWidget();
   void SetupPointWidget();

   QComboBox* mSystemComboBoxPtr;
   QComboBox* mPointComboBoxPtr;
};

class KinematicStateSelectionWidget : public QWidget
{
public:
   explicit KinematicStateSelectionWidget(QWidget* aParentPtr = nullptr);
   ~KinematicStateSelectionWidget() override = default;

   // Queries
   QString GetX_Position() const;
   QString GetY_Position() const;
   QString GetZ_Position() const;
   QString GetX_Velocity() const;
   QString GetY_Velocity() const;
   QString GetZ_Velocity() const;
   bool    HasVelocity() const;

   // Mutators
   void SetX_Position(const QString& aPosition);
   void SetY_Position(const QString& aPosition);
   void SetZ_Position(const QString& aPosition);
   void SetX_Velocity(const QString& aVelocity);
   void SetY_Velocity(const QString& aVelocity);
   void SetZ_Velocity(const QString& aVelocity);

   void Clear();

private:
   UtQtLengthValueEdit* mPositionX_Ptr;
   UtQtLengthValueEdit* mPositionY_Ptr;
   UtQtLengthValueEdit* mPositionZ_Ptr;
   UtQtSpeedValueEdit*  mVelocityX_Ptr;
   UtQtSpeedValueEdit*  mVelocityY_Ptr;
   UtQtSpeedValueEdit*  mVelocityZ_Ptr;
};

class TargetSelectionConfigWidget : public ConfigWidget
{
public:
   explicit TargetSelectionConfigWidget(bool aAllowTrackSelection, QWidget* aParentPtr = nullptr);
   ~TargetSelectionConfigWidget() override = default;

   void                  Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject           GetValues() const override;
   bool                  SetValues(const QJsonObject& aData) override;
   void                  ClearValues() override;
   static const QString& Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   void SetupPlatformSelectionPanel();
   void SetupTrackSelectionPanel();
   void SetupLibrationPointSelectionPanel();
   void SetupKinematicStateSelectionPanel();

   void ChangePanel(int aIndex);

   QString                        mPropertyName{};
   QComboBox*                     mComboBoxPtr;
   QStackedWidget*                mStackedWidgetPtr;
   TrackSelectionWidget*          mTrackWidgetPtr{nullptr};
   PlatformSelectionWidget*       mPlatformWidgetPtr{nullptr};
   LibrationPointSelectionWidget* mLibrationWidgetPtr{nullptr};
   KinematicStateSelectionWidget* mKinematicStateWidgetPtr{nullptr};
   int                            mTrackPanelIndex{-1};
   int                            mPlatformPanelIndex{-1};
   int                            mLibrationPanelIndex{-1};
   int                            mKinematicStatePanelIndex{-1};
};

} // namespace spaceg

} // namespace wsfg

#endif // TARGETSELECTIONCONFIGWIDGET_HPP
