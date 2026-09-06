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

#include "WsfDraw.hpp"

#include <fstream>

#include "GenBuffer.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptDataPack.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

double WsfDraw::cFOREVER = 1.0E20;

namespace
{
class DrawApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfCloneableScenarioExtension>(new WsfDrawManager()));
   }
};
} // namespace

void WSF_EXPORT Register_wsf_draw(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_draw"))
   {
      aApplication.RegisterExtension("wsf_draw", ut::make_unique<DrawApplicationExtension>());
   }
}

WsfDrawManager* WsfDrawManager::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfDrawManager*>(aSimulation.FindExtension("wsf_draw"));
}

WsfDrawManager::WsfDrawManager()
   : WsfCloneableSimulationExtension()
   , DrawUpdate()
   , mNextNewId(45823)
   , mDrawCommands()
   , mDrawFilePtr(nullptr)
   , mDrawFilePath()
{
}

WsfDrawManager::WsfDrawManager(const WsfDrawManager& aSrc)
   : WsfCloneableSimulationExtension()
   , DrawUpdate()
   , mNextNewId(aSrc.mNextNewId)
   , mDrawCommands(aSrc.mDrawCommands)
   , mDrawFilePtr(nullptr)
   , mDrawFilePath(aSrc.mDrawFilePath)
{
}

WsfDrawManager::~WsfDrawManager()
{
   delete mDrawFilePtr;
}

bool WsfDrawManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string cmd       = aInput.GetCommand();
   if (cmd == "draw_file")
   {
      aInput.ReadValueQuoted(mDrawFilePath);
      mDrawFilePath = aInput.SubstitutePathVariables(mDrawFilePath);
   }
   else if (cmd == "draw")
   {
      DrawCommand drawCommand;
      const char* cDRAW_TYPES[] = {"route", "route_network"};

      std::string drawType;
      aInput.ReadValue(drawType);
      for (size_t i = 0; i < sizeof(cDRAW_TYPES) / sizeof(char*); ++i)
      {
         if (cDRAW_TYPES[i] == drawType)
         {
            drawCommand.mDrawTypeId = static_cast<int>(i);
            break;
         }
      }

      if (drawCommand.mDrawTypeId != -1)
      {
         std::string objectId;
         aInput.ReadValue(objectId);
         drawCommand.mObjectNameId = WsfStringId(objectId);
         UtInputBlock block(aInput);
         std::string  cmd2;
         while (block.ReadCommand(cmd2))
         {
            if (cmd2 == "color")
            {
               aInput.ReadValue(drawCommand.mColor1);
            }
            else if (cmd2 == "color2")
            {
               aInput.ReadValue(drawCommand.mColor2);
            }
            else if (cmd2 == "line_size")
            {
               aInput.ReadValue(drawCommand.mLineSize);
            }
            else if (cmd2 == "point_size")
            {
               aInput.ReadValue(drawCommand.mPointSize);
            }
            else if (cmd2 == "timer_size")
            {
               aInput.ReadValue(drawCommand.mPointSize);
            }
            else if (cmd2 == "layer")
            {
               aInput.ReadValue(drawCommand.mLayer);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Invalid draw command");
            }
         }
         mDrawCommands.push_back(drawCommand);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid draw command");
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfDrawManager::SendDrawUpdate(double                           aSimTime,
                                    const std::string&               aTarget,
                                    const std::string&               aLayer,
                                    std::vector<WsfDraw::DrawEvent>& aEvents)
{
   if (mDrawFilePtr != nullptr)
   {
      WriteToFile(aSimTime, aTarget, aLayer, aEvents);
   }
   DrawUpdate(aTarget, aLayer, aEvents);
}

namespace
{
void Write(GenBuffer& aIO, WsfDraw::VertexObject& aVertex)
{
   WsfDraw::VertexType vertexType = aVertex.mVertexType;
   aIO.Put(static_cast<unsigned char>(vertexType));
   // only absolute points supported...
   if (vertexType == WsfDraw::cABSOLUTE_WCS)
   {
      aIO.Put(aVertex.mPosition[0]);
      aIO.Put(aVertex.mPosition[1]);
      aIO.Put(aVertex.mPosition[2]);
   }
}
void Write(GenBuffer& aIO, const std::string& aString)
{
   aIO.Put(static_cast<int>(aString.size()));
   aIO.PutRaw(aString.c_str(), aString.size());
}

} // namespace

void WsfDrawManager::WriteToFile(double                           aSimTime,
                                 const std::string&               aTarget,
                                 const std::string&               aLayer,
                                 std::vector<WsfDraw::DrawEvent>& aEvents)
{
   GenBuffer data;
   data.SetNativeByteOrder();
   Write(data, aLayer);
   data.Put(static_cast<int>(aEvents.size()));
   for (WsfDraw::DrawEvent& event : aEvents)
   {
      data.Put(static_cast<unsigned char>(event.mDrawType));
      data.Put(event.mID);
      data.Put(static_cast<float>(aSimTime));

      if (event.mDrawType != WsfDraw::cERASE)
      {
         unsigned char r, g, b, a;
         r = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, event.mColor.GetData()[0] * 255.0f)));
         g = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, event.mColor.GetData()[1] * 255.0f)));
         b = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, event.mColor.GetData()[2] * 255.0f)));
         a = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, event.mColor.GetData()[3] * 255.0f)));
         data.Put(r);
         data.Put(g);
         data.Put(b);
         data.Put(a);
         data.Put((float)event.mDuration);
      }

      switch (event.mDrawType)
      {
      case WsfDraw::cLINE:
         Write(data, event.mVerts[0]);
         Write(data, event.mVerts[1]);
         data.Put(static_cast<unsigned char>(event.mLineSize));
         data.Put(static_cast<unsigned char>(event.mLineStyle));
         break;
      case WsfDraw::cPOINT:
         Write(data, event.mVerts[0]);
         data.Put(static_cast<unsigned char>(event.mPointSize));
         break;
      case WsfDraw::cICON:
         Write(data, event.mVerts[0]);
         data.Put(event.mHeading);
         Write(data, event.mIcon);
         break;
      case WsfDraw::cELLIPSE:
         Write(data, event.mVerts[0]);
         data.Put(static_cast<unsigned char>(event.mLineSize));
         data.Put(static_cast<unsigned char>(event.mLineStyle));
         data.Put(static_cast<unsigned char>(event.mEllipseMode));
         data.Put(event.mHeading);
         data.Put(event.mAxisA);
         data.Put(event.mAxisB);
         break;
      case WsfDraw::cERASE:
         break;
      case WsfDraw::cELLIPSOID:
         Write(data, event.mVerts[0]);
         data.Put(static_cast<unsigned char>(event.mLineSize));
         data.Put(static_cast<unsigned char>(event.mLineStyle));
         data.Put(static_cast<unsigned char>(event.mEllipseMode));
         data.Put(event.mHeading);
         data.Put(event.mPitch);
         data.Put(event.mRoll);
         data.Put(event.mAxisA);
         data.Put(event.mAxisB);
         data.Put(event.mAxisC);
         break;
      case WsfDraw::cQUADRILATERAL:
         Write(data, event.mVerts[0]);
         Write(data, event.mVerts[1]);
         Write(data, event.mVerts[2]);
         Write(data, event.mVerts[3]);
         break;
      case WsfDraw::cTEXT:
         Write(data, event.mVerts[0]);
         data.Put(static_cast<unsigned char>(event.mTextSize));
         Write(data, event.mText);
         break;
      case WsfDraw::cTIMER:
         Write(data, event.mVerts[0]);
         data.Put(static_cast<unsigned char>(event.mPointSize));
         break;
      case WsfDraw::cNONE:
         break;
      default:
         break;
      }
   }
   mDrawFilePtr->write(data.GetBuffer(), data.GetPutPos());
}

