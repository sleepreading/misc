Trace alloc
-----------
Purpose:
 
  Implement a allocation check routine that reports the whole
  callstack for each leaked allocation.
 
  Based on the code for ExtendedTrace written by
  Zoltan Csizmadia, zoltan_csizmadia@yahoo.com.
  
Author:
 
  Erik Rydgren, erik@rydgrens.net.
  
Licence:

  MIT (http://www.opensource.org/licenses/mit-license.php)

Usage:

  Include the file tracealloc.cpp in all application modules (dll:s or exe:s) and
  define DETECT_LEAKS in the project settings under C++/preprocessor.
  Also make sure that all modules has dependency on MemLeakFindDll.dll.
 
  If you want checking of overwrites then define DETECT_OVERWRITES
  in the project settings. Change the frequency of the checks by altering the 
  NML_CHECK_EVERY define in traceallocations.cpp.
  
  Compile and run.
  
  If you get multiple defined symbols (overloaded new and delete)
  add linker switch /FORCE:MULTIPLE on the exe and make sure the
  tracealloc new and delete is the ones used. If not, reorder the
  included libraries until they do.

  Memory leak detection works for release builds but debug symbols needs to be
  generated to be able to get stack information.

How it works:

  http://www.codeproject.com/Articles/5735/Memory-leak-finder

Note:

  Normally RtlCaptureStackBackTrace is used but if
  STACKWALK_PRECISE is defined then StackWalk64 is used instead.
  It creates more correct positions and handles more situations
  but it is an order of several magnitudes slower.
 
History:

  2003-12-30 Initial version released on CodeProject
  2004-09-22 Updated code to handle multi dll environments
  2008-10-13 Updated projects to Visual Studio 2008
  2010-03-19 Changes by PEK (thanks man!)
    Fixed bug in GetStackTrace (could write data in no mans land) 
    Changed stack level size to 50
    Fixed bug in LeakDump (output was corrupted).
    Added stack level monitor
    Added defintions to leading and tail-no mans land and set these to new none-identical values
    Added defintion to magic number.
  2012-02-20 Long overdue upgrade to Visual Studio 2010 and some code cleanup
