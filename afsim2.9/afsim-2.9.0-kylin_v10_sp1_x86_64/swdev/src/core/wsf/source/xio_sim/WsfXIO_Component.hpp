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

#ifndef WSFXIO_COMPONENT_HPP
#define WSFXIO_COMPONENT_HPP

class UtCallbackHolder;
class PakProcessor;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfObject;
class WsfPlatform;
class WsfPlatformPart;
#include "WsfSimpleComponent.hpp"
class WsfSimulation;
class WsfTaskProcessor;
class WsfTaskResource;
class WsfTrack;
class WsfXIO_EM_RcvrInfo;
class WsfXIO_EM_XmtrInfo;
class WsfXIO_Interface;
class WsfXIO_PartStateChangePkt;
class WsfXIO_PartStateChangeRequestPkt;
class WsfXIO_PlatformPartInfo;
class WsfXIO_Simulation;
class WsfXIO_TaskCommandPkt;
class WsfXIO_TaskService;

//! An component that may be used to extend the XIO simulation components.
class WSF_EXPORT WsfXIO_Component : public WsfSimpleComponentParentT<WsfXIO_Simulation>
{
public:
   WsfXIO_Component(int aRole, const std::string& aName);

   //! Called by WsfXIO_Interface to allow initialization of the component.
   bool Initialize() { return true; }

   //! @name Extensions of xio_sim/WsfXIO_ObjectInfoLoad
   //@{
   virtual void EM_RcvrInfo_Load(WsfXIO_EM_RcvrInfo& aInfo, const WsfEM_Rcvr& aRcvr) {}
   virtual void EM_XmtrInfo_Load(WsfXIO_EM_XmtrInfo& aInfo, const WsfEM_Xmtr& aXmtr) {}

   //! Create a WsfXIO_PlatformPartInfo from a WsfPlatformPart.
   //! @returns 0 if the supplied part type is not recognized.
   virtual WsfXIO_PlatformPartInfo* CreatePlatformPartInfo(WsfPlatformPart& aPart) { return nullptr; }
   //@}

   //! @name Extensions of xio_sim/WsfXIO_PlatformInfoService
   //@{
   virtual void RegisterSimulationObservers(WsfSimulation& aSimulation, UtCallbackHolder& aCallbacks) {}

   virtual bool FillPartState(const WsfPlatformPart& aPart, WsfXIO_PartStateChangePkt& aPkt) { return false; }

   virtual WsfObject* FindPartType(int aPartType, WsfStringId aPartTypeName) { return nullptr; }

   virtual WsfObject* FindPart(const WsfPlatform& aPlatform, int aPartType, WsfStringId aPartName) { return nullptr; }
   //@}

   //! @name Extensions of xio_sim/WsfXIO_Simulation
   //@}
   virtual bool HandleStateChangeRequest(double                            aSimTime,
                                         WsfXIO_PartStateChangeRequestPkt& aPkt,
                                         WsfPlatform*                      aPlatformPtr,
                                         WsfEM_Xmtr*&                      aXmtrPtr,
                                         WsfEM_Rcvr*&                      aRcvrPtr)
   {
      return false;
   }
   //@}

   //! @name Extensions of xio_sim/WsfXIO_TaskService.
   //@{
   virtual bool HandleTaskCommand(double                 aSimTime,
                                  WsfXIO_TaskCommandPkt& aPkt,
                                  WsfTaskProcessor*      aProcA_Ptr,
                                  WsfTaskProcessor*      aProcB_Ptr,
                                  WsfTrack*              aTrackPtr,
                                  WsfTaskResource*       aResourcePtr)
   {
      return false;
   }
   //@}
};

#endif
