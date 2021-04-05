/* Macros for memory checking. These macros redefine some standard memory
 * allocation calls to provide strong error checking.
 *
 * This header can be included permanently, with the presence of checking
 * controlled by the MEMCHECK macro.
 * Use
 *   #define MEMCHECK 1
 * to enable checking, and use
 *   #define MEMCHECK 0
 * to disable checking. All function calls will revert to their normal 
 * behavior, and utility macros will be stripped from the source.
 *
 * tpminka@media.mit.edu
 */

/* Error checks currently provided:
 *
 * Array bounds
 *     A picket is maintained before and after every memory block
 *     to check for writes just outside the allocated space (an error
 *     which often does not cause an immediate segmentation fault).
 *     These pickets are checked on calls to every memory function.
 *
 * Freeing unallocated blocks
 *     A list of currently active blocks is maintained to catch
 *     illegal calls to free().
 *
 * Allocation failure
 *     Checks for allocation failures (malloc returns NULL, etc).
 *
 * The functions screened by the checker are:
 *   malloc, free, calloc, realloc, strdup
 */

/* Utility macros:
 *
 * MEM_ABORT_ON_ERROR(flag)
 *     If flag is nonzero,
 *       Instructs the memory checker to abort execution on any error. 
 *       This is the default behavior.
 *     Otherwise,
 *       Instructs the memory checker to not abort execution on error.
 *       Messages are printed to stderr.
 *     This macro can be called at any time to permit/unpermit errors
 *     on the fly.
 *
 * MEM_ALLOC_NOTIFY(ptr)
 *     Lets the memory checker know when a library routine has allocated
 *     a memory block to be freed by the client program. Put this line
 *     after the call to said library function. "ptr" is the allocated block.
 *     Using this macro assumes that the library was not compiled with 
 *     memory checking.
 *
 * MEM_FREE_NOTIFY(ptr)
 *     Similar to MEM_ALLOC_NOTIFY(ptr). Lets the memory checker know when
 *     a library routine frees a block. Should be placed before the call to
 *     said routine. In the case of a library routine that realloc's a block,
 *     place a MEM_FREE_NOTIFY before the call and a MEM_ALLOC_NOTIFY after 
 *     the call.
 *     Using this macro assumes that the library was not compiled with
 *     memory checking.
 *
 * MEM_VERIFY_ALL()
 *     Checks all pickets for underflow or overflow. This is done
 *     automatically with a call to MEM_STATUS or MEM_BLOCKS.
 *
 * MEM_STATUS()
 *     Shows the number of currently allocated bytes 
 *     and the maximum number of bytes simultaneously allocated 
 *     up to this time.
 *
 * MEM_BLOCKS()
 *     Shows sizes and allocation locations (in the code) of every
 *     active (currently allocated) memory block.
 *
 */

/* BUGS/RESTRICTIONS
 *
 * This file must be included *after* malloc.h and string.h; preferably
 * make it the last header included.
 *
 * Function pointers to the screened memory functions will return their
 * unscreened counterparts. This means, among other things, that calls
 * like (malloc)(100) will go unscreened and may confuse the checker.
 * Use the MEM_ALLOC_NOTIFY and MEM_FREE_NOTIFY macros around such calls
 * to avoid this.
 *
 */

#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#if MEMCHECK

#ifdef AP
#undef AP
#endif

#if defined(__STDC__) || defined(__cplusplus)
#define AP(x) x
#else
#define AP(x) ()
#endif

#if defined(__STDC__) || defined(__cplusplus)

void *_mc_malloc(size_t size, char *file, unsigned line);
void _mc_free(void *p, char *file, unsigned line);
void *_mc_calloc(size_t nitems, size_t item_size, 
		 char *file, unsigned line);
void *_mc_realloc(void *p, size_t size, char *file, unsigned line);

#else

char *_mc_malloc();
void _mc_free();
void *_mc_calloc();
char *_mc_realloc();

#endif

char *_mc_strdup AP((char *p, char *file, unsigned line));

void _mc_abort_on_error AP((int flag));
void _mc_alloc_notify AP((void *p, char *file, unsigned line));
void _mc_free_notify AP((void *p, char *file, unsigned line));
void _mc_verify_all AP((char *file, unsigned line));
void _mc_status AP((char *file, unsigned line));
void _mc_mem_nodes AP((char *file, unsigned line));

#define malloc(x) _mc_malloc(x, __FILE__, __LINE__)
#define free(x) _mc_free(x, __FILE__, __LINE__)
#define calloc(n,s) _mc_calloc(n, s, __FILE__, __LINE__)
#define realloc(x, s) _mc_realloc(x, s, __FILE__, __LINE__)
#define strdup(s) _mc_strdup(s, __FILE__, __LINE__)

/* Hopefully this will be aligned on all processors */
#define PICKET_SIZE 16

#define MEM_ABORT_ON_ERROR(flag) _mc_abort_on_error(flag)
#define MEM_ALLOC_NOTIFY(ptr) _mc_alloc_notify(ptr, __FILE__, __LINE__)
#define MEM_FREE_NOTIFY(ptr) _mc_free_notify(ptr, __FILE__, __LINE__)
#define MEM_VERIFY_ALL() _mc_verify_all(__FILE__, __LINE__)
#define MEM_STATUS() _mc_status(__FILE__, __LINE__)
#define MEM_BLOCKS() _mc_mem_nodes(__FILE__, __LINE__)
#define MEM_USE_PICKETS(flag) _mc_use_pickets(flag)

#else /* MEMCHECK */

#define MEM_ABORT_ON_ERROR(flag)
#define MEM_ALLOC_NOTIFY(ptr)
#define MEM_FREE_NOTIFY(ptr)
#define MEM_VERIFY_ALL()
#define MEM_STATUS()
#define MEM_BLOCKS()
#define MEM_USE_PICKETS(flag)

#endif /* MEMCHECK */

#endif /* MEMCHECK_H_INCLUDED */
