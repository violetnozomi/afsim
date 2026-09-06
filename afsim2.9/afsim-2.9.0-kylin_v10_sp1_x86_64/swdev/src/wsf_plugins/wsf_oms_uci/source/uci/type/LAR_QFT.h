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
#ifndef Uci__Type__LAR_QFT_h
#define Uci__Type__LAR_QFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__LAR_ID_Type_h)
# include "uci/type/LAR_ID_Type.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_QFT sequence accessor class */
      class LAR_QFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~LAR_QFT()
         { }

         /** Returns this accessor's type constant, i.e. LAR_QFT
           *
           * @return This accessor's type constant, i.e. LAR_QFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_QFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_QFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the LARs ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::LAR_ID_Type, uci::type::accessorType::lAR_ID_Type> LAR_ID;

         /** Returns the bounded list that is identified by the LAR_ID.
           *
           * @return The bounded list identified by LAR_ID.
           */
         virtual const uci::type::LAR_QFT::LAR_ID& getLAR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LAR_ID.
           *
           * @return The bounded list identified by LAR_ID.
           */
         virtual uci::type::LAR_QFT::LAR_ID& getLAR_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LAR_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLAR_ID(const uci::type::LAR_QFT::LAR_ID& value)
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
         LAR_QFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_QFT to copy from
           */
         LAR_QFT(const LAR_QFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_QFT to the contents of the LAR_QFT on the right hand side
           * (rhs) of the assignment operator.LAR_QFT [only available to derived classes]
           *
           * @param rhs The LAR_QFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::LAR_QFT
           * @return A constant reference to this LAR_QFT.
           */
         const LAR_QFT& operator=(const LAR_QFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_QFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_QFT_h

