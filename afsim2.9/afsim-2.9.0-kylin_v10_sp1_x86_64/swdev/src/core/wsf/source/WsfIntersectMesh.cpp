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

#include "WsfIntersectMesh.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtIntersectMesh.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

class WsfIntersectMesh::Shared : public UtReferenceCounted
{
public:
   Shared()
      : mOffsetIndex(0)
   {
      mMeshDictionaryPtr = new UtDictionary();
   }
   ~Shared() override
   {
      for (WsfIntersectMesh::MeshMap::iterator it = mMeshMap.begin(); it != mMeshMap.end(); ++it)
      {
         delete it->second;
      }
      delete mMeshDictionaryPtr;
   }
   int GetNextIndex() { return ++mOffsetIndex; }

   UtDictionary*             mMeshDictionaryPtr; // convert filename to index
   WsfIntersectMesh::MeshMap mMeshMap;

private:
   int mOffsetIndex;
};

WsfIntersectMesh::WsfIntersectMesh()
   : WsfSimplePlatformComponent()
   , mMeshIndex(-1)
   , mSharePtr(new Shared())
   , mPlatformPtr(nullptr)
   , mNameId()
{
}

WsfIntersectMesh::WsfIntersectMesh(const WsfIntersectMesh& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , mMeshIndex(aSrc.mMeshIndex)
   , mOffsetNameMap(aSrc.mOffsetNameMap)
   , mOffsetMap(aSrc.mOffsetMap)
   , mSharePtr(aSrc.mSharePtr)
   , mPlatformPtr(aSrc.mPlatformPtr)
   , mNameId(aSrc.mNameId)
{
   mSharePtr->AddRef();
}

// virtual
WsfIntersectMesh::~WsfIntersectMesh()
{
   mSharePtr->Unref();
}

// virtual
WsfObject* WsfIntersectMesh::Clone() const
{
   return new WsfIntersectMesh(*this);
}

WsfComponent* WsfIntersectMesh::CloneComponent() const
{
   return new WsfIntersectMesh(*this);
}

// virtual
bool WsfIntersectMesh::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "define_offset")
   {
      std::string   offsetName;
      UtLengthValue offsetX;
      UtLengthValue offsetY;
      UtLengthValue offsetZ;

      aInput.ReadValue(offsetName);
      aInput.ReadUnitValue(offsetX);
      aInput.ReadUnitValue(offsetY);
      aInput.ReadUnitValue(offsetZ);

      // store
      DefineOffset(offsetName, offsetX, offsetY, offsetZ);
   }
   else if (command == "file")
   {
      std::string meshName;
      aInput.ReadValue(meshName);
      std::string meshFileName = aInput.LocateFile(meshName);
      LoadMesh(meshFileName);
      if (mMeshIndex == -1)
      {
         std::string err = "Could not find mesh name: " + meshFileName;
         throw UtException(err);
      }
   }
   else
   {
      myCommand = WsfPlatformComponent::ProcessInput(aInput);
   }

   return myCommand;
}

