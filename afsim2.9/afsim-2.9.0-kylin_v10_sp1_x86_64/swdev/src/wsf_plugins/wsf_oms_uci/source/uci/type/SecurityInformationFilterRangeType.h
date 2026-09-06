// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SecurityInformationFilterRangeType_h
#define Uci__Type__SecurityInformationFilterRangeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Specifies that the content being filtered has the provided security information */
      class SecurityInformationFilterRangeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SecurityInformationFilterRangeType()
         { }

         /** Returns this accessor's type constant, i.e. SecurityInformationFilterRangeType
           *
           * @return This accessor's type constant, i.e. SecurityInformationFilterRangeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::securityInformationFilterRangeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SecurityInformationFilterRangeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * HighestInclusiveInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by HighestInclusiveInformation.
           */
         virtual const uci::type::SecurityInformationType& getHighestInclusiveInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * HighestInclusiveInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by HighestInclusiveInformation.
           */
         virtual uci::type::SecurityInformationType& getHighestInclusiveInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the HighestInclusiveInformation to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by HighestInclusiveInformation
           */
         virtual void setHighestInclusiveInformation(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HighestInclusiveInformation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HighestInclusiveInformation is emabled or not
           */
         virtual bool hasHighestInclusiveInformation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HighestInclusiveInformation
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableHighestInclusiveInformation(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HighestInclusiveInformation */
         virtual void clearHighestInclusiveInformation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * LowestInclusiveInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by LowestInclusiveInformation.
           */
         virtual const uci::type::SecurityInformationType& getLowestInclusiveInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * LowestInclusiveInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by LowestInclusiveInformation.
           */
         virtual uci::type::SecurityInformationType& getLowestInclusiveInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LowestInclusiveInformation to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LowestInclusiveInformation
           */
         virtual void setLowestInclusiveInformation(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LowestInclusiveInformation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LowestInclusiveInformation is emabled or not
           */
         virtual bool hasLowestInclusiveInformation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LowestInclusiveInformation
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableLowestInclusiveInformation(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LowestInclusiveInformation */
         virtual void clearLowestInclusiveInformation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityInformationFilterRangeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityInformationFilterRangeType to copy from
           */
         SecurityInformationFilterRangeType(const SecurityInformationFilterRangeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityInformationFilterRangeType to the contents of the
           * SecurityInformationFilterRangeType on the right hand side (rhs) of the assignment
           * operator.SecurityInformationFilterRangeType [only available to derived classes]
           *
           * @param rhs The SecurityInformationFilterRangeType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SecurityInformationFilterRangeType
           * @return A constant reference to this SecurityInformationFilterRangeType.
           */
         const SecurityInformationFilterRangeType& operator=(const SecurityInformationFilterRangeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityInformationFilterRangeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SecurityInformationFilterRangeType_h

