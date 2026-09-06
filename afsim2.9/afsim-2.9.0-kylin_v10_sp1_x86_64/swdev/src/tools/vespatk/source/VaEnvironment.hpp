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

#ifndef VaEnvironment_hpp
#define VaEnvironment_hpp

#include "vespatk_export.h"

#include <vector>

#include "UtException.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "VaPosition.hpp"
#include "VaScenario.hpp"

#undef BadValue
#include "UtInput.hpp"
class UtLLAPos;

#include "VaFactory.hpp"

namespace vespa
{
template<typename T>
class EntityPtr;

class VaAttachment;
class VaBoundingBox;
class VaEntity;
class VaEvent;
class VaEventManager;
class VaModelDatabase;
class VaObserver;
class VaSelectionBox;
class VaTimeManager;
class VaViewer;
class VaViewerManager;

//! VaEnvironment manages the global set of scenario objects and provides
//! access to application-wide utilities.
//! VaEnvironment is a singleton object that can extended. To use this class you must
//! create an VaEnvironment object (or your derived version) inside you main function
//! before calling 'Instance', otherwise it will return null.
class VESPATK_EXPORT VaEnvironment
{
public:
   friend class VaScenario;

   struct UnitFormat
   {
      int mAltFormat;    // altitude (UtLength::FmtX)
      int mDistFormat;   // distance (UtLength::FmtX)
      int mSpeedFormat;  // speed (UtSpeed::FmtX)
      int mTimeFormat;   // time (UtTime::FmtX)
      int mLatLonFormat; // angle (UtAngle::FmtX)
      int mAngleFormat;  // angle (UtAngle::FmtX)
   };

   enum
   {
      AltitudeMeasure = 0,
      DistanceMeasure = 1,
      SpeedMeasure    = 2,
      LatLonMeasure   = 3,
      TimeMeasure     = 4,
      AngleMeasure    = 5
   };

public:
   static VaEnvironment& Instance();
   static bool           Exists();
   static void           Shutdown();

   VaEnvironment(VaTimeManager* aTimePtr, VaViewerManager* aViewerManagerPtr, VaFactory* aFactoryPtr, VaObserver* aObserverPtr);

   virtual bool Initialize();

   virtual void Load(VaViewer* aViewerPtr);

   VaModelDatabase* GetModelDatabase() { return mModelDatabasePtr.get(); }

   //------------------------------------------------------------------------------
   //! Time Management Methods.
   //------------------------------------------------------------------------------
   virtual VaTimeManager* GetTime() const { return mTimePtr; }

   //------------------------------------------------------------------------------
   //! Event Management Methods.
   //------------------------------------------------------------------------------
   virtual void AddSimTimeEvent(VaEvent* aEventPtr);
   virtual void AddRealTimeEvent(VaEvent* aEventPtr);

   //------------------------------------------------------------------------------
   //! Factory methods.
   //------------------------------------------------------------------------------
   const VaFactory* GetFactory() const { return mFactoryPtr; }
   template<class T>
   static T*        CreateEntity(const std::string& aEntityType);
   static VaEntity* CreateEntity(const std::string& aEntityType);
   template<class T>
   static T*            CreateAttachment(const std::string& aAttachmentType,
                                         VaEntity&          aEntity,
                                         VaViewer*          aViewerPtr = nullptr,
                                         bool               aLoad      = true);
   static VaAttachment* CreateAttachment(const std::string& aAttachmentType,
                                         VaEntity&          aEntity,
                                         VaViewer*          aViewerPtr = nullptr,
                                         bool               aLoad      = true);
   static VaScenario*   CreateScenario(const std::string& aScenarioType);

   //------------------------------------------------------------------------------
   // Scenario Management Methods.
   //------------------------------------------------------------------------------

   virtual VaScenario* NewScenario(const std::string& aFormat);
   virtual bool        AddScenario(VaScenario* aScenarioPtr);
   virtual bool        RemoveScenario(VaScenario* aScenarioPtr);
   virtual bool        ClearScenarios();
   virtual VaScenario* FindScenario(const VaScenarioKey& aKey) const;
   virtual VaScenario* FindScenario(const std::string& aKey) const;
   VaScenario*         SetCurrentScenario(const VaScenarioKey& aKey);
   VaScenario*         GetCurrentScenario() const;
   VaScenario*         NextScenario();
   VaScenario*         PrevScenario();
   bool                ResetScenarios();