// virtual
void WsfDrawManager::Start()
{
   if (mDrawFilePtr != nullptr)
   {
      delete mDrawFilePtr;
      mDrawFilePtr = nullptr;
   }
   if ((mDrawFilePtr == nullptr) && (!mDrawFilePath.empty()))
   {
      mDrawFilePtr = new std::ofstream(mDrawFilePath.c_str(), std::ios_base::binary);
      mDrawFilePtr->write("WSF_DRAW 0001", 13);
   }

   // If we are using a deferred connection, wait until later
   WsfExtInterface* extPtr = static_cast<WsfExtInterface*>(GetSimulation().FindExtension("ext_interface"));
   double           deferredConnectionTime = 0.0;
   if (extPtr != nullptr)
   {
      deferredConnectionTime = extPtr->GetDeferredConnectionTime();
   }
   if (deferredConnectionTime > GetSimulation().GetSimTime())
   {
      GetSimulation().AddEvent(ut::make_unique<WsfOneShotEvent>(deferredConnectionTime + .5, [this]() { Start(); }));
      return;
   }
   for (DrawCommand& command : mDrawCommands)
   {
      WsfDraw d(this);
      if (!command.mLayer.empty())
      {
         d.SetLayer(command.mLayer);
      }
      d.SetPointSize(command.mPointSize);
      d.SetLineSize(command.mLineSize);
      if (command.mDrawTypeId == cROUTE)
      {
         WsfRoute* routePtr = WsfRouteTypes::Get(GetScenario()).Find(command.mObjectNameId.GetString());
         if (routePtr == nullptr)
         {
            auto out = ut::log::error() << "Draw route: Rout not found.";
            out.AddNote() << "Route: " << command.mObjectNameId;
            continue;
         }
         d.SetColor(command.mColor1);
         if (command.mDuration > 0)
         {
            d.SetDuration(command.mDuration);
         }

         d.BeginPolyline();
         d.VertexRoute(*routePtr);
         d.End();

         d.SetColor(command.mColor2);
         d.BeginPoints();
         d.VertexRoute(*routePtr);
         d.End();
      }
      else if (command.mDrawTypeId == cROUTE_NETWORK)
      {
         WsfRouteNetwork* networkPtr = WsfRouteNetworkTypes::Get(GetScenario()).Find(command.mObjectNameId);
         if (networkPtr == nullptr)
         {
            auto out = ut::log::error() << "Draw route_network: Route not found.";
            out.AddNote() << "Route: " << command.mObjectNameId;
            continue;
         }
         d.SetColor(command.mColor1);
         if (command.mDuration > 0)
         {
            d.SetDuration(command.mDuration);
         }
         for (unsigned int j = 0; j < networkPtr->GetRouteCount(); ++j)
         {
            d.BeginPolyline();
            d.VertexRoute((*networkPtr)[j]);
            d.End();
         }

         d.SetColor(command.mColor2);
         d.BeginPoints();
         for (unsigned int j = 0; j < networkPtr->GetRouteCount(); ++j)
         {
            d.VertexRoute((*networkPtr)[j]);
         }
         d.End();
      }
   }
}

