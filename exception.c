/**
 * Terry Moreland - tmorelan@uoguelph.ca
 * August 11, 2001
 * -------------------------------------
 * Exception Handler
 * - Ansi C Compliant Java-like Exception Handling
 * - provides macros that mimic java's try, catch and throw statements
 * - sets up signal handlers to translate certain signals such as SIGFPE to
 *   exceptions.  Requires init_exception_handler() to be called inorder to set
 *   up the signal handlers, and Unhandled Exception Handler
 * - provides a print stack trace function to display previously called
 *   functions, similar to how Java's printStackTrace()
 * - Handles Unhandled Exceptions in the same way Java does... print a stack
 *   trace and quit the program
 * - a global enumeration provides constants for signal based, and common
 *   exceptions
 * - *WARNING* try statements must end with a catch statement, the execution
 *   of the program must go through the catch statement before continuing as
 *   a result you can *NOT* return within a try statement or any unhandled 
 *   exception will return to that point and *NOT* the unhandled exception
 *   handler
 *
 *  Copyright (c) 2002 Terry Moreland.  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License  
 *  along with this program; if not, write to the Free Software          
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            
 */

#include <stdio.h>
#include <string.h>
#include "exception.h"

/**
 * Common Exception Names ( for printout )
 *   delcared here to avoid multiple definitions of the variable
 */
char *Exception_Names[] = {
  "NoException",
  "SegmentationFaultException", 
  "AbortException",
  "CpuTimeExceeded",
  "BusErrorException",
  "IllegalInstructionException",
  "BreakPointException",
  "BadSystemCallException",
  "FloatingPointException", 
  "AlarmException",
  "NullPointerException",
  "PreConditionException",
  "IOException",
  "ParseException",
  "IndexOutOfBoundsException",
  "InsufficientArgumentsException",
  "ConsoleInterruptException",
  "UnknownException"
};

/**
 * Prototype(s) that should be in system header files, but don't appear to be
 */
FILE *popen( const char *, const char * );
int  pclose( FILE * );

/**
 * exception_handler
 *   standard exception handler for all the bad signals, or at least the ones
 *   that we consider to be exceptions
 *   in theory this function should have a switch statement to convert signals
 *   to an enumerated exception number
 * Arguments
 *  sig
 *    the signal being handled
 * Return Values
 *   exception_handler is re-registered as the signal handler, I've heard 
 *   bad things happen if this isn't done; I figure it can't hurt.  longjmp
 *   is called which will cause it to go the catch component of the
 *   last try statement.
 */
void exception_handler( int sig ) {
  int    exception;
  size_t size;
  int    i;

  signal( sig, exception_handler );
   
  /**
   * Translate Signals to Exceptions
   */
  switch ( sig ) {
    case SIGFPE:  exception = FloatingPointException;      break;
    case SIGSEGV: exception = SegmentationFaultException;  break;
    case SIGABRT: exception = AbortException;              break;
    case SIGBUS:  exception = BusErrorException;           break;
    case SIGILL:  exception = IllegalInstructionException; break;
    case SIGTRAP: exception = BreakPointException;         break;
    case SIGSYS:  exception = BadSystemCallException;      break;
    case SIGALRM: exception = AlarmException;              break;
    case SIGXCPU: exception = CpuTimeExceeded;             break;
    case SIGINT:  exception = ConsoleInterruptException;   break;
    default:      exception = UnknownException;
  }

  /**
   * Clear any previous backtraces
   */
  for ( i=0; i< MAX_STACK_SIZE; i++ )
    _stack[i] = NULL;
  size = backtrace( _stack, MAX_STACK_SIZE ); 

  if ( size < MAX_STACK_SIZE ) 
    _stack[ MAX_STACK_SIZE - 1 ] = NULL; 

  longjmp( __global_env__, exception );

} /* exception_handler */

/**
 * printStackTrace
 *   prints a stack trace, which in theory should display all functions called
 *   prior to this function call
 * Arguments
 *   nil
 * Return Values
 *   prints a stack trace to stderr, eventually if it doesn't already this
 *   should display information like line numbers, and file and function names
 */
void printStackTrace( void ) { 
  size_t i;
  FILE   *fp;
  char   command[255];
  char   filename_and_line_number[255];
  char   function_name[255];
  int    j;
  int    exception;

  try {
    strcpy( filename_and_line_number, "??:0" );
    strcpy( function_name, "??" );

    /**
     * If no Stack Trace is stored obtain one
     */
    if ( _stack[0] == NULL ) {
      size_t size;

      for ( i=0; i< MAX_STACK_SIZE; i++ )
        _stack[i] = NULL;

      size = backtrace( _stack, MAX_STACK_SIZE );
      if ( size < MAX_STACK_SIZE - 1 ) 
        _stack[ MAX_STACK_SIZE - 1 ] = NULL;

    }

    fprintf( stderr, "Stack Trace For %s\n", __program_name__ );

    for (i = 0; _stack[i] != NULL && i < MAX_STACK_SIZE; i++) {
      if ( __program_name__ == NULL )
        continue;
      sprintf(command,"addr2line -e `which %s` -f -C %p", 
              __program_name__, _stack[i]);

      strcpy( filename_and_line_number, "??:0" );
      strcpy( function_name, "??" );
      
      fp = popen( command, "r" );
      if ( fgets( function_name, 255, fp ) == NULL )
        fgets( function_name, 255, fp );
      if ( fgets( filename_and_line_number, 255, fp ) == NULL )
        fgets( filename_and_line_number, 255, fp );
      pclose(fp);
      
      /**
       * Replace \n's with spaces
       */
      for ( j=0; j < strlen(function_name); j++ )
        if ( function_name[j] == '\n' )
          function_name[j] = '\0';
      for ( j=0; j < strlen(filename_and_line_number); j++ )
        if ( filename_and_line_number[j] == '\n' )
          filename_and_line_number[j] = '\0';

      if ( strcmp( function_name, "exception_handler" ) != 0 ) {
        fprintf(stderr, "  %p in %s", _stack[i], function_name ); 
        fprintf(stderr, " at %s\n", filename_and_line_number ); 
      }

    }
  } catch( exception ) {
    e_getMessage( exception, "printStackTrace" );
  }

} /* print_stack_trace */


