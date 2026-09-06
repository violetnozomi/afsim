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

#ifndef RVANNOTATIONPLUGIN_HPP
#define RVANNOTATIONPLUGIN_HPP

#include "RvAnnotationEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "annotation/WkfAnnotationPlugin.hpp"

namespace Annotation
{
class AnnotationDisplayInterface;
}

namespace RvAnnotation
{
class Plugin : public rv::PluginT<Annotation::Plugin>
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void AnnotationReadCB(rv::MsgBase& aData);
   void PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive);

private:
   std::unordered_map<std::string, rv::MsgAnnotationDecoration>     mDeferredDecorations;
   std::unordered_multimap<std::string, rv::MsgAnnotationRangeRing> mDeferredPlatform_RangeRings;
   std::unordered_map<std::string, rv::MsgAnnotationRangeRing>      mDeferredPoi_RangeRings;
   float                                                            mLastTime{0.0};
   vespa::VaCallbackHolder                                          mCallbacks;
};
} // namespace RvAnnotation

#endif
