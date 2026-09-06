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

/////////////////////////////////////////////////////////////////////////////
// UtoViewer.hpp : header file

#if !defined(_UTOVIEWER_HPP_)
#define _UTOVIEWER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include <map>
#include <vector>

#include <osg/ref_ptr>

#include "UtoCallback.hpp"
#include "UtoCameraOrtho.hpp"
#include "UtoCameraPerspective.hpp"
#include "UtoHitInfo.hpp"
#include "UtoScreenOverlayManager.hpp"
#include "UtoTypes.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"
#include "UtoWindowImp.hpp"
#include "UtoWorld.hpp"

namespace osg
{
class Texture2D;
}

namespace osgDB
{
class DatabasePager;
}

namespace osgViewer
{
class ScreenCaptureHandler;
class StatsHandler;
class Viewer;
} // namespace osgViewer

namespace osgUtil
{
class UtoCmePickVisitor;
}

class UtoEntityDB;
class UtoEntity;
class UtoHittestStrategy;
class UtoShapeImp;
class UtoViewFilter;
class UtoViewerScreenCaptureHandler;
class UtoWindow;
// class UtoSymbol;

class UTILOSG_EXPORT UtoViewer : public UtoCallback<UtoViewer>
{
public:
   enum CameraMode
   {
      ORTHOGONAL,             // The orthogonal camera
      PERSPECTIVE,            // The perspective camera
      DEFAULTCAM = ORTHOGONAL // The default camera (orthogonal)
   };

   // Hittest types
   typedef std::map<UtoShape*, UtoHitInfoList> SymbolHittestList;

   // constructor / deconstructor
   UtoViewer(const std::string& name = "Viewer");
   UtoViewer(osgViewer::Viewer* aViewer, const std::string& name = "Viewer");
   ~UtoViewer() override;

   virtual void Draw();
   void         PreSceneDraw();
   void         SceneDraw();
   void         PostSceneDraw();

   std::string Name();

   void                  SetActiveCamera(UtoViewer::CameraMode mode);
   UtoViewer::CameraMode ActiveCamera() const { return m_ActiveCamera; }

   virtual bool Connect(UtoEntityDB& edb);
   virtual bool Disconnect(bool shutdown = false);
   bool         IsConnected() const;
   std::string  ConnectedDB() const;

   UtoEntityDB&       EntityDB() { return *m_pConnectedDB; }
   const UtoEntityDB& EntityDB() const { return *m_pConnectedDB; }

   // SymbolMap::iterator FindSymbol(const UtoEntity & e, const std::string& name);
   // UtoSymbol* FindSymbolPtr(const UtoEntity& ent, const std::string& name);
   // const SymbolMap& GetSymbolMap() const { return m_Symbols; }

   void UpdateOffsets();

   SymbolHittestList*
   HitTestSymbol(UtoPoint pos, const std::string& symbols, unsigned int max, bool aScreenScaled, float aIconScale);

   UtoCameraOrtho*             GetOrtho() { return &m_OrthoCamera; }
   const UtoCameraOrtho*       GetOrtho() const { return &m_OrthoCamera; }
   UtoCameraOrtho&             OrthoCamera() { return m_OrthoCamera; }
   UtoCameraPerspective*       GetPerspective() { return &m_PerspectiveCamera; }
   const UtoCameraPerspective* GetPerspective() const { return &m_PerspectiveCamera; }
   UtoCameraPerspective&       PerspectiveCamera() { return m_PerspectiveCamera; }


   UtoWorld&       World() { return m_World; }
   UtoWorld*       GetWorld() { return &m_World; }
   const UtoWorld* GetWorld() const { return &m_World; }

   UtoScreenOverlayManager*       GetScreenOverlayManager() { return &mScreenOverlayManager; }
   const UtoScreenOverlayManager* GetScreenOverlayManager() const { return &mScreenOverlayManager; }

   UtoSize DisplaySize() const;
   void    SetDisplaySize(const UtoSize& aSize);
   bool    DisplayToWorld(UtoPosition& aPos);
   bool    DisplayToWorld(UtoRect& aRect); // This function is currently unused in AFSIM
   bool    AttachToWindowId(UtoWindowId aId, bool aReleaseContext);
   bool    AttachToGraphicsContext(osg::GraphicsContext* aGC);

