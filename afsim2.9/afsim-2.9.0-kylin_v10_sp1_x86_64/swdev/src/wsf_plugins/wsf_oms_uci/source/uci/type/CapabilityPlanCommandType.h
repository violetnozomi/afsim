// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CapabilityPlanCommandType_h
#define Uci__Type__CapabilityPlanCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__SupportCapabilityCommandBaseType_h)
# include "uci/type/SupportCapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__SegmentAssociationType_h)
# include "uci/type/SegmentAssociationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityPlanCommandType sequence accessor class */
      class CapabilityPlanCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityPlanCommandType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityPlanCommandType
           *
           * @return This accessor's type constant, i.e. CapabilityPlanCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityPlanCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityPlanCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityCommand.
           */
         virtual const uci::type::CapabilityCommandBaseType& getCapabilityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityCommand.
           */
         virtual uci::type::CapabilityCommandBaseType& getCapabilityCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityCommand to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityCommand
           */
         virtual void setCapabilityCommand(const uci::type::CapabilityCommandBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityCommand exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityCommand is emabled or not
           */
         virtual bool hasCapabilityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityCommand
           *
           * @param type = uci::type::accessorType::capabilityCommandBaseType This Accessor's accessor type
           */
         virtual void enableCapabilityCommand(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityCommand */
         virtual void clearCapabilityCommand()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportingCapabilityCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportingCapabilityCommand.
           */
         virtual const uci::type::SupportCapabilityCommandBaseType& getSupportingCapabilityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportingCapabilityCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportingCapabilityCommand.
           */
         virtual uci::type::SupportCapabilityCommandBaseType& getSupportingCapabilityCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportingCapabilityCommand to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportingCapabilityCommand
           */
         virtual void setSupportingCapabilityCommand(const uci::type::SupportCapabilityCommandBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportingCapabilityCommand exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportingCapabilityCommand is emabled or not
           */
         virtual bool hasSupportingCapabilityCommand() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportingCapabilityCommand
           *
           * @param type = uci::type::accessorType::supportCapabilityCommandBaseType This Accessor's accessor type
           */
         virtual void enableSupportingCapabilityCommand(uci::base::accessorType::AccessorType type = uci::type::accessorType::supportCapabilityCommandBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportingCapabilityCommand */
         virtual void clearSupportingCapabilityCommand()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentAssociation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentAssociation.
           */
         virtual const uci::type::SegmentAssociationType& getSegmentAssociation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentAssociation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentAssociation.
           */
         virtual uci::type::SegmentAssociationType& getSegmentAssociation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SegmentAssociation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SegmentAssociation
           */
         virtual void setSegmentAssociation(const uci::type::SegmentAssociationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SegmentAssociation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SegmentAssociation is emabled or not
           */
         virtual bool hasSegmentAssociation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SegmentAssociation
           *
           * @param type = uci::type::accessorType::segmentAssociationType This Accessor's accessor type
           */
         virtual void enableSegmentAssociation(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentAssociationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SegmentAssociation */
         virtual void clearSegmentAssociation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityPlanCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityPlanCommandType to copy from
           */
         CapabilityPlanCommandType(const CapabilityPlanCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityPlanCommandType to the contents of the
           * CapabilityPlanCommandType on the right hand side (rhs) of the assignment operator.CapabilityPlanCommandType [only
           * available to derived classes]
           *
           * @param rhs The CapabilityPlanCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CapabilityPlanCommandType
           * @return A constant reference to this CapabilityPlanCommandType.
           */
         const CapabilityPlanCommandType& operator=(const CapabilityPlanCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityPlanCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityPlanCommandType_h

