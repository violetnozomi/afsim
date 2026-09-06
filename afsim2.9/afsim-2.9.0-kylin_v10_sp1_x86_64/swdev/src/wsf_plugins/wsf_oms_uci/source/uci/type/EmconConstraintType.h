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
#ifndef Uci__Type__EmconConstraintType_h
#define Uci__Type__EmconConstraintType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EmconOverrideType_h)
# include "uci/type/EmconOverrideType.h"
#endif

#if !defined(Uci__Type__EmconERP_Type_h)
# include "uci/type/EmconERP_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EmconConstraintType sequence accessor class */
      class EmconConstraintType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EmconConstraintType()
         { }

         /** Returns this accessor's type constant, i.e. EmconConstraintType
           *
           * @return This accessor's type constant, i.e. EmconConstraintType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::emconConstraintType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EmconConstraintType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultEmconOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultEmconOverride.
           */
         virtual const uci::type::EmconOverrideType& getDefaultEmconOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultEmconOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultEmconOverride.
           */
         virtual uci::type::EmconOverrideType& getDefaultEmconOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultEmconOverride to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultEmconOverride
           */
         virtual void setDefaultEmconOverride(const uci::type::EmconOverrideType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultEmconOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultEmconOverride is emabled or not
           */
         virtual bool hasDefaultEmconOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultEmconOverride
           *
           * @param type = uci::type::accessorType::emconOverrideType This Accessor's accessor type
           */
         virtual void enableDefaultEmconOverride(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconOverrideType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultEmconOverride */
         virtual void clearDefaultEmconOverride()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ERP_Selection.
           *
           * @return The acecssor that provides access to the complex content that is identified by ERP_Selection.
           */
         virtual const uci::type::EmconERP_Type& getERP_Selection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ERP_Selection.
           *
           * @return The acecssor that provides access to the complex content that is identified by ERP_Selection.
           */
         virtual uci::type::EmconERP_Type& getERP_Selection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ERP_Selection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ERP_Selection
           */
         virtual void setERP_Selection(const uci::type::EmconERP_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ERP_Selection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ERP_Selection is emabled or not
           */
         virtual bool hasERP_Selection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ERP_Selection
           *
           * @param type = uci::type::accessorType::emconERP_Type This Accessor's accessor type
           */
         virtual void enableERP_Selection(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconERP_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ERP_Selection */
         virtual void clearERP_Selection()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EmconConstraintType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EmconConstraintType to copy from
           */
         EmconConstraintType(const EmconConstraintType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EmconConstraintType to the contents of the EmconConstraintType on
           * the right hand side (rhs) of the assignment operator.EmconConstraintType [only available to derived classes]
           *
           * @param rhs The EmconConstraintType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EmconConstraintType
           * @return A constant reference to this EmconConstraintType.
           */
         const EmconConstraintType& operator=(const EmconConstraintType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EmconConstraintType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EmconConstraintType_h