WsfDrawManager* WsfDrawManager::Clone() const
{
   return new WsfDrawManager(*this);
}

WsfDrawManager::DrawCommand::DrawCommand()
{
   mColor1.Set(0.F, 0.F, 1.F);
   mColor2.Set(1.F, 1.F, 0.F);
   mDuration   = -1.0;
   mDrawTypeId = -1;
   mLineSize   = 1;
   mPointSize  = 2;
}

WsfDraw::WsfDraw(WsfSimulation& aSimulation)
   : mCurrentColor()
   , mPointSize(1)
   , mTextSize(12)
   , mLineSize(1)
   , mLineStyle(cSOLID)
   , mTimerSize(10)
   , mEllipseMode(cELLIPSE_FILL)
   , mID(0)
   , mDuration(cFOREVER)
   , mCurrentHeading(0.0)
   , mCurrentPitch(0.0)
   , mCurrentRoll(0.0)
   , mCurrentAxisA(0.0)
   , mCurrentAxisB(0.0)
   , mCurrentAxisC(0.0)
   , mEvents()
   , mCurrentText()
   , mCurrentIconName()
   , mTarget()
   , mLayer()
   , mUniqueId()
   , mBeginState(cBEGIN_NONE)
   , mDrawManagerPtr(nullptr)
{
   mDrawManagerPtr = GetDrawManager(aSimulation);
   mUniqueId       = GetNewId();
   mCurrentColor   = UtColor(1.0f, 0.5f, 0.5f);
}

WsfDraw::WsfDraw(WsfDrawManager* aManagerPtr)
   : mCurrentColor()
   , mPointSize(1)
   , mTextSize(12)
   , mLineSize(1)
   , mLineStyle(cSOLID)
   , mTimerSize(10)
   , mEllipseMode(cELLIPSE_FILL)
   , mID(0)
   , mDuration(cFOREVER)
   , mCurrentHeading(0.0)
   , mCurrentPitch(0.0)
   , mCurrentRoll(0.0)
   , mCurrentAxisA(0.0)
   , mCurrentAxisB(0.0)
   , mCurrentAxisC(0.0)
   , mEvents()
   , mCurrentText()
   , mCurrentIconName()
   , mTarget()
   , mLayer()
   , mUniqueId()
   , mBeginState(cBEGIN_NONE)
   , mVertexBuffer()
   , mDrawManagerPtr(aManagerPtr)
{
   mUniqueId     = GetNewId();
   mCurrentColor = UtColor(1.0f, 0.5f, 0.5f);
}

WsfDraw::WsfDraw(const WsfDraw& aSrc)
   : mCurrentColor(aSrc.mCurrentColor)
   , mPointSize(aSrc.mPointSize)
   , mTextSize(aSrc.mTextSize)
   , mLineSize(aSrc.mLineSize)
   , mLineStyle(aSrc.mLineStyle)
   , mTimerSize(aSrc.mTimerSize)
   , mEllipseMode(aSrc.mEllipseMode)
   , mID(aSrc.mID)
   , mDuration(aSrc.mDuration)
   , mCurrentHeading(0.0)
   , mCurrentPitch(0.0)
   , mCurrentRoll(0.0)
   , mCurrentAxisA(0.0)
   , mCurrentAxisB(0.0)
   , mCurrentAxisC(0.0)
   , mEvents()
   , mCurrentText(aSrc.mCurrentText)
   , mCurrentIconName(aSrc.mCurrentIconName)
   , mTarget(aSrc.mTarget)
   , mLayer(aSrc.mLayer)
   , mUniqueId(aSrc.mUniqueId)
   , mBeginState(aSrc.mBeginState)
   , mVertexBuffer(aSrc.mVertexBuffer)
   , mDrawManagerPtr(aSrc.mDrawManagerPtr)
{
}

//! Returns a unique ID to be used with SetId()
unsigned int WsfDraw::GetNewId()
{
   return mDrawManagerPtr ? mDrawManagerPtr->GetNewId() : 0;
}

void WsfDraw::BeginClear()
{
   mVertexBuffer.clear();
}

//! Begins line drawing mode.  Draws a line segment between each pair of Vertexes given by following Vertex() commands.
void WsfDraw::BeginLines()
{
   BeginClear();
   mBeginState = cBEGIN_LINES;
}

