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

#ifndef WkfOverlayWsfDraw_hpp
#define WkfOverlayWsfDraw_hpp

#include "wkf_common_export.h"

#include <mutex>
#include <set>

#include <QObject>

#include "UtCast.hpp"
#include "UtVec3.hpp"
#include "Uto.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

class GenI;

class UtoEllipseShape;
class UtoMapProjection;
class UtoPointListShape;
class UtoPolyLineShape;
class UtoPolygonShape;
class UtoRawShape;
class UtoSphereShape;
class UtoTimerShape;

namespace wkf
{
class WKF_COMMON_EXPORT OverlayWsfDrawBase : public vespa::VaOverlay
{
public:
   OverlayWsfDrawBase();
   OverlayWsfDrawBase(const std::string& aName);
   enum VertexType
   {
      cUNSET_VERTEX,
      cABSOLUTE_WCS,
      cRELATIVE_ZERO,
      cRELATIVE_ECS,
      cRELATIVE_NED,
      cABSOLUTE_SCREEN
   };
   struct VertexObject
   {
      VertexObject()
         : mReferenceEntityPtr(nullptr)
      {
      }

      bool IsRelative() const
      {
         return (mVertexType != cUNSET_VERTEX && mVertexType != cABSOLUTE_WCS &&
                 (mVertexType != cABSOLUTE_SCREEN && mReferenceEntityPtr != nullptr));
      }
      bool IsUnreferenced() const
      {
         return ((mReferenceEntityPtr == nullptr) &&
                 ((mVertexType == cRELATIVE_ZERO) || (mVertexType == cRELATIVE_ECS) || (mVertexType == cRELATIVE_NED)));
      }

      VertexType       mVertexType;
      vespa::VaEntity* mReferenceEntityPtr;
      UtVec3d          mPosition;
   };
   struct BaseShape;

   struct ShapeIndexBase
   {
      ShapeIndexBase()
         : mShapePtr()
         , mShapeIndex()
      {
      }
      bool IsValid() const { return mShapePtr != nullptr; }
      bool operator<(const ShapeIndexBase& aRhs) const
      {
         return std::make_pair(mShapePtr, mShapeIndex) < std::make_pair(aRhs.mShapePtr, aRhs.mShapeIndex);
      }
      BaseShape* mShapePtr;
      void       SetIndex(int aIndex) { mShapeIndex = ut::safe_cast<unsigned int, int>(aIndex); }
      int        GetIndex() const { return ut::safe_cast<int, unsigned int>(mShapeIndex); }

   private:
      unsigned int mShapeIndex;
   };
   // Shapes added use a two-way indexing scheme for quick lookup on in this class, as well as the user class.
   struct ShapeIndex : public ShapeIndexBase
   {
      int mUserId;
   };
   enum ShapeType
   {
      cPOINT,
      cLINE,
      cICON,
      cELLIPSE,
      cELLIPSOID,
      cQUADRILATERAL,
      cTEXT,
      cTIMER,
      cRELATIVE_BIT = 0xA0000000
   };
   struct BaseShape
   {
      BaseShape(ShapeType aShapeType, bool aIsRelative)
         : mShapeType(aShapeType | (aIsRelative ? cRELATIVE_BIT : 0))
      {
      }
      bool IsRelative() { return 0 != (mShapeType & cRELATIVE_BIT); }
      bool IsLine() const { return cLINE == (mShapeType & 0xf); }
      bool IsPoint() const { return cPOINT == (mShapeType & 0xf); }
      bool IsIcon() const { return cICON == (mShapeType & 0xf); }
      bool IsEllipse() const { return cELLIPSE == (mShapeType & 0xf); }
      bool IsEllipsoid() const { return cELLIPSOID == (mShapeType & 0xf); }
      bool IsQuadrilateral() const { return cQUADRILATERAL == (mShapeType & 0xf); }
      bool IsText() const { return cTEXT == (mShapeType & 0xf); }
      bool IsTimer() const { return cTIMER == (mShapeType & 0xf); }
      int  mShapeType;
   };

   ShapeIndex AddLine(int           aUserId,
                      VertexObject& aPoint1,
                      VertexObject& aPoint2,
                      unsigned char aLineSize,
                      unsigned char aLineStyle,
                      unsigned char aRed,
                      unsigned char aGreen,
                      unsigned char aBlue,
                      unsigned char aAlpha = 255);

   ShapeIndex AddPoint(int           aUserId,
                       VertexObject& aPoint,
                       int           aPointSize,
                       unsigned char aRed,
                       unsigned char aGreen,
                       unsigned char aBlue,
                       unsigned char aAlpha = 255);

