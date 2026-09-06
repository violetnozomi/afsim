# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

''' WSF Sphinx extension

todo: document directives here

'''
import re
from docutils.parsers.rst import directives
from sphinx.domains import Domain, ObjType, Index
import sphinx.roles as roles
from sphinx.locale import _
from sphinx.directives import ObjectDescription
from sphinx import addnodes
import sys
from docutils import nodes
from sphinx.util.nodes import make_refnode
from docutils.transforms import Transform
from docutils.parsers.rst import Directive
from docutils.nodes import fully_normalize_name as normalize_name
from sphinx.util import logging

logger = logging.getLogger(__name__)

def get_env(directive):
    return directive.env
    #return directive.state.document.settings.env

def get_temp(directive):
    temp = directive.env.temp_data
    if 'wsf:command' not in temp:
        temp['wsf:command']=[]
    return temp

'''
    TODO:
    -Reliability of the code is questionable
        -Make sure that invalid inputs don't crash, and do report errors
        -Make sure that appropriate things are 'pickled' so that
            incremental documentation builds don't fail
        -Quiet all the print messages
    -Sphinx can generate multiple indexes, we would like a command index, script class index, and maybe a method index
        (I haven't investigated this yet)
    -Probably need more directives for things I haven't thought of yet
        and make the output prettier

    ---- far-future ----
    -Sphinx supports code blocks, can we get an WSF code block working with syntax highlighting?
        -maybe use wsf_parser project
    -Make things output prettier

'''

class WSFObject(ObjectDescription):
    ''' mostly boiler-plate code stolen from C's extension...
        of questionable use
    '''
    def add_target_and_index(self, name, sig, signode):
        return
        targetname = name
        if targetname not in self.state.document.ids:
            #print 'WSFObject: adding node names', targetname
            signode['names'].append(targetname+"obj")
            if not len(signode['ids']):
                signode['ids'].append(targetname)
            signode['first'] = (not self.names)
        #indextext = name + " ( " + self.get_current_class() + " method )" #self.get_index_text(name)
        #if indextext:
        #    self.indexnode['entries'].append(('single', indextext,
        #                                      targetname, ''))

class WSFIndexEntry:
    ''' WSFIndexEntry -- our in-memory index

     Commands are stored in a layered index
        dis_interface
           - unicast
           - multicast
       platform
           - side
           - location

     Script data is stored with Class -> Method
       Vec3
           - x
           - y
    '''

    NextSequenceNumber = 1

    def __init__(self, name, doc):
        self.entityName = name
        self.doc = doc
        self.children = None
        self.parent = None
        self.sequence_number = WSFIndexEntry.NextSequenceNumber
        WSFIndexEntry.NextSequenceNumber += 1
    def add_child(self, child):
        if self.children is None: self.children = {}
        self.children[child.entityName] = child
        child.parent = self
        return child
    def get_child(self, name):
        if self.children is None: return None
        if name in self.children: return self.children[name]
        return None
    def get_or_add_child(self, name, doc):
        c = self.get_child(name)
        if c is None:
            c = self.add_child(WSFIndexEntry(name, doc))
        return c
    def debug_print(self, tab=''):
        print tab, self.entityName, '('+str(self.doc)+')'
        if self.children:
            for k,v in self.children.iteritems():
                v.debug_print(tab+' ')

    def split_cmd_block(self):
        if hasattr(self, 'sigs'):
            for sig in self.sigs:
                words = sig.split(' ')
                if len(words)>1 and words[-1] == 'end_' + words[0]:
                    return ' '.join(sig.split()[:-1]), sig.split()[-1]
        return None

    def collect_sigs(self, depth, showThis=True):
        if depth == 0: return []
        subtree = []
        if showThis:
            blockParts = self.split_cmd_block()
            if blockParts:
                subtree += [(depth, blockParts[0].strip().rstrip('.'), self)]
            else:
                if hasattr(self, 'sigs'):
                    for sig in self.sigs:
                        subtree += [(depth, sig, self)]
        if self.children and depth > 1:
            children = list(self.children.iteritems())
            children.sort(key=lambda item: item[1].sequence_number)
            for k,v in children:
                # Don't show models in this list
                if hasattr(v, 'is_model') and v.is_model:
                    continue
                # Ignore children with an underscore for a name
                # otherwise recurse
                if k != '_':
                    # if a child starts with an underscore, don't change depth
                    # because it's a category not a subcommand
                    newDepth = depth - 1
                    if k.startswith('_'):
                        newDepth = depth
                    subtree.extend(v.collect_sigs(newDepth))
        if showThis and blockParts:
            subtree += [(depth, blockParts[1], None)]

        return subtree

    def get_full_path(self):
        if not self.entityName: return ''
        if self.parent:
            pp = self.parent.get_full_path()
            if len(pp):
                return pp + '.' + self.entityName
        return self.entityName

    def get_parent_path(self):
        if not self.parent: return ''
        return self.parent.get_full_path()

    # Remove all nodes in tree which are documented on a specific page
    def prune_page(self, docname):
        if self.children:
            pruned = []
            for name, child in self.children.iteritems():
                if child.prune_page(docname):
                    pruned.append(name)
            for c in pruned:
                del self.children[c]
            return len(self.children) == 0
        else:
            return self.doc == docname


# A marker to be added to the rst doctree indicating
# we want to compute and replace this node with a listing of methods or commands
class WSFIndexMarker(nodes.General, nodes.Element):
    pass


