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
#ifndef VAFWD_HPP
#define VAFWD_HPP

// forward declarations for Uto and vespa

/////////////////////////////////////////////////////////////////////////////////
namespace osg
{
class Vec3d;
}
typedef osg::Vec3d UtoPosition;
class UtoCameraOrtho;
class UtoCameraPerspective;
class UtoCircleShape;
class UtoCmeModelShape;
class UtoCustomAnimatedShape;
class UtoEllipseShape;
class UtoEntity;
class UtoEntityDB;
class UtoGLShape;
class UtoGroupShape;
class UtoHitInfoList;
class UtoModelPart;
class UtoModelShape;
class UtoModelSwitch;
class UtoOverlay;
class UtoPointListShape;
class UtoPolyLineShape;
class UtoPolygonShape;
class UtoResourceCallback;
class UtoResourceDB;
class UtoSphereShape;
class UtoSymbol;
class UtoTerrain;
class UtoViewFilter;
class UtoViewer;
class UtoWindow;
class UtoWorld;
class UtoColor;

class CmeModelShape;
class CmeGeode;

namespace vespa
{
class VaAttachment;
class VaAttachmentZone;
class VaBoundingBox;
class VaCallback;
class VaCallbackList;
class VaCamera;
class VaCameraMotion;
class VaCameraMotionData;
class VaEntity;
class VaEnvironment;
class VaEvent;
class VaEventManager;
class VaFileChooser;
class VaObject;
class VaObserver;
class VaOverlay;
class VaOverlayCompass;
class VaOverlayScale;
class VaOverlayZoomBox;
class VaPosition;
class VaScenario;
class VaSelectionBox;
class VaTimeManager;
class VaViewer;
class VaViewerManager;
class VaZonePoint;

struct VaAttachmentSubId;
} // namespace vespa

#endif