   ShapeIndex AddIcon(int           aUserId,
                      VertexObject& aPoint,
                      float         aHeading,
                      std::string   aIconName,
                      unsigned char aRed,
                      unsigned char aGreen,
                      unsigned char aBlue,
                      unsigned char aAlpha = 255);

   ShapeIndex AddEllipse(int           aUserId,
                         VertexObject& aPoint,
                         float         aHeading,
                         float         aAxisA,
                         float         aAxisB,
                         unsigned char aLineSize,
                         unsigned char aLineStyle,
                         unsigned char aEllipseMode,
                         unsigned char aRed,
                         unsigned char aGreen,
                         unsigned char aBlue,
                         unsigned char aAlpha = 255);

   ShapeIndex AddEllipsoid(int           aUserId,
                           VertexObject& aPoint,
                           float         aHeading,
                           float         aPitch,
                           float         aRoll,
                           float         aAxisA,
                           float         aAxisB,
                           float         aAxisC,
                           unsigned char aLineSize,
                           unsigned char aLineStyle,
                           unsigned char aEllipseMode,
                           unsigned char aRed,
                           unsigned char aGreen,
                           unsigned char aBlue,
                           unsigned char aAlpha = 255);

   ShapeIndex AddQuadrilateral(int           aUserId,
                               VertexObject& aPoint1,
                               VertexObject& aPoint2,
                               VertexObject& aPoint3,
                               VertexObject& aPoint4,
                               unsigned char aRed,
                               unsigned char aGreen,
                               unsigned char aBlue,
                               unsigned char aAlpha = 255);

   ShapeIndex AddText(int           aUserId,
                      VertexObject& aPoint,
                      int           aTextSize,
                      std::string   aText,
                      unsigned char aRed,
                      unsigned char aGreen,
                      unsigned char aBlue,
                      unsigned char aAlpha = 255);

   ShapeIndex AddTimer(int           aUserId,
                       VertexObject& aPoint,
                       int           aSize,
                       double        aStartTime,
                       double        aDuration,
                       unsigned char aRed,
                       unsigned char aGreen,
                       unsigned char aBlue,
                       unsigned char aAlpha = 255);

   // When a shape is erased, it may trigger a reindexing of a shape.
   // This method should be implemented to update the index on the user side if necessary.
   // Values of aOldIndex should be changed to aNewIndex.
   virtual void Reindex(const ShapeIndex& aOldIndex, ShapeIndex& aNewIndex) = 0;
   // Informs derived class that an index is being removed
   virtual void RemovedIndex(const ShapeIndex& aIndex);

   void SetUserId(const ShapeIndexBase& aIndex, int aNewUserId);
   void DeleteShape(const ShapeIndexBase& aShapeIndex);

protected:
   void ConvertLocalOrientation(UtEntity& aEntity, UtoOrientation& aOri);
   void GetCurrentLocationECEF(const OverlayWsfDrawBase::VertexObject& aVertex, double aLocationWCS[3]);
   void GetCurrentLocation(const OverlayWsfDrawBase::VertexObject& aVertex, double aLocationWCS[3]);

   ShapeIndex GetIndex(const ShapeIndexBase& aBaseIndex);

   void UpdateDynamicPoints();

   void RemovedIndexP(const ShapeIndexBase& aIndex);

   void ReindexP(const ShapeIndexBase& aOldIndex, const ShapeIndexBase& aNewIndex);

   void AddLogarithmicDepthBuffer(UtoShape* aShape);

   struct PointConfiguration
   {
      int          GetSize() { return mPointSize & (~cRELATIVE_BIT); }
      unsigned int mPointSize;
      unsigned int mColor;
      bool         operator<(const PointConfiguration& aRhs) const
      {
         if (mPointSize < aRhs.mPointSize)
            return true;
         if (mPointSize > aRhs.mPointSize)
            return false;
         return mColor < aRhs.mColor;
      }
   };
   struct LineConfiguration
   {
      unsigned int mLineStyle;
      unsigned int mColor;
      bool         operator<(const LineConfiguration& aRhs) const
      {
         if (mLineStyle < aRhs.mLineStyle)
            return true;
         if (mLineStyle > aRhs.mLineStyle)
            return false;
         return mColor < aRhs.mColor;
      }
   };

   struct LineShape : public BaseShape
   {
      LineShape(bool aIsRelative = false)
         : BaseShape(cLINE, aIsRelative)
         , mPolyShapePtr()
      {
      }
      LineConfiguration mConfiguration;
      UtoPolyLineShape* mPolyShapePtr;
      std::vector<int>  mUserIndices;
   };


