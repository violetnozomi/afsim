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
#ifndef Uci__Type__OpPointQFT_h
#define Uci__Type__OpPointQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__OpPointID_Type_h)
# include "uci/type/OpPointID_Type.h"
#endif

#if !defined(Uci__Type__OpPointCategoryEnum_h)
# include "uci/type/OpPointCategoryEnum.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpPointQFT sequence accessor class */
      class OpPointQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~OpPointQFT()
         { }

         /** Returns this accessor's type constant, i.e. OpPointQFT
           *
           * @return This accessor's type constant, i.e. OpPointQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opPointQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpPointQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the system ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Filter on the OpPoint ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OpPointID_Type, uci::type::accessorType::opPointID_Type> OpPointID;

         /** Filter on the Category [Minimum occurrences: 0] [Maximum occurrences: 14] */
         typedef uci::base::BoundedList<uci::type::OpPointCategoryEnum, uci::type::accessorType::opPointCategoryEnum> Category;

         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::OpPointQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::OpPointQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::OpPointQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpPointID.
           *
           * @return The bounded list identified by OpPointID.
           */
         virtual const uci::type::OpPointQFT::OpPointID& getOpPointID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OpPointID.
           *
           * @return The bounded list identified by OpPointID.
           */
         virtual uci::type::OpPointQFT::OpPointID& getOpPointID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OpPointID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOpPointID(const uci::type::OpPointQFT::OpPointID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::OpPointQFT::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::OpPointQFT::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::OpPointQFT::Category& value)
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
         OpPointQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpPointQFT to copy from
           */
         OpPointQFT(const OpPointQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpPointQFT to the contents of the OpPointQFT on the right hand
           * side (rhs) of the assignment operator.OpPointQFT [only available to derived classes]
           *
           * @param rhs The OpPointQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::OpPointQFT
           * @return A constant reference to this OpPointQFT.
           */
         const OpPointQFT& operator=(const OpPointQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpPointQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpPointQFT_h

