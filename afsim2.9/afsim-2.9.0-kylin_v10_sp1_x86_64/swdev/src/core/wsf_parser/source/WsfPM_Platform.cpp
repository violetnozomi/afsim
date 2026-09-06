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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfPM_Platform.hpp"

#include "UtSphericalEarth.hpp"
#include "WsfPM_Root.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyStructType.hpp"

WsfPM_Platform::WsfPM_Platform() {}

WsfPM_Platform::WsfPM_Platform(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

// When setting commander to self, this value should be used:
const char* WsfPM_Platform::CommanderSelfValue()
{
   return "SELF";
}

std::string WsfPM_Platform::Icon() const
{
   return GetValue().GetAttr("icon").ValueToString();
}

std::string WsfPM_Platform::Side() const
{
   return GetValue().GetAttr("side").ValueToString();
}

bool WsfPM_Platform::Destructible() const
{
   return GetValue().GetAttr("destructible").GetBoolValue();
}

WsfProxy::Time WsfPM_Platform::CreationTime() const
{
   WsfProxy::Time* creationTime = GetValue().GetAttr("creationTime")["constant"]["value"];
   if (creationTime)
   {
      return *creationTime;
   }
   return WsfProxy::Time();
}


WsfProxy::Position WsfPM_Platform::Position() const
{
   return GetAttributeT<WsfProxy::Position>("position");
}

WsfPProxyNode WsfPM_Platform::PositionNode() const
{
   return *this + "position";
}

bool WsfPM_Platform::SetPosition(const WsfProxy::Position& aPosition)
{
   return AssignAttribute("position", aPosition);
}

WsfProxy::Angle WsfPM_Platform::Heading() const
{
   return GetAttributeT<WsfProxy::Angle>("heading");
}

WsfPProxyNode WsfPM_Platform::HeadingNode() const noexcept
{
   return *this + "heading";
}

WsfProxy::Angle WsfPM_Platform::InitialHeading() const
{
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull())
   {
      if (!waypt.Heading().IsUnset())
      {
         return waypt.Heading();
      }
      else if (Mover().InitialRoute().WaypointCount() > 1)
      {
         WsfPM_Waypoint next = Mover().InitialRoute().GetWaypoint(1);
         if (next.GetWaypointType() == WsfPM_Waypoint::cPOSITION)
         {
            double heading, distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(InitialLocation().GetLatitude(),
                                                            InitialLocation().GetLongitude(),
                                                            next.Position().GetLatitude(),
                                                            next.Position().GetLongitude(),
                                                            heading,
                                                            distance);
            heading *= UtMath::cRAD_PER_DEG;
            return WsfProxy::Angle(heading);
         }
      }
   }
   return Heading();
}

void WsfPM_Platform::SetHeading(const WsfProxy::Angle& aAngle)
{
   WsfProxy::Angle* value = GetValue().GetAttr("heading");
   if (value)
   {
      *value = aAngle;
      NotifyAttributeChanged("heading");
   }
}

WsfProxy::Angle WsfPM_Platform::Pitch() const
{
   return GetAttributeT<WsfProxy::Angle>("pitch");
}

void WsfPM_Platform::SetPitch(const WsfProxy::Angle& aAngle)
{
   WsfProxy::Angle* value = GetValue().GetAttr("pitch");
   if (value)
   {
      *value = aAngle;
      NotifyAttributeChanged("pitch");
   }
}

WsfProxy::Angle WsfPM_Platform::Roll() const
{
   return GetAttributeT<WsfProxy::Angle>("roll");
}


void WsfPM_Platform::SetRoll(const WsfProxy::Angle& aAngle)
{
   WsfProxy::Angle* value = GetValue().GetAttr("roll");
   if (value)
   {
      *value = aAngle;
      NotifyAttributeChanged("roll");
   }
}

WsfPM_Fuel WsfPM_Platform::Fuel() const
{
   return WsfPM_Fuel(FuelMap() + "fuel");
}

WsfPM_Mover WsfPM_Platform::Mover() const
{
   return WsfPM_Mover(MoverMap() + "mover");
}

