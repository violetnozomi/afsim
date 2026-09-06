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

#ifndef WsfIntersectMesh_HPP
#define WsfIntersectMesh_HPP

#include "wsf_export.h"

#include <map>
#include <string>

#include "UtDictionary.hpp"
#include "UtIntersectMesh.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfObject.hpp"
#include "WsfSimpleComponent.hpp"
class WsfPlatform;

//! WsfIntersectMesh computes a ray-traced intersection with a model
//! (intersection mesh).  It is useful for determining specific incidence
//! angle information and occlusion with a sub-portion of a target.
//! It can be expanded to perform hi-fidelity sensor interactions.
class WSF_EXPORT WsfIntersectMesh : public WsfObject, public WsfSimplePlatformComponent, public UtIntersectMesh
{
public:
   WsfIntersectMesh();
   WsfIntersectMesh(const WsfIntersectMesh& aSrc);
   WsfIntersectMesh& operator=(const WsfIntersectMesh&) = delete;
   ~WsfIntersectMesh() override;

   WsfComponent* CloneComponent() const override;
   void* QueryInterface(int aRole) override { return (aRole == cWSF_COMPONENT_INTERSECT_MESH) ? this : nullptr; }

   WsfObject* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;

   void ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }

   bool Intersect(WsfPlatform*       aSourcePlatform,
                  const std::string& aTargetOffset,
                  const double       aErrorAngle, // in radians
                  double&            aSurfaceDotProduct,
                  int&               aMaterialPropertyCode,
                  double&            aDistance);

   UtIntersectMesh* LookupMesh(const int aIndex);

   void DefineOffset(const std::string& aOffsetName, double aOffsetX, double aOffsetY, double aOffsetZ);

   bool RemoveOffset(const std::string& aOffsetName);

   unsigned int GetTargetOffsetCount() { return static_cast<unsigned int>(mOffsetMap.size()); }

   WsfStringId GetTargetOffsetName(unsigned aOffsetEntry) const;
   bool        TargetOffsetExists(const std::string& aTargetOffsetName) const;
   using MeshMap = std::map<unsigned int, UtIntersectMesh*>; // convert index to meshes

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

   int LoadMesh(const std::string& aMeshFileName);

   int           mMeshIndex;
   OffsetNameMap mOffsetNameMap;
   OffsetMap     mOffsetMap;
   Shared*       mSharePtr;
   WsfPlatform*  mPlatformPtr;
   WsfStringId   mNameId;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfIntersectMesh, cWSF_COMPONENT_INTERSECT_MESH)

#endif
