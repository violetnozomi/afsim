// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MissionPlanQFT_h
#define Uci__Type__MissionPlanQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanApprovalEnum_h)
# include "uci/type/MissionPlanApprovalEnum.h"
#endif

#if !defined(Uci__Type__RouteID_Type_h)
# include "uci/type/RouteID_Type.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanQFT sequence accessor class */
      class MissionPlanQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~MissionPlanQFT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanQFT
           *
           * @return This accessor's type constant, i.e. MissionPlanQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on who the mission plan is allocated to [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> AllocatedToID;

         /** Filter on the Mission Plan ID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Filter on the state of the mission plan. [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::MissionPlanApprovalEnum, uci::type::accessorType::missionPlanApprovalEnum> MissionPlanState;

         /** Filter on the Route ID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RouteID_Type, uci::type::accessorType::routeID_Type> RouteID;

         /** Returns the bounded list that is identified by the AllocatedToID.
           *
           * @return The bounded list identified by AllocatedToID.
           */
         virtual const uci::type::MissionPlanQFT::AllocatedToID& getAllocatedToID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AllocatedToID.
           *
           * @return The bounded list identified by AllocatedToID.
           */
         virtual uci::type::MissionPlanQFT::AllocatedToID& getAllocatedToID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AllocatedToID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllocatedToID(const uci::type::MissionPlanQFT::AllocatedToID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanQFT::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanQFT::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanQFT::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanState.
           *
           * @return The bounded list identified by MissionPlanState.
           */
         virtual const uci::type::MissionPlanQFT::MissionPlanState& getMissionPlanState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanState.
           *
           * @return The bounded list identified by MissionPlanState.
           */
         virtual uci::type::MissionPlanQFT::MissionPlanState& getMissionPlanState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanState(const uci::type::MissionPlanQFT::MissionPlanState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RouteID.
           *
           * @return The bounded list identified by RouteID.
           */
         virtual const uci::type::MissionPlanQFT::RouteID& getRouteID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RouteID.
           *
           * @return The bounded list identified by RouteID.
           */
         virtual uci::type::MissionPlanQFT::RouteID& getRouteID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RouteID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRouteID(const uci::type::MissionPlanQFT::RouteID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual const uci::type::GeoFiltersType& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeoFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeoFilter.
           */
         virtual uci::type::GeoFiltersType& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeoFilter to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeoFilter
           */
         virtual void setGeoFilter(const uci::type::GeoFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GeoFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GeoFilter is emabled or not
           */
         virtual bool hasGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GeoFilter
           *
           * @param type = uci::type::accessorType::geoFiltersType This Accessor's accessor type
           */
         virtual void enableGeoFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::geoFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GeoFilter */
         virtual void clearGeoFilter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanQFT to copy from
           */
         MissionPlanQFT(const MissionPlanQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanQFT to the contents of the MissionPlanQFT on the right
           * hand side (rhs) of the assignment operator.MissionPlanQFT [only available to derived classes]
           *
           * @param rhs The MissionPlanQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::MissionPlanQFT
           * @return A constant reference to this MissionPlanQFT.
           */
         const MissionPlanQFT& operator=(const MissionPlanQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanQFT_h