WsfPM_CommMap WsfPM_Platform::Comms() const
{
   return WsfPM_CommMap(*this + "comms");
}

WsfPM_RouterMap WsfPM_Platform::Routers() const
{
   return WsfPM_RouterMap(*this + "routers");
}

WsfPM_SensorMap WsfPM_Platform::Sensors() const
{
   return WsfPM_SensorMap(*this + "sensors");
}

WsfPM_ProcessorMap WsfPM_Platform::Processors() const
{
   return WsfPM_ProcessorMap(*this + "processors");
}

WsfPProxyNode WsfPM_Platform::FindPart(const std::string& aName) const
{
   WsfPM_ObjectMap partMaps[4] = {WsfPM_ObjectMap(*this + "comms"),
                                  WsfPM_ObjectMap(*this + "movers"),
                                  WsfPM_ObjectMap(*this + "sensors"),
                                  WsfPM_ObjectMap(*this + "processors")};
   for (int i = 0; i < 4; ++i)
   {
      WsfPProxyNode node = partMaps[i].Find(aName);
      if (node)
      {
         return node;
      }
   }
   return WsfPProxyNode();
}

bool WsfPM_Platform::AddPart(const WsfPProxyNode& partTypeNode, const std::string& partName)
{
   WsfPProxyValue             partTypeVal    = partTypeNode.GetValue();
   const WsfPProxyType*       partType       = partTypeVal.GetType();
   const WsfPProxyStructType* partTypeStruct = dynamic_cast<const WsfPProxyStructType*>(partType);

   assert(partTypeStruct);
   WsfPM_ObjectMap platObjMap   = GetTypeObjectMap(*partTypeStruct);
   WsfPProxyValue  newPartValue = partTypeNode.GetValue().Copy(WsfProxy::cCOPY_INHERITED);
   WsfPProxyStructValue(newPartValue).SetBase(partTypeNode.GetPath());

   return platObjMap.Add(partName, newPartValue).IsValid();
}

void WsfPM_Platform::SetIcon(const std::string& aIcon)
{
   AssignAttribute("icon", aIcon);
}

void WsfPM_Platform::SetSide(const std::string& aSide)
{
   if (!GetProxy())
   {
      return;
   }
   WsfProxy::String newVal(aSide);
   if (aSide.empty())
   {
      newVal.SetUnset();
   }
   AssignAttribute("side", newVal);
}

void WsfPM_Platform::SetDestructible(bool destructible)
{
   WsfPProxyNode destructibleNode = *this + "destructible";
   destructibleNode.Assign(destructible);
}

void WsfPM_Platform::LocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   WsfProxy::Position* position = GetValue().GetAttr("position");
   if (position)
   {
      aLat = position->GetLatitude();
      aLon = position->GetLongitude();
   }
   WsfProxy::Length* alt = GetValue().GetAttr("altitude");
   if (alt)
   {
      aAlt = alt->GetValue();
   }
}

void WsfPM_Platform::InitialLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   aLat = aLon = aAlt   = 0.0; // If this node is invalid, return a reasonable value
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull() && waypt.GetWaypointType() == WsfPM_Waypoint::cPOSITION)
   {
      WsfProxy::Position position = waypt.Position();
      aLat                        = position.GetLatitude();
      aLon                        = position.GetLongitude();

      WsfProxy::Length alt = waypt.Altitude();
      aAlt                 = alt.GetValue();
   }
   else
   {
      LocationLLA(aLat, aLon, aAlt);
   }
}

WsfProxy::Position WsfPM_Platform::InitialLocation() const
{
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull() && waypt.GetWaypointType() == WsfPM_Waypoint::cPOSITION)
   {
      return waypt.Position();
   }
   return *(WsfProxy::Position*)GetValue().GetAttr("position");
}

bool WsfPM_Platform::SetInitialLocation(const WsfProxy::Position& aPosition) const
{
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull() && waypt.GetWaypointType() == WsfPM_Waypoint::cPOSITION)
   {
      waypt.AssignAttribute("position", aPosition);
   }
   else
   {
      AssignAttribute("position", aPosition);
   }
   return true;
}

