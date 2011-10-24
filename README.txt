A tool for managing 3rd party dependencies for projects in git
==============================================================

The idea is that the deps are stored outside of git. We don't care where, so
long as (a) they're accessible through a URL; and (b) the contents of a
particular URL don't change over tuime.

The deps are described by a simple structured text file (format described
below) which *is* versioned in git. We provide a tool which parses this file
into a set of URLs, which it downloads one by one. If the URL points at a zip
file or tarball, we'll extract it automatically after downloading.


Project structure
-----------------

The project should have a root folder for all third party dependencies, which
git is set to ignore. The dependencies will be unzipped in here after
downloading.

A good structure would look something like

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


