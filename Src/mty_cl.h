/*! \file mty_cl.h
* \brief Header for the Host side of the OpenCL implementation of the MTY tripcode searcher
* \authors Mads Buvik Sandvei
* \date 2013
*
* This file contains definitions related to the Host side of the OpenCL implementation of the
* MTY tripcode searcher
*/ 

#include <stdint.h>
#include <string.h>
#include "mtytypes.h"
#include "crypt64.h"

//! \brief An array of characters that determine what salt to use.
extern char salt_chars[];

/*! \brief Entry point for GPU searching thread
 *
 *	Entry point for GPU handler threads.
 *	If N gpus are found, N threads should be spawned calling this function.
 *
 *	\param[in] dummyarg	A dummy argument, function takes no argument.
 */
void *gpu_main(void *dummyarg /* Takes no argument */);

/*! \brief Initializes items common across GPUs
 *
 *	\param[in] seed a random seed to use for random generation.
 *	\return	the number of GPUs detected by OpenCL.
 */
int gpu_init(uint32_t seed);

/*! \brief Add current thread to list of gpus.
 *
 *	\return	A unique ID
 */
int register_gpu();

/*! \brief increment number of trips generated by this thread's gpu for global statistics.
 *
 *	\param[in] gpu ID of this GPU (as returned by register_gpu)
 */
void register_trips_generated(int gpu, uint64_t n);

//!	\brief Avoid polluting statistics with time spent on first compilation.
void set_start_time();

/*! \brief Epoch time in milliseconds
 *	\return Epoch time in milliseconds
 */
uint64_t usec();

/*
 * printout defines for ease of debugging
 */
//! \brief prints the string "a" as-is.
#define prints(a) printf("%s\n", a );
/*! \brief prints the integer value of a as well as "a".
 *
 *	E.g.
 *	int some_fancy_variable = 52;
 *	printi(some_fancy_variable);
 *
 *	Result on stdout is "some_fancy_variable: 52"
 */
#define printi(a) printf(#a ": %d\n",a);
/*! \brief prints the character of a as well as "a".
 *
 *	E.g.
 *	char some_fancy_variable = 'b';
 *	printc(some_fancy_variable);
 *
 *	Result on stdout is "some_fancy_variable: b"
 */
#define printc(a) printf(#a ": %c\n",a);
/*! \brief prints the unsigned integer value of a as well as "a".
 *
 *	E.g.
 *	int some_fancy_variable = 52;
 *	printi(some_fancy_variable);
 *
 *	Result on stdout is "some_fancy_variable: 52"
 */
#define printu(a) printf(#a ": %u\n",a);
/*! \brief prints the long integer value of a as well as "a".
 *
 *	E.g.
 *	int64_t some_fancy_variable = 52;
 *	printl(some_fancy_variable);
 *
 *	Result on stdout is "some_fancy_variable: 52"
 */
#define printl(a) printf(#a ": %I64\n",a);
/*! \brief prints the unsigned long integer value of a as well as "a".
 *
 *	E.g.
 *	uint64_t some_fancy_variable = 52;
 *	printul(some_fancy_variable);
 *
 *	Result on stdout is "some_fancy_variable: 52"
 */
#define printul(a) printf(#a ": %I64u\n",a);