void WsfPM_Platform::SetInitialLocationLLA(double aLat, double aLon, double aAlt)
{
   WsfProxy::Position loc = InitialLocation();

   if (loc.GetLatitude() != aLat || loc.GetLongitude() != aLon)
   {
      loc.SetLatitude(aLat);
      loc.SetLongitude(aLon);
      SetInitialLocation(loc);
   }
   WsfProxy::Length alt = EffectiveAltitude();
   alt.SetValue(aAlt);
   SetEffectiveAltitude(alt);
}

std::string WsfPM_Platform::CommanderName(const std::string& aChainName) const
{
   return GetValue().GetAttr("commandChains").GetAttr(aChainName).ValueToString();
}

bool WsfPM_Platform::RemoveFromCommandChain(const std::string& aChainName) const
{
   return SetCommanderName(aChainName, CommanderSelfValue());
}

bool WsfPM_Platform::SetCommanderName(std::string aChainName, std::string aCommanderName) const
{
   WsfPM_ObjectMap     cmdrs  = Commanders();
   WsfPProxyObjectMap* mapPtr = cmdrs.GetObjectMap();
   if (mapPtr)
   {
      auto cmdrName = WsfPProxyValue::ConstructNew(mapPtr->GetDataType());
      cmdrName.SetValue(aCommanderName);
      cmdrs.AssignAttribute(aChainName, cmdrName);
      return (bool)cmdrs.AssignEntry(aChainName, cmdrName);
   }
   return false;
}

std::vector<std::string> WsfPM_Platform::CommandChainNames() const
{
   WsfPProxyObjectMap* mapPtr = GetValue().GetAttr("commandChains").GetObjectMap();

   if (mapPtr)
   {
      return mapPtr->GetObjectNames();
   }

   return std::vector<std::string>();
}

// translate <local:master> and <local:slave> into a real comm network name
std::string WsfPM_Platform::TrueCommNetworkName(const std::string& aNetworkName) const
{
   if (aNetworkName == "<local:master>")
   {
      return "master:" + GetName();
   }
   else if (aNetworkName == "<local:slave>")
   {
      std::string cmdr = CommanderName("default");
      if (!cmdr.empty())
      {
         return "master:" + cmdr;
      }
      return ""; // undefined
   }
   else
   {
      return aNetworkName;
   }
}

// Return list of all network names used by all comm devices
std::vector<std::string> WsfPM_Platform::CommNetworkNames() const
{
   std::vector<std::string> names;
   WsfPM_CommMap            comms = Comms();
   std::vector<WsfPM_Comm>  commList;
   comms.GetValuesT(commList);
   for (size_t i = 0; i < commList.size(); ++i)
   {
      WsfProxy::String netName = commList[i].GetNetworkName();
      if (!netName.IsUnset())
      {
         std::string name = TrueCommNetworkName(netName.GetValue());
         if (!name.empty())
         {
            names.push_back(name);
         }
      }
   }
   return names;
}