class WSFClass(WSFObject):
    ''' A directive for a script class
        -Adds the class to our index
        -Set the current class name (for method directive use)
        -Links to inherited class

        Syntax:
        .. class:: MyClass inherites AnotherClass

            A description of the class

    '''
    option_spec = {
        'constructible': directives.flag,
        'cloneable': directives.flag,
        'container': directives.flag,
        'arrow': directives.flag,
    }

    def add_class_to_index(self, className, doc):
        classNode = self.env.domaindata['wsf']['script'].get_or_add_child(className, doc)

    def get_index_text(self, modname, name):
        return 'this-is-index'

    def handle_signature(self, sig, signode):
        words = sig.split()
        base = None
        name = words[0]
        if len(words)>1:
            if words[1] == 'inherits':
                base= words[2]
        self.env.temp_data['wsf:prefix'] = name
        #JAJ signode += addnodes.desc_name(name, 'class ' + name)
        signode += nodes.Text(name, 'class ' + name)
        #signode['ids'] = [name]
        self.add_class_to_index(name, self.env.docname)
        # Link to base class if there is one
        if base:
            #JAJ signode += addnodes.desc_annotation("  inherits ", "  inherits ")
            signode += nodes.Text("  inherits ", "  inherits ")
            xref = addnodes.pending_xref('', refdomain='wsf', reftype='class', reftarget=base, modname=None, classname=None)
            xref += nodes.Text(base,base)
            signode += xref

        details = []
        if 'constructible' in self.options:
            details += [[nodes.strong(text=" Construction: "), nodes.literal(text=name + " newObj = " + name + "();")]]
        if 'cloneable' in self.options:
            details += [[nodes.strong(text=" Clone: "), nodes.literal(text=name+ " newObj = " + name + "(other);")]]
        if 'container' in self.options:
            details += [[nodes.strong(text=' Container')]]
        if 'arrow' in self.options:
            details += [[nodes.strong(text=' Variable Access: '), nodes.literal(text='obj->VarName')]]

        if len(details):
            extraDetail = nodes.paragraph()
            for n in details:
                L = nodes.line()
                for i in n: L+=i
                extraDetail += L
            signode += extraDetail

        return name


class WSFCommandListing(Directive):
    ''' A directive to insert a listing of commands
        .. commandlisting:: my_command              -- lists all sub-commands below my_command
        or
        .. commandlisting:: page:PageName           -- lists all commands defined on a page

    '''
    required_arguments = 1
    has_content = False
    option_spec = {'depth': directives.nonnegative_int,
                   'indent': directives.nonnegative_int,
                  }

    def run(self):
        # Just insert a marker node, and take care of it later
        self.env = self.state.document.settings.env
        marker = WSFIndexMarker()
        marker.WSFCommand = self.arguments[0].strip()
        if marker.WSFCommand == '.':
            marker.WSFCommand = ''
        marker.depth = 2
        if 'depth' in self.options:
            marker.depth = self.options['depth']
        marker.indent = 0
        if 'indent' in self.options:
            marker.indent = self.options['indent']

        return [marker]

class WSFMethodListing(Directive):
    ''' A directive to insert a listing of methods
        ..methodlisting:: MyClass
    '''
    required_arguments = 1
    has_content = False

    def run(self):
        self.env = self.state.document.settings.env
        marker = WSFIndexMarker()
        marker.WSFClassname = self.arguments[0]
        return [marker]


class WSFMethod(WSFObject):
    ''' A directive to document a script method.
        Overloaded methods may specify additional signatures on multiple lines

        .. method::  double MethodName(double x, OtherClass y)
                     int    MethodName(int x, OtherClass y)

           description...
    '''
    RE_TEMPLATE_ARGS = re.compile(r'(<[^>]*>)')
    def get_index_text(self, name):
        return 'method ' + name

    def add_method_to_index(self, className, methodName, docname, sig):
        #print 'adding method to index:',className, methodName, docname
        classNode = self.env.domaindata['wsf']['script'].get_child(className)
        if classNode:
            idx = classNode.get_child(methodName)
            if not idx:
                idx = WSFIndexEntry(methodName,docname)
                idx.sigs = []
                classNode.add_child(idx)
            idx.sigs += [sig]
        else:
            print 'couldn''t add method ' + methodName + 'to index because class wasn''t found'

        # To perform clear() (for partial re-builds), remember which pages document which methods
        byPage = self.env.domaindata['wsf']['classes_by_page']
        if docname not in byPage:
            byPage[docname]=[]
        if className not in byPage[docname]:
            byPage[docname].append(className)

    def make_typeref(self, typename):
        typeRef = addnodes.pending_xref('', refdomain='wsf', reftype='class', reftarget=typename, modname=None, classname=None)
        typeRef += nodes.Text(typename, typename)
        return typeRef

    def handle_signature(self, sig, signode):
        env = self.env

        if not 'wsf:prefix' in env.temp_data:
            raise ValueError('no active class')

        className = env.temp_data['wsf:prefix']

        #print("HS:",sig,signode)
        bp=sig.find('(')
        ep=sig.find(')')
        if bp==-1 or bp>=ep:
            raise ValueError()

        def splitTemplate(remainder):
            bi = remainder.find('<')
            ei = remainder.find('>')
            if ei == -1: return remainder,''
            if bi < 0 or bi > ei: return (remainder[:ei+1], remainder[ei+1:])
            L,(RL,RR) = remainder[:bi+1], splitTemplate(remainder[bi+1:])
            ei = RR.find('>')
            if ei == -1: return remainder,''
            return (L+RL+RR[:ei+1], RR[ei+1:])

        def subTemplateCommas(args):
            beginTemplate = args.find('<')
            if beginTemplate == -1:
                return args
            L,R = splitTemplate(args[beginTemplate+1:])
            return args[:beginTemplate+1] + L.replace(',','`') + subTemplateCommas(R)


        sigArgsText = subTemplateCommas(sig[bp+1:ep])
        # Split out args, sub backticks for commas again
        args = [w.strip().replace('`',',') for w in sigArgsText.split(',') if len(w.strip())]

        rtype = sig[:bp].rsplit(' ',1)[0]
        name = sig[:bp].rsplit(' ',1)[1]
        signode['ids'] = [className + '.' + name]
        self.add_method_to_index(className, name, self.env.docname, sig)
        #signode += addnodes.desc_addname(className + '.' + name)
        # process 'static' and return type
        for w in rtype.split():
            if w=='static':
                #JAJ signode += addnodes.desc_annotation(w,w+' ')
                signode += nodes.Text(w+' ',w+' ')
            else:
                signode += self.make_typeref(w)
                signode += nodes.Text(' ', ' ')
                #addnodes.desc_type(w,w+' ')
        #JAJ signode += addnodes.desc_name(name,name)
        signode += nodes.Text(name,name)

        # do parameters
        paramlist = addnodes.desc_parameterlist()
        for p in args:
            argType, argName = p.rsplit()
            pnode = addnodes.desc_parameter('','')

            typeRef = self.make_typeref(argType)
            pnode += typeRef

            #JAJ pnode += nodes.emphasis(' '+argName, u'\xa0'+argName)
            pnode += nodes.Text(' '+argName, u'\xa0'+argName)
            paramlist += pnode

        #print 'gotmethod: ', sig
        signode += paramlist
        #print(sig[:bp], ',',rtype,',',name,',', sig)
        return name
        #self.sig=sig

