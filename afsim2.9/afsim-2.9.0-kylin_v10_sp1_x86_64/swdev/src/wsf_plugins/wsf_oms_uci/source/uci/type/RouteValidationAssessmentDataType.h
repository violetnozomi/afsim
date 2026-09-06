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
#ifndef Uci__Type__RouteValidationAssessmentDataType_h
#define Uci__Type__RouteValidationAssessmentDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RouteValidationInvalidPathType_h)
# include "uci/type/RouteValidationInvalidPathType.h"
#endif

#if !defined(Uci__Type__LAR_MDT_h)
# include "uci/type/LAR_MDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines whether a route is flyable. */
      class RouteValidationAssessmentDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RouteValidationAssessmentDataType()
         { }

         /** Returns this accessor's type constant, i.e. RouteValidationAssessmentDataType
           *
           * @return This accessor's type constant, i.e. RouteValidationAssessmentDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routeValidationAssessmentDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RouteValidationAssessmentDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element defines any paths that have invalid segments in it. If there are none listed then the route was fully
           * valid. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RouteValidationInvalidPathType, uci::type::accessorType::routeValidationInvalidPathType> InvalidPath;

         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InvalidPath.
           *
           * @return The bounded list identified by InvalidPath.
           */
         virtual const uci::type::RouteValidationAssessmentDataType::InvalidPath& getInvalidPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InvalidPath.
           *
           * @return The bounded list identified by InvalidPath.
           */
         virtual uci::type::RouteValidationAssessmentDataType::InvalidPath& getInvalidPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InvalidPath.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInvalidPath(const uci::type::RouteValidationAssessmentDataType::InvalidPath& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ValidLAR.
           *
           * @return The acecssor that provides access to the complex content that is identified by ValidLAR.
           */
         virtual const uci::type::LAR_MDT& getValidLAR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ValidLAR.
           *
           * @return The acecssor that provides access to the complex content that is identified by ValidLAR.
           */
         virtual uci::type::LAR_MDT& getValidLAR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ValidLAR to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ValidLAR
           */
         virtual void setValidLAR(const uci::type::LAR_MDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ValidLAR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ValidLAR is emabled or not
           */
         virtual bool hasValidLAR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ValidLAR
           *
           * @param type = uci::type::accessorType::lAR_MDT This Accessor's accessor type
           */
         virtual void enableValidLAR(uci::base::accessorType::AccessorType type = uci::type::accessorType::lAR_MDT)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ValidLAR */
         virtual void clearValidLAR()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RouteValidationAssessmentDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RouteValidationAssessmentDataType to copy from
           */
         RouteValidationAssessmentDataType(const RouteValidationAssessmentDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RouteValidationAssessmentDataType to the contents of the
           * RouteValidationAssessmentDataType on the right hand side (rhs) of the assignment
           * operator.RouteValidationAssessmentDataType [only available to derived classes]
           *
           * @param rhs The RouteValidationAssessmentDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RouteValidationAssessmentDataType
           * @return A constant reference to this RouteValidationAssessmentDataType.
           */
         const RouteValidationAssessmentDataType& operator=(const RouteValidationAssessmentDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RouteValidationAssessmentDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RouteValidationAssessmentDataType_h