void WsfPM_Platform::PlatformRenamed(const std::string& aOldName, const std::string& aNewName)
{
   // Update any commanders from aOldName to aNewName
   WsfPProxyObjectMap* cmdMapPtr = GetValue().GetAttr("commandChains").GetObjectMap();
   if (!cmdMapPtr)
   {
      return;
   }
   for (auto i = cmdMapPtr->GetValues().begin(); i != cmdMapPtr->GetValues().end(); ++i)
   {
      if (i->second.ValueToString() == aOldName)
      {
         i->second.SetValue(aNewName);
      }
   }
}
bool WsfPM_Platform::Rename(const std::string& aNewName)
{
   if (GetPath().size() < 2)
   {
      return false;
   }
   WsfPM_Root r(Root());

   bool isPlatformInstance = r.platforms().GetPath().IsPrefixOf(GetPath());     // is platform instance
   bool isPlatformType     = r.platformTypes().GetPath().IsPrefixOf(GetPath()); // is platform type
   if (!isPlatformInstance && !isPlatformType)
   {
      return false;
   }
   WsfPM_PlatformMap owningMap(GetParent());

   // check if name already exists
   if (owningMap.Exists(aNewName))
   {
      return false;
   }

   std::string oldName = GetName();

   std::set<WsfPProxyNode> updatedObjects;

   WsfPProxyPath oldPath = GetPath();

   updatedObjects.insert(*this);
   WsfPProxyPath newPath = GetPath();
   newPath.Back()        = aNewName;

   // Update path map from type -> platform
   if (GetBasePath())
   {
      WsfPProxyPathMap::iterator pathMapIter = GetProxy()->mPathMap->find(*GetBasePath());
      if (pathMapIter != GetProxy()->mPathMap->end())
      {
         pathMapIter->second.erase(oldPath);
         pathMapIter->second.insert(newPath);
      }
   }
   if (isPlatformType)
   {
      // Update derived types to use new name
      WsfPProxyPathMap::iterator pathMapIter = GetProxy()->mPathMap->find(GetPath());
      if (pathMapIter != GetProxy()->mPathMap->end())
      {
         // update path map to point to new name
         const std::set<WsfPProxyPath>& inheritedPaths = (*GetProxy()->mPathMap)[newPath] = pathMapIter->second;
         GetProxy()->mPathMap->erase(pathMapIter);

         // Update inherited types to point to the new type name
         newPath.Back() = aNewName;
         for (std::set<WsfPProxyPath>::const_iterator i = inheritedPaths.begin(); i != inheritedPaths.end(); ++i)
         {
            WsfPProxyNode        derived(GetProxy(), *i);
            const WsfPProxyPath* basePtr = derived.GetBasePath();
            if (basePtr && *basePtr == GetPath()) // expected true:
            {
               derived.SetBasePath(newPath);
               updatedObjects.insert(derived);
            }
         }
      }
   }

   if (isPlatformInstance)
   {
      std::vector<WsfPM_Platform> allPlatforms;
      r.platforms().GetValuesT(allPlatforms);
      r.platformTypes().GetValuesT(allPlatforms);
      for (size_t i = 0; i < allPlatforms.size(); ++i)
      {
         WsfPM_Platform& p = allPlatforms[i];
         p.PlatformRenamed(oldName, aNewName);
      }
   }

   owningMap.Move(oldName, aNewName);
   return true;
}

WsfPM_MoverMap WsfPM_Platform::MoverMap() const
{
   return WsfPM_MoverMap(*this + "movers");
}

WsfPM_FuelMap WsfPM_Platform::FuelMap() const
{
   return WsfPM_FuelMap(*this + "fuels");
}

WsfPM_ObjectMap WsfPM_Platform::GetTypeObjectMap(const WsfPProxyStructType& aProxyStruct)
{
   WsfPM_ObjectMap typeObjMap;

   if (aProxyStruct.IsOfBasicType("Sensor"))
   {
      typeObjMap = WsfPM_ObjectMap(Sensors());
   }
   else if (aProxyStruct.IsOfBasicType("Comm"))
   {
      typeObjMap = WsfPM_ObjectMap(Comms());
   }
   else if (aProxyStruct.IsOfBasicType("Processor"))
   {
      typeObjMap = WsfPM_ObjectMap(Processors());
   }
   else if (aProxyStruct.IsOfBasicType("Mover"))
   {
      typeObjMap = WsfPM_ObjectMap(MoverMap());
   }
   else if (aProxyStruct.IsOfBasicType("Fuel"))
   {
      typeObjMap = WsfPM_ObjectMap(FuelMap());
   }

   return typeObjMap;
}

WsfProxy::Length WsfPM_Platform::EffectiveAltitude() const
{
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull() && !waypt.Altitude().IsUnset())
   {
      return waypt.Altitude();
   }
   else
   {
      return Altitude();
   }
}