   struct PointShape : public BaseShape
   {
      PointShape(bool aIsRelative = false)
         : BaseShape(cPOINT, aIsRelative)
         , mPointShapePtr()
      {
      }
      PointConfiguration mConfiguration;
      UtoPointListShape* mPointShapePtr;
      std::vector<int>   mUserIndices;
   };

   struct IconShape : public BaseShape
   {
      IconShape(bool aIsRelative = false)
         : BaseShape(cICON, aIsRelative)
         , mModelPtr()
      {
      }
      UtoRawShape*            mModelPtr;
      osg::ref_ptr<osg::Node> mDB_ModelRefPtr;
      std::string             mName;
      std::string             mIcon;
      int                     mUserIndex;
   };

   struct EllipseShape : public BaseShape
   {
      EllipseShape(bool aIsRelative = false)
         : BaseShape(cELLIPSE, aIsRelative)
         , mEllipseShapePtr()
      {
      }
      UtoEllipseShape* mEllipseShapePtr;
      std::string      mName;
      int              mUserIndex;
   };

   struct EllipsoidShape : public BaseShape
   {
      EllipsoidShape(bool aIsRelative = false)
         : BaseShape(cELLIPSOID, aIsRelative)
         , mEllipsoidShapePtr()
      {
      }
      UtoSphereShape* mEllipsoidShapePtr;
      std::string     mName;
      int             mUserIndex;
   };

   struct QuadrilateralShape : public BaseShape
   {
      QuadrilateralShape(bool aIsRelative = false)
         : BaseShape(cQUADRILATERAL, aIsRelative)
         , mPolygonShapePtr()
      {
      }
      UtoPolygonShape* mPolygonShapePtr;
      std::string      mName;
      int              mUserIndex;
   };

   struct TextShape : public BaseShape
   {
      TextShape(bool aIsRelative = false)
         : BaseShape(cTEXT, aIsRelative)
         , mTextShapePtr()
      {
      }
      UtoRawShape* mTextShapePtr;
      std::string  mName;
      int          mUserIndex;
   };

   struct TimerShape : public BaseShape
   {
      TimerShape(bool aIsRelative = false)
         : BaseShape(cTIMER, aIsRelative)
         , mTimerShapePtr()
      {
      }
      UtoTimerShape* mTimerShapePtr;
      std::string    mName;
      int            mUserIndex;
   };

   typedef std::map<LineConfiguration, LineShape>   LineShapeMap;
   typedef std::map<PointConfiguration, PointShape> PointShapeMap;
   typedef std::set<IconShape*>                     IconSet;
   typedef std::set<EllipseShape*>                  EllipseShapeSet;
   typedef std::set<EllipsoidShape*>                EllipsoidShapeSet;
   typedef std::set<QuadrilateralShape*>            QuadrilateralShapeSet;
   typedef std::set<TextShape*>                     TextShapeSet;
   typedef std::set<TimerShape*>                    TimerShapeSet;

   struct LinePosition : public ShapeIndexBase
   {
      LinePosition(const ShapeIndexBase& aIndex)
         : ShapeIndexBase(aIndex)
      {
      }
      mutable VertexObject mPoint1;
      mutable VertexObject mPoint2;
   };
   struct QuadrilateralPosition : public ShapeIndexBase
   {
      QuadrilateralPosition(const ShapeIndexBase& aIndex)
         : ShapeIndexBase(aIndex)
      {
      }
      mutable VertexObject mPoint1;
      mutable VertexObject mPoint2;
      mutable VertexObject mPoint3;
      mutable VertexObject mPoint4;
   };
   struct PointPosition : public ShapeIndexBase
   {
      PointPosition(const ShapeIndexBase& aIndex)
         : ShapeIndexBase(aIndex)
      {
      }
      mutable VertexObject mPoint;
   };
   void DeletePoint(const ShapeIndexBase& aShapeIndex);
   void DeleteLine(const ShapeIndexBase& aShapeIndex);
   void DeleteIcon(const ShapeIndexBase& aShapeIndex);
   void DeleteEllipse(const ShapeIndexBase& aShapeIndex);
   void DeleteEllipsoid(const ShapeIndexBase& aShapeIndex);
   void DeleteQuadrilateral(const ShapeIndexBase& aShapeIndex);
   void DeleteText(const ShapeIndexBase& aShapeIndex);
   void DeleteTimer(const ShapeIndexBase& aShapeIndex);

