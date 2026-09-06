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
#ifndef Uci__Type__HSB_ColorControlType_h
#define Uci__Type__HSB_ColorControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the HSB_ColorControlType sequence accessor class */
      class HSB_ColorControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~HSB_ColorControlType()
         { }

         /** Returns this accessor's type constant, i.e. HSB_ColorControlType
           *
           * @return This accessor's type constant, i.e. HSB_ColorControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::hSB_ColorControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const HSB_ColorControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hue.
           *
           * @return The value of the simple primitive data type identified by Hue.
           */
         virtual uci::type::PercentTypeValue getHue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHue(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Hue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Hue is emabled or not
           */
         virtual bool hasHue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Hue
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableHue(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Hue */
         virtual void clearHue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Saturation.
           *
           * @return The value of the simple primitive data type identified by Saturation.
           */
         virtual uci::type::PercentTypeValue getSaturation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Saturation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSaturation(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Saturation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Saturation is emabled or not
           */
         virtual bool hasSaturation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Saturation
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableSaturation(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Saturation */
         virtual void clearSaturation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Brightness.
           *
           * @return The value of the simple primitive data type identified by Brightness.
           */
         virtual uci::type::PercentTypeValue getBrightness() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Brightness.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBrightness(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Brightness exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Brightness is emabled or not
           */
         virtual bool hasBrightness() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Brightness
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableBrightness(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Brightness */
         virtual void clearBrightness()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         HSB_ColorControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The HSB_ColorControlType to copy from
           */
         HSB_ColorControlType(const HSB_ColorControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this HSB_ColorControlType to the contents of the HSB_ColorControlType
           * on the right hand side (rhs) of the assignment operator.HSB_ColorControlType [only available to derived classes]
           *
           * @param rhs The HSB_ColorControlType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::HSB_ColorControlType
           * @return A constant reference to this HSB_ColorControlType.
           */
         const HSB_ColorControlType& operator=(const HSB_ColorControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // HSB_ColorControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__HSB_ColorControlType_h