class WSFArgumentType(WSFObject):
    def add_to_index(self, sig):
        self.env.domaindata['wsf']['argtype'].add_child(WSFIndexEntry(sig,self.env.docname))

    def handle_signature(self, sig, signode):
        toks = sig.split()
        argtype = toks[0]
        self.add_to_index(argtype)
        signode['ids'] = [argtype]
        #JAJ signode += addnodes.desc_name(argtype, argtype)
        signode += nodes.Text(argtype, argtype)

        if len(toks)>1:
            #JAJ signode += nodes.Text(' : ', ' : ')
            signode += nodes.Text(' ', ' ')
        for t in toks[1:]:
            if t.startswith('<') and t.endswith('>'):
                xref = addnodes.pending_xref('', refdomain='wsf', reftype='argtype', reftarget=t[1:-1], modname=None, classname=None)
                # An 'emphasis' node rather than a 'Text' node is used so the text is bracketed in <em> </em>.
                # The CSS is defined to render this as italic. This makes it like the wiki where all variable
                # input (items in '<...>')') was by convention was rendered italic.
                xref += nodes.emphasis(' '+t,' '+t)
                signode += xref
            else:
                #JAJ signode += addnodes.desc_name(t,' '+t)
                signode += nodes.Text(t,' '+t)

        return sig

# .. model: modelType modelName
class WSFModel(WSFObject):
    def add_target_and_index(self, name, sig, signode):
        pass

    # inserts the command signature in the index
    def add_cmd_to_index(self, modelType, modelName):
        modelNode = self.env.domaindata['wsf']['models'].get_or_add_child(modelType, self.env.docname)
        modelNode.get_or_add_child(modelName, self.env.docname)

        self.env.domaindata['wsf']['allmodels'].get_or_add_child(modelName, self.env.docname)

        # Mark the command node (the parent of any commands for this model) as being a model
        cmdNode = self.env.domaindata['wsf']['commands'].get_or_add_child(modelName, self.env.docname)
        cmdNode.is_model = True

    def before_content(self):
        self.cmd=None

    def handle_signature(self, sig, signode):
        temp = self.env.temp_data
        toks = sig.split()
        if len(toks) != 2:
            self.state_machine.reporter.warning('model must have two arguments, arguments given: %s, ' % sig, line=self.lineno)
            modelType = 'none'
            modelName = 'none'
        else:
            modelType = toks[0]
            modelName = toks[1]

        cmd = sig
        self.cmd = cmd

        self.add_cmd_to_index(modelType, modelName)
        signode['ids'].append(modelName)
        #signode += addnodes.desc_signature('command:'+'.'.join(currentCmds))
        #JAJ signode += addnodes.desc_name(sig, sig)
        signode += nodes.Text(sig, sig)

        # Set the current block as this model
        # Subsequent commands will be nested in this model
        temp['wsf:command'] = [modelName]

        self.addedCommand=True

        return sig

    def after_content(self):
        pass

# .. demo: demoName
class WSFDemo(WSFObject):
    def add_target_and_index(self, name, sig, signode):
        pass

    # inserts the demo signature in the index
    def add_demo_to_index(self, demoName):
        demoNode = self.env.domaindata['wsf']['demos'].get_or_add_child(demoName, self.env.docname)
        demoNode.get_or_add_child(demoName, self.env.docname)

        self.env.domaindata['wsf']['alldemos'].get_or_add_child(demoName, self.env.docname)

    def before_content(self):
        pass

    def handle_signature(self, sig, signode):
        demoName = sig

        self.add_demo_to_index(demoName)
        signode['ids'].append(demoName)
        signode += addnodes.desc_name(sig, sig)

        return sig

    def after_content(self):
        pass