//! Begins poly-line drawing mode.  Draws a continuous line between each adjacent pair of Vertices given by following
//! Vertex() commands.
void WsfDraw::BeginPolyline()
{
   BeginClear();
   mBeginState = cBEGIN_POLYLINE;
}

//! Begins point drawing mode.  Draws a point at each vertex given by following Vertex() commands.
void WsfDraw::BeginPoints()
{
   BeginClear();
   mBeginState = cBEGIN_POINTS;
}

//! Begins icon drawing mode.  Draws an icon each vertex given by following Vertex() commands.
void WsfDraw::BeginIcons(double aHeading, const std::string& aIconName)
{
   BeginClear();
   mCurrentHeading  = aHeading;
   mCurrentIconName = aIconName;
   mBeginState      = cBEGIN_ICONS;
}

// begin ellipse drawing mode. Draws a 2D ellipse at each vertex given by the following Vertex() commands.
void WsfDraw::BeginEllipse(double aHeading, double aSemiMinor, double aSemiMajor)
{
   BeginClear();

   mCurrentHeading = aHeading;
   mCurrentAxisA   = aSemiMajor;
   mCurrentAxisB   = aSemiMinor;

   mBeginState = cBEGIN_ELLIPSE;
}

//! Begin ellipsoid drawing mode. Draws a 3D ellipsoid at each vertex given by the following Vertex() commands.
void WsfDraw::BeginEllipsoid(double aHeading, double aPitch, double aRoll, double aRadiusA, double aRadiusB, double aRadiusC)
{
   BeginClear();

   mCurrentHeading = aHeading;
   mCurrentPitch   = aPitch;
   mCurrentRoll    = aRoll;
   mCurrentAxisA   = aRadiusA;
   mCurrentAxisB   = aRadiusB;
   mCurrentAxisC   = aRadiusC;

   mBeginState = cBEGIN_ELLIPSOID;
}

//! Begins quarilateral drawing mode.  Draws a filled quarilateral defined by the four Vertices given by following
//! Vertex() commands. The first vertex is the top left corner and rotates clockwise to subsequent vertices. The four
//! vertices must define a convex quadrilateral (square, rectangle)
void WsfDraw::BeginQuadrilateral()
{
   BeginClear();
   mBeginState = cBEGIN_QUADRILATERAL;
}

void WsfDraw::BeginText(const std::string& aText)
{
   BeginClear();

   mCurrentText = aText;
   mBeginState  = cBEGIN_TEXT;
}

void WsfDraw::BeginTimer()
{
   BeginClear();
   mBeginState = cBEGIN_TIMER;
}

//! Completes a drawing mode initiated with a Begin method.
void WsfDraw::End()
{
   BeginClear();
   mBeginState = cBEGIN_NONE;
   Flush();
}