   bool GetScenarioBound(VaBoundingBox& aBoundingBox) const;
   void GetBound(const VaEntityMap& aEntities, VaBoundingBox& aBoundingBox) const;
   void GetBound(const VaEntityList& aEntities, VaBoundingBox& aBoundingBox) const;

   //------------------------------------------------------------------------------
   // Scenario Selection Methods
   //------------------------------------------------------------------------------

   bool BoxSelect(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside);
   bool DeleteSelection(VaViewer* aViewerPtr);
   void MoveSelection(VaViewer*       aViewerPtr,
                      const UtLLAPos& aBefore,
                      const UtLLAPos& aAfter,
                      int             aFeedbackFormat,
                      double          aFeedbackPrecision);
   void MoveSelectionComplete(VaViewer* aViewerPtr);
   void RotateSelection(VaViewer* aViewerPtr, const UtLLAPos& aCenterPt, double aAngle);

   //------------------------------------------------------------------------------
   // Scenario Entity Methods
   //------------------------------------------------------------------------------
   void DeleteEntity(VaEntity* aEntityPtr);

   template<typename T>
   void DeleteEntity(vespa::EntityPtr<T> aEntityPtr)
   {
      DeleteEntity(aEntityPtr.get());
   }

   VaEntity* FindEntity(const std::string& aName) const;
   VaEntity* FindEntity(unsigned int aUniqueId) const;
   template<class T>
   T* FindEntity(const std::string& aName) const;

   VaAttachment* FindAttachment(unsigned int aUniqueId) const;
   VaAttachment* FindAttachment(const std::string& aName) const;
   template<class T>
   T* FindFirstAttachment(const std::string& aName) const;


   void GetEntityList(VaEntityList& aEntityList) const;
   template<class T>
   void GetEntityListOfType(VaEntityList& aEntityList) const;

   //------------------------------------------------------------------------------
   //! Viewer and Camera Methods.
   //------------------------------------------------------------------------------
   virtual double HeightOfTerrain(const VaPosition& aPosition, VaViewer* aViewerPtr) const;

   virtual void SetCenter(const VaPosition& aPosition, VaViewer* aViewerPtr);

   VaViewerManager* GetViewerManager() const { return mViewerManagerPtr; }

   // this is a convenience method for single viewer applications, mostly.
   VaViewer* GetFirstViewer() const;

   virtual UnitFormat& GetUnitFormat() { return mUnitFormat; }
   virtual void        SetUnitFormat(int aMeasureType, int aUnitType);

   void              SetCurrentCursorData(const VaPosition& aCursorPosition, double aCursorMetersPerPixel);
   const VaPosition& GetCurrentCursorPosition() const { return mCurrentCursorPosition; }
   double            GetCurrentCursorMetersPerPixel() const { return mCurrentCursorMetersPerPixel; }

   void              SetSavedCursorData(const VaPosition& aCursorPosition, double aCursorMetersPerPixel);
   const VaPosition& GetSavedCursorPosition() const { return mSavedCursorPosition; }
   double            GetSavedCursorMetersPerPixel() const { return mSavedCursorMetersPerPixel; }

   bool GetUseEphemeris() const { return mUseEphemeris; }
   void SetUseEphemeris(bool aUseEphemeris) { mUseEphemeris = aUseEphemeris; }

   VaScenarioList& GetScenarios() { return mScenarios; }

   virtual void DirtyTimeRange();
   virtual void HandleDirtyTimeRange();

   void DirtySelection() { mSelectionDirty = true; }

   void BeginMultiSelect();
   void EndMultiSelect();

   double GetStartTime() const { return mStart; }
   double GetEndTime() const { return mEnd; }

   void               SetGL_Info(const char* aVersion, const char* aShadingVersion, const char* aExtensions);
   const std::string& GetGL_Version() const { return mGL_Version; }
   const std::string& GetGLSL_Version() const { return mGLSL_Version; }
   const std::string& GetGL_Extensions() const { return mGL_Extensions; }
   std::string        GetRecommendedGLSL_ShaderString() const;

protected:
   virtual ~VaEnvironment();

   virtual void Update(double aTime);

   virtual void UpdateFrame(double aTime);

   virtual void DispatchSimTimeEvents(double aTime);
   virtual void DispatchRealTimeEvents(double aTime);

