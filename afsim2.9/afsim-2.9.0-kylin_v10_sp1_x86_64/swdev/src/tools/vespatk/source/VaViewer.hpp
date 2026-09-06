// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaViewer_hpp
#define VaViewer_hpp

#include "vespatk_export.h"

#include <list>
#include <map>

#include <osg/Matrix>

#include "UtColor.hpp"
#include "UtEarth.hpp"
#include "UtoCmeTerrain.hpp"
#include "UtoMapProjection.hpp"
#include "VaCallback.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "VaEvent.hpp"
#include "VaObject.hpp"
#include "VaOverlay.hpp"
#include "VaOverlayModelInstantiations.hpp"
#include "VaPosition.hpp"
#include "VaUniqueId.hpp"


#ifdef KeyPress
#undef KeyPress // X11 defines KeyPress :(
#endif
#undef KeyRelease

class UtoViewer;
using UtoPosition = osg::Vec3d;
namespace osg
{
class Node;
class Group;
} // namespace osg
namespace osgViewer
{
class Viewer;
}
namespace vespa
{
class VaEntity;
class VaCamera;
class VaCameraBase;
class VaChooser;
class VaOverlayZoomBox;
class VaPosition;

//! A class for application viewers.
class VESPATK_EXPORT VaViewer : public VaObject
{
public:
   friend class VaAttachment;
   friend class VaCameraBase;
   friend class VaWidget;

   using Overlays = std::map<std::string, VaOverlay*>;

   enum ProjectionMode
   {
      PmInvalid        = 0,
      PmOrthographic2D = 1,
      PmOrthographic3D = 2,
      PmPerspective    = 3,
      PmMaxNumberProjections
   };

   enum
   {
      ShiftKey          = 1,
      CtrlKey           = 2,
      AltKey            = 4,
      CapsLockState     = 8,
      NumLockState      = 16,
      EscKey            = 32,
      ShiftCtrlAltKeys  = ShiftKey | CtrlKey | AltKey,
      CapsNumLockStates = CapsLockState | NumLockState
   };

   enum
   {
      PassOn         = 1,
      DontPassOn     = 2,
      AddIdleProc    = 8,
      RemoveIdleProc = 16
   };

   enum
   {
      SimpleViewer    = 0,
      HighlightViewer = 1,
      RenderToTexture = 2,
      ModelViewer     = 3,
      NoViewer        = 5
   };
   enum
   {
      SmallFeatureCulling = 1,
      UseMaps             = 2,
      Ephemeris           = 4,
      Standard            = 6
   };

   VaViewer(int aViewerType = HighlightViewer, int aViewerOptions = Standard);
   ~VaViewer() override;

   virtual void SetSelectionMask(int aMask) { mSelectionMask = aMask; }

   virtual void Initialize(osgViewer::Viewer* aViewerPtr, bool aShaderable, bool aGeomShaderable);

   virtual void SetupViewer(osgViewer::Viewer* aViewerPtr, bool aUseShaders);

   virtual UtoViewFilter* GetNewViewFilter();

   bool IsInitialized() const { return mInitialized; }

   virtual void Resize(int aX, int aY, int aWidth, int aHeight);

   // Viewer Interface

   virtual int GetWidth() const;

   virtual int GetHeight() const;

   void         GetMousePosition(int& aX, int& aY);
   bool         GetMouseInViewer() { return mMouseIsIn; }
   virtual void SetCenter(const VaPosition& aPosition);
   virtual void AnimateToCenter(const VaPosition& aPosition, double aAz = 0.0, double aEl = 90.0, double aDuration = 0.0);

   virtual void CenterCamera(bool aCenterOnSelection = true);
   virtual void CenterOnEntity(const VaEntity& aEntity);
   void         CenterOnBoundingBox(VaBoundingBox aBoundingBox);

   virtual bool          SetProjectionMode(ProjectionMode aProjectionMode);
   ProjectionMode        GetProjectionMode() const { return mProjectionMode; }
   static std::string    GetProjectionModeString(ProjectionMode aProjectionMode);
   static ProjectionMode GetProjectionModeEnum(const std::string& aProjectionModeString);


   ////////////////TODO ACCESS THROUGH CAMERA/////////////////////
   virtual bool ScreenToWorld(unsigned int aX, unsigned int aY, VaPosition& aPosition) const;

