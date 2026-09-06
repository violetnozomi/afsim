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

#ifndef WKFAIRCOMBATOVERLAY_HPP
#define WKFAIRCOMBATOVERLAY_HPP

#include "wkf_air_combat_common_export.h"

#include <memory>

#include <QObject>
#include <osgText/Text>

#include "UtoRawShape.hpp"
#include "VaOverlay.hpp"
#include "WkfAirCombatDataContainer.hpp"
#include "WkfPlatform.hpp"

namespace wkf
{
enum class TargetingMode
{
   cMANUAL,
   cTARGETS,
   cTHREATS
};

// A simple 2D point.
struct Point
{
   Point(int aX = 0, int aY = 0)
      : mX(aX)
      , mY(aY)
   {
   }

   int mX;
   int mY;
};

// A base class for more complex graphical items (labels, buttons, progress bars, etc).
class OverlayObject : public QObject
{
   Q_OBJECT

public:
   OverlayObject(OverlayObject* aParent = nullptr);

   // Used for non-data related updates (i.e. progress bar interpolation)
   virtual void Update() {}

   // Sets own colors plus children -- Should be implemented by any object that wants to share colors with its parent
   virtual void Recolor(const UtColor& aPrimary, const UtColor& aSecondary) {}

   // Only sets own colors -- Should be implemented by any object that wants to share colors with its parent
   virtual void SetColor(const UtColor& aPrimary, const UtColor& aSecondary) {}

   virtual void Resize(double aScalar);

   // Must by overwritten by child classes.
   virtual void Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const = 0;

   // These can be overwritten by child classes to provide input functionality.
   virtual bool Btn1Click(int aMouseX, int aMouseY) { return false; }
   virtual bool Btn1Down(int aMouseX, int aMouseY) { return false; }
   virtual bool Btn1Up(int aMouseX, int aMouseY) { return false; }
   virtual bool Btn1DblClick(int aMouseX, int aMouseY) { return false; }
   virtual bool MouseWheel(int aMouseX, int aMouseY, int aDelta) { return false; }

   virtual bool SelectableAt(int aMouseX, int aMouseY) const;

   void           SetParent(OverlayObject* aParent) { mParent = aParent; }
   OverlayObject* GetParent() const { return mParent; }

   void  SetPosition(const Point& aPoint) { mPosition = aPoint; }
   Point GetPosition() const { return mPosition; }

   void  SetOffset(const Point& aPoint) { mOffset = aPoint; }
   Point GetOffset() const { return mOffset; }

   void SetWidth(int aWidth) { mWidth = aWidth; }
   int  GetWidth() const { return mWidth; }

   void SetHeight(int aHeight) { mHeight = aHeight; }
   int  GetHeight() const { return mHeight; }

   void   SetScalar(double aScalar) { mScalar = aScalar; }
   double GetScalar() const { return mScalar; }

private:
   OverlayObject* mParent;

   Point  mPosition; // Typically the bottom left corner of the object.
   Point  mOffset;   // Used for positioning the object relative to a parent object.
   int    mWidth;
   int    mHeight;
   double mScalar;
};

// A block of information that can contain and display multiple other OverlayObjects.
class DataBlock : public OverlayObject
{
public:
   DataBlock(OverlayObject* aParent = nullptr);

   void Update() override;

   // Sets own colors plus children
   void Recolor(const UtColor& aPrimary, const UtColor& aSecondary) override;

   // Only sets own colors
   void SetColor(const UtColor& aPrimary, const UtColor& aSecondary) override;

   void Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const override;
   void Resize(double aScalar) override;

   bool SelectableAt(int aMouseX, int aMouseY) const override;
   bool Btn1Click(int aMouseX, int aMouseY) override;
   bool Btn1Down(int aMouseX, int aMouseY) override;
   bool Btn1Up(int aMouseX, int aMouseY) override;
   bool Btn1DblClick(int aMouseX, int aMouseY) override;
   bool MouseWheel(int aMouseX, int aMouseY, int aDelta) override;

