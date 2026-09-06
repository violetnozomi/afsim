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

# Imports
from docutils import nodes
from sphinx import roles
from sphinx.util import logging

logger = logging.getLogger(__name__)

# setup: Sphinx Extension Initialization
# During initialization new roles, directives, and configuration values may be added
def setup(app):
   app.add_config_value('afsim_issue_role_url_base',     # Define configuration value
                        'https://jira.di2e.net/browse/', # Default value
                        'html')                          # Rebuild association

   app.add_role('issue',                                              # Define role
                AFSIMIssueRole(app.config.afsim_issue_role_url_base)) # Attach handler
   return

# Class AFSIMIssueRole: Inherits sphinx.roles.SphinxRole
# Implements: Custom Sphinx Role :issue:`<issue-signature>`
class AFSIMIssueRole(roles.SphinxRole):

   # __init__: Python Built-In, executed at object initialization
   # param urlBase: The base URL pointing to the AFSIM issue tracking location
   def __init__(self, urlBase):
      self.urlBase = urlBase

   # run: overrides sphinx.roles.SphinxRole.run
   # Returned by sphinx.roles.SphinxRole.__call__
   def run(self):
      if self.text.startswith('AFSIM-'):
         url = self.urlBase + self.text
         node = nodes.reference(self.rawtext, self.text, refuri=url, **self.options)
         node['classes'] += ['afsim-issue-role']
         return [node], [] # Return a hyperlinked node, linking to a specific issue page
      else:
         # Log a warning if provided an invalid signature
         logger.warning("".join(["Invalid signature passed to :issue: role: ", self.text,
                                 ". ('AFSIM-<XXX>' expected)"]))
         # Handle invalid signature elegantly. Display the raw text, and don't attempt a link.
         return [nodes.reference(self.rawtext, self.rawtext, refuri=None, **self.options)], []
