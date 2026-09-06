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
#ifndef Uci__Type__TargetMobilityAssessmentDataType_h
#define Uci__Type__TargetMobilityAssessmentDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__TimedZoneType_h)
# include "uci/type/TimedZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines the possible area a system could have traveled. */
      class TargetMobilityAssessmentDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TargetMobilityAssessmentDataType()
         { }

         /** Returns this accessor's type constant, i.e. TargetMobilityAssessmentDataType
           *
           * @return This accessor's type constant, i.e. TargetMobilityAssessmentDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::targetMobilityAssessmentDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TargetMobilityAssessmentDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MobilityZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by MobilityZone.
           */
         virtual const uci::type::TimedZoneType& getMobilityZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MobilityZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by MobilityZone.
           */
         virtual uci::type::TimedZoneType& getMobilityZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MobilityZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MobilityZone
           */
         virtual void setMobilityZone(const uci::type::TimedZoneType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TargetMobilityAssessmentDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TargetMobilityAssessmentDataType to copy from
           */
         TargetMobilityAssessmentDataType(const TargetMobilityAssessmentDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TargetMobilityAssessmentDataType to the contents of the
           * TargetMobilityAssessmentDataType on the right hand side (rhs) of the assignment
           * operator.TargetMobilityAssessmentDataType [only available to derived classes]
           *
           * @param rhs The TargetMobilityAssessmentDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::TargetMobilityAssessmentDataType
           * @return A constant reference to this TargetMobilityAssessmentDataType.
           */
         const TargetMobilityAssessmentDataType& operator=(const TargetMobilityAssessmentDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TargetMobilityAssessmentDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TargetMobilityAssessmentDataType_h