//! Reset the drawing options to the default
void WsfDraw::Reset()
{
   mDuration     = cFOREVER;
   mLineStyle    = cSOLID;
   mLineSize     = 1;
   mPointSize    = 1;
   mTextSize     = 12;
   mTimerSize    = 10;
   mID           = 0;
   mCurrentColor = UtColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//! Erases anything drawn with the specified ID.  This, in effect, shortens the duration of items already drawn.
void WsfDraw::Erase(unsigned int aId)
{
   DrawEvent e;
   e.mDrawType = cERASE;
   SetCurrentState(e);
   e.mID = aId;
   mEvents.push_back(e);
   Flush();
}

//! Erases anything drawn with the specified ID.  This, in effect, shortens the duration of items already drawn.
void WsfDraw::Erase(WsfStringId aId)
{
   Erase(static_cast<int>(WsfStringInt(aId)) | cSTRING_ID_MASK);
}

//! Set the target of the draw commands.  Used for extending WsfDraw to select one of multiple outputs.
void WsfDraw::SetTarget(const std::string& aTarget)
{
   if (mTarget != aTarget)
   {
      Flush();
      mTarget = aTarget;
   }
}

//! Set the layer of the draw commands. Can be used to put commands into different layers for filtering.
void WsfDraw::SetLayer(const std::string& aLayer)
{
   if (mLayer != aLayer)
   {
      Flush();
      mLayer = aLayer;
   }
}

//! Specifies a vertex using a LLA position
void WsfDraw::VertexLLA(double aLat, double aLon, double aAlt)
{
   VertexObject v;
   UtEntity::ConvertLLAToWCS(aLat, aLon, aAlt, v.mPosition.GetData());
   v.mVertexType = cABSOLUTE_WCS;
   AddVertex(v);
}

//! Specifies a vertex using a WCS position
void WsfDraw::VertexWCS(const double aWCS[3])
{
   VertexObject v;
   v.mPosition.Set(aWCS);
   v.mVertexType = cABSOLUTE_WCS;
   AddVertex(v);
}

//! Specifies a vertex in screen coordinates
void WsfDraw::VertexScreen(const double aLocation[3])
{
   VertexObject v;
   v.mPosition.Set(aLocation);
   v.mVertexType = cABSOLUTE_SCREEN;
   AddVertex(v);
}

//! Specifies a vertex that stays at a NED location relative to aPlatform
void WsfDraw::VertexScreenPlat(WsfPlatform& aPlatform, double aLocation[3])
{
   VertexObject v;
   v.mPlatformIndex = aPlatform.GetIndex();
   v.mPosition.Set(aLocation);
   v.mVertexType = cABSOLUTE_SCREEN;
   AddVertex(v);
}

//! Specifies a vertex that stays collocated with aPlatform
void WsfDraw::Vertex(WsfPlatform& aPlatform)
{
   VertexObject v;
   v.mPlatformIndex = aPlatform.GetIndex();
   v.mVertexType    = cRELATIVE_ZERO;
   AddVertex(v);
}

//! Specifies a vertex that stays at a ECS location relative to aPlatform
void WsfDraw::VertexECS(WsfPlatform& aPlatform, double aLocationECS[3])
{
   VertexObject v;
   v.mPlatformIndex = aPlatform.GetIndex();
   v.mPosition.Set(aLocationECS);
   v.mVertexType = cRELATIVE_ECS;
   AddVertex(v);
}

//! Specifies a vertex that stays at a NED location relative to aPlatform
void WsfDraw::VertexNED(WsfPlatform& aPlatform, double aLocationNED[3])
{
   VertexObject v;
   v.mPlatformIndex = aPlatform.GetIndex();
   v.mPosition.Set(aLocationNED);
   v.mVertexType = cRELATIVE_NED;
   AddVertex(v);
}

//! Specifies each point of a route as a vertex point.
void WsfDraw::VertexRoute(WsfRoute& aRoute)
{
   for (unsigned int i = 0; i < aRoute.GetSize(); ++i)
   {
      if (aRoute[i].GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         double alt = aRoute[i].GetAlt();
         if (alt < 0.0f)
         {
            alt = 0.0f;
         }
         VertexLLA(aRoute[i].GetLat(), aRoute[i].GetLon(), alt);
      }
   }
}

void WsfDraw::SetCurrentState(DrawEvent& aEvent)
{
   aEvent.mLineSize = mLineSize;
   if (mBeginState != cBEGIN_TIMER)
   {
      aEvent.mPointSize = mPointSize;
   }
   else
   {
      aEvent.mPointSize = mTimerSize;
   }
   aEvent.mTextSize = mTextSize;
   aEvent.mColor    = mCurrentColor;
   aEvent.mID       = mID;
   aEvent.mDuration = (float)mDuration;
}

void WsfDraw::Flush()
{
   if (!mEvents.empty())
   {
      mDrawManagerPtr->SendDrawUpdate(mDrawManagerPtr->GetSimulation().GetSimTime(), mTarget, mLayer, mEvents);
      mEvents.clear();
   }
}

void WsfDraw::AddVertex(const VertexObject& aVertex)
{
   mVertexBuffer.push_back(aVertex);
   size_t vertexCount = mVertexBuffer.size();
   if (vertexCount >= 2 && mBeginState == cBEGIN_LINES)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType  = cLINE;
      e.mLineStyle = mLineStyle;
      while (mVertexBuffer.size() >= 2)
      {
         e.mVerts[0] = mVertexBuffer.back();
         mVertexBuffer.pop_back();
         e.mVerts[1] = mVertexBuffer.back();
         mVertexBuffer.pop_back();
         mEvents.push_back(e);
      }
   }
   if (vertexCount >= 2 && mBeginState == cBEGIN_POLYLINE)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType  = cLINE;
      e.mLineStyle = mLineStyle;
      while (mVertexBuffer.size() >= 2)
      {
         e.mVerts[0] = mVertexBuffer[0];
         e.mVerts[1] = mVertexBuffer[1];
         mVertexBuffer.erase(mVertexBuffer.begin());
         mEvents.push_back(e);
      }
   }
   else if (mBeginState == cBEGIN_POINTS)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType = cPOINT;
      e.mVerts[0] = mVertexBuffer.back();
      mVertexBuffer.pop_back();
      e.mPointSize = mPointSize;
      mEvents.push_back(e);
   }
   else if (mBeginState == cBEGIN_ICONS)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType = cICON;
      e.mVerts[0] = mVertexBuffer.back();
      mVertexBuffer.pop_back();
      e.mIcon    = mCurrentIconName;
      e.mHeading = (float)mCurrentHeading;
      mEvents.push_back(e);
   }
   else if (mBeginState == cBEGIN_ELLIPSE)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType    = cELLIPSE;
      e.mLineStyle   = mLineStyle;
      e.mEllipseMode = mEllipseMode;
      e.mVerts[0]    = mVertexBuffer.back();
      mVertexBuffer.pop_back();
      e.mHeading = (float)mCurrentHeading;
      e.mAxisA   = (float)mCurrentAxisA;
      e.mAxisB   = (float)mCurrentAxisB;
      mEvents.push_back(e);
   }
   else if (mBeginState == cBEGIN_ELLIPSOID)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType    = cELLIPSOID;
      e.mLineStyle   = mLineStyle;
      e.mEllipseMode = mEllipseMode;
      e.mVerts[0]    = mVertexBuffer.back();
      mVertexBuffer.pop_back();
      e.mHeading = (float)mCurrentHeading;
      e.mPitch   = (float)mCurrentPitch;
      e.mRoll    = (float)mCurrentRoll;
      e.mAxisA   = (float)mCurrentAxisA;
      e.mAxisB   = (float)mCurrentAxisB;
      e.mAxisC   = (float)mCurrentAxisC;
      mEvents.push_back(e);
   }
   if (vertexCount >= 4 && mBeginState == cBEGIN_QUADRILATERAL)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType = cQUADRILATERAL;
      while (mVertexBuffer.size() >= 4)
      {
         e.mVerts[0] = mVertexBuffer[0];
         e.mVerts[1] = mVertexBuffer[1];
         e.mVerts[2] = mVertexBuffer[2];
         e.mVerts[3] = mVertexBuffer[3];
         mVertexBuffer.erase(mVertexBuffer.begin());
         mEvents.push_back(e);
      }
   }
   else if (mBeginState == cBEGIN_TEXT)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType = cTEXT;
      e.mVerts[0] = mVertexBuffer.back();
      mVertexBuffer.pop_back();
      e.mText = mCurrentText;
      mEvents.push_back(e);
   }
   else if (mBeginState == cBEGIN_TIMER)
   {
      DrawEvent e;
      SetCurrentState(e);
      e.mDrawType  = cTIMER;
      e.mVerts[0]  = mVertexBuffer.back();
      e.mPointSize = mTimerSize;
      mVertexBuffer.pop_back();
      mEvents.push_back(e);
   }
}