   virtual bool WorldToScreen(const VaPosition& aPosition, unsigned int& aX, unsigned int& aY, unsigned int& aZ) const;

   virtual bool WorldToScreen(const VaPosition& aPosition, double& aX, double& aY, double& aZ) const;
   ////////////////////////END////////////////////////////////////
   virtual bool LLAToWorld(double aLat, double aLon, double aAlt, VaPosition& aPosition) const;

   virtual double HeightOfTerrain(const VaPosition& aPosition) const;

   // Camera Interface

   VaCamera* GetCamera() const { return mCameraPtr; }

   void GetCameraPosition(VaPosition& aPosition) const;

   unsigned int GetUpdateViewCount() const { return mUpdateViewCount; }

   double GetMetersPerPixel() const;
   double GetMetersPerDegreeLat() const;
   double GetMetersPerDegreeLon() const;

   int    GetFeedbackFormat() const { return mFeedbackFormat; }
   double GetFeedbackPrecision() const { return mFeedbackPrecision; }

   // Overlay Interface

   VaOverlay* AddOverlay(const std::string&      aOverlayType,
                         const std::string&      aName         = "",
                         VaOverlay::OverlayScope aOverlayScope = VaOverlay::cOVERLAY_GLOBAL);
   VaOverlay* AddOverlay(const VaOverlay& aOverlay);
   bool       AddOverlay(VaOverlay* aOverlayPtr);
   bool       RemoveOverlay(const std::string& aOverlayName);
   void       DeleteLater(VaOverlay* aOverlayPtr);

   const UtoMapProjection& GetMapProjection() const { return mMapProjection; }
   // this can be used as a rough check of visibility vs. the earth, if true the view is clear of the sphere
   bool CheckSphereOcclusion(const vespa::VaPosition& aPos, double aSphereSize = UtEarth::cA) const;
   bool MapProjectionIsGeocentric() const { return (mMapProjection.projection == UtoMapProjection::Geocentric); }

   UtoViewer*       GetViewer() { return (UtoViewer*)mViewerPtr; }
   const UtoViewer* GetViewer() const { return (UtoViewer*)mViewerPtr; }

   bool WindowToWorld(UtoPosition& aPos);

   virtual void CameraUpdated() {}

   bool UseECI() const { return (mECI && !mFlatMap); }

   void SuppressMouseSelectionChanges(bool aSuppressMouseSelectionChanges)
   {
      mSuppressMouseSelectionChanges = aSuppressMouseSelectionChanges;
   }

   virtual void ToggleStats();
   virtual void ScreenShot(const std::string& aOutputDir, int aInstanceId = 1);
   virtual void ScreenShotToFilename(const std::string& aFilename);

   virtual UtoCmeTerrain* CreateDatabase(const std::string& aFiles, const std::string& aProjection, const double aOffset);
   virtual UtoTerrain*    GetDatabase();

   virtual void ClearTerrain();

   virtual void EnableLighting(bool aState);
   virtual void UpdateLighting(const UtoColor& aColor, // for diffuse and specular components
                               float           aSpaceFactor);    // sharpens shadow, 0 will be standard gl lighting

   virtual void SetFlatMapMode(bool aState); // makes a more appropriate (well thought out) flat map

   virtual void SetTrueScale(bool aValue);
   virtual void SetIconScale(float aValue);

