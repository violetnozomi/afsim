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
#ifndef Uci__Type__EntityPositionType_h
#define Uci__Type__EntityPositionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FixedPositionType_h)
# include "uci/type/FixedPositionType.h"
#endif

#if !defined(Uci__Type__RelativePositionType_h)
# include "uci/type/RelativePositionType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityPositionType sequence accessor class */
      class EntityPositionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityPositionType()
         { }

         /** Returns this accessor's type constant, i.e. EntityPositionType
           *
           * @return This accessor's type constant, i.e. EntityPositionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityPositionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityPositionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FixedPositionType.
           *
           * @return The acecssor that provides access to the complex content that is identified by FixedPositionType.
           */
         virtual const uci::type::FixedPositionType& getFixedPositionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FixedPositionType.
           *
           * @return The acecssor that provides access to the complex content that is identified by FixedPositionType.
           */
         virtual uci::type::FixedPositionType& getFixedPositionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FixedPositionType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FixedPositionType
           */
         virtual void setFixedPositionType(const uci::type::FixedPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FixedPositionType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FixedPositionType is emabled or not
           */
         virtual bool hasFixedPositionType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FixedPositionType
           *
           * @param type = uci::type::accessorType::fixedPositionType This Accessor's accessor type
           */
         virtual void enableFixedPositionType(uci::base::accessorType::AccessorType type = uci::type::accessorType::fixedPositionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FixedPositionType */
         virtual void clearFixedPositionType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePosition.
           */
         virtual const uci::type::RelativePositionType& getRelativePosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativePosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativePosition.
           */
         virtual uci::type::RelativePositionType& getRelativePosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelativePosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelativePosition
           */
         virtual void setRelativePosition(const uci::type::RelativePositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RelativePosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RelativePosition is emabled or not
           */
         virtual bool hasRelativePosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RelativePosition
           *
           * @param type = uci::type::accessorType::relativePositionType This Accessor's accessor type
           */
         virtual void enableRelativePosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::relativePositionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RelativePosition */
         virtual void clearRelativePosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual const uci::type::ZoneType& getZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Zone.
           *
           * @return The acecssor that provides access to the complex content that is identified by Zone.
           */
         virtual uci::type::ZoneType& getZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Zone to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Zone
           */
         virtual void setZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Zone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Zone is emabled or not
           */
         virtual bool hasZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Zone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Zone */
         virtual void clearZone()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityPositionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityPositionType to copy from
           */
         EntityPositionType(const EntityPositionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityPositionType to the contents of the EntityPositionType on
           * the right hand side (rhs) of the assignment operator.EntityPositionType [only available to derived classes]
           *
           * @param rhs The EntityPositionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EntityPositionType
           * @return A constant reference to this EntityPositionType.
           */
         const EntityPositionType& operator=(const EntityPositionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityPositionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityPositionType_h

