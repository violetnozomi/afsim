// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFDRAW_HPP
#define WSFDRAW_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "UtCallback.hpp"
#include "UtColor.hpp"
class UtInput;
class UtScriptClass;
class UtScriptTypes;
#include "UtVec3.hpp"
class WsfDrawManager;
class WsfExtInterface;
class WsfPlatform;
class WsfRoute;
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"

//! Provides the capability to draw shapes into the WSF replay file.
class WSF_EXPORT WsfDraw
{
public:
   static double             cFOREVER;
   static const unsigned int cSTRING_ID_MASK = 0x10000000;

   enum BeginState
   {
      cBEGIN_NONE,
      cBEGIN_LINES,
      cBEGIN_POLYLINE,
      cBEGIN_POINTS,
      cBEGIN_ICONS,
      cBEGIN_ELLIPSE,
      cBEGIN_ELLIPSOID,
      cBEGIN_QUADRILATERAL,
      cBEGIN_TEXT,
      cBEGIN_TIMER
   };

   //! Styles available for drawn lines
   enum LineStyle
   {
      cDASHED,
      cLONG_DASHED,
      cDOTTED,
      cDOTTED2,
      cDASH_DOT,
      cDASH_DOT2,
      cSOLID
   };

   enum DrawType
   {
      cLINE,
      cPOINT,
      cICON,
      cELLIPSE,
      cERASE,
      cELLIPSOID,
      cQUADRILATERAL,
      cTEXT,
      cTIMER,
      cNONE
   };

   enum VertexType
   {
      cUNSET_VERTEX,
      cABSOLUTE_WCS,
      cRELATIVE_ZERO,
      cRELATIVE_ECS,
      cRELATIVE_NED,
      cABSOLUTE_SCREEN
   };

   enum EllipseMode
   {
      cELLIPSE_FILL,
      cELLIPSE_LINE
   };

   //! Represents a vertex for a drawn item
   struct VertexObject
   {
      VertexObject()
         : mVertexType(cUNSET_VERTEX)
         , mPlatformIndex(0)
         , mPosition()
      {
      }
      VertexType mVertexType;
      size_t     mPlatformIndex;
      UtVec3d    mPosition;
   };

   struct DrawEvent
   {
      DrawType     mDrawType{DrawType::cLINE};
      unsigned int mID{0};
      UtColor      mColor;
      VertexObject mVerts[4];
      float        mDuration{0.0};
      int          mLineSize{0};
      int          mPointSize{0};
      int          mTextSize{0};
      int          mLineStyle{0};
      std::string  mIcon;
      std::string  mText;
      int          mEllipseMode{0};
      float        mHeading{0.0};
      float        mPitch{0.0};
      float        mRoll{0.0};
      float        mAxisA{0.0}; // semi minor
      float        mAxisB{0.0}; // semi major
      float        mAxisC{0.0}; // for ellipsoids
   };

   using DrawEventList = std::vector<DrawEvent>;

   WsfDraw(WsfDrawManager* aManagerPtr);
   WsfDraw(WsfSimulation& aSimulation);
   WsfDraw(const WsfDraw& aSrc);
   WsfDraw& operator=(const WsfDraw&) = delete;

   static WsfDrawManager* GetDrawManager(WsfSimulation& aSimulation);

   void BeginLines();
   void BeginPolyline();
   void BeginPoints();
   void BeginIcons(double aHeading, const std::string& aIconName = "Wedge");
   void BeginEllipse(double aHeading, double aSemiMinor, double aSemiMajor);
   void BeginEllipsoid(double aHeading, double aPitch, double aRoll, double aRadiusA, double aRadiusB, double aRadiusC);
   void BeginQuadrilateral();
   void BeginText(const std::string& aText);
   void BeginTimer();
   void End();

   BeginState GetBeginState() const { return mBeginState; }

   //! Changes the ID assigned to new draw items.  The sole purpose of Id is the Erase() method.
   void SetId(unsigned int aId) { mID = aId; }

   unsigned int GetNewId();

   //! Sets the ID given to items drawn
   void SetId(WsfStringId aId) { mID = (int)(WsfStringInt(aId) | cSTRING_ID_MASK); }

   //! Sets the ID given to items drawn
   int GetId() const { return mID; }

   //! Sets the size of points drawn with DrawPoint()
   void SetPointSize(int aSize) { mPointSize = aSize; }

   //! Gets the size of points drawn with DrawPoint()
   int GetPointSize() const { return mPointSize; }

   //! Sets the size of text
   void SetTextSize(int aSize) { mTextSize = aSize; }

   //! Sets the size (width) of lines drawn with DrawLine()
   void SetLineSize(int aSize) { mLineSize = aSize; }

   //! Gets the size (width) of lines drawn with DrawLine()
   int GetLineSize() const { return mLineSize; }

   //! Sets the style of lines drawn with DrawLine()
   void SetLineStyle(LineStyle aStyle) { mLineStyle = aStyle; }

   //! Gets the style of lines drawn with DrawLine()
   int GetLineStyle() const { return mLineStyle; }

   //! Sets the size of timers drawn with DrawPoint()
   void SetTimerSize(int aSize) { mTimerSize = aSize; }

   //! Gets the size of points drawn with DrawPoint()
   int GetTimerSize() const { return mTimerSize; }

