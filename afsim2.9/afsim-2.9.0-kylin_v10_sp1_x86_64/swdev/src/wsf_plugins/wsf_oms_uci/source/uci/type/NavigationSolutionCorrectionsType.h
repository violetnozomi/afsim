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
#ifndef Uci__Type__NavigationSolutionCorrectionsType_h
#define Uci__Type__NavigationSolutionCorrectionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Velocity3D_Type_h)
# include "uci/type/Velocity3D_Type.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the NavigationSolutionCorrectionsType sequence accessor class */
      class NavigationSolutionCorrectionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~NavigationSolutionCorrectionsType()
         { }

         /** Returns this accessor's type constant, i.e. NavigationSolutionCorrectionsType
           *
           * @return This accessor's type constant, i.e. NavigationSolutionCorrectionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::navigationSolutionCorrectionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const NavigationSolutionCorrectionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VelocityCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by VelocityCorrection.
           */
         virtual const uci::type::Velocity3D_Type& getVelocityCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VelocityCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by VelocityCorrection.
           */
         virtual uci::type::Velocity3D_Type& getVelocityCorrection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VelocityCorrection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VelocityCorrection
           */
         virtual void setVelocityCorrection(const uci::type::Velocity3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VelocityCorrection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VelocityCorrection is emabled or not
           */
         virtual bool hasVelocityCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VelocityCorrection
           *
           * @param type = uci::type::accessorType::velocity3D_Type This Accessor's accessor type
           */
         virtual void enableVelocityCorrection(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VelocityCorrection */
         virtual void clearVelocityCorrection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PositionCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionCorrection.
           */
         virtual const uci::type::Point3D_Type& getPositionCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PositionCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by PositionCorrection.
           */
         virtual uci::type::Point3D_Type& getPositionCorrection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PositionCorrection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PositionCorrection
           */
         virtual void setPositionCorrection(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PositionCorrection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PositionCorrection is emabled or not
           */
         virtual bool hasPositionCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PositionCorrection
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enablePositionCorrection(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PositionCorrection */
         virtual void clearPositionCorrection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationCorrection.
           */
         virtual const uci::type::OrientationType& getOrientationCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OrientationCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by OrientationCorrection.
           */
         virtual uci::type::OrientationType& getOrientationCorrection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OrientationCorrection to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OrientationCorrection
           */
         virtual void setOrientationCorrection(const uci::type::OrientationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OrientationCorrection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OrientationCorrection is emabled or not
           */
         virtual bool hasOrientationCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OrientationCorrection
           *
           * @param type = uci::type::accessorType::orientationType This Accessor's accessor type
           */
         virtual void enableOrientationCorrection(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OrientationCorrection */
         virtual void clearOrientationCorrection()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NavigationSolutionCorrectionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NavigationSolutionCorrectionsType to copy from
           */
         NavigationSolutionCorrectionsType(const NavigationSolutionCorrectionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NavigationSolutionCorrectionsType to the contents of the
           * NavigationSolutionCorrectionsType on the right hand side (rhs) of the assignment
           * operator.NavigationSolutionCorrectionsType [only available to derived classes]
           *
           * @param rhs The NavigationSolutionCorrectionsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::NavigationSolutionCorrectionsType
           * @return A constant reference to this NavigationSolutionCorrectionsType.
           */
         const NavigationSolutionCorrectionsType& operator=(const NavigationSolutionCorrectionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NavigationSolutionCorrectionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__NavigationSolutionCorrectionsType_h