   virtual void SuppressTeamColor(bool aValue);
   virtual void SetSunVector(const osg::Vec3& aVector);
   virtual void SetUniform(const char* aName, const float aValue);
   virtual void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3);
   virtual void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4);

   virtual void HighlightShape(osg::Node* aNodePtr, bool aEnableHighlight);
   virtual void HighlightShapes(VaEntity*                       aEntityPtr,
                                const std::vector<std::string>& aShapes,
                                bool                            aEnableHighlight,
                                const UtColor&                  aColor);
   virtual void HighlightShape(osg::Node* aNodePtr, bool aEnableHighlight, const UtColor& aColor);
   virtual void HighlightNode(osg::Node*     aNodePtr,
                              bool           aEnableHighlight,
                              const UtColor& aColor); // this is for loose nodes, not in the standard scenegraph

   virtual void SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename);

   virtual void SetUniform(const char*          aName,
                           const unsigned int   aIndex,
                           const unsigned char* aData,
                           const unsigned int   aWidth,
                           const unsigned int   aHeight);

   virtual void DumpSceneGraph(const char* aFilename);

   virtual osg::Group* GetSceneRoot();

   virtual void
   PickFromScreen(VaHitList& aHits, int aMouseX, int aMouseY, bool aHoverMode = false, bool aDoubleClick = false) const;

   // Returns the chosen entity or attachment from a list of hits. If the list contains multiple entries, a menu will be
   // displayed for the user.
   virtual bool ChooseHit(const VaHitList& aHits, int aMouseX, int aMouseY, VaHitEntry& aPicked);

   /// Called when a box selection occurs. The implementation is empty.
   /// Expect the subclass to provide the implementation.
   /// @param aBox The rectangle selection box.
   /// @param aAddSelection Add to the current selections if aAddSelection is true.
   virtual void      SetSelection(const vespa::VaSelectionBox& aBox, bool aAddSelection){};
   virtual void      ClearSelection();
   virtual bool      SelectionIncludes(VaHitList& aSet);
   virtual VaHitSet& GetSelection() { return mSelection; }

   void RemoveAttachmentSelection(VaAttachment* aAttachmentPtr, int aSubpartId = 0);

   virtual void OverridePerspective(double aXMin, double aXMax, double aYMin, double aYMax, double aNear, double aFar);

   void               GetViewport(double& aX, double& aY, double& aW, double& aH) const;
   const osg::Matrix& GetViewMatrix() const;
   const osg::Matrix  GetProjectionMatrix() const;

   const float GetIconScale() const;

   const float GetTrueScale() const;

   void BringOverlayToFront(VaOverlay* aOverlay);

   void SetChooser(VaChooser* aChooser) { mChooserPtr = aChooser; }

   void AddSubordinate(VaViewer* aViewerPtr);
   void RemoveSubordinate(VaViewer* aViewerPtr);

   template<class OType>
   OType* PickOverlayFromScreen(int aX, int aY) const;

   VaOverlayModelInstantiations& GetOrCreateModelInstantiationsOverlay();

   template<class T>
   std::vector<T*> FindOverlaysOfType() const;

   template<class T>
   T* FindFirstOverlayOfType() const;

   void        ToggleMode();
   osg::Group* GetCelestialNode();

   void RequestUpdate() { mUpdateView = true; }