   void           Clear();
   void           AddObject(const std::string& aObjectID, std::unique_ptr<OverlayObject> aObject);
   void           RemoveObject(const std::string& aObjectID);
   OverlayObject* GetObject(const std::string& aObjectID);

   template<class T>
   std::vector<T*> GetObjectsOfType();

private:
   std::map<std::string, std::unique_ptr<OverlayObject>> mObjects;

   UtColor mPrimaryColor;
   UtColor mSecondaryColor;
};

// A class consisting of a string label.
class Label : public OverlayObject
{
   Q_OBJECT

public:
   Label(OverlayObject* aParent = nullptr);

   void Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const override;
   void Resize(double aScalar) override;

   void SetText(const std::string& aText) { mText = aText; }
   void SetFontColor(const UtColor& aColor) { mFontColor = aColor; }
   void SetFontSize(double aSize) { mFontSize = aSize; }
   void SetAlignment(const osgText::Text::AlignmentType& aAlignment) { mAlignment = aAlignment; }

private:
   osg::ref_ptr<osgText::Font>  mFontPtr;
   osgText::Text::AlignmentType mAlignment;

   std::string mText;
   UtColor     mFontColor;
   double      mFontSize;
   double      mFontResolution;
};

// A progress bar that displays data on a normalized 0-1 scale.
class ProgressBar : public OverlayObject
{
   Q_OBJECT

public:
   enum class Orientation
   {
      cVERTICAL,
      cHORIZONTAL
   };

   struct TickMark
   {
      bool                   mEnabled;
      double                 mValue;
      int                    mLevel;
      std::unique_ptr<Label> mLabel;
   };

   ProgressBar(OverlayObject* aParent = nullptr);

   void Update() override; // { UpdateValue(); }
   void Resize(double aScalar) override;
   void Recolor(const UtColor& aPrimary, const UtColor& aSecondary) override;
   void Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const override;

   void SetLineColor(const UtColor& aColor) { mLineColor = aColor; }
   void SetFillColor(const UtColor& aColor) { mFillColor = aColor; }
   void SetDangerColor(const UtColor& aColor) { mDangerColor = aColor; }
   void SetValue(double aValue) { mTargetValue = aValue; }
   void SetSpeed(double aSpeed) { mSpeed = aSpeed; }
   void SetOrientation(const Orientation& aOrientation) { mOrientation = aOrientation; }

   void AddTickMark(const std::string& aName, double aValue, int aLevel);
   void RemoveTickMark(const std::string& aName);
   void UpdateTickMark(const std::string& aName, double aValue);
   void EnableTickMark(const std::string& aName, bool aEnabled);
   void ClearTickMarks();

private:
   void UpdateValue();

   UtColor mLineColor;
   UtColor mFillColor;
   UtColor mDangerColor;

   double mTargetValue;  // Current value
   double mCurrentValue; // Value to interpolate towards
   double mSpeed;        // Interpolation speed
   int    mLevel;        // Level (distance below bar)

   Orientation mOrientation;

   std::map<std::string, TickMark> mTickMarks;
};

// A button that can be clicked and optionally holds a selected state.
class Button : public OverlayObject
{
   Q_OBJECT

public:
   Button(OverlayObject* aParent = nullptr);

   void Recolor(const UtColor& aPrimary, const UtColor& aSecondary) override;
   void Draw(osg::ref_ptr<osg::Geode> aGeodePtr) const override;
   void Resize(double aScalar) override;

   bool Btn1Click(int aMouseX, int aMouseY) override;
   bool Btn1DblClick(int aMouseX, int aMouseY) override { return Btn1Click(aMouseX, aMouseY); }
   bool Btn1Down(int aMouseX, int aMouseY) override;
   bool Btn1Up(int aMouseX, int aMouseY) override;

