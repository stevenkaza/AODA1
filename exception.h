/**
 * Terry Moreland - tmorelan@uoguelph.ca
 * August 11, 2001
 *
 * [30-Jan-11] Added SIGINT handling to catch ^C, so as to interrupt
 *	program with infinite loop. This is risky in that there is no
 *	convenient way to fully break out of a misbehaving program.
 *	In exception.c this translates to "ConsoleInterruptException". (R.Schwarting)
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

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__ 1

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>
#include <sys/types.h>

/**
 * #define Constants
 */
#define MAX_STACK_SIZE  10

/**
 * global envrionment 
 *   this variable is declared globablly so the signal handler can jump back
 *   to the current try ... catch statement
 */
jmp_buf __global_env__;
char    *__program_name__;

/**
 * stack
 *   used to store stack trace information
 */
void *_stack[ MAX_STACK_SIZE ];

/**
 * Common Exception Definitions
 */
typedef enum { 
  NoException,
  SegmentationFaultException, 
  AbortException,
  CpuTimeExceeded,
  BusErrorException,
  IllegalInstructionException,
  BreakPointException,
  BadSystemCallException,
  FloatingPointException, 
  AlarmException,
  NullPointerException,
  PreConditionException,
  IOException,
  ParseException,
  IndexOutOfBoundsException,
  InsufficientArgumentsException,
  ConsoleInterruptException,
  UnknownException
} Exceptions;

/**
 * Common Exception Names ( for printout )
 */
extern char *Exception_Names[];
   
/**
 * init_exception_handler
 *   initializes the exception handler
 *   sets up all the signal handlers causing all signals which are to be 
 *   translated to exceptions to call the exception_handler function
 *   the Unhandled Exception Handler is setup to handle any exceptions that
 *   aren't caught
 * Arguments
 *   program_name
 *   programs name, argv[0]
 * Return Values
 *   
 */
#define init_exception_handler( program_name ) {                             \
  int __unhandled;                                                           \
                                                                             \
  __program_name__ = program_name;                                           \
  signal( SIGFPE,  exception_handler );                                      \
  signal( SIGBUS,  exception_handler );                                      \
  signal( SIGABRT, exception_handler );                                      \
  signal( SIGSEGV, exception_handler );                                      \
  signal( SIGILL,  exception_handler );                                      \
  signal( SIGIOT,  exception_handler );                                      \
  signal( SIGTRAP, exception_handler );                                      \
  signal( SIGSYS,  exception_handler );                                      \
  signal( SIGXCPU, exception_handler );                                      \
  signal( SIGINT,  exception_handler );                                      \
                                                                             \
  for ( __unhandled = 0; __unhandled < MAX_STACK_SIZE; __unhandled++ )       \
    _stack[ __unhandled ] = NULL;                                            \
                                                                             \
  if ( ( __unhandled = setjmp( __global_env__ ) ) != 0 ) {                   \
    if ( __unhandled <= UnknownException && __unhandled >= 0 )               \
      fprintf( stderr,                                                       \
               "Unhandled Exception %s Occurred\n",                          \
               Exception_Names[__unhandled] );                               \
    else                                                                     \
      fprintf( stderr,                                                       \
               "Unhandled Exception %d Occurred\n",                          \
               __unhandled );                                                \
    printStackTrace();                                                       \
    exit( __unhandled );                                                     \
  }                                                                          \
}

/**
 * try
 *   opens a try .. catch statement, between which any exception such as bad
 *   signals, or explicitly thrown exceptions are caught
 *   if an exception occurs between the try and catch statement then the program
 *   will drop out of the code block found between the try .. catch and continue
 *   in the catch statement
 * Arguments
 *   nil
 * Return Values
 *   try .. catch statement is opened, *MUST* be closed by a catch( e ) 
 *   statement
 */
#define try {                                                                \
  jmp_buf __prev_env;                                                        \
  int   __exception = 0;                                                     \
  __prev_env[0] = __global_env__[0];                                         \
  if ( !( __exception = setjmp( __global_env__ ) ) ) 

/**
 * throw
 *   explictly throws an exception
 * Arguments
 *  e
 *    the exception to throw, must be an integer
 * Return Values
 *   exception is thrown, if thrown in a try .. catch block the code will 
 *   drop out of the block between the try and catch and continue in the catch
 *   block
 */
#define throw( e ) {                                                         \
  int    i;                                                                  \
  size_t size;                                                               \
                                                                             \
  for ( i=0; i< MAX_STACK_SIZE; i++ )                                        \
    _stack[i] = NULL;                                                        \
                                                                             \
  size = backtrace( _stack, MAX_STACK_SIZE );                                \
  if ( size < MAX_STACK_SIZE - 1 ) _stack[ MAX_STACK_SIZE - 1 ] = NULL;      \
  longjmp( __global_env__, e );                                              \
}
/**
 * catch
 *   closing statement of the try .. catch statement, any exceptions that
 *   occured between the try and the catch are handled here
 *   code in the catch block will not be excecuted unless an exception was
 *   thrown
 * Arguments
 *  e 
 *    storage for the exception value
 * Return Values
 *   try .. catch block is closed
 */
#define catch( e )                                                           \
  e = __exception;                                                           \
  __global_env__[0] = __prev_env[0];                                         \
}                                                                            \
  if ( e != 0 )

/**
 * IS_DEADLY
 *   boolean macro, if the exception is deadly return true, otherwise return
 *   false
 * Arguments
 *  e
 *    the exception
 * Return Values
 *   if Exception is deadly ( bad things happened to the system ) then true
 *   is returned, otherwise false is returned
 *   it is highly recommend that if the exception is deadly that the program be
 *   exited immediately
 */
#define IS_DEADLY( e )                                                       \
  ( ( (e) <= BadSystemCallException && (e) >= 0 ) ? TRUE : FALSE )

/**
 * e_getMessage
 *   prints the exception message, statement which exception occured and which
 *   function it occured in
 * Arguments
 *   e
 *     the exception that occured
 *   f_name
 *     function that exception occured in
 * Return Values
 *   message is printed to stderr stating which exception occured and which 
 *   function it occured in
 */
#define e_getMessage( e, f_name ) {                                          \
  if ( (e) >= 0 && (e) <= UnknownException )                                 \
    fprintf( stderr, "Exception %s occured in %s\n",                         \
             Exception_Names[e], f_name );                                   \
  else                                                                       \
    fprintf( stderr, "Exception %s occured in %s\n",                         \
             Exception_Names[UnknownException], f_name );                    \
}

/**
 * Prototype(s) that should be in System Header Files
 */
int kill( pid_t, int );

/**
 * Public Prototype(s)
 */
void exception_handler ( int );
void printStackTrace   ( void );

#endif
