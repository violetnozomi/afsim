// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// Doxygen.h : Doxygen-formatted comments to create documentation groups

#ifndef DOXYGEN_H
#define DOXYGEN_H

// these comments define the doxygen groups that
// will be used to group items together.  comments
// must use the \ingroup tag to be associated to a group
////////////////////////////////////////////////////////
// Defines the Util OSG
////////////////////////////////////////////////////////

/**
\defgroup uto_main util OSG
*/

/**
\defgroup concepts UTO Concepts
\ingroup uto_main
*/

/**
\defgroup api API Overview
\ingroup concepts
\htmlinclude APIOverview.html
*/

/**
\defgroup tutorial Tutorial
\ingroup concepts
\htmlinclude Tutorial.html
*/

/**
\defgroup distribution UTO Distribution
\ingroup uto_main
*/

/**
\defgroup req Requirements
\ingroup distribution
\htmlinclude Requirements.html
*/

/**
\defgroup install Installation
\ingroup distribution
\htmlinclude Installation.html
*/

/**
\defgroup content Content
\ingroup distribution
\htmlinclude OverviewContents.html
*/

/**
\defgroup r_note Release Notes
\ingroup distribution
\htmlinclude ReleaseNotes.html
*/

/**
\defgroup uto_help UTO Help
\ingroup uto_main
*/

/**
\defgroup using_help Using Help
\ingroup uto_help
\htmlinclude UsingHelp.html
*/

/**
\defgroup www Web Sites
\ingroup uto_help
\htmlinclude WebSite.html
*/

/**
\defgroup contacts Contacts
\ingroup uto_help
\htmlinclude Contacts.html
*/

////////////////////////////////////////////////////////
// Defines the UTO Reference Section
////////////////////////////////////////////////////////

/**
\defgroup uto_ref UTO Reference
In addition to this Reference, there is sample code provided with the distribution. This
sample code provides a rich set of examples that enhances the value of this help
documentation. Whenever possible, refer to the sample code for details on usage or for
clarification.
*/

/**
\defgroup init Initialization
\ingroup uto_ref
UTO allows options to be specified enabling, disabling, and configuring certain behaviors.
These options are set by calling the UtoInit method. UtoInit also performs other one-time
initialization that UTO requires for use. If UtoInit is not called, UTO will not function
correctly. Likewise, UtoTerm can be used when shutting down your application to cleanup the
UTO resources. Use of this method is highly recommended, but not mandatory.
*/

/**
\defgroup rt_opts Runtime Options
\ingroup init
\htmlinclude Options.html
*/

/**
\defgroup rt_init UtoInit
\ingroup init
This is the UTO initialization function. This function must be called prior to creating
any UTO objects. This function initializes the run-time options, and configures internal
state for later use.

See the \ref rt_opts "UTO Runtime Options".
*/

/**
\defgroup rt_term utoTerm
\ingroup init
This is the UTO terminate function. Call this function to cleanup UTO resources. Calling
this function is optional as resources will be automatically freed when the application
shuts down. However calling this function may be necessary in some cases when the video
driver is causing an exception when exiting your application.
*/

/**
\defgroup diag Diagnostics
\ingroup uto_ref
Provides statistical performance information.
*/

/**
\defgroup entity Entity Management
\ingroup uto_ref
UTO provides entity databases to efficiently manage entity information. Entities have position,
orientation, traits, and shapes. When entity position or orientation changes occur, UTO takes
care of efficiently updating the visual scenes of all connected viewers. Entity shapes are
shared accross viewers so that access to the shape configuration is always possible.

*/

/**
\defgroup resource Resource Management
\ingroup uto_ref
UTO provides the resource database to manage a collection of external resources defined in resource
definition files. The resource database enables external control of resources enabling icons, models,
terrain and other graphics to be changed without recompiling your code.
*/

/**
\defgroup def_file Resource Definition Files
\ingroup resource
\htmlinclude "Resource definition files.html"
*/

/**
\defgroup shape_file Shape File Format
\ingroup resource
\htmlinclude "ESRI Shape File Format.html"
*/

/**
\defgroup res_db_lookup UtoResDBLookup
\ingroup resource
*/

/**
\defgroup shape Shapes
\ingroup uto_ref
The shapes in UTO range from simple polygons and lines to complex spatial history
and sensor volumes. Every UTO shape derives from the UtoShape base class. By design,
you shouldn't need to derive your own shapes, as UTO provides the building blocks for
you to build shapes that are more complex. Often, the UTO shapes provide everything
you need, but occasionally you need a way to draw custom images. The UtoGLShape provides
a way for you to specify a callback function for you to perform your custom GL drawing. In
other situations, you need to combine multiple shapes and treat them as one, for this
purpose the UtoGroupShape exists.
*/

/**
\defgroup sound Sounds
\ingroup uto_ref
Basic 2D and 3D sounds.
*/

/**
\defgroup terrain Terrains
\ingroup uto_ref
A collection of objects for rendering terrains.
*/

/**
\defgroup types Type Definitions
\ingroup uto_ref
The basic types used by most of the uto API.
*/

/**
\defgroup view Viewer
\ingroup uto_ref
The visual capabilities in Uto are exposed in the UtoViewer class. There are symbols, camera, window,
world, and overlays. These components work together to provide the features necessary to build
interesting and useful displays.
*/

/**
\defgroup util Utility
\ingroup uto_ref
Basic utility objects and functions.
*/

////////////////////////////////////////////////////
// This defines the main page
////////////////////////////////////////////////////
/**
\mainpage
\htmlinclude UtoOverview.html
*/

#endif // DOXYGEN_H