class WSF_EXPORT WsfScriptDrawClass : public UtScriptClass
{
public:
   WsfScriptDrawClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   void* Create(const UtScriptContext& aContext) override;
   void  Destroy(void* aObject) override { delete (static_cast<WsfDraw*>(aObject)); }
   void* Clone(void* aObject) override { return new WsfDraw(*static_cast<WsfDraw*>(aObject)); }

   UT_DECLARE_SCRIPT_METHOD(SetLineStyle);
   UT_DECLARE_SCRIPT_METHOD(SetLineSize);
   UT_DECLARE_SCRIPT_METHOD(SetEllipseMode);
   UT_DECLARE_SCRIPT_METHOD(SetPointSize);
   UT_DECLARE_SCRIPT_METHOD(SetTextSize);
   UT_DECLARE_SCRIPT_METHOD(SetTimerSize);
   UT_DECLARE_SCRIPT_METHOD(BeginLines);
   UT_DECLARE_SCRIPT_METHOD(BeginPoints);
   UT_DECLARE_SCRIPT_METHOD(BeginIcons);
   UT_DECLARE_SCRIPT_METHOD(BeginPolyline);
   UT_DECLARE_SCRIPT_METHOD(BeginQuadrilateral);
   UT_DECLARE_SCRIPT_METHOD(BeginEllipse);
   UT_DECLARE_SCRIPT_METHOD(BeginEllipsoid);
   UT_DECLARE_SCRIPT_METHOD(BeginCircle);
   UT_DECLARE_SCRIPT_METHOD(BeginSphere);
   UT_DECLARE_SCRIPT_METHOD(BeginText);
   UT_DECLARE_SCRIPT_METHOD(BeginTimer);
   UT_DECLARE_SCRIPT_METHOD(End);
   UT_DECLARE_SCRIPT_METHOD(Vertex_1);
   UT_DECLARE_SCRIPT_METHOD(Vertex_2);
   UT_DECLARE_SCRIPT_METHOD(VertexLLA);
   UT_DECLARE_SCRIPT_METHOD(VertexWCS);
   UT_DECLARE_SCRIPT_METHOD(VertexWCS_2);
   UT_DECLARE_SCRIPT_METHOD(VertexECS);
   UT_DECLARE_SCRIPT_METHOD(VertexNED);
   UT_DECLARE_SCRIPT_METHOD(VertexScreen);
   UT_DECLARE_SCRIPT_METHOD(VertexScreenPlat);
   UT_DECLARE_SCRIPT_METHOD(VertexRoute);
   UT_DECLARE_SCRIPT_METHOD(SetColor_1);
   UT_DECLARE_SCRIPT_METHOD(SetColor_2);
   UT_DECLARE_SCRIPT_METHOD(SetColor_3);
   UT_DECLARE_SCRIPT_METHOD(SetTarget);
   UT_DECLARE_SCRIPT_METHOD(SetLayer);
   UT_DECLARE_SCRIPT_METHOD(Erase);
   UT_DECLARE_SCRIPT_METHOD(Erase_2);
   UT_DECLARE_SCRIPT_METHOD(SetId);
   UT_DECLARE_SCRIPT_METHOD(SetId_2);
   UT_DECLARE_SCRIPT_METHOD(NewId);
   UT_DECLARE_SCRIPT_METHOD(SetDuration);
   UT_DECLARE_SCRIPT_METHOD(Reset);
};

std::unique_ptr<UtScriptClass> WsfDraw::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptDrawClass>(aClassName, aScriptTypesPtr);
}

