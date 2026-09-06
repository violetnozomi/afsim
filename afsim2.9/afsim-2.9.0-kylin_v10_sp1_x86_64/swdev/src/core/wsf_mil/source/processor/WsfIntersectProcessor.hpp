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

#ifndef WSFINTERSECTPROCESSOR_HPP
#define WSFINTERSECTPROCESSOR_HPP

#include "wsf_mil_export.h"

#include <map>
#include <string>

#include "UtDictionary.hpp"
class UtIntersectMesh;
class WsfPlatform;
#include "WsfProcessor.hpp"

//! WsfIntersectProcessor computes a ray-traced intersection with a model
//! (intersection mesh).  It is useful for determining specific incidence
//! angle information and occlusion with a sub-portion of a target.
//! Currently it is used for high-energy laser fluence calculations,
//! but it can be expanded to perform hi-fidelity sensor interactions.
class WSF_MIL_EXPORT WsfIntersectProcessor : public WsfProcessor
{
public:
   WsfIntersectProcessor(WsfScenario& aScenario);
   WsfIntersectProcessor(const WsfIntersectProcessor& aSrc);
   WsfIntersectProcessor& operator=(const WsfIntersectProcessor&) = delete;
   ~WsfIntersectProcessor() override;

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;

   bool Intersect(WsfPlatform*       aSourcePlatform,
                  const std::string& aTargetOffset,
                  const double       aErrorAngle, // in radians
                  double&            aBeamSurfaceDotProduct,
                  int&               aMaterialPropertyCode,
                  double&            aDistance);

   UtIntersectMesh* LookupMesh(const int aIndex);
   int              LoadMesh(const std::string& aFileName, UtIntersectMesh* aMeshPtr = nullptr);

   void DefineOffset(const std::string& aOffsetName, double aOffsetX, double aOffsetY, double aOffsetZ);

   bool RemoveOffset(const std::string& aOffsetName);

   unsigned int GetTargetOffsetCount() { return static_cast<unsigned int>(mOffsetMap.size()); }

   WsfStringId GetTargetOffsetName(unsigned aOffsetEntry) const;
   bool        TargetOffsetExists(const std::string& aTargetOffsetName) const;
   using MeshMap = std::map<unsigned int, UtIntersectMesh*>; // convert index to meshes

protected:
private:
   struct Offset
   {
      Offset()
         : x(0.0)
         , y(0.0)
         , z(0.0)
      {
      }
      Offset(double ax, double ay, double az)
         : x(ax)
         , y(ay)
         , z(az)
      {
      }

      double x;
      double y;
      double z;
   };
   using OffsetMap     = std::map<unsigned int, Offset>; // convert index to offsets
   using OffsetNameMap = std::map<WsfStringId, int>;     // convert offset name to index
   class Shared;

   int           mMeshIndex;
   OffsetNameMap mOffsetNameMap;
   OffsetMap     mOffsetMap;
   Shared*       mSharePtr;
};

#endif
