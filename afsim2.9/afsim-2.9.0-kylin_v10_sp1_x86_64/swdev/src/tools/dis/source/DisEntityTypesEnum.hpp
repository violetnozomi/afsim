// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISENTITYTYPESENUM_HPP
#define DISENTITYTYPESENUM_HPP

#include <memory>
#include <string>
#include <vector>

#include "DisEnumConversion.hpp"
#include "DisTypes.hpp"

struct Data
{
   DisEnum8  mKind;
   DisEnum8  mDomain;
   DisEnum16 mCountry;
   DisEnum8  mCatagory;
   DisEnum8  mSubcatagory;
   DisEnum8  mSpecific;
   DisEnum8  mExtra;
};

namespace Layout
{

enum TypeLayout
{
   EntityKind,
   Domain,
   Country,
   Category,
   Subcategory,
   Specific,
   Extra

};
}

struct SubCategory
{
   virtual const std::string ToString(DisEnum8 aIn) = 0;
};

struct Motorcycle : public SubCategory
{
public:
   Motorcycle(struct Data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      ScooterSmall,
      SportStreet,
      Cruiser,
      DirtBike
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct Car : public SubCategory
{
public:
   Car(struct Data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      Generic = 10,
      GenericMiniMicrocar,
      GenericEconomyCompact,
      GenericIntermediateStandard,
      GenericFullPremiumLuxury,
      GenericOversize,
      _2DoorCoupe = 20,
      _2DoorCoupeMiniMicrocar,
      _2DoorCoupeEconomyCompact,
      _2DoorCoupeIntermediateStandard,
      _3DoorHatchback = 30,
      _3DoorHatchbackMiniMicrocar,
      _3DoorHatchbackEconomyCompact,
      _4DoorSedan = 40,
      _4DoorSedanMiniMicrocar,
      _4DoorSedanEconomyCompact,
      _4DoorSedanIntermediateStandard,
      _4DoorSedanFullPremiumLuxury,
      _4DoorSedanOversize,
      _5DoorHatchback = 50,
      _5DoorHatchbackMiniMicrocar,
      _5DoorHatchbackEconomyCompact,
      _5DoorHatchbackIntermediateStandard,
      DoorHatchbackFullPremiumLuxury,
      Wagon               = 60,
      WagonEconomyCompact = 62,
      WagonIntermediateStandard,
      WagonFullPremiumLuxury,
      Minivan                    = 70,
      Limousine                  = 80,
      LimousineFullPremiumLuxury = 84,
      LimousineOversize,
      Sports      = 90,
      Convertible = 100,
      ConvertibleMiniMicrocar,
      ConvertibleEconomyCompact,
      ConvertibleIntermediateStandard,
      ConvertibleFullPremiumLuxury,
      SportsUtilityVehicle               = 110,
      SportsUtilityVehicleEconomyCompact = 112,
      SportsUtilityVehicleIntermediateStandard,
      SportsUtilityVehicleFullPremiumLuxury,
      SportsUtilityVehicleOversize
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct Bus : public SubCategory
{
public:
   Bus(struct Data&, std::vector<std::string>&);

   enum Enum
   {
      Other,
      CommuterFlatNose,
      CommuterSnoutNose,
      Shuttle,
      DoubleDecker,
      Guided,
      Kneeling,
      Midibus,
      Minibus,
      MiniWheelchair,
      Motorcoach,
      PrisonBus,
      Schoolbus,
      SchoolWheelchair,
      Tour,
      TramParkingLot,
      Trolley,
      AirportTransport,
      Articulated
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct Category
{
   std::unique_ptr<SubCategory> subcategory;

   virtual const std::string ToString(DisEnum8 aIn) = 0;
};

struct LandCategory : Category
{
   LandCategory(struct Data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      Tank,
      ArmoredFightingVehicle,
      ArmoredUtilityVehicle,
      SelfPropelledArtillery,
      TowedArtillery,
      SmallWheeledUtilityVehicle,
      LargeWheeledUtilityVehicle,
      SmallTrackedUtilityVehicle,
      LargeTrackedUtilityVehicle,
      Mortar,
      MinePlow,
      MineRake,
      MineRoller,
      CargoTrailer,
      FuelTrailer,
      GeneratorTrailer,
      WaterTrailer,
      EngineerEquipment,
      HeavyEquipmentTransportTrailer,
      MaintenanceEquipmentTrailer,
      Limber,
      ChemicalDecontaminationTrailer,
      WarningSystem,
      TrainEngine,
      TrainCar,
      TrainCaboose,
      CivilianVehicle,
      AirDefenseMissileDefenseUnitEquipment,
      C3ISystem,
      OperationsFacility,
      IntelligenceFacility,
      SurveillanceFacility,
      CommunicationsFacility,
      CommandFacility,
      C4IFacility,
      ControlFacility,
      FireControlFacility,
      MissileDefenseFacility,
      FieldCommandPost,
      ObservationPost,
      Unmanned   = 50,
      Motorcycle = 80,
      Car,
      Bus,
      SingleUnitCargoTruck,
      SingleUnitUtilityEmergencyTruck,
      MultipleUnitCargoTruck,
      MultipleUnitUtilityEmergencyTruck,
      ConstructionSpecialtyVehicle,
      FarmSpecialtyVehicle,
      Trailer,
      Recreational,
      NonMotorized,
      Trains,
      UtilityEmergencyCar
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct AirCategory : Category
{
   AirCategory(struct data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      FighterAirDefense,
      AttackStrike,
      Bomber,
      CargoTanker,
      ASWPatrolObservation,
      ElectronicWarfare,
      Reconnaissance,
      SurveillanceC2,
      AirSeaRescue,
      AttackHelicopter = 20,
      UtilityHelicopter,
      AntiSubmarineWarfarePatrolHelicopter,
      CargoHelicopter,
      ObservationHelicopter,
      SpecialOperationsHelicopter,
      Trainer                                = 40,
      Unmanned                               = 50,
      NonCombatantCommercialAircraft         = 57,
      CivilianUltralightAircraftNonrigidWing = 80,
      CivilianUltralightAircraftRigidWing,
      CivilianFixedWingAircraftGlider = 83,
      CivilianFixedWingAircraftLightSport,
      CivilianFixedWingAircraftSmall,
      CivilianFixedWingAircraftMedium,
      CivilianFixedWingAircraftLarge,
      CivilianFixedWingAircraftHeavy,
      CivilianHelicopterSmall = 90,
      CivilianHelicopterMedium,
      CivilianHelicopterLarge,
      CivilianAutogyro,
      CivilianLighterthanAirBalloon,
      CivilianLighterthanAirAirship
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct SurfaceCategory : Category
{
   SurfaceCategory(struct data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      Carrier,
      CommandShipCruiser,
      GuidedMissileCruiser,
      GuidedMissileDestroyer,
      Destroyer,
      GuidedMissileFrigate,
      LightPatrolCraft,
      MineCountermeasureShipCraft,
      DockLandingShip,
      TankLandingShip,
      LandingCraft,
      LightCarrier,
      CruiserHelicopterCarrier,
      Hydrofoil,
      AirCushionSurfaceEffect,
      Auxiliary,
      AuxiliaryMerchantMarine,
      Utility,
      UnmannedSurfaceVehicle,
      LittoralCombatShips,
      SurveillanceShip,
      Frigate = 50,
      Battleship,
      HeavyCruiser,
      DestroyerTender,
      AmphibiousAssaultShip,
      AmphibiousCargoShip,
      AmphibiousTransportDock,
      AmmunitionShip,
      CombatStoresShip,
      SurveillanceTowedArraySonarSystem,
      FastCombatSupportShip,
      NonCombatantShip,
      CoastGuardCutters,
      CoastGuardBoats,
      FastAttackCraft,
      PassengerVessel = 80,
      DryCargoShip,
      Tanker,
      SupportVessel,
      PrivateMotorboat,
      PrivateSailboat,
      FishingVessel,
      OtherVessels,
      SearchandRescueVessels = 100,
      LifeSavingEquipment    = 101
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct SubsurfaceCategory : Category
{
   SubsurfaceCategory(struct data&, std::vector<std::string>&);

   enum Enum
   {
      Other,
      SSBN,
      SSGN,
      SSN,
      SSG,
      SS,
      SSAN,
      SSA,
      UnmannedUnderwaterVehicle,
      SSB,
      SSC,
      SSP,
      SSM,
      SSNR,
      SST,
      AGSS,
      SemiSubmersibleBoats,
      CivilianSubmarines,
      CivilianSubmersibles,
      CivilianSemiSubmersibleBoats
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct SpaceCategory : Category
{
   SpaceCategory(struct data&, std::vector<std::string>&);

   enum Enum
   {
      Other,
      Manned,
      Unmanned,
      Booster
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct MunitionCategory : Category
{
   MunitionCategory(struct data&, std::vector<std::string>&);

   enum Enum
   {
      Other,
      Guided,
      Ballistic,
      Fixed
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 iaInn);
};

struct Domain
{
   std::unique_ptr<Category> mCategory;

   virtual const std::string ToString(DisEnum8 aIn) = 0;
};

// domains applicable to Platform, Lifeform, Environmental,
// Cultural Feature, Radio, Expendable, and Sensor/Emitter Kinds.
struct OtherDomain : Domain
{
   OtherDomain(struct data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      Land,
      Air,
      Surface,
      Subsurface,
      Space
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct MunitionDomain : Domain
{
   MunitionDomain(struct data&, std::vector<std::string>&);
   enum Enum
   {
      Other,
      AntiAir,
      AntiArmor,
      AntiGuidedWeapon,
      AntiRadar,
      AntiSatellite,
      AntiShip,
      AntiSubmarine,
      AntiPersonnel,
      BattlefieldSupport,
      Strategic,
      Tactical,
      DirectedEnergyWeapon
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct SupplyDomain : Domain
{
   SupplyDomain(struct data&, std::vector<std::string>&);

   enum Enum
   {
      NotUsed,
      Class1,
      Class2,
      Class3,
      Class4,
      Class5,
      Class6,
      Class7,
      Class8,
      Class9,
      Class10,
      Class11,
      Class12
   };

   const std::vector<std::pair<Enum, std::string>> mData;
   std::unique_ptr<DisEnum::Conversion<Enum>>      mConversionPtr;

   const std::string ToString(Enum aIn);
   const std::string ToString(DisEnum8 aIn);
};

struct Kind
{
   Kind(struct data&, std::vector<std::string>&);

   enum Enum
   {
      Other,
      Platform,
      Munition,
      Lifeform,
      Environmental,
      CulturalFeature,
      Supply,
      Radio,
      Expendable,
      SensorEmitter
   };

   std::unique_ptr<Domain>                    mDomain;
   std::vector<std::pair<Enum, std::string>>  mData;
   std::unique_ptr<DisEnum::Conversion<Enum>> mConversionPtr;

   const std::string         ToString(Enum aKind);
   virtual const std::string ToString(DisEnum8 aIn);
};

struct PlatformKind : public Kind
{
   PlatformKind(struct data&, std::vector<std::string>&);
};

struct MunitionKind : Kind
{
   MunitionKind(struct data&, std::vector<std::string>&);
};

struct LifeformKind : Kind
{
   // const std::string ToString(DisEnum8 aIn);
};

struct Country
{
   Country(struct data&, std::vector<std::string>&);
   const std::string ToString(DisEnum16 aIn);
};

struct EntityTypeString
{
   EntityTypeString(struct data aData);

   std::vector<std::string> stringList;

   const std::string KindString(DisEnum8);
   const std::string DomainString(DisEnum8);
   const std::string CountryString(DisEnum16);
   const std::string CategoryString(DisEnum8);
   const std::string SubCategoryString(DisEnum8);
   const std::string SpecificString(DisEnum8);
   const std::string ExtraString(DisEnum8);

   std::unique_ptr<Kind>    mKind;
   std::unique_ptr<Country> mCountry;
};

#endif