WsfDrawManager* WsfDraw::GetDrawManager()
{
   return mDrawManagerPtr;
}

WsfDrawManager* WsfDraw::GetDrawManager(WsfSimulation& aSimulation)
{
   return static_cast<WsfDrawManager*>(aSimulation.FindExtension("wsf_draw"));
}

WsfScriptDrawClass::WsfScriptDrawClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);
   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<SetLineStyle>());
   AddMethod(ut::make_unique<SetLineSize>());
   AddMethod(ut::make_unique<SetEllipseMode>());
   AddMethod(ut::make_unique<SetPointSize>());
   AddMethod(ut::make_unique<SetTextSize>());
   AddMethod(ut::make_unique<SetTimerSize>());
   AddMethod(ut::make_unique<BeginLines>("BeginLines"));
   AddMethod(ut::make_unique<BeginPolyline>("BeginPolyline"));
   AddMethod(ut::make_unique<BeginQuadrilateral>("BeginQuadrilateral"));
   AddMethod(ut::make_unique<BeginPoints>("BeginPoints"));
   AddMethod(ut::make_unique<BeginIcons>("BeginIcons"));
   AddMethod(ut::make_unique<BeginEllipse>("BeginEllipse"));
   AddMethod(ut::make_unique<BeginEllipsoid>("BeginEllipsoid"));
   AddMethod(ut::make_unique<BeginCircle>());
   AddMethod(ut::make_unique<BeginSphere>());
   AddMethod(ut::make_unique<BeginText>());
   AddMethod(ut::make_unique<BeginTimer>());
   AddMethod(ut::make_unique<End>("End"));
   AddMethod(ut::make_unique<Vertex_1>("Vertex"));
   AddMethod(ut::make_unique<Vertex_2>("Vertex"));
   AddMethod(ut::make_unique<VertexLLA>("VertexLLA"));
   AddMethod(ut::make_unique<VertexWCS>("VertexWCS"));
   AddMethod(ut::make_unique<VertexWCS_2>("VertexWCS"));
   AddMethod(ut::make_unique<VertexECS>("VertexECS"));
   AddMethod(ut::make_unique<VertexNED>("VertexNED"));
   AddMethod(ut::make_unique<VertexScreen>("VertexScreen"));
   AddMethod(ut::make_unique<VertexScreenPlat>("VertexScreenPlat"));
   AddMethod(ut::make_unique<VertexRoute>());
   AddMethod(ut::make_unique<SetColor_1>("SetColor"));
   AddMethod(ut::make_unique<SetColor_2>("SetColor"));
   AddMethod(ut::make_unique<SetColor_3>("SetColor"));
   AddMethod(ut::make_unique<SetTarget>());
   AddMethod(ut::make_unique<SetLayer>());
   AddMethod(ut::make_unique<Erase>());
   AddMethod(ut::make_unique<Erase_2>("Erase"));
   AddMethod(ut::make_unique<SetId>());
   AddMethod(ut::make_unique<SetId_2>("SetId"));
   AddMethod(ut::make_unique<NewId>());
   AddMethod(ut::make_unique<SetDuration>());
   AddMethod(ut::make_unique<Reset>());
}

