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
#ifndef Uci__Type__PositionReportDetailedMDT_h
#define Uci__Type__PositionReportDetailedMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__NavigationSolutionStateEnum_h)
# include "uci/type/NavigationSolutionStateEnum.h"
#endif

#if !defined(Uci__Type__NAV_FigureOfMeritType_h)
# include "uci/type/NAV_FigureOfMeritType.h"
#endif

#if !defined(Uci__Type__DetailedKinematicsType_h)
# include "uci/type/DetailedKinematicsType.h"
#endif

#if !defined(Uci__Type__DetailedKinematicsErrorType_h)
# include "uci/type/DetailedKinematicsErrorType.h"
#endif

#if !defined(Uci__Type__NavigationSolutionCorrectionsType_h)
# include "uci/type/NavigationSolutionCorrectionsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PositionReportDetailedMDT sequence accessor class */
      class PositionReportDetailedMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PositionReportDetailedMDT()
         { }

         /** Returns this accessor's type constant, i.e. PositionReportDetailedMDT
           *
           * @return This accessor's type constant, i.e. PositionReportDetailedMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::positionReportDetailedMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PositionReportDetailedMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ComponentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ComponentID is emabled or not
           */
         virtual bool hasComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ComponentID
           *
           * @param type = uci::type::accessorType::componentID_Type This Accessor's accessor type
           */
         virtual void enableComponentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ComponentID */
         virtual void clearComponentID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NavigationSolutionState.
           *
           * @return The value of the enumeration identified by NavigationSolutionState.
           */
         virtual const uci::type::NavigationSolutionStateEnum& getNavigationSolutionState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NavigationSolutionState.
           *
           * @return The value of the enumeration identified by NavigationSolutionState.
           */
         virtual uci::type::NavigationSolutionStateEnum& getNavigationSolutionState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NavigationSolutionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNavigationSolutionState(const uci::type::NavigationSolutionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NavigationSolutionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNavigationSolutionState(uci::type::NavigationSolutionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FigureOfMerit.
           *
           * @return The value of the simple primitive data type identified by FigureOfMerit.
           */
         virtual uci::type::NAV_FigureOfMeritTypeValue getFigureOfMerit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FigureOfMerit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFigureOfMerit(uci::type::NAV_FigureOfMeritTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FigureOfMerit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FigureOfMerit is emabled or not
           */
         virtual bool hasFigureOfMerit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FigureOfMerit
           *
           * @param type = uci::type::accessorType::nAV_FigureOfMeritType This Accessor's accessor type
           */
         virtual void enableFigureOfMerit(uci::base::accessorType::AccessorType type = uci::type::accessorType::nAV_FigureOfMeritType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FigureOfMerit */
         virtual void clearFigureOfMerit()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kinematics.
           */
         virtual const uci::type::DetailedKinematicsType& getKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kinematics.
           */
         virtual uci::type::DetailedKinematicsType& getKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Kinematics to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Kinematics
           */
         virtual void setKinematics(const uci::type::DetailedKinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KinematicsError.
           *
           * @return The acecssor that provides access to the complex content that is identified by KinematicsError.
           */
         virtual const uci::type::DetailedKinematicsErrorType& getKinematicsError() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KinematicsError.
           *
           * @return The acecssor that provides access to the complex content that is identified by KinematicsError.
           */
         virtual uci::type::DetailedKinematicsErrorType& getKinematicsError()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the KinematicsError to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by KinematicsError
           */
         virtual void setKinematicsError(const uci::type::DetailedKinematicsErrorType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SolutionCorrections.
           *
           * @return The acecssor that provides access to the complex content that is identified by SolutionCorrections.
           */
         virtual const uci::type::NavigationSolutionCorrectionsType& getSolutionCorrections() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SolutionCorrections.
           *
           * @return The acecssor that provides access to the complex content that is identified by SolutionCorrections.
           */
         virtual uci::type::NavigationSolutionCorrectionsType& getSolutionCorrections()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SolutionCorrections to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SolutionCorrections
           */
         virtual void setSolutionCorrections(const uci::type::NavigationSolutionCorrectionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SolutionCorrections exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SolutionCorrections is emabled or not
           */
         virtual bool hasSolutionCorrections() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SolutionCorrections
           *
           * @param type = uci::type::accessorType::navigationSolutionCorrectionsType This Accessor's accessor type
           */
         virtual void enableSolutionCorrections(uci::base::accessorType::AccessorType type = uci::type::accessorType::navigationSolutionCorrectionsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SolutionCorrections */
         virtual void clearSolutionCorrections()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PositionReportDetailedMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PositionReportDetailedMDT to copy from
           */
         PositionReportDetailedMDT(const PositionReportDetailedMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PositionReportDetailedMDT to the contents of the
           * PositionReportDetailedMDT on the right hand side (rhs) of the assignment operator.PositionReportDetailedMDT [only
           * available to derived classes]
           *
           * @param rhs The PositionReportDetailedMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PositionReportDetailedMDT
           * @return A constant reference to this PositionReportDetailedMDT.
           */
         const PositionReportDetailedMDT& operator=(const PositionReportDetailedMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PositionReportDetailedMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PositionReportDetailedMDT_h