   UtoViewFilter* GetFilter() { return m_Filter; }
   void           SetFilter(UtoViewFilter*);

   void DumpScene(std::ostream& out);
   void DumpScene(std::ostream& out, int skiptypes);
   void Dump(std::ostream& o);

   void                 SetProjection(const UtoViewerProjection& p);
   UtoViewerProjection* Projection() { return &m_pProjection; }

   // Returns the value of m_FrameNumber which is incremented each time
   // Draw is called.
   int GetFrameNumber() const { return m_FrameNumber; }

   typedef Event1<0, UtoViewer*> ConnectedEvent;
   typedef Event1<1, UtoViewer*> DisconnectedEvent;
   typedef Event1<2, UtoViewer*> ProjectionChangedEvent;
   typedef Event0<3>             CameraChangedEvent;

   osg::Group* GetScene() { return mScene.get(); }
   osg::Group* GetOrCreateLayer(int layer);

   void SetSimulationTime(double aSimTime);

   virtual void SetSunVector(const osg::Vec3& aVector);

   virtual void SetUniform(const char* aName, const float aValue);

   virtual void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3);

   virtual void SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4);

   virtual void SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename);


   virtual void SetUniform(const char*          aName,
                           const unsigned int   aIndex,
                           const unsigned char* aData,
                           const unsigned int   aWidth,
                           const unsigned int   aHeight);

   virtual void SetUniform(const char* aName, const osg::Matrix& aMatrix);

   osg::Group* AddRTTGroup(const std::string aName);

   virtual void HighlightSymbolsWithColor(UtoEntity*                      aEntityPtr,
                                          const std::vector<std::string>& aSymbolNames,
                                          bool                            aState,
                                          const float                     aColor[4],
                                          bool                            aAutoScale = false)
   {
   }

   virtual void HighlightWithColor(osg::Node* aNodePtr, bool aState, const float aColor[4]) {}

   virtual void HighlightNode(osg::Node* aNodePtr, bool aState, const float aColor[4]) {}

   virtual void ToggleStats();
   virtual void ScreenShot(const std::string& aOutputDir, int aInstanceId = 1);
   virtual void ScreenShotToFilename(const std::string& aFilename);

   virtual void SetViewMatrix(const osg::Matrix& aMatrix);
   virtual void SetViewMatrixAsLookAt(const osg::Vec3& aEye, const osg::Vec3& aCenter, const osg::Vec3& aUp);
   virtual void SetProjectionMatrixAsPerspective(float aFOV, float aAspectRatio, float aHither, float aYon);
   virtual void SetProjectionMatrixAsOrtho(float aMinX, float aMaxX, float aMinY, float aMaxY, float aMinZ, float aMaxZ);
   virtual void
   SetProjectionMatrixAsFrustum(double aLeft, double aRight, double aBottom, double aTop, double aNear, double aFar);
   virtual void
   GetProjectionMatrixAsFrustum(double& aLeft, double& aRight, double& aBottom, double& aTop, double& aNear, double& aFar);
   virtual void
   GetProjectionMatrixAsOrtho(double& aMinX, double& aMaxX, double& aMinY, double& aMaxY, double& aMinZ, double& aMaxZ);

   // Note, please confirm that the OSG Viewer is valid before calling GetViewMatrix or GetProjectionMatrix
   // While a nullptr is returned when it isn't, the OSG Viewer validity is an earlier escape from code
   // that really shouldn't be executing
   virtual const osg::Matrix*   GetViewMatrix() const;
   virtual const osg::Matrix*   GetProjectionMatrix() const;
   virtual const osg::Viewport* GetViewport() const;
   virtual void                 SetViewport(int aX, int aY, int aW, int aH);

   virtual void SetFrameStamp(osg::FrameStamp* aFrameStamp);
   virtual void SetComputeNearFarMode(osg::CullSettings::ComputeNearFarMode aSettings);
   virtual void SetCullingMode(int mode);
   virtual void FlushAllDeletedGLObjects();
   virtual void CompileAndFlushGLObjects(double aTime);
   virtual void SetDatabasePager(osgDB::DatabasePager* aPager);

   virtual void BuildGraphicsContext();
   virtual void UseGraphicsContext(osg::GraphicsContext* aGC);

   virtual void DrawScene();


   virtual void SetSceneData(osg::Node* aRootNode);
   virtual void SetUpdateVisitor(osgUtil::UpdateVisitor* aVisitor);

   virtual void              SetLightingMode(osg::View::LightingMode aMode);
   virtual const osg::Light* GetLight();
   virtual void SetGlobalAttributeAndModes(osg::StateAttribute* aAttribute, osg::StateAttribute::Values aMode);
   virtual const osg::StateSet* GetGlobalStateSet();
   virtual void                 SetClearColor(const osg::Vec4& aInput);
   virtual const osg::Vec4&     GetClearColor();
   virtual void                 SuppressSelectionIndication(bool aState) {}

   virtual void BeginUserStat(const std::string& aName);
   virtual void EndUserStat(const std::string& aName);
   static void  Preinitialize();


   void   RegisterStat(const std::string& aName,
                       const std::string& aBeginName,
                       const std::string& aEndName,
                       const std::string& aElapsedName,
                       double             aTime);
   double GetStatStart(const std::string& aName);

   // { To be called only from UtoViewerEntity
   virtual void ShapeAdded(UtoShape* aShapePtr);
   virtual void ShapeRemoved(UtoShape* aShapePtr);
   void         EntityDestroyed(UtoEntity* aEntityPtr);
   // }

   virtual osg::Group* GetRootNode() { return m_RootNode.get(); }
   osgViewer::Viewer*  GetOSGViewer() const { return m_OSGViewer.get(); }

   void EnableSmallFeatureCulling(bool aState = true);

   virtual osg::Texture2D* GetTexture() { return nullptr; }

   void AddSubordinate(UtoViewer* aChildPtr);
   void RemoveSubordinate(UtoViewer* aChildPtr);

   void        ToggleMode() const;
   osg::Group* GetCelestialNode();

   void SetupCelestialCamera();
   void CopyTerrainToCelestial();