'''
Command blocks
--------------

    Command blocks define the context for subcommands.  There are several top-level commands, like
    end_time, platform_availability, platform, sensor, etc...  But most commands are nested in other
    blocks.  'icon' for instance is only valid in the platform block.  To index commands correctly
    and provide unambiguous links we address commands using their outer block names like this:

        platform.icon
        dis_interface.connections.port

    Some blocks can be used many places, like 'receiver ... end_receiver'.  In these cases,
    we use a dummy hidden block named '_'.  Linking to a receiver command looks like this:

        _.receiver.frequency

    Usually the name of a block is the same command used to initiate the block, but this isn't
    always possible.  The block name can be any alphanumeric string with no spaces.

    Linking to commands is accomplished with the :command: role:

        :command:`_receiver.frequency`
        :command:`platform`
        :command:`platform.icon`

    The easiest way to declare a command block is to use the :block: option of the .. command:: directive:

        .. command:: platform <name> <type> ... end_platform
            :block:

            Creates a platform ...

            .. command:: icon <name>

                sets the platform's icon

    Here, the :block: option adds the 'platform' block to any following commands.  (regardless of indent level)
    Switching to a block can also be accomplished with the .. block:: directive:

        .. block:: platform

        .. command:: icon <name>
            ...

    .. block:: throws away any current block and sets the new current block.

    Alternatively, .. begin:: can be used to append to the current block:

        .. begin:: WSF_GEOMETRIC_SENSOR

        .. begin:: mode

            // current block is WSF_GEOMETRIC_SENSOR.mode, document mode commands here

        .. end::

            // current block is WSF_GEOMETRIC_SENSOR, continue documenting WSF_GEOMETRIC_SENSOR commands here

        .. end::

            // no current block

    Commands are usually addressed by the first word in their signature, ie, icon, port, position, frequency.
    But sometimes it is necessary to choose a different address to resolve ambiguity.  For instance:

        // in platform:

        .. command:: add sensor <name> ....
            :id: add_sensor

        .. command:: add mover <name> ...
            :id: add_mover

        Here are some add commands:  :command:`platform.add_sensor`, :command:`platform.add_mover`

'''


# .. block::  outer.inner.inner_inner...
class WSFBlock(Directive):
    required_arguments = 1
    has_content = False
    def run(self):
        env = self.state.document.settings.env
        sig = self.arguments[0].strip().split('.')
        env.temp_data['wsf:command']=sig
        return []

# .. begin:: outer
class WSFBeginBlock(Directive):
    required_arguments = 1
    has_content = False
    def run(self):
        env = self.state.document.settings.env
        temp = env.temp_data
        sig = self.arguments[0].strip().split('.')
        current = temp['wsf:command'] if 'wsf:command' in temp else []
        current += sig
        temp['wsf:command'] = current
        return []

# .. end::
class WSFEndBlock(Directive):
    required_arguments = 0
    has_content = False
    def run(self):
        env = self.state.document.settings.env
        temp = env.temp_data
        cmd = temp['wsf:command']
        if cmd and len(cmd):
            temp['wsf:command'] = cmd[:-1]
        return []

# For links to commands
class WSFCommandRole(roles.XRefRole):
    def process_link(self, env, refnode, has_explicit_title, title, target):
        from sphinx.util import ws_re
        # only show the command, not the full path
        return title.split('.')[-1], ws_re.sub(' ', target)

