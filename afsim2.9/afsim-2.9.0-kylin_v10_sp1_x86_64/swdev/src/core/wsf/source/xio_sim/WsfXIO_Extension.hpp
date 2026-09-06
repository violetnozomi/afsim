// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_EXTENSION_HPP
#define WSFXIO_EXTENSION_HPP

#include "wsf_export.h"

class WsfScenario;
class WsfSimulation;
class WsfXIO_DisExtension;
class WsfXIO_EventPipe;
class WsfXIO_InputData;
class WsfXIO_Interface;
class WsfXIO_Simulation;

//! Register the XIO application extension in a WSF application.
//! A WSF application registers the XIO interface by including the following in their code:
//!
//! \code
//!    WSF_REGISTER_EXTENSION(app, xio_interface);
//! \endcode
//!
//! NOTE: You do not need to include this file to do the above.
//!
//! This will register the scenario extension to process the 'xio_interface' block. If the
//! scenario input includes a non-trivial 'xio_interface' block the extension will then
//! register a simulation extension that provides all the XIO simulation services.
//!
//! The static methods in this class simply provide access to the various low level components.
class WsfXIO_Extension
{
public:
   //! @name Access to primary XIO objects created by the XIO extension.
   //! Applications that use WSF_REGISTER_EXTENSION to request XIO services can use these methods
   //! to gain access to the XIO objects created by the extension. This is sometimes needed to
   //! force specific input values or to register new packet types.
   //@{
   static WSF_EXPORT WsfXIO_InputData* Find(const WsfScenario& aScenario);
   static WSF_EXPORT WsfXIO_Interface* Find(const WsfSimulation& aSimulation);
   //@}

   //! @name Access to lower level objects.
   //! These are typically used internally by the XIO simulation components.
   //@{
   static WSF_EXPORT WsfXIO_Simulation*   FindSimulation(const WsfSimulation& aSimulation);
   static WSF_EXPORT WsfXIO_DisExtension* FindDisExtension(const WsfSimulation& aSimulation);
   //@}
};

#endif