protected:
   virtual void PrepareToRedraw();
   // **********************************************************************
   // The following implement methods that ARE defined in the base class.
   // **********************************************************************
   // Button 1 operations.

   virtual int Btn1Click(int aMouseX, int aMouseY, int aState);
   virtual int Btn1DblClick(int aMouseX, int aMouseY, int aState);
   virtual int Btn1Down(int aMouseX, int aMouseY, int aState);
   virtual int Btn1Drag(int aMouseX, int aMouseY, int aState);
   virtual int Btn1Up(int aMouseX, int aMouseY, int aState);

   // Button 2 operations.

   virtual int Btn2Down(int aMouseX, int aMouseY, int aState);
   virtual int Btn2Drag(int aMouseX, int aMouseY, int aState);
   virtual int Btn2Up(int aMouseX, int aMouseY, int aState);
   virtual int Btn2Click(int aMouseX, int aMouseY, int aState);

   // Button 3 operations.

   virtual int Btn3Down(int aMouseX, int aMouseY, int aState);
   virtual int Btn3Drag(int aMouseX, int aMouseY, int aState);
   virtual int Btn3Up(int aMouseX, int aMouseY, int aState);
   virtual int Btn3Click(int aMouseX, int aMouseY, int aState);

   virtual int KeyPress(int aMouseX, int aMouseY, int aKey, int aState);

   virtual int KeyRelease(int aMouseX, int aMouseY, int aKey, int aState);

   virtual int KeyDblUp(int aMouseX, int aMouseY, int aKey, int aState);

   virtual int MouseMove(int aMouseX, int aMouseY);

   virtual int WheelUp(int aDelta);
   virtual int WheelDown(int aDelta);

   virtual int MouseEnter();
   virtual int MouseLeave();

   virtual bool ChooseAndPickHit(const VaHitList& aHits,
                                 int              aMouseX,
                                 int              aMouseY,
                                 bool             aToggleSelection,
                                 bool             aDoubleClick,
                                 VaHitEntry&      aPicked);
   virtual void PostChoose(const VaHitEntry& aPicked, const std::map<VaHitEntry, std::string>& aPickMap) {}

   // Touch Screen Operations.
   virtual int Touch(int aScreenX, int aScreenY);
   virtual int Swipe(vespa::SwipeDirection aHorizontalDirection, vespa::SwipeDirection aVerticalDirection, double aAngle);
   virtual int Pan(double aAcceleration, double aDelta);
   virtual int Pinch(double aScale, double aAngle, int aCenterX, int aCenterY);


   // mouse overlay operations

   Overlays::const_iterator GetMousedOverlay(int aMouseX, int aMouseY) const;

   virtual bool Btn1ClickOverlays(int aMouseX, int aMouseY, int aState);
   virtual bool Btn1DblClickOverlays(int aMouseX, int aMouseY, int aState);
   virtual bool Btn1DownOverlays(int aMouseX, int aMouseY, int aState);
   virtual bool Btn1DragOverlays(int aMouseX, int aMouseY, int aState);
   virtual bool Btn1UpOverlays(int aMouseX, int aMouseY, int aState);

   virtual bool Btn3DownOverlays(int aMouseX, int aMouseY, int aState);

   virtual bool KeyPressOverlays(int aMouseX, int aMouseY, int aKey, int aState);

   virtual void MouseMoveOverlays(int aMouseX, int aMouseY);

   virtual bool WheelOverlays(int aMouseX, int aMouseY, int aDelta);

   virtual void SetPerspectiveCamera(const osg::Matrix& aViewMatrix,
                                     double             aXYZ[3],
                                     double             aFovY,
                                     double             aNear,
                                     double             aFar,
                                     bool               aProjectionManaged);

   virtual void
   SetOrthographicCamera(const osg::Matrix& aViewMatrix, double aZoom, double aNear, double aFar, bool aProjectionManaged);

   VaViewer(const VaViewer& aSrc); // Disable copy construction.

   void SetCurrentCursorPosition(int aMouseX, int aMouseY);

   void SetSavedCursorPosition(int aMouseX, int aMouseY);

   virtual void ShowSelectionIndication(bool aState);
   virtual bool SelectionIndicationIsShown() { return mShowSelectionIndication; }

   virtual void CameraUpdatedCB(VaViewer* aViewerPtr);

   void SetPickOnRightClick(bool aCanPick);

   template<typename... Args>
   static bool EventOverride(const VaCallbackListN<bool(Args...)>& aCallbackList, Args... args)
   {
      if (aCallbackList.IsEmpty())
      {
         return false;
      }
      bool eventProcessed = false;

      using CallbackType = typename VaCallbackListN<bool(Args...)>::CallbackType;
      std::vector<CallbackType*> callbackList;
      aCallbackList.GetCallbacks(callbackList);
      for (auto cb : callbackList)
      {
         eventProcessed |= (*cb)(args...);
      }
      return eventProcessed;
   }

   UtoViewer*       mViewerPtr{nullptr};
   VaCallbackHolder mCallbacks;
   bool             mUpdateView{true};
   ProjectionMode   mProjectionMode{PmPerspective};
   bool             mInitialized{false};
   VaHitEntry       mPickedEntry;
   VaCamera*        mCameraPtr{nullptr};