# Implements the .. command::   behavior
#
# .. command::  my_command  <parameters>
#               my_command  <alternate> <signature>
#
#    Description here...
#
class WSFCommand(WSFObject):

    option_spec = {'class': directives.class_option,
                   'hidden': directives.flag,
                   'block': directives.flag,
                   'id': directives.uri }

    def add_target_and_index(self, name, sig, signode):
        pass

    # inserts the command signature in the index
    def add_cmd_to_index(self, cmdList, sig):
        parentCmd = self.env.domaindata['wsf']['commands']
        for c in cmdList:
            nextParent = parentCmd.get_child(c)
            if not nextParent:
                nextParent = parentCmd.add_child(WSFIndexEntry(c,self.env.docname))
                nextParent.sigs = []
            parentCmd = nextParent

        if sig not in parentCmd.sigs:
            parentCmd.sigs.append(sig)

        # add top-level commands to an index by page
        # for use by commandlisting
        if len(cmdList) == 1:
            root = self.env.domaindata['wsf']['commands_by_page']
            pageRoot = root.get_child(self.env.docname)
            if not pageRoot:
                pageRoot = root.add_child(WSFIndexEntry(self.env.docname, self.env.docname))
                pageRoot.contained_commands = []
            pageRoot.contained_commands.append(cmdList[0])

    def before_content(self):
        self.cmd=None
        temp = self.env.temp_data
        if 'wsf:command' not in temp:
            temp['wsf:command']=[]

    def handle_signature(self, sig, signode):
        doc = self.state.document.settings.env.docname
        def sub_inline_ref(prefixText, role, target, linkText=None):
            if role in ['command','class','method','model']:
                xref = addnodes.pending_xref('', refdomain='wsf', reftype=role, reftarget=target, modname=None, classname=None)
                if linkText is None:
                    linkText = target.split('.')[-1]
                #JAJ xref += nodes.emphasis( ' '+linkText,' '+linkText)
                xref += nodes.Text( ' '+linkText,' '+linkText)
                return xref
            if role == 'ref':
                xref = addnodes.pending_xref('', refdomain='std', reftype=role, reftarget=target, modname=None, classname=None)
                xref['refexplicit'] = False
                xref['refdoc'] = doc
                if linkText is None:
                    linkText = target
                #JAJ xref += nodes.emphasis(' '+linkText,' '+linkText)
                xref += nodes.Text(' '+linkText,' '+linkText)
                return xref
            if role == 'commandlisting' and linkText is None:
                # :commandlisting:`zone`
                # :commandlisting:`zone;depth 4`
                indentCount = 0
                for c in prefixText[::-1]:
                    if c == '\n':
                        break
                    else: indentCount += 1
                parts = target.split(';')
                marker = WSFIndexMarker()
                marker.WSFCommand = parts[0]
                marker.depth = 3
                for p in parts[1:]:
                    if p.startswith('depth '):
                        marker.depth = int(p[6:])
                marker.indent = indentCount

                return marker
            print('bad inline markup', role, target)
            return None

        # use first form of links:   :role:`target`
        def sub_content(c):
            if type(c) is type(u''):
                # Splitting with 3 captures, getting a array of length N*3
                sp = re.split(WSFInputs.inlineRef, c)
                for i in range(1,len(sp),3):
                    sp[i] = sub_inline_ref(sp[i-1], sp[i], sp[i+1])
                    sp[i+1] = None
                return [s for s in sp if s is not None]
            return [c]

        # use second form of links:   :role:`Link Text <target>`
        def sub_content2(c):
            if type(c) is type(u''):
                # Splitting with 3 captures, getting a array of length N*4
                sp = re.split(WSFInputs.inlineRefWithLabel, c)
                for i in range(1,len(sp),4):
                    sp[i] = sub_inline_ref(sp[i-1], sp[i], sp[i+2], sp[i+1])
                    sp[i+1] = None
                    sp[i+2] = None
                return [s for s in sp if s is not None]
            return [c]

        def sub_local_links(c):
            if type(c) is type(u''):
                sp = re.split(WSFInputs.localLink, c)
                for i in range(1,len(sp),3):
                    target = sp[i] if sp[i] else sp[i+1]
                    reference_node = nodes.reference('|' + target, '')
                    reference_node['refname'] = normalize_name(target)
                    #self.document.note_refname(reference_node)
                    #reference_node += subref_node
                    #xref = addnodes.pending_xref('', reftype=role, reftarget=target, modname=None, classname=None)
                    #JAJ reference_node += nodes.emphasis(' '+target,' '+target)
                    reference_node += nodes.Text(' '+target,' '+target)
                    sp[i] = reference_node
                    sp[i+1] = None
                return [s for s in sp if s is not None]
            return [c]

        def sub_content_list(cs):
            import itertools
            cs = list(itertools.chain(*[sub_content(c) for c in cs]))
            cs = list(itertools.chain(*[sub_content2(c) for c in cs]))
            cs = list(itertools.chain(*[sub_local_links(c) for c in cs]))
            return cs

        def fix_content(contentText):
            content = [contentText]
            content = sub_content_list(content)
            contentNodes = []
            for c in content:
                if type(c) == type(u''):
                    contentNodes.append(nodes.Text(c,c))
                else:
                    contentNodes.append(c)
            return contentNodes

        temp = self.env.temp_data
        # TODO: we can parse argument types, and allow automatic linking to commonly used argument types
        #   ex:   <time-value>   should link to a description of valid time units
        # cmd_name <param> tok <param> [<option> | <option>]
        toks = sig.split()
        cmd = toks[0]

        if 'wsf:command' not in temp:
            temp['wsf:command']=[]
        currentCmds = temp['wsf:command']

        # Let the user override the command id
        cmdId = cmd
        if 'id' in self.options:
            cmdId = str(self.options['id'])

        self.cmd = cmdId

        thisCmd = currentCmds + [cmdId]
        self.add_cmd_to_index(thisCmd, sig)
        for i in range(len(thisCmd)):
            signode['ids'].append('.'.join(thisCmd[i:]))

        #signode += addnodes.desc_signature('command:'+'.'.join(currentCmds))
        if 'hidden' in self.options:
            #JAJ signode += addnodes.desc_name('','')
            signode += nodes.Text('','')
        else:
            #JAJ signode += addnodes.desc_name(cmd,cmd)
            signode += nodes.Text(cmd,cmd)

        if 'block' in self.options:
            temp['wsf:command'] = thisCmd

        for t in toks[1:]:
            if t.startswith('<') and t.endswith('>'):
                xref = addnodes.pending_xref('', refdomain='wsf', reftype='argtype', reftarget=t[1:-1], modname=None, classname=None)
                #xref += nodes.Text(' '+t,' '+t)
                # An 'emphasis' node rather than a 'Text' node is used so the text is bracketed in <em> </em>.
                # The CSS is defined to render this as italic. This makes it like the wiki where all variable
                # input (items in '<...>')') was by convention was rendered italic.
                xref += nodes.emphasis(' '+t,' '+t)
                signode += xref
            elif t.startswith(':'):
                # note that this will not handle spaces...due to tokenizing
                signode += fix_content(t)
            else:
                #JAJ signode += addnodes.desc_name(t,' '+t)
                signode += nodes.Text(' '+t,' '+t)

        self.addedCommand=True

        if cmdId not in self.state.document.ids:
            signode['names'].append(cmdId)
            #signode['ids'].append(cmd)
            #signode['first'] = (not self.names)
            self.state.document.note_explicit_target(signode)
            #inv = self.env.domaindata['c']['objects']
            #if name in inv:
            #    self.state_machine.reporter.warning(
            #        'duplicate C object description of %s, ' % name +
            #        'other instance in ' + self.env.doc2path(inv[name][0]),
            #        line=self.lineno)
            #inv[name] = (self.env.docname, self.objtype)
        if len(thisCmd) == 1:
            self.indexnode['entries'].append(('single', thisCmd[0], thisCmd[0], ''))
        return cmd
    def after_content(self):
        pass