   PointShapeMap                   mPointShapes;
   LineShapeMap                    mLineShapes;
   IconSet                         mIconShapes;
   EllipseShapeSet                 mEllipseShapes;
   EllipsoidShapeSet               mEllipsoidShapes;
   QuadrilateralShapeSet           mQuadrilateralShapes;
   TextShapeSet                    mTextShapes;
   TimerShapeSet                   mTimerShapes;
   std::set<LinePosition>          mDynamicLines;
   std::set<QuadrilateralPosition> mDynamicQuadrilaterals;
   std::set<PointPosition>         mDynamicPoints;
};

class WKF_COMMON_EXPORT OverlayWsfDraw : public QObject, public OverlayWsfDrawBase
{
   Q_OBJECT
public:
   static double cFOREVER;
   // Extended element information that is only needed for more expensive shapes
   struct Extended
   {
      unsigned int mEllipseMode;
      float        mHeading;
      float        mPitch;
      float        mRoll;
      std::string  mText; // icon or text
      float        mAxisA, mAxisB, mAxisC;
   };
   struct Element
   {
      friend class OverlayWsfDraw;
      Element()
         : mExtended(nullptr)
      {
      }
      ShapeIndex    mShapeIndex;
      ShapeType     mElementType;
      unsigned int  mID;
      float         mStartTime;
      float         mStopTime;
      VertexObject  mPoints[4];
      unsigned char mLineStyle;
      unsigned char mGraphicSize;
      unsigned int  mColor;

      Extended& GetExtended();
      QString   GetType() const;
      bool      operator<(const Element& aElement) const { return mStartTime < aElement.mStartTime; }
      bool      IsDisplayable(double aTime) const
      {
         if ((aTime > mStartTime) && (aTime < mStopTime))
         {
            unsigned int vcount = 1;
            switch (mElementType & ~cRELATIVE_BIT)
            {
            case (cLINE):
               vcount = 2;
               break;
            case (cQUADRILATERAL):
               vcount = 4;
               break;
            default:
               break;
            }
            for (unsigned int i = 0; i < vcount; ++i)
            {
               if (mPoints[i].IsUnreferenced()) // if it is relative without a platform
               {
                  return false;
               }
            }
            return true;
         }
         return false;
      }

   private:
      Extended* mExtended;
   };

   typedef std::vector<Element> ElementList;
   OverlayWsfDraw();
   OverlayWsfDraw(const std::string& aName);
   ~OverlayWsfDraw() override;

   OverlayWsfDraw* Clone() const override;

   bool Initialize() override;
   void Update() override;

   // Interface for dispatching a single draw source to multiple draw layers
   class ByLayer
   {
   public:
      virtual OverlayWsfDraw* GetLayer(const std::string& aLayerName) = 0;
   };

   void DeactivateElementsWithReference(const std::string& aReferenceEntityName);

   static bool ReadDrawFile(std::istream& aStream, ByLayer& aDrawByLayer);

   void AddLine(double        aStartTime,
                double        aStopTime,
                unsigned int  aId,
                VertexObject& aPoint1,
                VertexObject& aPoint2,
                unsigned char aLineSize,
                unsigned char aLineStyle,
                unsigned char aRed,
                unsigned char aGreen,
                unsigned char aBlue,
                unsigned char aAlpha = 255);

   void AddPoint(double        aStartTime,
                 double        aStopTime,
                 unsigned int  aId,
                 VertexObject& aPoint,
                 int           aPointSize,
                 unsigned char aRed,
                 unsigned char aGreen,
                 unsigned char aBlue,
                 unsigned char aAlpha = 255);

   void AddIcon(double        aStartTime,
                double        aStopTime,
                unsigned int  aId,
                VertexObject& aPoint,
                float         aHeading,
                std::string   aIconName,
                unsigned char aRed,
                unsigned char aGreen,
                unsigned char aBlue,
                unsigned char aAlpha = 255);

   void AddEllipse(double        aStartTime,
                   double        aStopTime,
                   unsigned int  aId,
                   VertexObject& aPoint,
                   float         aHeading,
                   float         aAxisA,
                   float         aAxisB,
                   unsigned char aLineSize,
                   unsigned char aLineStyle,
                   unsigned char aEllipseMode,
                   unsigned char aRed,
                   unsigned char aGreen,
                   unsigned char aBlue,
                   unsigned char aAlpha = 255);

   void AddEllipsoid(double        aStartTime,
                     double        aStopTime,
                     unsigned int  aId,
                     VertexObject& aPoint,
                     float         aHeading,
                     float         aPitch,
                     float         aRoll,
                     float         aAxisA,
                     float         aAxisB,
                     float         aAxisC,
                     unsigned char aLineSize,
                     unsigned char aLineStyle,
                     unsigned char aEllipseMode,
                     unsigned char aRed,
                     unsigned char aGreen,
                     unsigned char aBlue,
                     unsigned char aAlpha = 255);

