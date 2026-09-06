// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AboutType_h
#define Uci__Type__AboutType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AboutType sequence accessor class */
      class AboutType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AboutType()
         { }

         /** Returns this accessor's type constant, i.e. AboutType
           *
           * @return This accessor's type constant, i.e. AboutType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aboutType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AboutType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Model.
           *
           * @return The value of the string data type identified by Model.
           */
         virtual const uci::type::VisibleString32Type& getModel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Model.
           *
           * @return The value of the string data type identified by Model.
           */
         virtual uci::type::VisibleString32Type& getModel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setModel(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Model to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModel(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SerialNumber.
           *
           * @return The value of the string data type identified by SerialNumber.
           */
         virtual const uci::type::VisibleString256Type& getSerialNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SerialNumber.
           *
           * @return The value of the string data type identified by SerialNumber.
           */
         virtual uci::type::VisibleString256Type& getSerialNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SerialNumber to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSerialNumber(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SerialNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSerialNumber(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SerialNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSerialNumber(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SerialNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SerialNumber is emabled or not
           */
         virtual bool hasSerialNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SerialNumber
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableSerialNumber(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SerialNumber */
         virtual void clearSerialNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SoftwareVersion.
           *
           * @return The value of the string data type identified by SoftwareVersion.
           */
         virtual const uci::type::VisibleString256Type& getSoftwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SoftwareVersion.
           *
           * @return The value of the string data type identified by SoftwareVersion.
           */
         virtual uci::type::VisibleString256Type& getSoftwareVersion()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareVersion to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSoftwareVersion(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSoftwareVersion(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSoftwareVersion(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SoftwareVersion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SoftwareVersion is emabled or not
           */
         virtual bool hasSoftwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SoftwareVersion
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableSoftwareVersion(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SoftwareVersion */
         virtual void clearSoftwareVersion()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the BootloaderSoftwareVersion.
           *
           * @return The value of the string data type identified by BootloaderSoftwareVersion.
           */
         virtual const uci::type::VisibleString256Type& getBootloaderSoftwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the BootloaderSoftwareVersion.
           *
           * @return The value of the string data type identified by BootloaderSoftwareVersion.
           */
         virtual uci::type::VisibleString256Type& getBootloaderSoftwareVersion()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BootloaderSoftwareVersion to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setBootloaderSoftwareVersion(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BootloaderSoftwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBootloaderSoftwareVersion(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BootloaderSoftwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBootloaderSoftwareVersion(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BootloaderSoftwareVersion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BootloaderSoftwareVersion is emabled or not
           */
         virtual bool hasBootloaderSoftwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BootloaderSoftwareVersion
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableBootloaderSoftwareVersion(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BootloaderSoftwareVersion */
         virtual void clearBootloaderSoftwareVersion()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HardwareVersion.
           *
           * @return The value of the string data type identified by HardwareVersion.
           */
         virtual const uci::type::VisibleString32Type& getHardwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HardwareVersion.
           *
           * @return The value of the string data type identified by HardwareVersion.
           */
         virtual uci::type::VisibleString32Type& getHardwareVersion()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HardwareVersion to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setHardwareVersion(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HardwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHardwareVersion(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HardwareVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHardwareVersion(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HardwareVersion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HardwareVersion is emabled or not
           */
         virtual bool hasHardwareVersion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HardwareVersion
           *
           * @param type = uci::type::accessorType::visibleString32Type This Accessor's accessor type
           */
         virtual void enableHardwareVersion(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString32Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HardwareVersion */
         virtual void clearHardwareVersion()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AboutType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AboutType to copy from
           */
         AboutType(const AboutType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AboutType to the contents of the AboutType on the right hand side
           * (rhs) of the assignment operator.AboutType [only available to derived classes]
           *
           * @param rhs The AboutType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::AboutType
           * @return A constant reference to this AboutType.
           */
         const AboutType& operator=(const AboutType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AboutType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AboutType_h

