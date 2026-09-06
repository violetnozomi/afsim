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
#ifndef Uci__Type__ReconnaissanceStatusType_h
#define Uci__Type__ReconnaissanceStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RECCE_SystemEnum_h)
# include "uci/type/RECCE_SystemEnum.h"
#endif

#if !defined(Uci__Type__CapabilityAvailabilityEnum_h)
# include "uci/type/CapabilityAvailabilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ReconnaissanceStatusType sequence accessor class */
      class ReconnaissanceStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ReconnaissanceStatusType()
         { }

         /** Returns this accessor's type constant, i.e. ReconnaissanceStatusType
           *
           * @return This accessor's type constant, i.e. ReconnaissanceStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::reconnaissanceStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReconnaissanceStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemType.
           *
           * @return The value of the enumeration identified by SystemType.
           */
         virtual const uci::type::RECCE_SystemEnum& getSystemType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemType.
           *
           * @return The value of the enumeration identified by SystemType.
           */
         virtual uci::type::RECCE_SystemEnum& getSystemType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemType(const uci::type::RECCE_SystemEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemType(uci::type::RECCE_SystemEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemStatus.
           *
           * @return The value of the enumeration identified by SystemStatus.
           */
         virtual const uci::type::CapabilityAvailabilityEnum& getSystemStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SystemStatus.
           *
           * @return The value of the enumeration identified by SystemStatus.
           */
         virtual uci::type::CapabilityAvailabilityEnum& getSystemStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemStatus(const uci::type::CapabilityAvailabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SystemStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSystemStatus(uci::type::CapabilityAvailabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReconnaissanceStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReconnaissanceStatusType to copy from
           */
         ReconnaissanceStatusType(const ReconnaissanceStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReconnaissanceStatusType to the contents of the
           * ReconnaissanceStatusType on the right hand side (rhs) of the assignment operator.ReconnaissanceStatusType [only
           * available to derived classes]
           *
           * @param rhs The ReconnaissanceStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ReconnaissanceStatusType
           * @return A constant reference to this ReconnaissanceStatusType.
           */
         const ReconnaissanceStatusType& operator=(const ReconnaissanceStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReconnaissanceStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReconnaissanceStatusType_h

