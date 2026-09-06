# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# Inspired by Sphinx.ext.autosectionlabel
# Adds optional handling of duplicate labels
# To introduce automatic section labels, simply set the following config value
# afsim_autosectionlabel_handle_duplicate_labels = True
# in the python configuration file (conf.py.in / conf.py)
# If in the future, warnings result from this plugin, see updated versions of
# Sphinx.ext.autosectionlabel

# Imports
from typing import Any, Dict
from typing import cast

from docutils import nodes
from docutils.nodes import Node

from sphinx.application import Sphinx
from sphinx.locale import __
from sphinx.util import logging
from sphinx.util.nodes import clean_astext

logger = logging.getLogger(__name__)

debughelp = """\n\tDebug Help:
\t   Does this document contain a duplicate page title? (Check the markup characters below the duplicate section name.)
\t   Does this document include a file (via .. include:: or _<filename>.txt) that shares a top-level section name?
\t   Try using a unique name for each section in this document.
\t   The given paths and line numbers refer to generated files and may differ from their respective source files. \n"""

# setup: Sphinx Extension Initialization
def setup(app: Sphinx):
   app.add_config_value('afsim_autosectionlabel_prefix_document', False, 'env')
   app.add_config_value('afsim_autosectionlabel_maxdepth', None, 'env')
   app.add_config_value('afsim_autosectionlabel_handle_duplicate_labels', False, 'env')
   app.connect('doctree-read', register_sections_as_label)
   return

# Class ListNode
# Simple class designed to avoid a pesky "Future Warning"
# in docutils.nodes when using the traverse function (See docutils v0.15.2 or v0.16 nodes.py)
# This is not an issue for newer versions of Sphinx. (Therefore, ListNode may be unnecessary in the future.)
class ListNode(Node):

   # Override Node.traverse
   # Do the same thing as Node.traverse, but do not send a FutureWarning to the console
   def traverse(self, condition=None, include_self=True, descend=True,
                siblings=False, ascend=False):
      return self._traverse(condition, include_self,
                            descend, siblings, ascend)

def get_node_depth(node: ListNode) -> int:
    i = 0
    cur_node = node
    while cur_node.parent != node.document:
        cur_node = cur_node.parent
        i += 1
    return i

# Similar to Sphinx.ext.autosectionlabel register_sections_as_label
# Additionally implements duplicate section name handling
#
# Use case:
#
# File "example.rst"
#
# /////////////////////////////////////////////////////////////////////////////////////////////////////
#
# Page Title      (Reference Syntax -> :ref:`docs/example:Page Title`)
#
# Section_1       (Reference Syntax -> :ref:`docs/example:Section_1`)
#
#   Overview      (Reference Syntax -> :ref:`docs/example:Overview`)
#     ...
#
# Section_2       (Reference Syntax -> :ref:`docs/example:Section_2`)
#
#   Overview      * This is where the built-in Sphinx.ext.autosectionlabel fails.
#                 * It throws a "duplicate label" warning, and this section can not be referenced.
#                 * Here, if afsim_autosectionlabel_handle_duplicate_labels = True,
#                 * duplicate sections are handled as follows:
#                 (Reference Syntax -> :ref:`docs/example:Section_2:Overview`)
#     ...
# /////////////////////////////////////////////////////////////////////////////////////////////////////
#
def register_sections_as_label(app: Sphinx, document: ListNode) -> None:
    labels = app.env.domaindata['std']['labels']
    anonlabels = app.env.domaindata['std']['anonlabels']
    labelmetadatalist = []
    for node in document.traverse(nodes.section):
        if (app.config.afsim_autosectionlabel_maxdepth and
                get_node_depth(node) >= app.config.afsim_autosectionlabel_maxdepth):
            continue
        labelid = node['ids'][0]
        docname = app.env.docname
        title = cast(nodes.title, node[0])
        ref_name = getattr(title, 'rawsource', title.astext())
        if app.config.afsim_autosectionlabel_prefix_document:
            name = nodes.fully_normalize_name(docname + ':' + ref_name)
        else:
            name = nodes.fully_normalize_name(ref_name)
        sectname = clean_astext(title)

        # If the current section title has already occurred in this document, regardless of depth
        if name in labels:

            # If the configuration value in conf.py is set to handle duplicates
            if app.config.afsim_autosectionlabel_handle_duplicate_labels:
                # If the duplicate section has a parent (i.e. If it is anything but a top level / h1 section)
                if node.parent['names']:
                    # Disambiguate duplicates by splicing the parent section name to the target reference.
                    node['ids'].append(docname + ':' + node.parent['names'][0] + ':' + ref_name)
                    name = nodes.fully_normalize_name(docname + ':' + node.parent['names'][0] + ':' + ref_name)
                else:
                    # The duplicate label occurs at the document root - May be caused by a second page title (thus has no parent).
                    # Conventionally, a given page has only one page title, so this is probably not intentional.
                    # Pass a warning with information to the console & log.

                    # Retrieve metadata for the original label
                    firstlabelline = 0;
                    for i in range(len(labelmetadatalist)):
                       if labelmetadatalist[i][0] == name:
                           firstlabel = labelmetadatalist[i][1]

                    logger.warning(('Ambiguous Target\n\n \tFirst occurrence: \t %s\t in\t %s.\n \tDuplicate label:\t %s\t in\t %s\t cannot be disambiguated.\n' + debughelp),
                    labels[name][0] + ':' + labels[name][1], firstlabel,
                    name, logging.get_node_location(node),
                    location=node, type='autosectionlabel', subtype=docname)

            else:
                # Default to the behavior defined in Sphinx.ext.autosectionlabel - Throw a warning
                logger.warning(__('Duplicate label %s, other instance in %s'),
                               name, app.env.doc2path(labels[name][0]),
                               location=node, type='autosectionlabel', subtype=docname)

        anonlabels[name] = docname, labelid
        labels[name] = docname, labelid, sectname
        labelmetadatalist.append((name, logging.get_node_location(node)))