protected:
   bool HadFirstDraw() { return mHadFirstDraw; }

   class VaHoverUpdateEvent : public VaEvent
   {
   public:
      VaHoverUpdateEvent(double aTime, unsigned int aViewerIndex)
         : VaEvent(aTime)
         , mViewerIndex(aViewerIndex)
      {
      }
      VaEvent::EventDisposition Execute() override;

   private:
      unsigned int mViewerIndex;
   };

   void UpdateView();

   void SetFeedbackFormatAndPrecision();

   virtual void MapLoadedMapCB();
   void         ECI_ModeChangedCB(bool aState);
   void         CameraSetCenterCB();

   void SetIntensityCB(const float& aValue);
   void SetSaturationCB(const float& aValue);
   void SetContrastCB(const float& aValue);

   virtual void EntitySelectCB(VaViewer* aViewerPtr, int aState, VaEntity* aEntityPtr);
   virtual void EntitySelectedCB(VaViewer* aViewerPtr, int aState, VaEntity* aEntityPtr);
   virtual void AttachmentSelect(int aState, VaAttachment* aAttachmentPtr, const VaAttachmentSubId& aSubId);
   virtual void AttachmentSelectedCB(VaViewer*                aViewerPtr,
                                     int                      aState,
                                     VaAttachment*            aAttachmentPtr,
                                     const VaAttachmentSubId& aSubId);

   virtual void BeginUserStatCB(const std::string& aName);
   virtual void EndUserStatCB(const std::string& aName);

   void SetActiveCamera(int aMode);

   // Private mouse wheel implementation
   int WheelImp(int aDelta);

   void ShowZoomBox(bool aZoomingIn);

   virtual void PrivateSaveState(std::map<std::string, std::string>& aMap) {}
   virtual void PrivateLoadState(const std::map<std::string, std::string>& aMap) {}

   virtual void ProcessPickOnRightClick(int aMouseX, int aMouseY, const int aState);
   void         UiHover(vespa::VaHitEntry aEntry);
   virtual void HighlightTempSelection(const vespa::VaHitEntry& aHitEntry, bool aEnable);

   static const std::string cProjectionModeStrings[PmMaxNumberProjections];

   unsigned int      mUpdateViewCount{0};
   int               mViewX{0};
   int               mViewY{0};
   int               mViewWidth{0};
   int               mViewHeight{0};
   int               mFeedbackFormat{UtAngle::FmtDMS};
   double            mFeedbackPrecision{1.0};
   Overlays          mOverlays;
   bool              mDragPossible{false};
   bool              mDragActive{false};
   VaEntity*         mDragEntity{nullptr};
   bool              mMoveActive{false};
   VaPosition        mDragLLA;
   VaOverlay*        mCompassPtr{nullptr};
   VaOverlay*        mScalePtr{nullptr};
   VaOverlay*        mHeldOverlayPtr{nullptr};
   VaOverlayZoomBox* mZoomBoxPtr{nullptr};
   bool              mCameraDrag{false};
   VaHitList         mHitList;
   UtoMapProjection  mMapProjection;
   bool              mECI{false};
   bool              mFlatMap{false};
   VaChooser*        mChooserPtr{nullptr};

   int mMouseX{0};
   int mMouseY{0};

   bool mSuppressMouseSelectionChanges{false};

   bool mShowSelectionIndication{true};
   bool mPickOnRightClick{true};

   osg::Vec3 mSunVector{1.0F, 0.0F, 0.0F};
   bool      mTrueScale{false};
   bool      mSuppressTeamColor{false};
   float     mIntensity{0.0F};
   float     mSaturation{0.0F};
   float     mContrast{0.0F};

   int mSelectionMask{cVESPA_ENTITY_SELECTION_MASK};

   VaHitSet mSelection;
   int      mViewerType;
   int      mOptions;

   bool  mOverridePerspective{false};
   float mOverrideYMin;
   float mOverrideYMax;
   float mOverrideXMin;
   float mOverrideXMax;
   float mOverrideNear;
   float mOverrideFar;

   float      mIconScale{1.0F};
   VaHitEntry mPreviousSelectionUiHover;

   bool                          mMouseIsIn{true};
   bool                          mHadFirstDraw{false};
   std::map<int, VaViewer*>      mSubordinateMap;
   std::vector<VaOverlay*>       mOverlaysToBeRemoved;
   VaOverlayModelInstantiations* mModelInstantitationsOverlayPtr{nullptr};
};

template<class OType>
OType* vespa::VaViewer::PickOverlayFromScreen(int aX, int aY) const
{
   OType* overlayPtr = nullptr;

   auto candidate = GetMousedOverlay(aX, aY);
   if (candidate != mOverlays.end())
   {
      return dynamic_cast<OType*>(candidate->second);
   }
   return overlayPtr;
}

template<class T>
std::vector<T*> vespa::VaViewer::FindOverlaysOfType() const
{
   std::vector<T*> typedList;
   for (auto&& overlay : mOverlays)
   {
      T* typedOverlay = dynamic_cast<T*>(overlay.second);
      if (typedOverlay != nullptr)
      {
         typedList.push_back(typedOverlay);
      }
   }
   return typedList;
}

template<class T>
T* vespa::VaViewer::FindFirstOverlayOfType() const
{
   for (auto&& overlay : mOverlays)
   {
      T* typedOverlay = dynamic_cast<T*>(overlay.second);
      if (typedOverlay != nullptr)
      {
         return typedOverlay;
      }
   }
   return nullptr;
}

} // end namespace vespa
VA_DECLARE_OBJECT_TYPE(vespa::VaViewer)

#endif
