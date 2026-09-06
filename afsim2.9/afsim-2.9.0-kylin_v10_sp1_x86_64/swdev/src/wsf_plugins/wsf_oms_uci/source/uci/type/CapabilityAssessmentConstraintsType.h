// This file was generated  on 10/18/2017 at 5:35:26 AM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* DISTRIBUTION STATEMENT D.  Distribution authorized to the Department 
*    of Defense and U.S. DoD contractors only; Critical Technology; 26 Mar 13.
*    Other requests shall be referred to the Air Force Rapid Capabilities 
*    Office, 202-404-5497 or 202-404-5498.
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
#ifndef Uci__Type__CapabilityAssessmentConstraintsType_h
#define Uci__Type__CapabilityAssessmentConstraintsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TimeWindowType_h)
# include "uci/type/TimeWindowType.h"
#endif

#if !defined(Uci__Type__RouteDependenciesType_h)
# include "uci/type/RouteDependenciesType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityAssessmentConstraintsType sequence accessor class */
      class CapabilityAssessmentConstraintsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityAssessmentConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityAssessmentConstraintsType
           *
           * @return This accessor's type constant, i.e. CapabilityAssessmentConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityAssessmentConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityAssessmentConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentWindow.
           */
         virtual const uci::type::TimeWindowType& getAssessmentWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentWindow.
           */
         virtual uci::type::TimeWindowType& getAssessmentWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssessmentWindow to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssessmentWindow
           */
         virtual void setAssessmentWindow(const uci::type::TimeWindowType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssessmentWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssessmentWindow is emabled or not
           */
         virtual bool hasAssessmentWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssessmentWindow
           *
           * @param type = uci::type::accessorType::timeWindowType This Accessor's accessor type
           */
         virtual void enableAssessmentWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::timeWindowType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssessmentWindow */
         virtual void clearAssessmentWindow()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteDependencies.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteDependencies.
           */
         virtual const uci::type::RouteDependenciesType& getRouteDependencies() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteDependencies.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteDependencies.
           */
         virtual uci::type::RouteDependenciesType& getRouteDependencies()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteDependencies to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteDependencies
           */
         virtual void setRouteDependencies(const uci::type::RouteDependenciesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RouteDependencies exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RouteDependencies is emabled or not
           */
         virtual bool hasRouteDependencies() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RouteDependencies
           *
           * @param type = uci::type::accessorType::routeDependenciesType This Accessor's accessor type
           */
         virtual void enableRouteDependencies(uci::base::accessorType::AccessorType type = uci::type::accessorType::routeDependenciesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RouteDependencies */
         virtual void clearRouteDependencies()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityAssessmentConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentConstraintsType to copy from
           */
         CapabilityAssessmentConstraintsType(const CapabilityAssessmentConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityAssessmentConstraintsType to the contents of the
           * CapabilityAssessmentConstraintsType on the right hand side (rhs) of the assignment
           * operator.CapabilityAssessmentConstraintsType [only available to derived classes]
           *
           * @param rhs The CapabilityAssessmentConstraintsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityAssessmentConstraintsType
           * @return A constant reference to this CapabilityAssessmentConstraintsType.
           */
         const CapabilityAssessmentConstraintsType& operator=(const CapabilityAssessmentConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityAssessmentConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityAssessmentConstraintsType_h

