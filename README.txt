A tool for managing platform-specific dependencies in a DVCS
============================================================

Although in most respects DVCSs are a big step forward from traditional
centralised version control systems like Subversion and CVS, one thing they do
poorly by comparison is managing platform-specific things like 3rd party
libraries and tools. Adding things like that to version control is nice because
it helps ensure that builds are repeatable. However people generally only want
the copy of these things that is relevant to the platform they're developing on;
if you check them all in to version control, then everyone gets all the copies
for all platforms - which can waste a lot of space and bandwidth, as well as
making repository operations take longer.

The observation here is that, unlikes source code, for a platform-specific
dependency (dep, from know on) you don't generally care about the version
history: you just need to be able to get the right version for your code.

So based on that, the idea behind this tool is to store the deps somewhere
outside the version control system. We don't care where exactly, so long as
(a) each dep is accessible through a URL; and (b) the contents of the URL
doesn't change over time. This could be a subversion server; a regular
webserver; or even a set of known locations on a shared file system.

All we keep in our DVCS is a simple structured text file (format described
below) which lists the URLS we depend on for each platform. We provide a tool
which parses this file into a set of URLs that it then downloads. If the URL
points at a zip file or tarball, we'll extract it automatically after
downloading.


Project structure
-----------------

The project should have a root folder for all third party dependencies, which
the DVCS is set to ignore. The dependencies will be unzipped in here after
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
                    stored in the DVCS.

We don't mandate a specific structure though: this is a configuration
management tool, not a build tool. So long as there is a location to extract
stuff into, we're good.


File format
-----------

The file should sit at the root of the project and have a name ending in .deps.
This isn't required: it'll be handled as the default, but it'll be possible to
specify a different file on the command line.

An example file might look like this:

  http://myserver/myproject/artwork-1.2.3.zip

  os "win":
    http://myserver/myproject/directx-wrapper-1.1.1.zip

  os "linux", "mac":
    http://myserver/myproject/opengl-wrapper-2.2.2.tar.gz

The first line is a plain URL. Because it's not inside a conditional section, it
will be downloaded for all platforms. The next line is a condition which checks
if the 'os' variable is set to "win". If it is, the URL nested inside it will be
downloaded as well. The next section is another condition, this time checking
whether the 'os' variable is set to either "linux" or "mac"; if it has either of
those values, the nested URL will be downloaded.

The list of strings in the second condition is a literal string set. Variables
also hold sets of strings, not just a single value. The condition is checking
whether the intersection of the two sets is non-empty. If the intersection is
non-empty then whatever's inside the condition is treated as if it appeared at
the top level of the file. If the intersection is empty, whatever's inside the
condition is ignored.

The file uses indentation to show what's inside a condition. It doesn't matter
how many spaces you indent by, so long as all the things inside the condition
are indented by the same amount. We only recognise spaces as an indentation
character; no tabs.

It's possible to nest conditions inside each other. For example, if you need
different copies of a library for 32 vs. 64 bit linux, you could write it like
this:

  os "linux":
    bits "64":
      http://myserver/myproject/mylibrary-linux-64-1.2.3.tar.gz
    bits not "64":
      http://myserver/myproject/mylibrary-linux-any.3.2.1.tar.gz

The 'bits not 64' line shows a slightly more complicated expression. You could
also write the above as

  os "linux":
    bits = "64":
      http://myserver/myproject/mylibrary-linux-64-1.2.3.tar.gz
    bits != "64":
      http://myserver/myproject/mylibrary-linux-any.3.2.1.tar.gz

Which will give identical results.

It's possible to combine conditions using 'and' and 'or', with the usual
semantics. For example both of the following are valid:

  os = "linux", "mac" and bits != "32"
  os "linux" or os "mac" and bits not "32"

The 'and' and 'or' operators are left associative and have the same precedence,
so will be evaluated in the order they're found. 


Simplified grammar for the file format
--------------------------------------

Tokens are in capitals, literals are surrounded by quotes and grammar rules are
in lower case. Other symbols are borrowed from regular expression syntax:
parentheses to denote grouping, * for zero or more of the previous item, |
meaning one or the other of the left and right values.

The (simplified) grammar productions are:

  file ::=                  block

  block ::=                 (URL | conditional_section)*

  conditional_section ::=   relation ((AND|OR) relation)* ':' INDENT block DEDENT

  relation ::=              VAR ('not' | '=' | '!=')? str_value

  str_value ::=             STR (',' STR)*

The tokens are:

  URL =     // the usual URL syntax.

  VAR =     // any number of letters, digits, underscores and hyphens; no
            // spaces or other punctuation.

  INDENT =  // When the parser detects a line indented relative to the one 
            // above it.

  DEDENT =  // When the parser detects the end of an indented section.

  STR =     // String surrounded by double quotes.

Some notes:
- Whitespace is only significant at the start of a line and only in so far as
  it determines the level of indentation.
- Tabs aren't allowed for indentation.


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

