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
#ifndef Uci__Type__StrikeActivityCommandType_h
#define Uci__Type__StrikeActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__StrikeWeaponCommandType_h)
# include "uci/type/StrikeWeaponCommandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StrikeActivityCommandType sequence accessor class */
      class StrikeActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~StrikeActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. StrikeActivityCommandType
           *
           * @return This accessor's type constant, i.e. StrikeActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strikeActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrikeActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandType.
           */
         virtual const uci::type::StrikeWeaponCommandType& getCommandType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandType.
           */
         virtual uci::type::StrikeWeaponCommandType& getCommandType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandType
           */
         virtual void setCommandType(const uci::type::StrikeWeaponCommandType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeActivityCommandType to copy from
           */
         StrikeActivityCommandType(const StrikeActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeActivityCommandType to the contents of the
           * StrikeActivityCommandType on the right hand side (rhs) of the assignment operator.StrikeActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The StrikeActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StrikeActivityCommandType
           * @return A constant reference to this StrikeActivityCommandType.
           */
         const StrikeActivityCommandType& operator=(const StrikeActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrikeActivityCommandType_h