wsf::proxy::AltitudeReferenceEnum WsfPM_Platform::EffectiveAGL() const noexcept
{
   if (IsNull())
   {
      return wsf::proxy::AltitudeReferenceEnum::Default;
   }
   // Get the platform's altitude reference
   auto altRefVal = AltitudeReferenceValue();
   // Check the waypoint's altitude reference
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (waypt && waypt.Altitude().IsSet())
   {
      auto waypointAltRef = waypt.AltitudeReferenceValue();
      // This waypoint is part of a global route and its altitude reference is unset
      if (waypointAltRef == wsf::proxy::AltitudeReferenceEnum::Default)
      {
         altRefVal = Mover().DefaultAltitudeReference();
      }
      else
      {
         altRefVal = waypointAltRef;
      }
   }
   return altRefVal;
}

// Returns the first waypoint assigned to the platform if it exists
// useful as it determines starting location
WsfPM_Waypoint WsfPM_Platform::FirstWaypoint() const
{
   WsfPM_Route    route = Mover().InitialRoute();
   WsfPM_Waypoint waypt = route.GetWaypoint(0);
   return waypt;
}

bool WsfPM_Platform::SetEffectiveAltitude(WsfProxy::Length aAltitude)
{
   WsfPM_Waypoint waypt = FirstWaypoint();
   if (!waypt.IsNull() && !waypt.Altitude().IsUnset())
   {
      waypt.AssignAttribute("altitude", aAltitude);
   }
   else
   {
      AssignAttribute("altitude", aAltitude);
   }
   return true;
}

bool WsfPM_Platform::SetEffectiveAGL(bool aIsAGL) noexcept
{
   WsfPM_Waypoint waypt  = FirstWaypoint();
   auto           altRef = aIsAGL ? wsf::proxy::AltitudeReferenceEnum::AGL : wsf::proxy::AltitudeReferenceEnum::MSL;
   if (waypt && waypt.Altitude().IsSet())
   {
      return waypt.SetAltitudeReferenceValue(altRef);
   }
   else
   {
      return SetAltitudeReferenceValue(altRef);
   }
}

std::string WsfPM_Platform::InfraredSignature() const
{
   return GetValue().GetAttr("infraredSignature").ValueToString();
}

std::string WsfPM_Platform::RadarSignature() const
{
   return GetValue().GetAttr("radarSignature").ValueToString();
}

std::string WsfPM_Platform::OpticalSignature() const
{
   return GetValue().GetAttr("opticalSignature").ValueToString();
}

std::string WsfPM_Platform::AcousticSignature() const
{
   return GetValue().GetAttr("acousticSignature").ValueToString();
}

WsfProxy::Length WsfPM_Platform::Altitude() const
{
   return GetAttributeT<WsfProxy::Length>("altitude");
}

WsfPProxyNode WsfPM_Platform::AltitudeNode() const
{
   return *this + "altitude";
}

WsfProxy::Int WsfPM_Platform::AltitudeReference() const
{
   return GetAttributeT<WsfProxy::Int>("altitudeReference");
}

WsfPProxyNode WsfPM_Platform::AltitudeReferenceNode() const
{
   return *this + "altitudeReference";
}

wsf::proxy::AltitudeReferenceEnum WsfPM_Platform::AltitudeReferenceValue() const noexcept
{
   WsfProxy::Int altRef    = AltitudeReference();
   auto          altRefVal = static_cast<wsf::proxy::AltitudeReferenceEnum>(altRef.GetValue());
   if (altRefVal == wsf::proxy::AltitudeReferenceEnum::Default)
   {
      return wsf::proxy::AltitudeReferenceEnum::MSL;
   }
   else
   {
      return altRefVal;
   }
}

bool WsfPM_Platform::SetAltitude(const WsfProxy::Length& aAlt)
{
   return AssignAttribute("altitude", aAlt);
}

bool WsfPM_Platform::SetAltitudeReference(const WsfProxy::Int& aAltRef)
{
   return AssignAttribute("altitudeReference", aAltRef);
}

bool WsfPM_Platform::SetAltitudeReferenceValue(wsf::proxy::AltitudeReferenceEnum aAltRef) noexcept
{
   return AssignAttribute("altitudeReference", static_cast<int>(aAltRef));
}
