// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__OrderOfBattleQFT_h
#define Uci__Type__OrderOfBattleQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__StandardIdentityEnum_h)
# include "uci/type/StandardIdentityEnum.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

#if !defined(Uci__Type__OrderOfBattleEnum_h)
# include "uci/type/OrderOfBattleEnum.h"
#endif

#if !defined(Uci__Type__String20Type_h)
# include "uci/type/String20Type.h"
#endif

#if !defined(Uci__Type__EOB_OperationalStatusEnum_h)
# include "uci/type/EOB_OperationalStatusEnum.h"
#endif

#if !defined(Uci__Type__String4Type_h)
# include "uci/type/String4Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OrderOfBattleQFT sequence accessor class */
      class OrderOfBattleQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~OrderOfBattleQFT()
         { }

         /** Returns this accessor's type constant, i.e. OrderOfBattleQFT
           *
           * @return This accessor's type constant, i.e. OrderOfBattleQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orderOfBattleQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrderOfBattleQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the environment or "domain" in which the Entity is operating. Derived from MIL-STD-6016; see MIL-STD-6016
           * DFI/DUI 275/004 for additional details. Each environment has a corresponding set of specific types. [Minimum
           * occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::EnvironmentEnum, uci::type::accessorType::environmentEnum> Environment;

         /** This element gives a geographical filter to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GeoFiltersType, uci::type::accessorType::geoFiltersType> GeoFilter;

         /** This element gives a temporal filter to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TemporalFilter;

         /** Indicates the "standard identity" of the Entity. Derived from MIL-STD-6016; see MIL-STD-6016 DFI/DUI 376/007 for
           * additional details. [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::StandardIdentityEnum, uci::type::accessorType::standardIdentityEnum> StandardIdentity;

         /** DOD standard country code designator to which an entity owes its allegiance. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CountryCodeType, uci::type::accessorType::countryCodeType> Allegiance;

         /** Indicates the type of Order-of-Battle to which a unit or equipment belongs. [Minimum occurrences: 0] [Maximum
           * occurrences: 11]
           */
         typedef uci::base::BoundedList<uci::type::OrderOfBattleEnum, uci::type::accessorType::orderOfBattleEnum> OB_Type;

         /** Organizational level of the unit. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::String20Type, uci::type::accessorType::string20Type> Echelon;

         /** Degree to which an entity is ready to perform the overall operational mission(s) for which it was organized and
           * equipped. [Minimum occurrences: 0] [Maximum occurrences: 23]
           */
         typedef uci::base::BoundedList<uci::type::EOB_OperationalStatusEnum, uci::type::accessorType::eOB_OperationalStatusEnum> OperationalStatus;

         /** A code describing the amount of unit participation in a deployment. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::String20Type, uci::type::accessorType::string20Type> DeploymentStatus;

         /** Indicates the principal type of mission that an entity is organized and equipped to perform. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::String4Type, uci::type::accessorType::string4Type> MissionPrimary;

         /** The specific name for a given force. For example, Force = ADF (Air Defense Force) and Force Name = Army Air Defense
           * Force. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString256Type, uci::type::accessorType::visibleString256Type> ForceName;

         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual const uci::type::OpZoneID_Type& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual uci::type::OpZoneID_Type& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpZoneID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpZoneID
           */
         virtual void setOpZoneID(const uci::type::OpZoneID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpZoneID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpZoneID is emabled or not
           */
         virtual bool hasOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpZoneID
           *
           * @param type = uci::type::accessorType::opZoneID_Type This Accessor's accessor type
           */
         virtual void enableOpZoneID(uci::base::accessorType::AccessorType type = uci::type::accessorType::opZoneID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpZoneID */
         virtual void clearOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual const uci::type::OrderOfBattleQFT::Environment& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual uci::type::OrderOfBattleQFT::Environment& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Environment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironment(const uci::type::OrderOfBattleQFT::Environment& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual const uci::type::OrderOfBattleQFT::GeoFilter& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual uci::type::OrderOfBattleQFT::GeoFilter& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeoFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeoFilter(const uci::type::OrderOfBattleQFT::GeoFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual const uci::type::OrderOfBattleQFT::TemporalFilter& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual uci::type::OrderOfBattleQFT::TemporalFilter& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TemporalFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTemporalFilter(const uci::type::OrderOfBattleQFT::TemporalFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual const uci::type::OrderOfBattleQFT::StandardIdentity& getStandardIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StandardIdentity.
           *
           * @return The bounded list identified by StandardIdentity.
           */
         virtual uci::type::OrderOfBattleQFT::StandardIdentity& getStandardIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the StandardIdentity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStandardIdentity(const uci::type::OrderOfBattleQFT::StandardIdentity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual const uci::type::OrderOfBattleQFT::Allegiance& getAllegiance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual uci::type::OrderOfBattleQFT::Allegiance& getAllegiance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Allegiance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllegiance(const uci::type::OrderOfBattleQFT::Allegiance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OB_Type.
           *
           * @return The bounded list identified by OB_Type.
           */
         virtual const uci::type::OrderOfBattleQFT::OB_Type& getOB_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OB_Type.
           *
           * @return The bounded list identified by OB_Type.
           */
         virtual uci::type::OrderOfBattleQFT::OB_Type& getOB_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OB_Type.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOB_Type(const uci::type::OrderOfBattleQFT::OB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Echelon.
           *
           * @return The bounded list identified by Echelon.
           */
         virtual const uci::type::OrderOfBattleQFT::Echelon& getEchelon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Echelon.
           *
           * @return The bounded list identified by Echelon.
           */
         virtual uci::type::OrderOfBattleQFT::Echelon& getEchelon()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Echelon.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEchelon(const uci::type::OrderOfBattleQFT::Echelon& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperationalStatus.
           *
           * @return The bounded list identified by OperationalStatus.
           */
         virtual const uci::type::OrderOfBattleQFT::OperationalStatus& getOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperationalStatus.
           *
           * @return The bounded list identified by OperationalStatus.
           */
         virtual uci::type::OrderOfBattleQFT::OperationalStatus& getOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperationalStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperationalStatus(const uci::type::OrderOfBattleQFT::OperationalStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DeploymentStatus.
           *
           * @return The bounded list identified by DeploymentStatus.
           */
         virtual const uci::type::OrderOfBattleQFT::DeploymentStatus& getDeploymentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DeploymentStatus.
           *
           * @return The bounded list identified by DeploymentStatus.
           */
         virtual uci::type::OrderOfBattleQFT::DeploymentStatus& getDeploymentStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DeploymentStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDeploymentStatus(const uci::type::OrderOfBattleQFT::DeploymentStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPrimary.
           *
           * @return The bounded list identified by MissionPrimary.
           */
         virtual const uci::type::OrderOfBattleQFT::MissionPrimary& getMissionPrimary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPrimary.
           *
           * @return The bounded list identified by MissionPrimary.
           */
         virtual uci::type::OrderOfBattleQFT::MissionPrimary& getMissionPrimary()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPrimary.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPrimary(const uci::type::OrderOfBattleQFT::MissionPrimary& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ForceName.
           *
           * @return The bounded list identified by ForceName.
           */
         virtual const uci::type::OrderOfBattleQFT::ForceName& getForceName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ForceName.
           *
           * @return The bounded list identified by ForceName.
           */
         virtual uci::type::OrderOfBattleQFT::ForceName& getForceName()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ForceName.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setForceName(const uci::type::OrderOfBattleQFT::ForceName& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrderOfBattleQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrderOfBattleQFT to copy from
           */
         OrderOfBattleQFT(const OrderOfBattleQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrderOfBattleQFT to the contents of the OrderOfBattleQFT on the
           * right hand side (rhs) of the assignment operator.OrderOfBattleQFT [only available to derived classes]
           *
           * @param rhs The OrderOfBattleQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OrderOfBattleQFT
           * @return A constant reference to this OrderOfBattleQFT.
           */
         const OrderOfBattleQFT& operator=(const OrderOfBattleQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrderOfBattleQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrderOfBattleQFT_h

