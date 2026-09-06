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
#ifndef Uci__Type__CommAssetPositionReportMDT_h
#define Uci__Type__CommAssetPositionReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommAssetID_Type_h)
# include "uci/type/CommAssetID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__SystemSourceEnum_h)
# include "uci/type/SystemSourceEnum.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__InertialStateType_h)
# include "uci/type/InertialStateType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommAssetPositionReportMDT sequence accessor class */
      class CommAssetPositionReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommAssetPositionReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommAssetPositionReportMDT
           *
           * @return This accessor's type constant, i.e. CommAssetPositionReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commAssetPositionReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommAssetPositionReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAssetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAssetID.
           */
         virtual const uci::type::CommAssetID_Type& getCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAssetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAssetID.
           */
         virtual uci::type::CommAssetID_Type& getCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommAssetID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommAssetID
           */
         virtual void setCommAssetID(const uci::type::CommAssetID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DisplayName.
           *
           * @return The value of the string data type identified by DisplayName.
           */
         virtual const uci::type::VisibleString256Type& getDisplayName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DisplayName.
           *
           * @return The value of the string data type identified by DisplayName.
           */
         virtual uci::type::VisibleString256Type& getDisplayName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DisplayName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDisplayName(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DisplayName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDisplayName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DisplayName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDisplayName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DisplayName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DisplayName is emabled or not
           */
         virtual bool hasDisplayName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DisplayName
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableDisplayName(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DisplayName */
         virtual void clearDisplayName()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual const uci::type::SystemSourceEnum& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual uci::type::SystemSourceEnum& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(const uci::type::SystemSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(uci::type::SystemSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CurrentOperatingDomain.
           *
           * @return The value of the enumeration identified by CurrentOperatingDomain.
           */
         virtual const uci::type::EnvironmentEnum& getCurrentOperatingDomain() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CurrentOperatingDomain.
           *
           * @return The value of the enumeration identified by CurrentOperatingDomain.
           */
         virtual uci::type::EnvironmentEnum& getCurrentOperatingDomain()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CurrentOperatingDomain.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCurrentOperatingDomain(const uci::type::EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CurrentOperatingDomain.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCurrentOperatingDomain(uci::type::EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InertialState.
           *
           * @return The acecssor that provides access to the complex content that is identified by InertialState.
           */
         virtual const uci::type::InertialStateType& getInertialState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InertialState.
           *
           * @return The acecssor that provides access to the complex content that is identified by InertialState.
           */
         virtual uci::type::InertialStateType& getInertialState()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InertialState to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InertialState
           */
         virtual void setInertialState(const uci::type::InertialStateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WanderAngle.
           *
           * @return The value of the simple primitive data type identified by WanderAngle.
           */
         virtual uci::type::AngleTypeValue getWanderAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WanderAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWanderAngle(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WanderAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WanderAngle is emabled or not
           */
         virtual bool hasWanderAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WanderAngle
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableWanderAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WanderAngle */
         virtual void clearWanderAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MagneticHeading.
           *
           * @return The value of the simple primitive data type identified by MagneticHeading.
           */
         virtual uci::type::AngleTypeValue getMagneticHeading() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MagneticHeading.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMagneticHeading(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MagneticHeading exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MagneticHeading is emabled or not
           */
         virtual bool hasMagneticHeading() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MagneticHeading
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableMagneticHeading(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MagneticHeading */
         virtual void clearMagneticHeading()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommAssetPositionReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommAssetPositionReportMDT to copy from
           */
         CommAssetPositionReportMDT(const CommAssetPositionReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommAssetPositionReportMDT to the contents of the
           * CommAssetPositionReportMDT on the right hand side (rhs) of the assignment operator.CommAssetPositionReportMDT [only
           * available to derived classes]
           *
           * @param rhs The CommAssetPositionReportMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommAssetPositionReportMDT
           * @return A constant reference to this CommAssetPositionReportMDT.
           */
         const CommAssetPositionReportMDT& operator=(const CommAssetPositionReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommAssetPositionReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommAssetPositionReportMDT_h

