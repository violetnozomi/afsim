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
#ifndef Uci__Type__ElectronicProtectionOptionsDefaultType_h
#define Uci__Type__ElectronicProtectionOptionsDefaultType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsEnableType_h)
# include "uci/type/ElectronicProtectionOptionsEnableType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ElectronicProtectionOptionsDefaultType sequence accessor class */
      class ElectronicProtectionOptionsDefaultType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ElectronicProtectionOptionsDefaultType()
         { }

         /** Returns this accessor's type constant, i.e. ElectronicProtectionOptionsDefaultType
           *
           * @return This accessor's type constant, i.e. ElectronicProtectionOptionsDefaultType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::electronicProtectionOptionsDefaultType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ElectronicProtectionOptionsDefaultType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which advertised EP options should be applied to all Activities associated with this Capability. This list
           * will completely replace any previously commanded list of EP options. A null list will indicate that the EP options
           * will be returned to default states which will be defined by the system. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsEnableType, uci::type::accessorType::electronicProtectionOptionsEnableType> ElectronicProtectionOptions;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @return The bounded list identified by ElectronicProtectionOptions.
           */
         virtual const uci::type::ElectronicProtectionOptionsDefaultType::ElectronicProtectionOptions& getElectronicProtectionOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @return The bounded list identified by ElectronicProtectionOptions.
           */
         virtual uci::type::ElectronicProtectionOptionsDefaultType::ElectronicProtectionOptions& getElectronicProtectionOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptions(const uci::type::ElectronicProtectionOptionsDefaultType::ElectronicProtectionOptions& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ElectronicProtectionOptionsDefaultType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ElectronicProtectionOptionsDefaultType to copy from
           */
         ElectronicProtectionOptionsDefaultType(const ElectronicProtectionOptionsDefaultType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ElectronicProtectionOptionsDefaultType to the contents of the
           * ElectronicProtectionOptionsDefaultType on the right hand side (rhs) of the assignment
           * operator.ElectronicProtectionOptionsDefaultType [only available to derived classes]
           *
           * @param rhs The ElectronicProtectionOptionsDefaultType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ElectronicProtectionOptionsDefaultType
           * @return A constant reference to this ElectronicProtectionOptionsDefaultType.
           */
         const ElectronicProtectionOptionsDefaultType& operator=(const ElectronicProtectionOptionsDefaultType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ElectronicProtectionOptionsDefaultType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ElectronicProtectionOptionsDefaultType_h