void* WsfScriptDrawClass::Create(const UtScriptContext& aContext)
{
   return new WsfDraw(*WsfScriptContext::GetSIMULATION(aContext));
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetLineStyle, 1, "void", "string")
{
   int         pattern    = 6; // 6 is a solid line
   std::string patternStr = aVarArgs[0].GetString();
   size_t      length     = patternStr.length();
   for (size_t i = 0; i < length; ++i)
   {
      patternStr[i] = tolower(patternStr[i]);
   }

   if (patternStr == "dashed")
   {
      pattern = 0;
   }
   else if (patternStr == "long_dashed")
   {
      pattern = 1;
   }
   else if (patternStr == "dotted")
   {
      pattern = 2;
   }
   else if (patternStr == "dotted2")
   {
      pattern = 3;
   }
   else if (patternStr == "dash_dot")
   {
      pattern = 4;
   }
   else if (patternStr == "dash_dot2")
   {
      pattern = 5;
   }
   aObjectPtr->SetLineStyle((WsfDraw::LineStyle)pattern);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetEllipseMode, 1, "void", "string")
{
   int         pattern    = 0; // 0 is fill mode
   std::string patternStr = aVarArgs[0].GetString();
   size_t      length     = patternStr.length();
   for (size_t i = 0; i < length; ++i)
   {
      patternStr[i] = tolower(patternStr[i]);
   }

   if (patternStr == "fill")
   {
      pattern = 0;
   }
   else if (patternStr == "line")
   {
      pattern = 1;
   }
   aObjectPtr->SetEllipseMode((WsfDraw::EllipseMode)pattern);
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetLineSize, 1, "void", "int")
{
   aObjectPtr->SetLineSize(aVarArgs[0].GetInt());
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetPointSize, 1, "void", "int")
{
   aObjectPtr->SetPointSize(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetTextSize, 1, "void", "int")
{
   aObjectPtr->SetTextSize(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetTimerSize, 1, "void", "int")
{
   aObjectPtr->SetTimerSize(aVarArgs[0].GetInt());
}

// WsfDraw.BeginEllipse(double heading, double semi_minor, double semi_major);
UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginEllipse, 3, "void", "double,double,double")
{
   aObjectPtr->BeginEllipse(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginEllipsoid, 6, "void", "double,double,double,double,double,double")
{
   aObjectPtr->BeginEllipsoid(aVarArgs[0].GetDouble(),
                              aVarArgs[1].GetDouble(),
                              aVarArgs[2].GetDouble(),
                              aVarArgs[3].GetDouble(),
                              aVarArgs[4].GetDouble(),
                              aVarArgs[5].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginCircle, 2, "void", "double,double")
{
   aObjectPtr->BeginEllipse(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginSphere, 4, "void", "double,double,double,double")
{
   aObjectPtr->BeginEllipsoid(aVarArgs[0].GetDouble(),
                              aVarArgs[1].GetDouble(),
                              aVarArgs[2].GetDouble(),
                              aVarArgs[3].GetDouble(),
                              aVarArgs[3].GetDouble(),
                              aVarArgs[3].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginText, 1, "void", "string")
{
   aObjectPtr->BeginText(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginLines, 0, "void", "")
{
   aObjectPtr->BeginLines();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginPolyline, 0, "void", "")
{
   aObjectPtr->BeginPolyline();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginQuadrilateral, 0, "void", "")
{
   aObjectPtr->BeginQuadrilateral();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginPoints, 0, "void", "")
{
   aObjectPtr->BeginPoints();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginIcons, 2, "void", "double, string")
{
   aObjectPtr->BeginIcons(aVarArgs[0].GetDouble(), aVarArgs[1].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, BeginTimer, 0, "void", "")
{
   aObjectPtr->BeginTimer();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, End, 0, "void", "")
{
   aObjectPtr->End();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, Vertex_1, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Vertex(*platformPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, Vertex_2, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       locationWCS[3];
   geoPointPtr->GetLocationWCS(locationWCS);
   aObjectPtr->VertexWCS(locationWCS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexLLA, 3, "void", "double,double,double")
{
   aObjectPtr->VertexLLA(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexWCS, 3, "void", "double,double,double")
{
   double locationWCS[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->VertexWCS(locationWCS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexWCS_2, 1, "void", "Vec3")
{
   UtVec3d* wcsPtr;
   UtScriptDataUnpack(aVarArgs, wcsPtr);
   aObjectPtr->VertexWCS(wcsPtr->GetData());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexECS, 4, "void", "WsfPlatform,double,double,double")
{
   WsfPlatform* platformPtr    = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       locationECS[3] = {aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble()};
   aObjectPtr->VertexECS(*platformPtr, locationECS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexNED, 4, "void", "WsfPlatform,double,double,double")
{
   WsfPlatform* platformPtr    = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       locationNED[3] = {aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble()};
   aObjectPtr->VertexNED(*platformPtr, locationNED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexScreen, 2, "void", "double,double")
{
   double locationScreen[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), 0.0};
   aObjectPtr->VertexScreen(locationScreen);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexScreenPlat, 3, "void", "WsfPlatform,double,double")
{
   WsfPlatform* platformPtr    = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       locationNED[3] = {aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), 0.0};
   aObjectPtr->VertexScreenPlat(*platformPtr, locationNED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, VertexRoute, 1, "void", "WsfRoute")
{
   WsfRoute* routePtr = static_cast<WsfRoute*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->VertexRoute(*routePtr);
}

// this version of SetColor assumes a default value of 1.0 as the alpha value.
UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetColor_1, 3, "void", "double,double,double")
{
   aObjectPtr->SetColor(
      UtColor((float)aVarArgs[0].GetDouble(), (float)aVarArgs[1].GetDouble(), (float)aVarArgs[2].GetDouble()));
}

// this version of SetColor has an extra parameter to explicitly set the alpha value of the WsfDraw shape.
UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetColor_2, 4, "void", "double,double,double,double")
{
   aObjectPtr->SetColor(UtColor((float)aVarArgs[0].GetDouble(),
                                (float)aVarArgs[1].GetDouble(),
                                (float)aVarArgs[2].GetDouble(),
                                (float)aVarArgs[3].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetColor_3, 1, "void", "Color")
{
   aObjectPtr->SetColor(*aVarArgs[0].GetPointer()->GetAppObject<UtColor>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetTarget, 1, "void", "string")
{
   aObjectPtr->SetTarget(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetLayer, 1, "void", "string")
{
   aObjectPtr->SetLayer(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetId, 1, "void", "string")
{
   aObjectPtr->SetId(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetId_2, 1, "void", "int")
{
   aObjectPtr->SetId(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, NewId, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNewId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, Erase, 1, "void", "string")
{
   aObjectPtr->Erase(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, Erase_2, 1, "void", "int")
{
   aObjectPtr->Erase(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, SetDuration, 1, "void", "double")
{
   aObjectPtr->SetDuration(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDrawClass, WsfDraw, Reset, 0, "void", "")
{
   aObjectPtr->Reset();
}
