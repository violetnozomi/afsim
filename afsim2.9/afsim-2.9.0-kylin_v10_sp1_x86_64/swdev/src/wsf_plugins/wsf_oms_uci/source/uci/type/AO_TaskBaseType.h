// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_TaskBaseType_h
#define Uci__Type__AO_TaskBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AO_CapabilityEnum_h)
# include "uci/type/AO_CapabilityEnum.h"
#endif

#if !defined(Uci__Type__AO_CodeType_h)
# include "uci/type/AO_CodeType.h"
#endif

#if !defined(Uci__Type__OpticalCollectionConstraintsType_h)
# include "uci/type/OpticalCollectionConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_TaskBaseType sequence accessor class */
      class AO_TaskBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AO_TaskBaseType()
         { }

         /** Returns this accessor's type constant, i.e. AO_TaskBaseType
           *
           * @return This accessor's type constant, i.e. AO_TaskBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_TaskBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_TaskBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::AO_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::AO_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::AO_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::AO_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedCode.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedCode.
           */
         virtual const uci::type::AO_CodeType& getSupportedCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedCode.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedCode.
           */
         virtual uci::type::AO_CodeType& getSupportedCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportedCode to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportedCode
           */
         virtual void setSupportedCode(const uci::type::AO_CodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportedCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportedCode is emabled or not
           */
         virtual bool hasSupportedCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportedCode
           *
           * @param type = uci::type::accessorType::aO_CodeType This Accessor's accessor type
           */
         virtual void enableSupportedCode(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_CodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportedCode */
         virtual void clearSupportedCode()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual const uci::type::OpticalCollectionConstraintsType& getEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual uci::type::OpticalCollectionConstraintsType& getEmissionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmissionConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmissionConstraints
           */
         virtual void setEmissionConstraints(const uci::type::OpticalCollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmissionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmissionConstraints is emabled or not
           */
         virtual bool hasEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmissionConstraints
           *
           * @param type = uci::type::accessorType::opticalCollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableEmissionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::opticalCollectionConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmissionConstraints */
         virtual void clearEmissionConstraints()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_TaskBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_TaskBaseType to copy from
           */
         AO_TaskBaseType(const AO_TaskBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_TaskBaseType to the contents of the AO_TaskBaseType on the
           * right hand side (rhs) of the assignment operator.AO_TaskBaseType [only available to derived classes]
           *
           * @param rhs The AO_TaskBaseType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AO_TaskBaseType
           * @return A constant reference to this AO_TaskBaseType.
           */
         const AO_TaskBaseType& operator=(const AO_TaskBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_TaskBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_TaskBaseType_h

