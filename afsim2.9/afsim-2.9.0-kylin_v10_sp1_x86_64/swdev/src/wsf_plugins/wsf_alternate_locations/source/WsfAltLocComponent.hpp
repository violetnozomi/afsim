// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSF_ALT_LOC_COMPONENT_HPP
#define WSF_ALT_LOC_COMPONENT_HPP

#include "WsfComponent.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"

namespace wsf
{
namespace altloc
{

enum class AltitudeReference
{
   MSL, // mean sea level
   AGL  // above ground level
};

struct WeightedLocation
{
   double            lat{0.0}; // latitude
   double            lon{0.0}; // longitude
   double            alt{0.0}; // altitude
   double            hed{0.0}; // heading
   AltitudeReference altRef{AltitudeReference::MSL};
   double            weight{0.0};
   bool              altSet{false};
   bool              hedSet{false};
   bool              wgtSet{false};
};

enum class BlockType
{
   LOCAL,
   REF_LOC,
   OFF_ARA,
   OFF_LLA,
   NONE
};

class Component : public WsfObject, public WsfPlatformComponent
{
public:
   enum
   {
      cCOMPONENT = 20181205
   };

   static constexpr const char* cNAME    = "AlternateLocationsComponent";
   static constexpr const char* cCOMMAND = "alternate_locations";

   static constexpr double cINVALID_DRAW           = -1.0;
   static constexpr int    cINVALID_LOCATION_INDEX = -1;

   union Offset
   {
      struct
      {
         double lat;
         double lon;
         double alt;
         double hed;
         bool   altSet;
         bool   hedSet;
      } LLA;

      struct
      {
         double angle;  // great circle heading-angle to determine position with relation to parent
         double radius; // radius of great circle w/ parent at center
         double alt;
         double hed; // platform heading
         bool   altSet;
         bool   hedSet;
      } ARA; // angle-radius-altitude
   };

   Component();
   Component(const Component& aSrc);
   Component& operator=(const Component&) = delete;
   ~Component() override                  = default;

   //! @name Component infrastructure methods.
   //@{
   WsfStringId       GetComponentName() const override;
   const int*        GetComponentRoles() const override;
   WsfComponent*     CloneComponent() const override;
   void*             QueryInterface(int aRole) override;
   static Component* Find(const WsfPlatform& aParent);
   static Component* FindOrCreate(WsfPlatform& aParent);
   //@}

   // Framework methods.
   //@{
   bool       ProcessInput(UtInput& aInput) override;
   WsfObject* Clone() const override;
   //@}

   bool PreInitialize(double aSimTime) override;

   BlockType mBlockType;

   WsfStringId mReferenceName;
   Component*  mReferenceComponent;

   Offset                        mOffset;
   std::vector<WeightedLocation> mLocations;

   double mDraw;
   double mWeightsSum;
   int    mLocationIndex;

private:
   enum class DebugType
   {
      ON,
      OFF,
      GLOBAL,
   };

   int GetRandomLocation();

   void ReadAltLocLine(UtInput& aInput, WeightedLocation& aLoc);
   bool ReadPlatformDebugFlag(UtInput& aInput);

   DebugType mDebugType;
};

} // namespace altloc
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::altloc::Component, wsf::altloc::Component::cCOMPONENT);

#endif