   //! Sets the style of an ellipse or ellipsoid
   void SetEllipseMode(EllipseMode aMode) { mEllipseMode = aMode; }

   //! Gets the style of an ellipse or ellipsoid
   int GetEllipseMode() const { return mEllipseMode; }

   //! Sets the color used to draw items
   void SetColor(const UtColor& aColor) { mCurrentColor = aColor; }

   //! Gets the color used to draw items
   const UtColor& GetColor() const { return mCurrentColor; }

   //! Sets the color used to draw items
   void SetColor(double aRed, double aGreen, double aBlue, double aAlpha = 1.0)
   {
      mCurrentColor.Set((float)aRed, (float)aGreen, (float)aBlue, (float)aAlpha);
   }

   void SetTarget(const std::string& aTarget);

   void SetLayer(const std::string& aLayer);

   void Erase(unsigned int aId);

   void Erase(WsfStringId aId);

   void SetDuration(double aDuration) { mDuration = aDuration; }

   double GetDuration() const { return mDuration; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // Accessors needed to serialize the type:
   double      GetCurrentHeading() const { return mCurrentHeading; }
   double      GetCurrentPitch() const { return mCurrentPitch; }
   double      GetCurrentRoll() const { return mCurrentRoll; }
   double      GetCurrentAxisA() const { return mCurrentAxisA; }
   double      GetCurrentAxisB() const { return mCurrentAxisB; }
   double      GetCurrentAxisC() const { return mCurrentAxisC; }
   std::string GetCurrentIconName() const { return mCurrentIconName; }
   std::string GetTarget() const { return mTarget; }
   std::string GetLayer() const { return mLayer; }
   int         GetUniqueId() const { return mUniqueId; }

   unsigned int     EventCount() const { return (unsigned int)mEvents.size(); }
   const DrawEvent& GetEvent(unsigned int aIndex) const { return mEvents[aIndex]; }

   void Reset();

   void VertexLLA(double aLat, double aLon, double aAlt);
   void VertexWCS(const double aWCS[3]);
   void Vertex(WsfPlatform& aPlatform);
   void VertexECS(WsfPlatform& aPlatform, double aLocationECS[3]);
   void VertexNED(WsfPlatform& aPlatform, double aLocationNED[3]);
   void VertexScreen(const double aLocation[3]);
   void VertexScreenPlat(WsfPlatform& aPlatform, double aLocation[3]);
   void VertexRoute(WsfRoute& aRoute);

   WsfDrawManager* GetDrawManager();

private:
   void Flush();

   void AddVertex(const VertexObject& aVertex);

   void BeginClear();

   void SetCurrentState(DrawEvent& aEvent);

   UtColor                   mCurrentColor;
   int                       mPointSize;
   int                       mTextSize;
   int                       mLineSize;
   int                       mLineStyle;
   int                       mTimerSize;
   int                       mEllipseMode;
   unsigned int              mID;
   double                    mDuration;
   double                    mCurrentHeading;
   double                    mCurrentPitch;
   double                    mCurrentRoll;
   double                    mCurrentAxisA; // semi minor
   double                    mCurrentAxisB; // semi major
   double                    mCurrentAxisC; // the third axis
   std::vector<DrawEvent>    mEvents;
   std::string               mCurrentText;
   std::string               mCurrentIconName;
   std::string               mTarget;
   std::string               mLayer;
   int                       mUniqueId;
   BeginState                mBeginState;
   std::vector<VertexObject> mVertexBuffer;
   WsfDrawManager*           mDrawManagerPtr;
};

//! Provides an observer interface into WsfDraw.  Use DrawUpdate to handle
//! draw commands.
class WSF_EXPORT WsfDrawManager : public WsfCloneableSimulationExtension
{
public:
   static WsfDrawManager* Find(const WsfSimulation& aSimulation);

   WsfDrawManager();
   ~WsfDrawManager() override;
   WsfDrawManager(const WsfDrawManager& aSrc);
   WsfDrawManager& operator=(const WsfDrawManager&) = delete;

   WsfDrawManager* Clone() const override;
   bool            ProcessInput(UtInput& aInput) override;

   unsigned int GetNewId() { return mNextNewId++; }

   //! This callback is invoked when new draw events need to be executed
   //! Interfaces should use this to output draw event data
   UtCallbackListN<void(const std::string&, const std::string&, const WsfDraw::DrawEventList&)> DrawUpdate;

   void SendDrawUpdate(double                           aSimTime,
                       const std::string&               aTarget,
                       const std::string&               aLayer,
                       std::vector<WsfDraw::DrawEvent>& aEvents);

private:
   enum DrawType
   {
      cROUTE,
      cROUTE_NETWORK
   };

   struct DrawCommand
   {
      DrawCommand();
      int         mDrawTypeId;
      UtColor     mColor1;
      UtColor     mColor2;
      double      mDuration;
      WsfStringId mObjectNameId;
      int         mLineSize;
      int         mPointSize;
      std::string mLayer;
   };

   void WriteToFile(double                           aSimTime,
                    const std::string&               aTarget,
                    const std::string&               aLayer,
                    std::vector<WsfDraw::DrawEvent>& aEvents);

   void Start() override;

   unsigned int             mNextNewId;
   std::vector<DrawCommand> mDrawCommands;
   std::ofstream*           mDrawFilePtr;
   std::string              mDrawFilePath;
};

#endif
