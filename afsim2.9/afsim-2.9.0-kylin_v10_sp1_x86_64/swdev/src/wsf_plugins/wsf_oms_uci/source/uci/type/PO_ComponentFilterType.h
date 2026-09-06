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
#ifndef Uci__Type__PO_ComponentFilterType_h
#define Uci__Type__PO_ComponentFilterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FilterID_Type_h)
# include "uci/type/PO_FilterID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Type__PO_FilterEnum_h)
# include "uci/type/PO_FilterEnum.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ComponentControllabilityA_Type_h)
# include "uci/type/ComponentControllabilityA_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentFilterType sequence accessor class */
      class PO_ComponentFilterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentFilterType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentFilterType
           *
           * @return This accessor's type constant, i.e. PO_ComponentFilterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentFilterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentFilterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filters exist within the optical pathway that leads to one or more Focal Plane Arrays (FPAs). This field specifies
           * those FPAs that are associated with this filter, i.e. those FPAs that record light that passes through this filter.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_FPA_ID_Type, uci::type::accessorType::pO_FPA_ID_Type> FPA_ID;

         /** This field is used to specify a frequency band the filter allows to pass through. If not specified, then it is
           * assumed that the filter does not pass through light of any frequency (this is true of filters of type "shutter").
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> BandpassFrequency;

         /** Returns the accessor that provides access to the complex content that is identified by the FilterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterID.
           */
         virtual const uci::type::PO_FilterID_Type& getFilterID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FilterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterID.
           */
         virtual uci::type::PO_FilterID_Type& getFilterID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FilterID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FilterID
           */
         virtual void setFilterID(const uci::type::PO_FilterID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual const uci::type::PO_ComponentFilterType::FPA_ID& getFPA_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual uci::type::PO_ComponentFilterType::FPA_ID& getFPA_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FPA_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFPA_ID(const uci::type::PO_ComponentFilterType::FPA_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterType.
           *
           * @return The value of the enumeration identified by FilterType.
           */
         virtual const uci::type::PO_FilterEnum& getFilterType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterType.
           *
           * @return The value of the enumeration identified by FilterType.
           */
         virtual uci::type::PO_FilterEnum& getFilterType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterType(const uci::type::PO_FilterEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterType(uci::type::PO_FilterEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandpassFrequency.
           *
           * @return The bounded list identified by BandpassFrequency.
           */
         virtual const uci::type::PO_ComponentFilterType::BandpassFrequency& getBandpassFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandpassFrequency.
           *
           * @return The bounded list identified by BandpassFrequency.
           */
         virtual uci::type::PO_ComponentFilterType::BandpassFrequency& getBandpassFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BandpassFrequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBandpassFrequency(const uci::type::PO_ComponentFilterType::BandpassFrequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual const uci::type::ComponentControllabilityA_Type& getControllability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual uci::type::ComponentControllabilityA_Type& getControllability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Controllability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Controllability
           */
         virtual void setControllability(const uci::type::ComponentControllabilityA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Controllability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Controllability is emabled or not
           */
         virtual bool hasControllability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Controllability
           *
           * @param type = uci::type::accessorType::componentControllabilityA_Type This Accessor's accessor type
           */
         virtual void enableControllability(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControllabilityA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Controllability */
         virtual void clearControllability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentFilterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentFilterType to copy from
           */
         PO_ComponentFilterType(const PO_ComponentFilterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentFilterType to the contents of the
           * PO_ComponentFilterType on the right hand side (rhs) of the assignment operator.PO_ComponentFilterType [only available
           * to derived classes]
           *
           * @param rhs The PO_ComponentFilterType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PO_ComponentFilterType
           * @return A constant reference to this PO_ComponentFilterType.
           */
         const PO_ComponentFilterType& operator=(const PO_ComponentFilterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentFilterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentFilterType_h