   void SetLineColor(const UtColor& aColor) { mLineColor = aColor; }
   void SetFillColor(const UtColor& aColor) { mFillColor = aColor; }
   void SetFontColor(const UtColor& aColor) { mFontColor = aColor; }
   void SetFontSize(double aSize) { mFontSize = aSize; }

   void SetText(const std::string& aText) { mText = aText; }

   void SetSelectable(bool aSelectable) { mSelectable = aSelectable; }
   void SetSelected(bool aSelected) { mSelected = aSelected; }
   bool IsSelected() { return mSelected; }

signals:
   void Clicked();

private:
   osg::ref_ptr<osgText::Font> mFontPtr;

   std::string mText;
   UtColor     mLineColor;
   UtColor     mFillColor;
   UtColor     mFontColor;
   double      mFontSize;
   double      mFontResolution;
   bool        mSelected;
   bool        mSelectable;
};

class WKF_AIR_COMBAT_COMMON_EXPORT AirCombatOverlay : public QObject, public vespa::VaOverlay
{
   Q_OBJECT

public:
   AirCombatOverlay(wkf::Platform& aSourcePlatform);

   AirCombatOverlay* Clone() const override;
   bool              Initialize() override;

   HandleEventMode GetHandleEventMode() const override { return cHANDLE_EVENTS_FOCUS; }

   bool Btn1Click(int aMouseX, int aMouseY, int aState) override;
   bool Btn1DblClick(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Down(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Up(int aMouseX, int aMouseY, int aState) override;
   bool MouseWheel(int aMouseX, int aMouseY, int aDelta) override;

   bool SelectableAt(int aMouseX, int aMouseY) const override;

   void Update() override;
   void Resize() override;

   void UpdateData(const std::vector<wkf::AirCombatDataEngagementSummary>& aDataList);

   void AddTarget(wkf::Platform& aTarget);
   void RemoveTarget(wkf::Platform& aTarget);

private:
   void InitializeSourcePlatformDataBlock();
   void InitializeSourcePlatformEngagementBlock();
   void InitializeTargetPlatformEngagementBlock();
   void InitializeTacticalDataBlock();

   void UpdateSourcePlatformDataBlock(const wkf::AirCombatDataEngagementSummary& aData);
   void UpdateSourcePlatformEngagementBlock(const wkf::AirCombatDataEngagementSummary& aData);
   void UpdateTargetPlatformEngagementBlock(const wkf::AirCombatDataEngagementSummary& aData);
   void UpdateTacticalDataBlock(const wkf::AirCombatDataEngagementSummary& aData);

   void GoToNextTarget();
   void GoToPrevTarget();

   void TargetChanged();
   void SetSourceColors(UtColor& aPrimaryColor, UtColor& aSecondaryColor);
   void SetTargetColors(UtColor& aPrimaryColor, UtColor& aSecondaryColor);

   UtoRawShape*             mShapePtr;
   osg::ref_ptr<osg::Geode> mGeodePtr;

   wkf::Platform*              mSourcePlatformPtr;
   wkf::Platform*              mCurrentPrioritizedThreat;
   wkf::Platform*              mCurrentPrioritizedTarget;
   wkf::Platform*              mCurrentManualTarget;
   std::vector<wkf::Platform*> mPrioritizedThreats;
   std::vector<wkf::Platform*> mPrioritizedTargets;
   std::vector<wkf::Platform*> mManualTargets;
   TargetingMode               mTargetingMode;

   std::unique_ptr<DataBlock> mSourcePlatformDataBlock;
   std::unique_ptr<DataBlock> mSourcePlatformEngagementBlock;
   std::unique_ptr<DataBlock> mTargetPlatformEngagementBlock;
   std::unique_ptr<DataBlock> mTacticalDataBlock;

   std::pair<int, int> mMinimumResolution;

   bool mIsInitialized;
};
} // namespace wkf

#endif // !WKFAirCombatOVERLAY_HPP