protected:
   void AddHandlers(osgViewer::Viewer* aViewerPtr);

   // private typedefs
   typedef std::pair<int, int> RefCntLayerPair;
   // typedef std::vector< osg::ref_ptr< osg::Group > >     LayerGroupVec;

   void MaskSymbolShapes(const std::string& aPrefix, bool aOnForPrefix, bool aOnForOthers);

   // disallow use of these:
private:
   UtoViewer(const UtoViewer&);
   UtoViewer& operator=(const UtoViewer&);

protected:
   void AddEntityCB(UtoEntity* entityImp);
   void RemoveEntityCB(UtoEntity* entity);
   // void     EntityDestroyedCB(UtoEntity * entity);
   void DBDestroyedCB(UtoEntityDB* EntityDBImp);
   void ResourcesChangedCB(UtoEntity* entity, UtoShapeImp* shapeOld, UtoShapeImp* shapeNew);

   virtual void NewSymbolAdded(UtoShape* aShapePtr) {}

   virtual void SetUpDummyTexture();

   // private forward declarations
   class UtoDBUpdateCallback;

   // private member variables
   UtoEntityDB* m_pConnectedDB;

   osg::ref_ptr<osg::Camera> mCelestialCamera; // this camera renders the sky to the texture

   UtoViewer::CameraMode m_ActiveCamera;

   UtoCameraOrtho       m_OrthoCamera;
   UtoCameraPerspective m_PerspectiveCamera;

   osg::ref_ptr<osgViewer::Viewer>    m_OSGViewer;
   osg::ref_ptr<osgDB::DatabasePager> m_DatabasePager;
   osg::ref_ptr<osg::Group>           m_RootNode;
   osg::ref_ptr<osg::Group>           mScene;

   osg::ref_ptr<UtoViewerScreenCaptureHandler> mScreenCapture;

   UtoWorld                m_World;
   UtoWindowImp            m_Window;
   UtoScreenOverlayManager mScreenOverlayManager;
   UtoViewFilter*          m_Filter;

   int    m_FrameNumber;
   double m_SimulationTime;

   UtoViewerProjection m_pProjection;
   UtoHittestStrategy* m_HittestStrategy;

   bool m_DoPrepareForRender;

   osg::ref_ptr<osgViewer::StatsHandler> mStatsHandler;

   std::map<std::string, double> mRegisteredStats;
};

#endif // !defined(_UTOVIEWER_HPP_)
