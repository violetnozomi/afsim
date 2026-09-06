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
#ifndef Uci__Type__OpLineType_h
#define Uci__Type__OpLineType_h 1

#if !defined(Uci__Type__LineType_h)
# include "uci/type/LineType.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpLineType sequence accessor class */
      class OpLineType : public virtual uci::type::LineType {
      public:

         /** The destructor */
         virtual ~OpLineType()
         { }

         /** Returns this accessor's type constant, i.e. OpLineType
           *
           * @return This accessor's type constant, i.e. OpLineType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opLineType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpLineType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual const uci::type::Velocity2D_Type& getVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Velocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Velocity.
           */
         virtual uci::type::Velocity2D_Type& getVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Velocity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Velocity
           */
         virtual void setVelocity(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Velocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Velocity is emabled or not
           */
         virtual bool hasVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Velocity
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Velocity */
         virtual void clearVelocity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpLineType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpLineType to copy from
           */
         OpLineType(const OpLineType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpLineType to the contents of the OpLineType on the right hand
           * side (rhs) of the assignment operator.OpLineType [only available to derived classes]
           *
           * @param rhs The OpLineType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::OpLineType
           * @return A constant reference to this OpLineType.
           */
         const OpLineType& operator=(const OpLineType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpLineType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpLineType_h