   static VaEnvironment*            mInstancePtr;
   bool                             mInitialized;
   VaScenarioList                   mScenarios;
   VaScenarioKey                    mCurrentScenarioKey;
   VaCallbackHolder                 mCallbacks;
   std::unique_ptr<VaModelDatabase> mModelDatabasePtr;

private:
   VaEnvironment();                          // disable default construction.
   VaEnvironment(const VaEnvironment& aSrc); // disable copy construction.

   //! An exception that is thrown when an input error is encountered.
   class InputError : public UtException
   {
   public:
      InputError()
         : UtException("Input Error"){};
   };

   class ErrorTracker
   {
   public:
      ErrorTracker(UtInput& aInput);
      ~ErrorTracker();
      void ThrowError();
      void ThrowError(UtException aException);
      void ThrowError(UtInput::ExceptionBase aException);
      bool mTesting;

   private:
      void        ThrowErrorMessageStart(UtException aException);
      std::string AdditionalMessage(UtException aException);
      UtInput&    mInput;
   };

   // Observer callbacks.
   void EntitySelectedCB(VaViewer* aViewerPtr, int aSelected, VaEntity* aEntityPtr);
   void EntityDeletedCB(VaEntity* aEntityPtr);
   void AttachmentDeletedCB(VaAttachment*, const VaAttachmentSubId&);
   void AttachmentSelectedCB(VaViewer* aViewerPtr, int, VaAttachment*, const VaAttachmentSubId&);
   void HandleSelectionBoxResultCB(const std::string&    aBoxOperation,
                                   VaViewer*             aViewerPtr,
                                   const VaSelectionBox& aSelectionBox);
   void SetUnitFormatCB(int aMeasureType, int aUnitType);
   void CameraUpdatedCB(VaViewer* aViewerPtr);
   void TimeUpdatedCB(double aTime, double aRate, double aMinTime, double aMaxTime);
   void RedrawWindowCB();
   void GetScenarioToEntityAndAttachmentMap(VaHitSet&                                                        aHitSet,
                                            std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>>& aMap);

   VaTimeManager*   mTimePtr;
   VaViewerManager* mViewerManagerPtr;
   VaFactory*       mFactoryPtr;
   VaObserver*      mObserverPtr;
   VaEventManager*  mSimTimeEventManagerPtr;
   VaEventManager*  mRealTimeEventManagerPtr;
   UnitFormat       mUnitFormat;
   mutable bool     mSelectionDirty;
   mutable VaHitSet mSelection;
   VaPosition       mCurrentCursorPosition;
   double           mCurrentCursorMetersPerPixel;
   VaPosition       mSavedCursorPosition;
   double           mSavedCursorMetersPerPixel;
   bool             mUseEphemeris;
   bool             mDirtyTimeRange;
   double           mStart;
   double           mEnd;
   int              mDoingMultiselect;
   bool             mApplicationTerminated;

   std::string mProxyHost;
   int         mProxyPort;
   std::string mCacheLocation;
   std::string mCacheMode;
   std::string mGL_Version;
   std::string mGLSL_Version;
   std::string mGL_Extensions;
};

template<class T>
T* VaEnvironment::CreateEntity(const std::string& aEntityType)
{
   return Instance().GetFactory()->CreateEntity<T>(aEntityType);
}

template<class T>
T* VaEnvironment::CreateAttachment(const std::string& aAttachmentType, VaEntity& aEntity, VaViewer* aViewerPtr, bool aLoad)
{
   return Instance().GetFactory()->CreateAttachment<T>(aAttachmentType, aEntity, aViewerPtr, aLoad);
}
} // end namespace vespa.

template<class T>
T* vespa::VaEnvironment::FindFirstAttachment(const std::string& aName) const
{
   T* attachmentPtr = nullptr;
   for (VaScenarioList::const_iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      attachmentPtr = dynamic_cast<T*>(iter->second->FindAttachment(aName));
      if (attachmentPtr != nullptr)
      {
         break;
      }
   }
   return attachmentPtr;
}

template<class T>
T* vespa::VaEnvironment::FindEntity(const std::string& aName) const
{
   T* entityPtr = nullptr;
   for (auto&& scen : mScenarios)
   {
      entityPtr = dynamic_cast<T*>(scen.second->FindEntity(aName));
      if (entityPtr != nullptr)
      {
         break;
      }
   }
   return entityPtr;
}

template<class T>
void vespa::VaEnvironment::GetEntityListOfType(VaEntityList& aEntityList) const
{
   for (auto&& scen : mScenarios)
   {
      scen.second->GetEntityListOfType<T>(aEntityList);
   }
}

#endif
