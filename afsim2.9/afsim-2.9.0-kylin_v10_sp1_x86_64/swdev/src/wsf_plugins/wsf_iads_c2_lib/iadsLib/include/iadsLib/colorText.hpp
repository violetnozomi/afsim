// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   colorText.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 *
 */

#ifndef COLORTEXT_HPP
#define COLORTEXT_HPP

#ifdef __unix__
// Colored output interferes with comparing output between platforms.
// Colored output needs to be disabled as long as regression tests
// rely on the logger to provide expected test results.
#if 0
#define DEFAULT_TEXT      "\033[0m"

#define BLACK_TEXT        "\033[0;30m"
#define RED_TEXT          "\033[0;31m"
#define GREEN_TEXT        "\033[0;32m"
#define BROWN_TEXT        "\033[0;33m"
#define BLUE_TEXT         "\033[0;34m"
#define PURPLE_TEXT       "\033[0;35m"
#define CYAN_TEXT         "\033[0;36m"
#define LIGHT_GRAY_TEXT   "\033[0;37m"
#define DARK_GRAY_TEXT    "\033[1;30m"
#define LIGHT_RED_TEXT    "\033[1;31m"
#define LIGHT_GREEN_TEXT  "\033[1;32m"
#define YELLOW_TEXT       "\033[1;33m"
#define LIGHT_BLUE_TEXT   "\033[1;34m"
#define LIGHT_PURPLE_TEXT "\033[1;35m"
#define LIGHT_CYAN_TEXT   "\033[1;36m"
#define WHITE_TEXT        "\033[1;37m"

#define BLACK_TEXT_BLACK_BG        "\033[40;30m"
#define RED_TEXT_BLACK_BG          "\033[40;31m"
#define GREEN_TEXT_BLACK_BG        "\033[40;32m"
#define BROWN_TEXT_BLACK_BG        "\033[40;33m"
#define BLUE_TEXT_BLACK_BG         "\033[40;34m"
#define PURPLE_TEXT_BLACK_BG       "\033[40;35m"
#define CYAN_TEX_BLACK_BGT         "\033[40;36m"
#define LIGHT_GRAY_TEXT_BLACK_BG   "\033[40;37m"
#define DARK_GRAY_TEXT_BLACK_BG    "\033[40;1;30m"
#define LIGHT_RED_TEXT_BLACK_BG    "\033[40;1;31m"
#define LIGHT_GREEN_TEXT_BLACK_BG  "\033[40;1;32m"
#define YELLOW_TEXT_BLACK_BG       "\033[40;1;33m"
#define LIGHT_BLUE_TEXT_BLACK_BG   "\033[40;1;34m"
#define LIGHT_PURPLE_TEXT_BLACK_BG "\033[40;1;35m"
#define LIGHT_CYAN_TEXT_BLACK_BG   "\033[40;1;36m"
#define WHITE_TEXT_BLACK_BG        "\033[40;1;37m"
#endif

#define DEFAULT_TEXT      ""

#define BLACK_TEXT        ""
#define RED_TEXT          ""
#define GREEN_TEXT        ""
#define BROWN_TEXT        ""
#define BLUE_TEXT         ""
#define PURPLE_TEXT       ""
#define CYAN_TEXT         ""
#define LIGHT_GRAY_TEXT   ""
#define DARK_GRAY_TEXT    ""
#define LIGHT_RED_TEXT    ""
#define LIGHT_GREEN_TEXT  ""
#define YELLOW_TEXT       ""
#define LIGHT_BLUE_TEXT   ""
#define LIGHT_PURPLE_TEXT ""
#define LIGHT_CYAN_TEXT   ""
#define WHITE_TEXT        ""

#define BLACK_TEXT_BLACK_BG        ""
#define RED_TEXT_BLACK_BG          ""
#define GREEN_TEXT_BLACK_BG        ""
#define BROWN_TEXT_BLACK_BG        ""
#define BLUE_TEXT_BLACK_BG         ""
#define PURPLE_TEXT_BLACK_BG       ""
#define CYAN_TEX_BLACK_BGT         ""
#define LIGHT_GRAY_TEXT_BLACK_BG   ""
#define DARK_GRAY_TEXT_BLACK_BG    ""
#define LIGHT_RED_TEXT_BLACK_BG    ""
#define LIGHT_GREEN_TEXT_BLACK_BG  ""
#define YELLOW_TEXT_BLACK_BG       ""
#define LIGHT_BLUE_TEXT_BLACK_BG   ""
#define LIGHT_PURPLE_TEXT_BLACK_BG ""
#define LIGHT_CYAN_TEXT_BLACK_BG   ""
#define WHITE_TEXT_BLACK_BG        ""

#else
#define DEFAULT_TEXT      ""

#define BLACK_TEXT        ""
#define RED_TEXT          ""
#define GREEN_TEXT        ""
#define BROWN_TEXT        ""
#define BLUE_TEXT         ""
#define PURPLE_TEXT       ""
#define CYAN_TEXT         ""
#define LIGHT_GRAY_TEXT   ""
#define DARK_GRAY_TEXT    ""
#define LIGHT_RED_TEXT    ""
#define LIGHT_GREEN_TEXT  ""
#define YELLOW_TEXT       ""
#define LIGHT_BLUE_TEXT   ""
#define LIGHT_PURPLE_TEXT ""
#define LIGHT_CYAN_TEXT   ""
#define WHITE_TEXT        ""

#define BLACK_TEXT_BLACK_BG        ""
#define RED_TEXT_BLACK_BG          ""
#define GREEN_TEXT_BLACK_BG        ""
#define BROWN_TEXT_BLACK_BG        ""
#define BLUE_TEXT_BLACK_BG         ""
#define PURPLE_TEXT_BLACK_BG       ""
#define CYAN_TEX_BLACK_BGT         ""
#define LIGHT_GRAY_TEXT_BLACK_BG   ""
#define DARK_GRAY_TEXT_BLACK_BG    ""
#define LIGHT_RED_TEXT_BLACK_BG    ""
#define LIGHT_GREEN_TEXT_BLACK_BG  ""
#define YELLOW_TEXT_BLACK_BG       ""
#define LIGHT_BLUE_TEXT_BLACK_BG   ""
#define LIGHT_PURPLE_TEXT_BLACK_BG ""
#define LIGHT_CYAN_TEXT_BLACK_BG   ""
#define WHITE_TEXT_BLACK_BG        ""
#endif

#endif
