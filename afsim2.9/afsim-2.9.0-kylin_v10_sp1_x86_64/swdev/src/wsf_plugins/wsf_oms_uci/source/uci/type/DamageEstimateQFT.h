// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DamageEstimateQFT_h
#define Uci__Type__DamageEstimateQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DamageEstimateID_Type_h)
# include "uci/type/DamageEstimateID_Type.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageEstimateQFT sequence accessor class */
      class DamageEstimateQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~DamageEstimateQFT()
         { }

         /** Returns this accessor's type constant, i.e. DamageEstimateQFT
           *
           * @return This accessor's type constant, i.e. DamageEstimateQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageEstimateQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageEstimateQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageEstimateID_Type, uci::type::accessorType::damageEstimateID_Type> DamageEstimateID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::DamageCategoryEnum, uci::type::accessorType::damageCategoryEnum> EstimateType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Returns the bounded list that is identified by the DamageEstimateID.
           *
           * @return The bounded list identified by DamageEstimateID.
           */
         virtual const uci::type::DamageEstimateQFT::DamageEstimateID& getDamageEstimateID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageEstimateID.
           *
           * @return The bounded list identified by DamageEstimateID.
           */
         virtual uci::type::DamageEstimateQFT::DamageEstimateID& getDamageEstimateID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageEstimateID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageEstimateID(const uci::type::DamageEstimateQFT::DamageEstimateID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimateType.
           *
           * @return The bounded list identified by EstimateType.
           */
         virtual const uci::type::DamageEstimateQFT::EstimateType& getEstimateType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimateType.
           *
           * @return The bounded list identified by EstimateType.
           */
         virtual uci::type::DamageEstimateQFT::EstimateType& getEstimateType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EstimateType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEstimateType(const uci::type::DamageEstimateQFT::EstimateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::DamageEstimateQFT::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::DamageEstimateQFT::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::DamageEstimateQFT::TaskID& value)
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
         DamageEstimateQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageEstimateQFT to copy from
           */
         DamageEstimateQFT(const DamageEstimateQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageEstimateQFT to the contents of the DamageEstimateQFT on the
           * right hand side (rhs) of the assignment operator.DamageEstimateQFT [only available to derived classes]
           *
           * @param rhs The DamageEstimateQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DamageEstimateQFT
           * @return A constant reference to this DamageEstimateQFT.
           */
         const DamageEstimateQFT& operator=(const DamageEstimateQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageEstimateQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageEstimateQFT_h

