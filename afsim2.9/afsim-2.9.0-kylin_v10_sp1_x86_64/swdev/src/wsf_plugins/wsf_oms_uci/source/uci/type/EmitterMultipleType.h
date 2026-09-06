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
#ifndef Uci__Type__EmitterMultipleType_h
#define Uci__Type__EmitterMultipleType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EmitterIdentityConfidenceType_h)
# include "uci/type/EmitterIdentityConfidenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EmitterMultipleType sequence accessor class */
      class EmitterMultipleType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EmitterMultipleType()
         { }

         /** Returns this accessor's type constant, i.e. EmitterMultipleType
           *
           * @return This accessor's type constant, i.e. EmitterMultipleType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::emitterMultipleType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EmitterMultipleType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a possible emitter type that has been identified/observed for the parent Emitter. The value of the child
           * Confidence element of all instances of this element should sum to 100%. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterIdentityConfidenceType, uci::type::accessorType::emitterIdentityConfidenceType> EmitterType;

         /** Returns the bounded list that is identified by the EmitterType.
           *
           * @return The bounded list identified by EmitterType.
           */
         virtual const uci::type::EmitterMultipleType::EmitterType& getEmitterType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EmitterType.
           *
           * @return The bounded list identified by EmitterType.
           */
         virtual uci::type::EmitterMultipleType::EmitterType& getEmitterType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EmitterType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitterType(const uci::type::EmitterMultipleType::EmitterType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EmitterMultipleType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EmitterMultipleType to copy from
           */
         EmitterMultipleType(const EmitterMultipleType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EmitterMultipleType to the contents of the EmitterMultipleType on
           * the right hand side (rhs) of the assignment operator.EmitterMultipleType [only available to derived classes]
           *
           * @param rhs The EmitterMultipleType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EmitterMultipleType
           * @return A constant reference to this EmitterMultipleType.
           */
         const EmitterMultipleType& operator=(const EmitterMultipleType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EmitterMultipleType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EmitterMultipleType_h

