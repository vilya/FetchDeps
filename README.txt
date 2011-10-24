A tool for managing 3rd party dependencies for projects in git
==============================================================

The idea is that the deps are stored outside of git. We don't care where, so
long as (a) they're accessible through a URL; and (b) the contents of a
particular URL don't change over tuime.

The deps are described by a simple structured text file (format described
below) which *is* versioned in git. We provide a tool which parses this file
into a set of URLs, which it downloads one by one. If the URL points at a zip
file, we'll unzip it automatically after downloading.


Project structure
-----------------

The project should have a root folder for all third party dependencies, which
git is set to ignore. The dependencies will be unzipped in here after
downloading.

A good structure would look like

  Project/
    ... project specific stuff ...
    Thirdparty/   - root for all thirdparty stuff
      bin/        - for thirdparty executables
      include/    - include files for thirdparty libraries; this will be added
                    to the compiler's include path.
      lib/        - thirdparty libraries; this will be added to the compiler's
                    library search path.
      resources/  - other resources, such as test images, which shouldn't be
                    stored in git.

We don't mandate a specific structure though: this is a configuration
management tool, not a build tool. So long as there is a location to extract
stuff into, we're good.


File format
-----------

The file should sit at the root of the project and have a name ending in .deps.
This isn't required: it'll be handled as the default, but it'll be possible to
specify a different file on the command line.

The file itself is a list of URLs, with conditional sections. Tokens are in
capitals, literals are surrounded by quotes and grammar rules are in lower case:

  file ::=  // empty
        |   URL file
        |   condition file

  condition ::= VAR value_list ':' INDENT file DEDENT

  value_list ::=  VALUE
              |   value_list ',' VALUE

The tokens are:

  URL =     // the usual URL syntax.

  VAR =     // any number of letters, digits, underscores and hyphens; no
            // spaces or other punctuation.

  INDENT =  // When the parser detects a line indented relative to the one 
            // above it.

  DEDENT =  // When the parser detects the end of an indented section.

  VALUE =   // String surrounded by double quotes.

Some notes:
- Whitespace is only significant at the start of a line and only in so far as
  it determines the level of indentation.
- The content of a conditional section is determined by indentation.
- Conditional sections can contain other conditional sections.


Semantics of the file
---------------------

The file describes a set of URLs to download. A URL at the top level is one
which is always downloaded. A URL inside a conditional section is downloaded
only if the condition is met. The resulting URLs are a set in the mathematical
sense, so any duplicates will be ignored.

A variable is an identifier for a set of strings. Variables are given values
outside of the file (e.g. on the command line, or picked up from the runtime
environment); there's no way to assign to them inside the file.

Conditions consist of a variable name and a set of one or more values. The test
is a set intersection: if the intersection is empty, the test fails; if it's
non-empty it passes. In other words, if any of the values for a variable are in
the list to the left of the variable name, the condition passes.

If a condition passes, everything inside it is treated as if it appeared at the
top level of the file. URLs are added to the download set and any nested
conditions are evaluated as above. If a condition fails, we ignore everything
nested inside it then carry on as normal after it.

When we get to the end of the file, we've got a set of URLs. We download each
of these in turn to either a default location, or a location specified on the
command line. If the contents of any URL is a zip file, we unzip it after
downloading it; ditto for tarballs.


So where do we store the deps?
------------------------------

Anywhere! So long as they're accessible by a URL, it doesn't matter. It could
very well be another version control system. SVN, for example, provides access
to repository contents via a URL. Or equally it could just be a directory tree
sitting behind a copy of apache.

One thing that's important is that the contents of a URL don't change over
time. Otherwise we'd get something unexpected when trying to rebuild an older
version of our program. An easy way to do this is to make sure that the URL
contains a version number; then a new version of the dependency will naturally
have a different URL.


To do
-----

At the moment we're just printing out the list of matching URLs, we're not
actually downloading anything. Will need to handle this; maybe static link
against libcurl? We'll need to make sure we can handle http, https, ftp and
file protocols as a minimum.

We need a way to keep track of what's been downloaded, so that we can avoid
downloading it again unnecessarily. The simplest way I can think of for this
is to keep a subversion-style cache dir containing an exact copy of everything
that's been downloaded so far. We look in there first before downloading
anything; provide command line options to flush it out; and so on. Another
option would be to store a manifest file for each download, listing out its
contents (and their hashes?). It would be good to see how well git's object
model matches this problem too.

It probably won't be long before we need compound expressions for conditions:
  
  os "win" and bits "64"
  os "win" or bits "64"
  os not "win"

We should try to keep the set of supported operations as small as possible
while remaining useful. We don't want to encourage people to write hideously
complicated conditions. But at the same time, we don't want users to have to
nest stuff really deeply and/or copy 'n' paste lots of stuff in order to get
the deps they need.

It'll probably be easier to support this if I use a proper flex/bison parser
instead of my custom horrible handwritten one...

It might also be a good idea to switch to dynamic allocation for the variable
lists, simply because sooner or later someone is bound to hit those limits.

We'll need a way to configure, per-project, where dependencies get stored. We
should be able to sue this to automatically figure out a local filename for
each dependency. The things we'll want to configure are:
- Whether we unzip dependencies into the same location, or a separate location.
  If we use separate locations for each dep url, this will make it easier to
  upgrade individual dependencies but will add complication to build scripts.
  Maybe we could keep a list of the files added by each dependency, so that we
  could uninstall exactly? --> Actually that would be good because we can flag
  an error if two dependencies install the same file into the same location.
- Where to store the unzipped dependencies.
- Which folder to use as temporary storage while downloading and unzipping.