//! This method will give you information about beam intersections from the aSourcePlatform in the
//! general direction of the named offset aTargetOffset on the processor's platform with an added aErrorAngle radians of
//! rotation about the source point. It will return true if there is an intersection. aBeamSurfaceDotProduct is the dot
//! product of the beam direction with the hit surface normal. aMaterialPropertyCode is the material code of the hit
//! surface with:
//!                 0 = target body
//!                 1 = canopy (material code 1)
//!                 2 = sensor   (material code 2)
//! aDistance is the distance to the intersection point from the source point.
//! It does not account for occlusions from the source platform, terrain, or any other platforms.
//! Add the intersect mesh definition
//!     intersect_mesh aircraft_intersect_mesh
//!        define_offset CANOPY
//!           3.461642 m
//!           0.000184 m
//!           -0.943480 m
//!        define_offset SENSOR
//!           5.138014 m
//!           0.000005 m
//!           -0.629733 m
//!        file ../meshes/aircraft.imesh
//!     end_intersect_mesh
//!
//! Then add the intersect mesh definition inline, like this
//! platform_type AIRCRAFT WSF_PLATFORM
//!   intersect_mesh aircraft_intersect_mesh
//!   ...
//! end_platform_type
//! Then in code, the call to intersect the SENSOR would be:
//! partPtr->GetTarget()->GetComponent<WsfSimplePlatformComponent>("aircraft_intersect_mesh")->Intersect(partPtr->GetPlatform(),
//! // the parent (attacker)
//!                                                                "SENSOR",
//!                                                                0.0175, // ~1 degree error
//!                                                                dotProd,
//!                                                                surfProp,
//!                                                                dist);
bool WsfIntersectMesh::Intersect(WsfPlatform*       aSourcePlatform,
                                 const std::string& aTargetOffset,
                                 const double       aErrorAngle, // in radians
                                 double&            aBeamSurfaceDotProduct,
                                 int&               aMaterialPropertyCode,
                                 double&            aDistance)
{
   if (mMeshIndex < 0)
   {
      return false;
   }
   UtIntersectMesh* meshPtr = LookupMesh(mMeshIndex);
   if (meshPtr == nullptr)
   {
      return false;
   }

   double srcEcs[3] = {0.0, 0.0, 0.0};
   double srcWcs[3];
   double tgtEcs[3] = {0.0, 0.0, 0.0};
   double tgtWcs[3];

   if (aTargetOffset != "")
   {
      // int index = mOffsetDictionary.GetNumber(aTargetOffset);
      OffsetNameMap::const_iterator iter = mOffsetNameMap.find(aTargetOffset);
      if (iter != mOffsetNameMap.end())
      {
         int index = iter->second;
         if (mOffsetMap.find(index) != mOffsetMap.end())
         {
            Offset tgtOffset = mOffsetMap[index];
            tgtEcs[0]        = tgtOffset.x;
            tgtEcs[1]        = tgtOffset.y;
            tgtEcs[2]        = tgtOffset.z;
         }
      }
   }
   aSourcePlatform->ConvertECSToWCS(srcEcs, srcWcs);
   mPlatformPtr->ConvertECSToWCS(tgtEcs, tgtWcs); // calculate the source and target in world coordinates

   if (aErrorAngle != 0.0) // this following code will add an angular error to the beam direction
   {
      double arbRot = aSourcePlatform->GetSimulation()->GetRandom().Uniform(0.0, UtMath::cTWO_PI); // get a random rotation
      double beamVector[3] = {tgtWcs[0] - srcWcs[0], tgtWcs[1] - srcWcs[1], tgtWcs[2] - srcWcs[2]};
      double beamDist =
         sqrt(beamVector[0] * beamVector[0] + beamVector[1] * beamVector[1] + beamVector[2] * beamVector[2]);
      beamVector[0] /= beamDist;
      beamVector[1] /= beamDist;
      beamVector[2] /= beamDist;
      double va[3] = {1, 0, 0};
      if (beamVector[0] * va[0] + beamVector[1] * va[1] + beamVector[2] * va[2] == 0)
      {
         va[0] = 0;
         va[1] = 1;
         va[2] = 0;
      }
      double vb[3]  = {beamVector[1] * va[2] - beamVector[2] * va[1],
                      beamVector[2] * va[0] - beamVector[0] * va[2],
                      beamVector[0] * va[1] - beamVector[1] * va[0]};
      double normer = 1 / sqrt(vb[0] * vb[0] + vb[1] * vb[1] + vb[2] * vb[2]);
      vb[0] *= normer;
      vb[1] *= normer;
      vb[2] *= normer;

      va[0]  = vb[1] * beamVector[2] - vb[2] * beamVector[1];
      va[1]  = vb[2] * beamVector[0] - vb[0] * beamVector[2];
      va[2]  = vb[0] * beamVector[1] - vb[1] * beamVector[0];
      normer = 1 / sqrt(va[0] * va[0] + va[1] * va[1] + va[2] * va[2]);
      va[0] *= normer;
      va[1] *= normer;
      va[2] *= normer; // [beamVector, va, vb] is the beam matrix
      double sa           = sin(arbRot);
      double ca           = cos(arbRot); // rotation about beam direction
      double sb           = sin(aErrorAngle);
      double cb           = cos(aErrorAngle); // rotation about arbitrary perpendicular vector (error)
      double errorBeam[3] = {cb * beamVector[0] + sb * (sa * va[0] + ca * vb[0]),
                             cb * beamVector[1] + sb * (sa * va[1] + ca * vb[1]),
                             cb * beamVector[2] + sb * (sa * va[2] + ca * vb[2])}; // unit beamVector with angular error
      tgtWcs[0]           = errorBeam[0] * beamDist * 1.1 + srcWcs[0];
      tgtWcs[1] =
         errorBeam[1] * beamDist * 1.1 + srcWcs[1]; // 1.1 is to account for changes in distance caused by rotation
      tgtWcs[2] = errorBeam[2] * beamDist * 1.1 + srcWcs[2]; // tgtWcs with angular error from srcWcs
   }

   double psi;
   double theta;
   double phi;
   double pos[3];

   mPlatformPtr->GetOrientationWCS(psi, theta, phi); // like heading pitch roll
   mPlatformPtr->GetLocationWCS(pos);

   meshPtr->SetPositionOrientation(pos[0], pos[1], pos[2], phi, theta, psi); // rotate and position the mesh properly in WCS
   return meshPtr->GetDistDotAndCode(srcWcs, tgtWcs, aDistance, aBeamSurfaceDotProduct, aMaterialPropertyCode);
}

