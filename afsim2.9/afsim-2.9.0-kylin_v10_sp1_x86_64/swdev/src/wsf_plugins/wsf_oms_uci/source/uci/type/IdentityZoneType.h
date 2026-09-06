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
#ifndef Uci__Type__IdentityZoneType_h
#define Uci__Type__IdentityZoneType_h 1

#if !defined(Uci__Type__IdentityType_h)
# include "uci/type/IdentityType.h"
#endif

#if !defined(Uci__Type__ZoneChoiceType_h)
# include "uci/type/ZoneChoiceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IdentityZoneType sequence accessor class */
      class IdentityZoneType : public virtual uci::type::IdentityType {
      public:

         /** The destructor */
         virtual ~IdentityZoneType()
         { }

         /** Returns this accessor's type constant, i.e. IdentityZoneType
           *
           * @return This accessor's type constant, i.e. IdentityZoneType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::identityZoneType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IdentityZoneType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual const uci::type::ZoneChoiceType& getZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual uci::type::ZoneChoiceType& getZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Zone to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Zone
           */
         virtual void setZone(const uci::type::ZoneChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Zone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Zone is emabled or not
           */
         virtual bool hasZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Zone
           *
           * @param type = uci::type::accessorType::zoneChoiceType This Accessor's accessor type
           */
         virtual void enableZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneChoiceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Zone */
         virtual void clearZone()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IdentityZoneType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IdentityZoneType to copy from
           */
         IdentityZoneType(const IdentityZoneType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IdentityZoneType to the contents of the IdentityZoneType on the
           * right hand side (rhs) of the assignment operator.IdentityZoneType [only available to derived classes]
           *
           * @param rhs The IdentityZoneType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::IdentityZoneType
           * @return A constant reference to this IdentityZoneType.
           */
         const IdentityZoneType& operator=(const IdentityZoneType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IdentityZoneType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IdentityZoneType_h