   void AddQuadrilateral(double        aStartTime,
                         double        aStopTime,
                         unsigned int  aId,
                         VertexObject& aPoint1,
                         VertexObject& aPoint2,
                         VertexObject& aPoint3,
                         VertexObject& aPoint4,
                         unsigned char aRed,
                         unsigned char aGreen,
                         unsigned char aBlue,
                         unsigned char aAlpha = 255);

   void AddText(double        aStartTime,
                double        aStopTime,
                unsigned int  aId,
                VertexObject& aPoint,
                int           aTextSize,
                std::string   aText,
                unsigned char aRed,
                unsigned char aGreen,
                unsigned char aBlue,
                unsigned char aAlpha = 255);

   void AddTimer(double        aStartTime,
                 double        aStopTime,
                 unsigned int  aId,
                 VertexObject& aPoint,
                 int           aPointSize,
                 unsigned char aRed,
                 unsigned char aGreen,
                 unsigned char aBlue,
                 unsigned char aAlpha = 255);

   void Erase(double aSimTime, unsigned int aId);

   const std::vector<Element>& GetElements() const { return mElements; }

   bool IsActive(int aIndex) const;
   void ActivateElement(int aIndex, bool aActive);
signals:
   void ElementAdded(unsigned int aIndex);
   void ElementActivated(unsigned int aIndex, bool aActive);

private:
   void Reindex(const ShapeIndex& aOldIndex, ShapeIndex& aNewIndex) override;
   OverlayWsfDraw(const OverlayWsfDraw& aSrc);

   void GetCurrentLocation(VertexObject& aVertex, double aLocationWCS[3]);

   void ConvertLocalOrientation(double aLat, double aLon, double& aYaw, double& aPitch, double& aRoll);

   void InsertElement(Element& aElement);

   void DeactivateElement(int aIndex);
   void ActivateElement(int aIndex);

   void ActivatePoint(int aPointIndex);

   void ActivateLine(int aLineIndex);

   void ActivateIcon(int aIconIndex);

   void ActivateEllipse(int aEllipseIndex);

   void ActivateEllipsoid(int aEllipsoidIndex);

   void ActivateQuadrilateral(int aQuadrilateralIndex);

   void ActivateText(int aTextIndex);

   void ActivateTimer(int aTimerIndex);

   void RemoveInactiveIcons();

   void UpdateEraseCache();

   virtual void MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjectionPtr);

   void Rebuild();


   typedef std::pair<double, int> ElementEvent;
   typedef std::set<ElementEvent> EventSet;

   //! List of all drawable elements
   std::vector<Element> mElements;
   //! List of all drawable elements in screen coordiantes
   std::vector<unsigned int> mScreenElements;
   //! List of events triggering elements to become visible
   EventSet mStartEvents;
   //! List of events triggering elements to become hidden
   EventSet mStopEvents;
   //! The last sim-time used to update
   double mCurrentTime;
   //! Stores the next element index which needs to be applied to mEraseCache
   size_t mEraseCacheNextElement;
   //! Maps element ID to the next element index that needs to be checked when Erase is called
   std::map<int, int> mEraseCache;

   vespa::VaCallbackHolder                                mCallbacks;
   bool                                                   mInitialized;
   std::mutex                                             mMutex;
   std::vector<std::pair<int, std::pair<double, double>>> mInsertedElementsOfInterest;
};

class WKF_COMMON_EXPORT OverlayWsfDrawManager
{
public:
   typedef std::map<std::string, OverlayWsfDraw*> Overlays;

   OverlayWsfDrawManager()
      : mOverlays()
      , mScenarioPtr(nullptr)
      , mCurrentLayerName("default")
   {
   }

   ~OverlayWsfDrawManager();

   void ProcessDisOtherData(GenI& aGenI, const std::string& aConnection, const int aVersion, double aSimTime = -1.0);
   void SetScenario(vespa::VaScenario* aScenarioPtr) { mScenarioPtr = aScenarioPtr; }

private:
   bool ReadVertexObject(GenI& aGenI, OverlayWsfDraw::VertexObject& aVertex);

   void EraseIdFromLayers(double aSimTime, unsigned int aId);

   Overlays           mOverlays;
   vespa::VaScenario* mScenarioPtr;
   std::string        mCurrentLayerName;
};

} // end namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::OverlayWsfDrawBase)
VA_DECLARE_OBJECT_TYPE(wkf::OverlayWsfDraw)
#endif
