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
#ifndef Uci__Factory__RGB_ColorControlTypeFactory_h
#define Uci__Factory__RGB_ColorControlTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__RGB_ColorControlType_h)
# include "uci/type/RGB_ColorControlType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class RGB_ColorControlTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new RGB_ColorControlTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::RGB_ColorControlTypeFactory* uci_getRGB_ColorControlTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a RGB_ColorControlTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyRGB_ColorControlTypeFactory(uci::factory::RGB_ColorControlTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class RGB_ColorControlTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~RGB_ColorControlTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::RGB_ColorControlTypeFactory FactoryType;
         typedef  uci::type::RGB_ColorControlType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::RGB_ColorControlTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getRGB_ColorControlTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::RGB_ColorControlTypeFactory* factory )
         {
            uci_destroyRGB_ColorControlTypeFactory( factory );
         }

         /** This method constructs a new RGB_ColorControlType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created RGB_ColorControlType message
           */
         virtual uci::type::RGB_ColorControlType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new RGB_ColorControlType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created RGB_ColorControlType message
           */
         virtual uci::type::RGB_ColorControlType& create(const uci::type::RGB_ColorControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a RGB_ColorControlType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::RGB_ColorControlType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RGB_ColorControlTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RGB_ColorControlTypeFactory to copy from
           */
         RGB_ColorControlTypeFactory(const RGB_ColorControlTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RGB_ColorControlTypeFactory to the contents of the
           * RGB_ColorControlTypeFactory on the right hand side (rhs) of the assignment operator.RGB_ColorControlTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The RGB_ColorControlTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::RGB_ColorControlTypeFactory
           * @return A constant reference to this RGB_ColorControlTypeFactory.
           */
         const RGB_ColorControlTypeFactory& operator=(const RGB_ColorControlTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RGB_ColorControlTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__RGB_ColorControlTypeFactory_h

