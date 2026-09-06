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
#ifndef Uci__Type__EnduranceType_h
#define Uci__Type__EnduranceType_h 1

#if !defined(Uci__Type__EnduranceBaseType_h)
# include "uci/type/EnduranceBaseType.h"
#endif

#if !defined(Uci__Type__EnduranceFootprintType_h)
# include "uci/type/EnduranceFootprintType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates endurance in terms of maximum ground/surface distance that can be reached given the current system state. */
      class EnduranceType : public virtual uci::type::EnduranceBaseType {
      public:

         /** The destructor */
         virtual ~EnduranceType()
         { }

         /** Returns this accessor's type constant, i.e. EnduranceType
           *
           * @return This accessor's type constant, i.e. EnduranceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::enduranceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EnduranceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Footprint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Footprint.
           */
         virtual const uci::type::EnduranceFootprintType& getFootprint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Footprint.
           *
           * @return The acecssor that provides access to the complex content that is identified by Footprint.
           */
         virtual uci::type::EnduranceFootprintType& getFootprint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Footprint to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Footprint
           */
         virtual void setFootprint(const uci::type::EnduranceFootprintType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Footprint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Footprint is emabled or not
           */
         virtual bool hasFootprint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Footprint
           *
           * @param type = uci::type::accessorType::enduranceFootprintType This Accessor's accessor type
           */
         virtual void enableFootprint(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceFootprintType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Footprint */
         virtual void clearFootprint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EnduranceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EnduranceType to copy from
           */
         EnduranceType(const EnduranceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EnduranceType to the contents of the EnduranceType on the right
           * hand side (rhs) of the assignment operator.EnduranceType [only available to derived classes]
           *
           * @param rhs The EnduranceType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EnduranceType
           * @return A constant reference to this EnduranceType.
           */
         const EnduranceType& operator=(const EnduranceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EnduranceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EnduranceType_h