//! This method will load the mesh from aFileName, and add it to its list of available meshes
//! If the file cannot be loaded, it sets mMeshIndex to -1, else the mesh lookup index
//! aMeshPtr allows an externally loaded intersect mesh to be added to the processor
int WsfIntersectMesh::LoadMesh(const std::string& aMeshFileName)
{
   mMeshIndex = mSharePtr->mMeshDictionaryPtr->GetNumber(aMeshFileName);

   // If mesh is already in map, do nothing. Otherwise, load the mesh
   if (mSharePtr->mMeshMap.find(mMeshIndex) == mSharePtr->mMeshMap.end())
   {
      bool             loaded     = false;
      UtIntersectMesh* newMeshPtr = new UtIntersectMesh(aMeshFileName, loaded);
      if (!loaded)
      {
         delete newMeshPtr;
         mMeshIndex = -1;
      }
      else
      {
         mSharePtr->mMeshMap[mMeshIndex] = newMeshPtr; // add mesh to lookup list
      }
   }

   return mMeshIndex;
}

void WsfIntersectMesh::DefineOffset(const std::string& aOffsetName, double aOffsetX, double aOffsetY, double aOffsetZ)
{
   // int offsetIndex = mOffsetDictionary.GetNumber(aOffsetName);
   int    offsetIndex = mSharePtr->GetNextIndex();
   Offset offset(aOffsetX, aOffsetY, aOffsetZ);
   mOffsetMap[offsetIndex]     = offset;
   mOffsetNameMap[aOffsetName] = offsetIndex;
}

bool WsfIntersectMesh::RemoveOffset(const std::string& aOffsetName)
{
   OffsetNameMap::iterator iter   = mOffsetNameMap.find(WsfStringId(aOffsetName));
   bool                    erased = false;
   if (iter != mOffsetNameMap.end())
   {
      int offsetIndex = iter->second;
      erased          = (mOffsetMap.erase(offsetIndex) == 1);
      mOffsetNameMap.erase(iter);
   }
   return erased;
}

//! This method will look for a mesh with the given lookup index in its list of available meshes and return the mesh.
//! If none is found, it returns 0
UtIntersectMesh* WsfIntersectMesh::LookupMesh(const int aIndex)
{
   if (mSharePtr->mMeshMap.find(aIndex) != mSharePtr->mMeshMap.end())
   {
      return mSharePtr->mMeshMap[aIndex];
   }
   else
   {
      return nullptr; // not loaded
   }
}

//! Get the name of the specified target offset entry id.
WsfStringId WsfIntersectMesh::GetTargetOffsetName(unsigned aOffsetEntry) const
{
   WsfStringId                   offsetName;
   OffsetNameMap::const_iterator iter = mOffsetNameMap.begin();
   for (unsigned increment = 1; increment < aOffsetEntry - 1; ++increment)
   {
      ++iter;
   }
   if (iter != mOffsetNameMap.end())
   {
      offsetName = iter->first;
   }
   return offsetName;
}

bool WsfIntersectMesh::TargetOffsetExists(const std::string& aTargetOffsetName) const
{
   bool                          exists = false;
   OffsetNameMap::const_iterator iter   = mOffsetNameMap.find(aTargetOffsetName);
   if (iter != mOffsetNameMap.end())
   {
      exists = (mOffsetMap.find(iter->second) != mOffsetMap.end());
   }
   return exists;
}