class WSFInputs(Directive):
    ''' A example input block.  Similar to a standard sphinx code block,
        but inline links are converted.
        Sphinx does not have this feature, so we have to convert links manually...
    '''
    # Links like this:    :command:`end_time`
    #               or    :ref:`blah`
    # also supports       :commandlisting:`zone`
    inlineRef = re.compile(r':([a-z]+):`([a-zA-Z_0-9. \-;]+)`')

    # Links like this:    :command:`End Time <end_time>`
    inlineRefWithLabel = re.compile(r':([a-z]+):`([a-zA-Z_0-9. \-;]+)<([^>`]+)>`')

    # Links with a trailing underscore:
    #       like this:    my_ref_
    localLink = re.compile(r'(?:([a-zA-Z0-9_]+)|`([a-zA-Z0-9_.]+)`)_(?=\s)')

    required_arguments = 0
    optional_arguments = 0
    #final_argument_whitespace = True
    option_spec = {'class': directives.class_option,
                   'name': directives.unchanged}
    has_content = True

    #node_class = nodes.topic

    def run(self):
        doc = self.state.document.settings.env.docname
        def sub_inline_ref(prefixText, role, target, linkText=None):
            if role in ['command','class','method','model','demo']:
                xref = addnodes.pending_xref('', refdomain='wsf', reftype=role, reftarget=target, modname=None, classname=None)
                if linkText is None:
                    linkText = target.split('.')[-1]
                xref += nodes.emphasis(linkText,linkText)
                return xref
            if role == 'ref':
                xref = addnodes.pending_xref('', refdomain='std', reftype=role, reftarget=target, modname=None, classname=None)
                xref['refexplicit'] = False
                xref['refdoc'] = doc
                if linkText is None:
                    linkText = target
                xref += nodes.emphasis(linkText,linkText)
                return xref
            if role == 'commandlisting' and linkText is None:
                # :commandlisting:`zone`
                # :commandlisting:`zone;depth 4`
                indentCount = 0
                for c in prefixText[::-1]:
                    if c == '\n':
                        break
                    else: indentCount += 1
                parts = target.split(';')
                marker = WSFIndexMarker()
                marker.WSFCommand = parts[0]
                marker.depth = 3
                for p in parts[1:]:
                    if p.startswith('depth '):
                        marker.depth = int(p[6:])
                marker.indent = indentCount

                return marker
            print 'bad inline markup', role, target
            return None

        # use first form of links:   :role:`target`
        def sub_content(c):
            if type(c) is type(u''):
                # Splitting with 3 captures, getting a array of length N*3
                sp = re.split(WSFInputs.inlineRef, c)
                for i in range(1,len(sp),3):
                    sp[i] = sub_inline_ref(sp[i-1], sp[i], sp[i+1])
                    sp[i+1] = None
                return [s for s in sp if s is not None]
            return [c]

        # use second form of links:   :role:`Link Text <target>`
        def sub_content2(c):
            if type(c) is type(u''):
                # Splitting with 3 captures, getting a array of length N*4
                sp = re.split(WSFInputs.inlineRefWithLabel, c)
                for i in range(1,len(sp),4):
                    sp[i] = sub_inline_ref(sp[i-1], sp[i], sp[i+2], sp[i+1])
                    sp[i+1] = None
                    sp[i+2] = None
                return [s for s in sp if s is not None]
            return [c]

        def sub_local_links(c):
            if type(c) is type(u''):
                sp = re.split(WSFInputs.localLink, c)
                for i in range(1,len(sp),3):
                    target = sp[i] if sp[i] else sp[i+1]
                    reference_node = nodes.reference('|' + target, '')
                    reference_node['refname'] = normalize_name(target)
                    #self.document.note_refname(reference_node)
                    #reference_node += subref_node
                    #xref = addnodes.pending_xref('', reftype=role, reftarget=target, modname=None, classname=None)
                    reference_node += nodes.emphasis(target,target)
                    sp[i] = reference_node
                    sp[i+1] = None
                return [s for s in sp if s is not None]
            return [c]

        def sub_content_list(cs):
            import itertools
            cs = list(itertools.chain(*[sub_content(c) for c in cs]))
            cs = list(itertools.chain(*[sub_content2(c) for c in cs]))
            cs = list(itertools.chain(*[sub_local_links(c) for c in cs]))
            return cs

        def fix_content(contentText):
            content = [contentText]
            content = sub_content_list(content)
            contentNodes = []
            for c in content:
                if type(c) == type(u''):
                    contentNodes.append(nodes.Text(c,c))
                else:
                    contentNodes.append(c)
            return contentNodes


        self.assert_has_content()
        args = ' '.join(self.arguments).strip()

        text = '\n'.join(self.content)

        #self.add_name(node)
        parts = None
        blk = nodes.literal_block()
        blk['classes'] += ['wsfcode']
        for n in fix_content(text):
            blk += n
        return [blk]


class WSFClassIndex(Index):
    """
    Index subclass to provide the script class index.
    """

    name = 'classindex'
    localname = _('Script Class Index')
    shortname = _('classes')

    def generate(self, docnames=None):
        content = {}
        scriptData = self.domain.data['script']
        for key, c in sorted(scriptData.children.iteritems()):
            #        name, groupType, page, anchor, extra, qualifier, description
            name = c.entityName
            entry = [name, 2, c.doc, c.entityName, '', '', '']
            letter = None
            if name.startswith('Wsf'):
                letter = name[3].lower()
            else:
                letter = name[0].lower()
            entries = content.setdefault(letter, [])
            entries.append(entry)

        return sorted(content.iteritems()), False


class WSFCommandIndex(Index):
    """
    Index subclass to provide the command index.
    """

    name = 'commandindex'
    localname = _('Command Index')
    shortname = _('commands')

    def generate(self, docnames=None):
        content = {}
        commandData = self.domain.data['commands']
        visited = set()
        for key, c in sorted(commandData.children.iteritems()):
            name = c.entityName
            if name.startswith('_'): continue
            if len(name)==0: continue
            if name in visited: continue
            if hasattr(c, 'is_model') and c.is_model: continue

            visited.add(name)

            #        name, groupType, page, anchor, extra, qualifier, description
            entry = [name, 2, c.doc, c.entityName, '', '', '']
            letter = name[0].lower()
            entries = content.setdefault(letter, [])
            entries.append(entry)

        return sorted(content.iteritems()), False

class WSFModelIndex(Index):
    """
    Index subclass to provide the models index.
    """

    name = 'modelindex'
    localname = _('Model Index')
    shortname = _('models')

    def generate(self, docnames=None):
        content = {}
        modelData = self.domain.data['models']
        visited = set()
        for kind, kindEntry in sorted(modelData.children.iteritems()):
            letter = kind
            entries = content.setdefault(letter, [])
            for modelName, c in sorted(kindEntry.children.iteritems()):
                name = c.entityName
                #        name, groupType, page, anchor, extra, qualifier, description
                entry = [name, 2, c.doc, c.entityName, '', '', '']
                entries.append(entry)

        return sorted(content.iteritems()), False

