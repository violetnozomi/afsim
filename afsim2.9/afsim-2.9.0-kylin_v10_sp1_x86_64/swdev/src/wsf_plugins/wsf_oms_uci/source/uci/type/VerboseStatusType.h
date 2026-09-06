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
#ifndef Uci__Type__VerboseStatusType_h
#define Uci__Type__VerboseStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the VerboseStatusType sequence accessor class */
      class VerboseStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VerboseStatusType()
         { }

         /** Returns this accessor's type constant, i.e. VerboseStatusType
           *
           * @return This accessor's type constant, i.e. VerboseStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::verboseStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VerboseStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Verbose.
           *
           * @return The value of the simple primitive data type identified by Verbose.
           */
         virtual xs::Boolean getVerbose() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Verbose.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVerbose(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VerboseRate.
           *
           * @return The value of the simple primitive data type identified by VerboseRate.
           */
         virtual xs::Float getVerboseRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VerboseRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVerboseRate(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VerboseRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VerboseRate is emabled or not
           */
         virtual bool hasVerboseRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VerboseRate
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableVerboseRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VerboseRate */
         virtual void clearVerboseRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VerboseStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VerboseStatusType to copy from
           */
         VerboseStatusType(const VerboseStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VerboseStatusType to the contents of the VerboseStatusType on the
           * right hand side (rhs) of the assignment operator.VerboseStatusType [only available to derived classes]
           *
           * @param rhs The VerboseStatusType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::VerboseStatusType
           * @return A constant reference to this VerboseStatusType.
           */
         const VerboseStatusType& operator=(const VerboseStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VerboseStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VerboseStatusType_h

