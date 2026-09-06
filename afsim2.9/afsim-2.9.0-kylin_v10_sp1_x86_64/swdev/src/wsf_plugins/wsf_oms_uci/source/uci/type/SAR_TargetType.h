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
#ifndef Uci__Type__SAR_TargetType_h
#define Uci__Type__SAR_TargetType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_TargetType sequence accessor class */
      class SAR_TargetType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_TargetType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_TargetType
           *
           * @return This accessor's type constant, i.e. SAR_TargetType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_TargetType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_TargetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::TargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::TargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MultiLook.
           *
           * @return The value of the simple primitive data type identified by MultiLook.
           */
         virtual xs::Boolean getMultiLook() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MultiLook.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMultiLook(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MultiLook exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MultiLook is emabled or not
           */
         virtual bool hasMultiLook() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MultiLook
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableMultiLook(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MultiLook */
         virtual void clearMultiLook()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_TargetType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_TargetType to copy from
           */
         SAR_TargetType(const SAR_TargetType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_TargetType to the contents of the SAR_TargetType on the right
           * hand side (rhs) of the assignment operator.SAR_TargetType [only available to derived classes]
           *
           * @param rhs The SAR_TargetType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::SAR_TargetType
           * @return A constant reference to this SAR_TargetType.
           */
         const SAR_TargetType& operator=(const SAR_TargetType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_TargetType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_TargetType_h