class WSFDemoIndex(Index):
    """
    Index subclass to provide the demos index.
    """

    name = 'demoindex'
    localname = _('Demo Index')
    shortname = _('demos')

    def generate(self, docnames=None):
        content = {}
        demoData = self.domain.data['demos']
        visited = set()
        if demoData.children:
           for key, c in sorted(demoData.children.iteritems()):
              name = c.entityName
              if name.startswith('_'): continue
              if len(name)==0: continue
              if name in visited: continue

              visited.add(name)

              #        name, groupType, page, anchor, extra, qualifier, description
              entry = [name, 2, c.doc, c.entityName, '', '', '']
              letter = name[0].lower()
              entries = content.setdefault(letter, [])
              entries.append(entry)

        return sorted(content.iteritems()), False

class WSFDomain(Domain):
    app = None

    # Look up an object in an index
    # we keep 2 index roots -- 'script' and 'commands'
    # fullName is the fully-specified name:
    #       ScriptClass
    #  or   ScriptClass.MethodName
    #  or   top_level_command
    #  or   top_level_command.subcommand_name
    #  or   top_level_command.subblock_name.subcommand_name
    #  etc...
    def lookup_in_index(self, index, fullName):
        if not len(fullName): return index
        for c in fullName.split('.'):
            index = index.get_child(c)
            if not index:
                return None
        return index

    def testResolved(self, app, doctree, docname):
        ''' Called after a document has been resolved.

            Implements .. methodlisting::
            and        .. commandlisting::
            replaces any WSFIndexMarker nodes with listings for methods or commands
        '''
        for node in doctree.traverse(WSFIndexMarker):

            # implement methodlisting
            if hasattr(node, 'WSFClassname'):
                scriptData = self.data['script']
                classData = scriptData.get_child(node.WSFClassname)
                memberNodes = []
                block = nodes.term('','')
                if classData:
                    for depth, sig, child in classData.collect_sigs(2, False):
                        itemText = make_refnode(app.builder, docname, child.doc, child.get_full_path(),
                                    nodes.term(sig, sig),                      # link text
                                    child.entityName)     # hover-text
                        memberFull = node.WSFClassname + '.' + child.entityName

                        memberNodes.append([itemText])
                        block += itemText

                node.replace_self([block])

            # Implement commandlisting
            if hasattr(node, 'WSFCommand'):
                heading = ''
                commandsInListing = []
                cmdRoot = ''
                cmdData = self.data['commands']
                if node.WSFCommand.startswith('page:'):
                    # do by page
                    page = node.WSFCommand[5:]
                    byPage = self.data['commands_by_page']

                    pageNode = byPage.get_child(page)
                    if pageNode:
                        for cmd in pageNode.contained_commands:
                            cmdNode = cmdData.get_child(cmd)
                            commandsInListing.extend(cmdNode.collect_sigs(1))
                else:
                    if node.WSFCommand == '':
                        cmdIndexNode = cmdData
                        showOuterCommand = False
                    if node.WSFCommand.endswith('.'):
                        cmdIndexNode = self.lookup_in_index(cmdData, node.WSFCommand[:-1])
                        showOuterCommand = False
                    else:
                        cmdIndexNode = self.lookup_in_index(cmdData, node.WSFCommand)
                        showOuterCommand = True
                    if cmdIndexNode and cmdIndexNode.children:
                        heading = cmdIndexNode.entityName
                        commandsInListing = cmdIndexNode.collect_sigs(node.depth, showOuterCommand)

                block = nodes.term('','')

                if len(commandsInListing):
                    maxCmdDepth = max([depth for depth,_,_ in commandsInListing])
                    for depth, sig, child in commandsInListing:
                        indent = ((maxCmdDepth - depth) * 3 + node.indent) * u'\xa0'
                        if child:
                            linkText = sig
                            itemText = make_refnode(app.builder, docname, child.doc, child.get_full_path(),
                                    nodes.term(indent+linkText, indent+linkText),                      # link text
                                    linkText + ' (' + child.get_parent_path() + ')')     # hover-text
                        else:
                            itemText = nodes.term(indent+sig,indent+sig)
                        block += itemText

                node.replace_self([block])

    def __init__(self, env):
        super(WSFDomain, self).__init__(env)
        WSFDomain.app.connect('doctree-resolved', self.testResolved)


    name = 'wsf'
    label = 'wsf'
    object_types = {
        'method': ObjType(_('method'), 'method'),
        'class': ObjType(_('class'), 'class'),
        'command': ObjType(_('command'), 'command'),
        'model': ObjType(_('model'), 'model'),
        'block': ObjType(_('block'), 'block'),
        'methodlisting': ObjType(_('methodlisting'), 'methodlisting'),
        'commandlisting': ObjType(_('commandlisting'), 'commandlisting'),
        'argtype': ObjType(_('argtype'), 'argtype'),
        'demo': ObjType(_('demo'), 'demo')
    }
    # 'innernodeclass=nodes.inline' prevents the emitted link from getting
    # bounded by <code> ... </code>
    roles = {
        'method': roles.XRefRole(innernodeclass=nodes.inline),
        'class': roles.XRefRole(innernodeclass=nodes.inline),
        'command': WSFCommandRole(innernodeclass=nodes.inline),
        'model': roles.XRefRole(innernodeclass=nodes.inline),
        'argtype': roles.XRefRole(innernodeclass=nodes.inline),
        'index': roles.XRefRole(innernodeclass=nodes.inline),
        'demo': roles.XRefRole(innernodeclass=nodes.inline),
    }
    directives = {
        'method':   WSFMethod,
        'class':    WSFClass,
        'command':  WSFCommand,
        'block':    WSFBlock,
        'begin':    WSFBeginBlock,
        'end':    WSFEndBlock,
        'model':    WSFModel,
        'methodlisting': WSFMethodListing,
        'commandlisting': WSFCommandListing,
        'argtype': WSFArgumentType,
        'inputs': WSFInputs,
        'demo': WSFDemo,
    }
    indices = [
        WSFClassIndex,
        WSFCommandIndex,
        WSFModelIndex,
        WSFDemoIndex
    ]
    # Data that is kept between runs is initialized with this
    # and accessed through self.env.domaindata['wsf']
    initial_data = {
        'objects': {},  # fullname -> docname, objtype
        'commands': WSFIndexEntry(None,None),
        'models': WSFIndexEntry(None,None),
        'allmodels': WSFIndexEntry(None,None),
        'commands_by_page': WSFIndexEntry(None,None),
        'classes_by_page': {}, #WSFIndexEntry(None,None),
        'script': WSFIndexEntry(None, None),
        'argtype': WSFIndexEntry(None,None),
        'demos': WSFIndexEntry(None,None),
        'alldemos': WSFIndexEntry(None,None),
    }
    def get_objects(self):
        for refname, (docname, type) in list(self.data['objects'].items()):
            yield (refname, refname, type, docname, refname, 1)

    # When doing a partial build, we need to remove old index data here
    def clear_doc(self, docname):
        # Script methods are cleaned up by remembering which pages document which classes
        scriptRoot = self.env.domaindata['wsf']['script']
        classes_by_page = self.env.domaindata['wsf']['classes_by_page']
        if docname in classes_by_page:
            # for each class documented by this document
            for c in classes_by_page[docname]:
                # remove index nodes belonging to this document
                cNode = scriptRoot.get_child(c)
                if cNode:
                    cNode.prune_page(docname)

        cmdsByPage = self.env.domaindata['wsf']['commands_by_page']
        pageRoot = cmdsByPage.get_child(docname)
        if pageRoot:
            cmdRoot = self.env.domaindata['wsf']['commands']
            for cmd in pageRoot.contained_commands:
                cmdNode = cmdRoot.get_child(cmd)
                cmdNode.prune_page(docname)

        demoRoot = self.env.domaindata['wsf']['demos']
        if demoRoot:
           demoNode = demoRoot.get_child(docname)
           if demoNode:
              demoNode.prune_page(docname)

        super(WSFDomain, self).clear_doc(docname)


    def process_doc(self, env, docname, doctree):
        env.temp_data['wsf:command']=[]
        super(WSFDomain, self).process_doc(env, docname, doctree)

    def resolve_xref(self, env, fromdocname, builder,
                     type, target, node, contnode):
        if type=='index':
            if target == 'command':
                return make_refnode(builder, fromdocname, 'wsf-commandindex', 'Command Index', contnode, 'command index')
            if target == 'class':
                return make_refnode(builder, fromdocname, 'wsf-classindex', 'Script Class Index', contnode, 'Script Class Index')
            if target == 'model':
                return make_refnode(builder, fromdocname, 'wsf-modelindex', 'Model Index', contnode, 'Model Index')
            if target == 'demo':
                return make_refnode(builder, fromdocname, 'wsf-demoindex', 'Demo Index', contnode, 'Demo Index')
            return None

        if type=='command' and len(target):
            #print("RESOLVE: ", target, type)
            cmd = self.data['commands']
            cmdNode = self.lookup_in_index(cmd, target)

            if cmdNode is not None:
                #print("XREF: ", cmdNode)
                parts = target.split('.')
                desc = parts[-1]
                if len(parts)>1:
                    desc += ' (' + '.'.join(parts[:-1]) + ')'
                return make_refnode(builder, fromdocname, cmdNode.doc, target,
                            contnode, desc)
            logger.warning("".join(["no xref: ", fromdocname, type, target]))
            #print "no xref: ", fromdocname, type, target
            #cmd.debug_print()
            return None

        if type=='class' and len(target):
            script = self.data['script']
            classNode = self.lookup_in_index(script, target)
            if classNode:
                return make_refnode(builder, fromdocname, classNode.doc, classNode.entityName,
                            contnode, target)
            #print 'no class:', target
            return None
        if type == 'argtype' and len(target):
            argtypes = self.data['argtype']
            argtypeNode = self.lookup_in_index(argtypes, target)
            if argtypeNode:
                return make_refnode(builder, fromdocname, argtypeNode.doc, target, contnode, target)
            return None
        if type=='method' and len(target):
            mth = self.data['script']
            mthNode = self.lookup_in_index(mth, target)
            if mthNode:
                return make_refnode(builder, fromdocname, mthNode.doc, target,
                            contnode, target)
            logger.warning("".join(['no method:', target]))
            #print 'no method:', target
            return None
        if type=='model' and len(target):
            models = self.data['allmodels']
            modelNode = self.lookup_in_index(models, target)
            if modelNode:
                return make_refnode(builder, fromdocname, modelNode.doc, target,
                            contnode, target)
            logger.warning("".join(['no model:', target]))
            #print 'no model:', target
            return None
        if type=='demo' and len(target):
            demos = self.data['alldemos']
            demoNode = self.lookup_in_index(demos, target)
            if demoNode:
                return make_refnode(builder, fromdocname, demoNode.doc, target,
                            contnode, target)
            logger.warning("".join(['no demo:', target]))
            #print 'no demo:', target
            return None

        if target not in self.data['objects']:
            return None

        obj = self.data['objects'][target]
        return make_refnode(builder, fromdocname, obj[0], target,
                            contnode, target)

def setup(app):
    WSFDomain.app = app
    app.add_domain(WSFDomain)

