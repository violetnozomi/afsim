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
#ifndef Uci__Type__FusionSourceType_h
#define Uci__Type__FusionSourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntitySourceEnum_h)
# include "uci/type/EntitySourceEnum.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__FusionUpdatesEnum_h)
# include "uci/type/FusionUpdatesEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FusionSourceType sequence accessor class */
      class FusionSourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FusionSourceType()
         { }

         /** Returns this accessor's type constant, i.e. FusionSourceType
           *
           * @return This accessor's type constant, i.e. FusionSourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fusionSourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FusionSourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SourceType.
           *
           * @return The value of the enumeration identified by SourceType.
           */
         virtual const uci::type::EntitySourceEnum& getSourceType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SourceType.
           *
           * @return The value of the enumeration identified by SourceType.
           */
         virtual uci::type::EntitySourceEnum& getSourceType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SourceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSourceType(const uci::type::EntitySourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SourceType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSourceType(uci::type::EntitySourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceSystemID.
           */
         virtual const uci::type::SystemID_Type& getSourceSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceSystemID.
           */
         virtual uci::type::SystemID_Type& getSourceSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceSystemID
           */
         virtual void setSourceSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceSystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceSystemID is emabled or not
           */
         virtual bool hasSourceSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceSystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSourceSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceSystemID */
         virtual void clearSourceSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Enabled.
           *
           * @return The value of the simple primitive data type identified by Enabled.
           */
         virtual xs::Boolean getEnabled() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Enabled.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEnabled(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AllowedUpdates.
           *
           * @return The value of the enumeration identified by AllowedUpdates.
           */
         virtual const uci::type::FusionUpdatesEnum& getAllowedUpdates() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AllowedUpdates.
           *
           * @return The value of the enumeration identified by AllowedUpdates.
           */
         virtual uci::type::FusionUpdatesEnum& getAllowedUpdates()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AllowedUpdates.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAllowedUpdates(const uci::type::FusionUpdatesEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AllowedUpdates.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAllowedUpdates(uci::type::FusionUpdatesEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InitiateAllowed.
           *
           * @return The value of the simple primitive data type identified by InitiateAllowed.
           */
         virtual xs::Boolean getInitiateAllowed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InitiateAllowed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInitiateAllowed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FusionSourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FusionSourceType to copy from
           */
         FusionSourceType(const FusionSourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FusionSourceType to the contents of the FusionSourceType on the
           * right hand side (rhs) of the assignment operator.FusionSourceType [only available to derived classes]
           *
           * @param rhs The FusionSourceType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::FusionSourceType
           * @return A constant reference to this FusionSourceType.
           */
         const FusionSourceType& operator=(const FusionSourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FusionSourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FusionSourceType_h

