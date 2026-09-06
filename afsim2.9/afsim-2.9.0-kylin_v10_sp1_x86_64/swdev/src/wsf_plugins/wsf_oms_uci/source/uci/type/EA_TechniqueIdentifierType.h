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
#ifndef Uci__Type__EA_TechniqueIdentifierType_h
#define Uci__Type__EA_TechniqueIdentifierType_h 1

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates an ID corresponding to an EA technique. The use of ForeignKeyType is an indication that EA techniques
        * aren't universally defined and come from multiple systems/sources but they are expected to be unique within the
        * context of their source. For an EA technique in UCI, the EA_TechniqueIdentifierType.Key element indicates the
        * technique ID and the EA_TechniqueIdentifierType.SystemName element indicates the system that assigned the ID and
        * ensures it is unique.
        */
      class EA_TechniqueIdentifierType : public virtual uci::type::ForeignKeyType {
      public:

         /** The destructor */
         virtual ~EA_TechniqueIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TechniqueIdentifierType
           *
           * @return This accessor's type constant, i.e. EA_TechniqueIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TechniqueIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TechniqueIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TechniqueIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TechniqueIdentifierType to copy from
           */
         EA_TechniqueIdentifierType(const EA_TechniqueIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TechniqueIdentifierType to the contents of the
           * EA_TechniqueIdentifierType on the right hand side (rhs) of the assignment operator.EA_TechniqueIdentifierType [only
           * available to derived classes]
           *
           * @param rhs The EA_TechniqueIdentifierType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EA_TechniqueIdentifierType
           * @return A constant reference to this EA_TechniqueIdentifierType.
           */
         const EA_TechniqueIdentifierType& operator=(const EA_TechniqueIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TechniqueIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TechniqueIdentifierType_h

