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
#ifndef Uci__Type__PO_ComponentFocalPlaneArrayOpticalBandType_h
#define Uci__Type__PO_ComponentFocalPlaneArrayOpticalBandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ComponentControlInterfacesEnum_h)
# include "uci/type/ComponentControlInterfacesEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentFocalPlaneArrayOpticalBandType sequence accessor class */
      class PO_ComponentFocalPlaneArrayOpticalBandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentFocalPlaneArrayOpticalBandType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentFocalPlaneArrayOpticalBandType
           *
           * @return This accessor's type constant, i.e. PO_ComponentFocalPlaneArrayOpticalBandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentFocalPlaneArrayOpticalBandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentFocalPlaneArrayOpticalBandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates whether and how the Optical band of the channel can be dynamically configured. Omission of this element
           * indicates the Optical band is not dynamically configurable. See annotation enumerations for further details. [Minimum
           * occurrences: 0] [Maximum occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::ComponentControlInterfacesEnum, uci::type::accessorType::componentControlInterfacesEnum> Configurable;

         /** Indicates whether and how the Optical band can be dynamically reset to it default/factory configuration. Omission of
           * this element indicates the Optical band cannot be dynamically reset. See annotation enumerations for further details.
           * [Minimum occurrences: 0] [Maximum occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::ComponentControlInterfacesEnum, uci::type::accessorType::componentControlInterfacesEnum> Reset;

         /** Returns the accessor that provides access to the complex content that is identified by the Band.
           *
           * @return The acecssor that provides access to the complex content that is identified by Band.
           */
         virtual const uci::type::FrequencyRangeType& getBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Band.
           *
           * @return The acecssor that provides access to the complex content that is identified by Band.
           */
         virtual uci::type::FrequencyRangeType& getBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Band to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Band
           */
         virtual void setBand(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Configurable.
           *
           * @return The bounded list identified by Configurable.
           */
         virtual const uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Configurable& getConfigurable() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Configurable.
           *
           * @return The bounded list identified by Configurable.
           */
         virtual uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Configurable& getConfigurable()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Configurable.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConfigurable(const uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Configurable& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Reset.
           *
           * @return The bounded list identified by Reset.
           */
         virtual const uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Reset& getReset() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Reset.
           *
           * @return The bounded list identified by Reset.
           */
         virtual uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Reset& getReset()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Reset.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReset(const uci::type::PO_ComponentFocalPlaneArrayOpticalBandType::Reset& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentFocalPlaneArrayOpticalBandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentFocalPlaneArrayOpticalBandType to copy from
           */
         PO_ComponentFocalPlaneArrayOpticalBandType(const PO_ComponentFocalPlaneArrayOpticalBandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentFocalPlaneArrayOpticalBandType to the contents of the
           * PO_ComponentFocalPlaneArrayOpticalBandType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentFocalPlaneArrayOpticalBandType [only available to derived classes]
           *
           * @param rhs The PO_ComponentFocalPlaneArrayOpticalBandType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::PO_ComponentFocalPlaneArrayOpticalBandType
           * @return A constant reference to this PO_ComponentFocalPlaneArrayOpticalBandType.
           */
         const PO_ComponentFocalPlaneArrayOpticalBandType& operator=(const PO_ComponentFocalPlaneArrayOpticalBandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentFocalPlaneArrayOpticalBandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentFocalPlaneArrayOpticalBandType_h

