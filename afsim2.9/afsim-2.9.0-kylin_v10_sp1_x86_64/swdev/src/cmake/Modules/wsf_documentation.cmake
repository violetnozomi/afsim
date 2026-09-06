# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

find_package(Sphinx)

if (SPHINX_FOUND)
   execute_process(COMMAND "${SPHINX_EXECUTABLE}" --version
                   OUTPUT_VARIABLE SPHINX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
   string(REGEX REPLACE ".* ([0-9]\\.([0-9]\\.[0-9])?)" "\\1"
          SPHINX_VERSION "${SPHINX_VERSION}")
   message(STATUS "Sphinx version: ${SPHINX_VERSION}")

   # Sphinx base data directory.
   set(WSF_SPHINX_DIR "${CMAKE_CURRENT_LIST_DIR}/sphinx")

   # Path to themes and theme.conf.
   if (NOT SPHINX_THEME_DIR)
      set(SPHINX_THEME_DIR "${CMAKE_BINARY_DIR}/documentation")
   endif()

   if (NOT SPHINX_THEME_NAME)
      set(SPHINX_THEME_NAME "wsftheme")
   endif()

   if (NOT WSF_SPHINX_SIDEBAR_LOGO)
      set(WSF_SPHINX_SIDEBAR_LOGO "${WSF_SPHINX_DIR}/_static/sidebar_logo.png")
   endif()

   if (NOT WSF_SPHINX_FAVICON)
      set(WSF_SPHINX_FAVICON "${WSF_SPHINX_DIR}/_static/favicon.ico")
   endif()

   # Path to extensions and extras.
   set(SPHINX_EXTENSIONS_DIR "${WSF_SPHINX_DIR}/extensions")

   # configured documentation tools and intermediate build results
   set(SPHINX_BUILD_DIR "${CMAKE_BINARY_DIR}/documentation")

   # Changelog PDF Output directory
   set(SPHINX_CHANGELOG_DIR "${CMAKE_BINARY_DIR}/documentation/changelog")

   # HTML output directory
   set(SPHINX_HTML_DIR "${CMAKE_BINARY_DIR}/documentation/html")

   # PDF Output Directory
   set(SPHINX_PDF_DIR "${CMAKE_BINARY_DIR}/documentation/pdf")

   # User Manual Output directory
   set(SPHINX_USER_MANUAL_DIR "${CMAKE_BINARY_DIR}/documentation/user_manual")

   # Sphinx cache with pickled ReST documents
   set(SPHINX_CACHE_DIR "${CMAKE_BINARY_DIR}/documentation/_doctrees")

   # When "clean" target is run, remove the Sphinx build directory
   set_property(DIRECTORY APPEND PROPERTY
                ADDITIONAL_MAKE_CLEAN_FILES
                "${SPHINX_CACHE_DIR}"
                "${SPHINX_CHANGELOG_DIR}"
                "${SPHINX_HTML_DIR}"
                "${SPHINX_PDF_DIR}"
                "${SPHINX_USER_MANUAL_DIR}"
               )
else()
   if(WSF_INSTALL_DOCUMENTATION)
      message(WARNING "Sphinx install selected but Sphinx exe not available for generation.")
   endif()
endif()

# Global property to contain WSF sub-project and extension Sphinx inputs
# Always enabled as it is used by other CMake files
define_property(GLOBAL PROPERTY WSF_SPHINX_INPUT
                BRIEF_DOCS "Input for Sphinx"
                FULL_DOCS "Paths to files and directories that will passed to the Sphinxfile INPUT variable"
               )
define_property(GLOBAL PROPERTY WSF_SPHINX_PYTHON_MODULES
                BRIEF_DOCS "Directories of python modules to be made available for autodoc"
                FULL_DOCS "Paths to directories that will be added to the python path in the Sphinxfile, thus making them available for documentation."
               )
# Global property to contain WSF sub-project and extension Sphinx input exclusions
# Always enabled as it is used by other CMake files
#define_property(GLOBAL PROPERTY WSF_SPHINX_EXCLUDE
#                BRIEF_DOCS "Exclude for Sphinx"
#                FULL_DOCS "Paths to files and directories that will be excluded from Sphinx INPUT"
#               )
define_property(GLOBAL PROPERTY WSF_SPHINX_SIDEBAR_EXTS
                BRIEF_DOCS "Sidebar Extensions for Sphinx"
                FULL_DOCS "Sidebar extensions names for Sphinx"
               )

define_property(GLOBAL PROPERTY WSF_SPHINX_SIDEBAR_PATHS
                BRIEF_DOCS "Sidebar Extensions paths for Sphinx"
                FULL_DOCS "Sidebar extension paths for Sphinx"
               )

###########################################################
# Macros
###########################################################

# Add the supplied list of directories or files (SPHINX_INPUT) to the global
# list of files that will be used to generate Sphinx documentation
# if they have not already been added. This macro should be called by WSF
# extensions to add documentation to the WSF build.
macro(add_wsf_doc_input SPHINX_INPUT)
   # Store global WSF_SPHINX_INPUT in a local variable for comparison
   get_property(CURRENT_SPHINX_INPUT GLOBAL PROPERTY WSF_SPHINX_INPUT)

   # Loop over the supplied dirs/files and add them one at a time. Because
   # we are expecting lists we use the built in ARGV variable that hold the
   # list of all variables passed in, not SPHINX_INPUT which would only hold
   # the first
   foreach(INPUT_SOURCE ${ARGV})
      # Only add things to INPUT list that are not already there
      get_filename_component(INPUT_SOURCE ${INPUT_SOURCE} ABSOLUTE)
      if(NOT (CURRENT_SPHINX_INPUT MATCHES "${INPUT_SOURCE};"))
         # NOTES: APPEND_STRING is used to build a string ; separated list that is
         # passed to the conf.py.in to create the conf.py.
         set_property(GLOBAL APPEND_STRING PROPERTY WSF_SPHINX_INPUT "${INPUT_SOURCE};")
      endif()
   endforeach()
endmacro()

# Makes all the python modules in the supplied directories available for documentation using sphinx autodoc.
# Takes as input a list of directories.
function(add_wsf_python_module_docs)
   get_property(CURRENT_SPHINX_INPUT GLOBAL PROPERTY WSF_SPHINX_PYTHON_MODULES)

   # Loop over the supplied dirs and add them one at a time.
   foreach(INPUT_SOURCE ${ARGV})
      # Only add things to INPUT list that are not already there
      get_filename_component(INPUT_SOURCE ${INPUT_SOURCE} ABSOLUTE)
      if(NOT (CURRENT_SPHINX_INPUT MATCHES "${INPUT_SOURCE};"))
         # NOTES: APPEND_STRING is used to build a string ; separated list that is
         # passed to the conf.py.in to create the conf.py.
         set_property(GLOBAL APPEND_STRING PROPERTY WSF_SPHINX_PYTHON_MODULES "${INPUT_SOURCE};")
      endif()
   endforeach()
endfunction()

# Add the supplied sidebar items in html format to the sidebar list.
# if they have not already been added. This macro should be called by WSF
# extensions to add sidebar items to the WSF build.
macro(add_wsf_doc_sidebar_ext SPHINX_SIDEBAR_EXT)
   # Store global WSF_SPHINX_SIDEBAR_EXTS in a local variable for comparison
   get_property(CURRENT_SPHINX_THEME_PATHS GLOBAL PROPERTY WSF_SPHINX_SIDEBAR_EXTS)

   # Loop over the supplied dirs/files and add them one at a time. Because
   # we are expecting lists we use the built in ARGV variable that hold the
   # list of all variables passed in, not SPHINX_THEME_PATH which would only hold
   # the first
   foreach(SIDEBAR_FILE ${ARGV})
      # Only add things to THEME path list that are not already there
      get_filename_component(SIDEBAR_PATH ${SIDEBAR_FILE} ABSOLUTE)
      get_filename_component(SIDEBAR_DIR ${SIDEBAR_PATH} DIRECTORY)
      get_filename_component(SIDEBAR_FILE ${SIDEBAR_PATH} NAME)
      if(NOT (CURRENT_SPHINX_THEME_PATHS MATCHES "'${SIDEBAR_FILE}', "))
         # NOTES: APPEND_STRING is used to build a string, separated list that is
         # passed to the conf.py.in to create the conf.py.
         set_property(GLOBAL APPEND_STRING PROPERTY WSF_SPHINX_SIDEBAR_EXTS "'${SIDEBAR_FILE}', ")
         set_property(GLOBAL APPEND_STRING PROPERTY WSF_SPHINX_SIDEBAR_PATHS "${SIDEBAR_DIR};")
      endif()
   endforeach()
endmacro()

set(DOCUMENTATION_LIST_FILE "${CMAKE_CURRENT_LIST_FILE}")

macro(create_doctarget)
   if (SPHINX_FOUND)
      set_property(GLOBAL APPEND_STRING PROPERTY WSF_SPHINX_SIDEBAR_PATHS "${WSF_SPHINX_DIR}/wsftheme;")
      get_property(SPHINX_SIDEBAR_PATHS GLOBAL PROPERTY WSF_SPHINX_SIDEBAR_PATHS)
      add_custom_target(DOCUMENTATION
                        COMMAND
                        ${CMAKE_COMMAND}
                        -E copy_directory
                        ${SPHINX_SIDEBAR_PATHS}
                        ${SPHINX_THEME_DIR}/wsftheme
                        COMMENT "Copying themes with CMake into ${SPHINX_THEME_DIR}/wsftheme"
                        WORKING_DIRECTORY ${SPHINX_BUILD_DIR}
                        SOURCES ${DOCUMENTATION_LIST_FILE} "${SPHINX_BUILD_DIR}/index.html"
                       )
      set_property(TARGET DOCUMENTATION PROPERTY FOLDER DocumentationTargets)

      add_custom_command(TARGET DOCUMENTATION POST_BUILD
                         COMMAND ${SPHINX_EXECUTABLE}
                         -q -b html
                         -t afsim-documentation
                         -c "${SPHINX_BUILD_DIR}"
                         -d "${SPHINX_CACHE_DIR}"
                         -w "${SPHINX_BUILD_DIR}/warning-log-html.txt"
                         #-D "pngmath_latex=pdflatex"
                         #-D "pngmath_latex_preamble=\\\\pdfoutput 0"
                         "${SPHINX_BUILD_DIR}"
                         "${SPHINX_HTML_DIR}"
                         COMMENT "Building HTML documentation with Sphinx into ${SPHINX_BUILD_DIR}"
                         WORKING_DIRECTORY ${SPHINX_BUILD_DIR}
                         COMMAND
                         ${Python3_EXECUTABLE}
                         "${WSF_SPHINX_DIR}/build_command_index.py"
                         "${SPHINX_HTML_DIR}"
                         "${SPHINX_HTML_DIR}"
                        )

      # Create CMake Target DOCUMENTATION_PDF (.rst -> .pdf)
      # DOCUMENTATION_PDF Step 1: Run LaTeX to parse all AFSIM documentation (.rst) to generate afsim.tex (.rst -> .tex)
      add_custom_target(DOCUMENTATION_PDF
                        WORKING_DIRECTORY ${SPHINX_BUILD_DIR} # Set working directory
                        COMMAND ${CMAKE_COMMAND} -E echo "   Step 1/3: Building LaTeX documentation with Sphinx into ${SPHINX_PDF_DIR}"
                        COMMAND ${SPHINX_EXECUTABLE}          # sphinx-build
                        -b latex                              # Builder name
                        -t afsim-documentation                # Set tag to generate AFSIM Documentation content
                        -c "${SPHINX_BUILD_DIR}"              # Path to configuration file (conf.py)
                        -d "${SPHINX_CACHE_DIR}"              # Path to cache doctree pickles of source documentation
                        -w "${SPHINX_BUILD_DIR}/warning-log-latex.txt"
                        "${SPHINX_BUILD_DIR}"                 # Source directory
                        "${SPHINX_PDF_DIR}"                   # Output directory                     
                        # For all command line options, see https://www.sphinx-doc.org/en/master/man/sphinx-build.html
                        SOURCES ${DOCUMENTATION_LIST_FILE}
                       )
      set_property(TARGET DOCUMENTATION_PDF PROPERTY FOLDER DocumentationTargets) # Place the target in DocumentationTargets Folder of VS Solution heirarchy
      
      # DOCUMENTATION_PDF Step 2: Run texfix on afsim.tex to resolve undefined references (.tex -> .tex)
      add_custom_command(TARGET DOCUMENTATION_PDF POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "   Step 2/3: Running texfix on LaTeX documentation to resolve undefined cross-references"
                         WORKING_DIRECTORY ${WSF_SPHINX_DIR}
                         COMMAND ${Python3_EXECUTABLE} texfix.py # Run texfix.py
                         "${SPHINX_PDF_DIR}/afsim-${AFSIM_VERSION}-documentation.tex"
                         "${SPHINX_BUILD_DIR}"                   # Source directory
                         "${SPHINX_PDF_DIR}"                     # Output directory  
                        )
      
      # DOCUMENTATION_PDF Step 3: Run latex make (latexmk) on afsim.tex to generate a PDF of the AFSIM Documentation (.tex -> .pdf)
      add_custom_command(TARGET DOCUMENTATION_PDF POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "   Step 3/3: Running latexmk on LaTeX documentation to generate documentation as PDF"
                         WORKING_DIRECTORY ${SPHINX_PDF_DIR} # Set working directory
                         COMMAND latexmk
                         -pdf                                # Enable  pdf output
                         -dvi-                               # Disable dvi output
                         -ps-                                # Disable postscript output
                         --interaction=nonstopmode           # Disable Stop for user input mid-execution
                         -f                                  # Enable  Force continued execution past errors. Guarantees that a pdf will always be generated
                         -silent                             # Enable  Silent execution to declutter console output. (For now)
                         # For all command line options, try latexmk -help
                        )


      # Create CMake Target DOCUMENTATION_USER_MANUAL (../User Manual/primer_*.rst -> AFSIM User Manual.pdf)
      # DOCUMENTATION_USER_MANUAL Step 1: Run LaTeX to parse all AFSIM documentation (.rst) to generate AFSIM User Manual.tex (.rst -> .tex)
      add_custom_target(DOCUMENTATION_USER_MANUAL
                        WORKING_DIRECTORY ${SPHINX_BUILD_DIR} # Set working directory
                        COMMAND ${CMAKE_COMMAND} -E echo "   Step 1/3: Building LaTeX documentation with Sphinx into ${SPHINX_PDF_DIR}"
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${SPHINX_BUILD_DIR}/docs/user_manual"
                        COMMAND ${SPHINX_EXECUTABLE}              # sphinx-build
                        -b latex                                  # Builder name
                        -t afsim-user-manual                      # Set tag to generate User Manual content
                        -c "${SPHINX_BUILD_DIR}"                  # Path to configuration file (conf.py)
                        -d "${SPHINX_CACHE_DIR}"                  # Path to cache doctree pickles of source documentation
                        -w "${SPHINX_BUILD_DIR}/warning-log-user-manual-latex.txt"
                        "${SPHINX_BUILD_DIR}/docs/user_manual"    # Source directory
                        "${SPHINX_USER_MANUAL_DIR}"               # Output directory                     
                        # For all command line options, see https://www.sphinx-doc.org/en/master/man/sphinx-build.html
                        SOURCES ${DOCUMENTATION_LIST_FILE}
                       )
      set_property(TARGET DOCUMENTATION_USER_MANUAL PROPERTY FOLDER DocumentationTargets) # Place the target in DocumentationTargets Folder of VS Solution heirarchy
      
      # DOCUMENTATION_USER_MANUAL Step 2: Run texfix on afsim.tex to resolve undefined references (.tex -> .tex)
      add_custom_command(TARGET DOCUMENTATION_USER_MANUAL POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "   Step 2/3: Running texfix on LaTeX documentation to resolve undefined cross-references"
                         WORKING_DIRECTORY ${WSF_SPHINX_DIR}
                         COMMAND ${Python3_EXECUTABLE} texfix.py # Run texfix.py
                         "${SPHINX_USER_MANUAL_DIR}/afsim-${AFSIM_VERSION}-user-manual.tex"
                         "${SPHINX_BUILD_DIR}"                   # Source directory
                         "${SPHINX_USER_MANUAL_DIR}"             # Output directory  
                        )
      
      # DOCUMENTATION_USER_MANUAL Step 3: Run latex make (latexmk) on AFSIM User Manual.tex to generate a PDF (.tex -> .pdf)
      add_custom_command(TARGET DOCUMENTATION_USER_MANUAL POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "   Step 3/3: Running latexmk on LaTeX documentation to generate documentation as PDF"
                         WORKING_DIRECTORY ${SPHINX_USER_MANUAL_DIR} # Set working directory
                         COMMAND latexmk
                         -pdf                                          # Enable  pdf output
                         -dvi-                                         # Disable dvi output
                         -ps-                                          # Disable postscript output
                         --interaction=nonstopmode                     # Disable Stop for user input mid-execution
                         -f                                            # Enable  Force continued execution past errors. Guarantees that a pdf will always be generated
                         -silent                                       # Enable  Silent execution to declutter console output. (For now)
                         # For all command line options, try latexmk -help
                        )


      # Create CMake Target DOCUMENTATION_CHANGELOG
      add_custom_target(DOCUMENTATION_CHANGELOG
                        WORKING_DIRECTORY ${SPHINX_BUILD_DIR} # Set working directory
                        COMMAND ${CMAKE_COMMAND} -E echo "   Step 1/2: Building LaTeX changelog with Sphinx into ${SPHINX_CHANGELOG_DIR}"
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${SPHINX_BUILD_DIR}/docs/changelog"
                        COMMAND ${SPHINX_EXECUTABLE}              # sphinx-build
                        -b latex                                  # Builder name
                        -t afsim-changelog                        # Set tag to build Changelog content
                        -Q                                        # Enable silent execution
                        -c "${SPHINX_BUILD_DIR}"                  # Path to configuration file (conf.py)
                        -d "${SPHINX_CACHE_DIR}"                  # Path to cache doctree pickles of source documentation
                        -w "${SPHINX_BUILD_DIR}/warning-log-changelog-latex.txt"
                        "${SPHINX_BUILD_DIR}/docs/changelog"      # Source directory
                        "${SPHINX_CHANGELOG_DIR}"                 # Output directory                     
                        # For all command line options, see https://www.sphinx-doc.org/en/master/man/sphinx-build.html
                        SOURCES ${DOCUMENTATION_LIST_FILE}
                       )
      set_property(TARGET DOCUMENTATION_CHANGELOG PROPERTY FOLDER DocumentationTargets) # Place the target in DocumentationTargets Folder of VS Solution heirarchy
      
      add_custom_command(TARGET DOCUMENTATION_CHANGELOG POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "   Step 2/2: Running latexmk on LaTeX changelog to generate as PDF"
                         WORKING_DIRECTORY ${SPHINX_CHANGELOG_DIR} # Set working directory
                         COMMAND latexmk
                         -pdf                                          # Enable  pdf output
                         -dvi-                                         # Disable dvi output
                         -ps-                                          # Disable postscript output
                         --interaction=nonstopmode                     # Disable Stop for user input mid-execution
                         -f                                            # Enable  Force continued execution past errors. Guarantees that a pdf will always be generated
                         -silent                                       # Enable  Silent execution to declutter console output. (For now)
                         # For all command line options, try latexmk -help
                        )

   endif()
endmacro()

macro(create_docfile)
   if (SPHINX_FOUND)
      # Store global Sphinx properties in a local variables that will
      # configure conf.py.in
      get_property(WSF_SPHINX_INPUT GLOBAL PROPERTY WSF_SPHINX_INPUT)
      #get_property(WSF_SPHINX_EXCLUDE GLOBAL PROPERTY WSF_SPHINX_EXCLUDE)
      get_property(WSF_SPHINX_SIDEBAR_EXTS GLOBAL PROPERTY WSF_SPHINX_SIDEBAR_EXTS)
      get_property(WSF_SPHINX_PYTHON_MODULES GLOBAL PROPERTY WSF_SPHINX_PYTHON_MODULES)

      # This message kept enabled as confirmation that Sphinx is being processed
      message(STATUS "Configuring Sphinx Documentation")

      set(SPHINX_DOC_PATHS "${WSF_SPHINX_INPUT}")
      set(SPHINX_SIDEBARS_EXTS "${WSF_SPHINX_SIDEBAR_EXTS}")

      # Generate the Sphinx configuration file (conf.py) which is used to generate both HTML and PDF documentation.
      configure_file("${WSF_SPHINX_DIR}/conf.py.in" "${SPHINX_BUILD_DIR}/conf.py" @ONLY)

      file(COPY "${WSF_SPHINX_DIR}/index.html" DESTINATION "${SPHINX_BUILD_DIR}")

      create_doctarget()
   endif()
endmacro()

# Install documentation HTML output in the 'install' location for the build. This macro
# depends on the WSF_INSTALL_DOCUMENTATION option set in the main CMakeLists.txt file.
macro(install_documentation)
   # For there to be documentation to install the build system must have Sphinx
   # installed and the optional DOCUMENTION target needs to be built
   if(WSF_INSTALL_DOCUMENTATION AND SPHINX_FOUND)
      if (NOT DOCUMENTATION_INSTALL_DIR)
         set (DOCUMENTATION_INSTALL_DIR "documentation")
      endif (NOT DOCUMENTATION_INSTALL_DIR)

      # Install the entire HTML generated documentation directory
      if(EXISTS "${SPHINX_HTML_DIR}")
         install(DIRECTORY "${SPHINX_HTML_DIR}" DESTINATION ${DOCUMENTATION_INSTALL_DIR} OPTIONAL COMPONENT Documentation)
         install(FILES "${SPHINX_BUILD_DIR}/index.html" DESTINATION ${DOCUMENTATION_INSTALL_DIR} OPTIONAL COMPONENT Documentation)
      endif()

      foreach(doc_directory IN ITEMS
         "${SPHINX_CHANGELOG_DIR}"
         "${SPHINX_PDF_DIR}"
         "${SPHINX_USER_MANUAL_DIR}"
      )
         install(DIRECTORY "${doc_directory}" DESTINATION ${DOCUMENTATION_INSTALL_DIR} OPTIONAL COMPONENT Documentation
                 FILES_MATCHING PATTERN "*.pdf")
      endforeach()
   endif()
endmacro()
