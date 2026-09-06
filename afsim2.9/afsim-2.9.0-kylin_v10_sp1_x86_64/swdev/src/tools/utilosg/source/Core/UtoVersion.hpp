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

//{secret}
#define UTO_MAJOR_VERSION 1

//{secret}
#define UTO_MINOR_VERSION 1

//{secret}
#define UTO_PATCH_VERSION 1

//{secret}
#define UTO_BUILD_VERSION 123

//{secret}
#define UTO_COMMENT_VERSION "Utility OSG"


// UTO_COMMA_VERSION_NUMBER produces the version string in a
// comma separated form as follows.
// 1,2,3,4
//{secret}
#define UTO_COMMA_VERSION_NUMBER UTO_MAJOR_VERSION, UTO_MINOR_VERSION, UTO_PATCH_VERSION, UTO_BUILD_VERSION

// UTO_STRING_VERSION_NUMBER produces a double quoted version string.
// "1.2.3.4"
// After the compiler is finished, the macro leaves it in the form
// "1" "." "2" "." "3" "." "4" "."
// This requires two macros with a convenience macro also listed.
// STRING_VERSION passes it's arguments to QUAD_ARG_STRING which then
// expands the arguments before QUOTED_QUAD_COMMA is called.
// QUOTED_QUAD_COMMA then puts double quotes around the arguments given
// but the arguments aren't expanded.  If STRING_VERSION were to call
// QUAD_ARG_STRING_VERSION directly the result would be
// "UTO_MAJOR_VERSION" "." ... (and so on).
//{secret}
#define UTO_QUOTED_QUAD_COMMA_VERSION(ARG1, ARG2, ARG3, ARG4) #ARG1 "." #ARG2 "." #ARG3 "." #ARG4
//{secret}
#define UTO_QUAD_ARG_STRING_VERSION(A1, A2, A3, A4) UTO_QUOTED_QUAD_COMMA_VERSION(A1, A2, A3, A4)
//{secret}
#define UTO_STRING_VERSION_NUMBER \
   UTO_QUAD_ARG_STRING_VERSION(UTO_MAJOR_VERSION, UTO_MINOR_VERSION, UTO_PATCH_VERSION, UTO_BUILD_VERSION)
