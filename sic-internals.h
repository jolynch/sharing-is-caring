#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>


#include <sys/mman.h>

#include "sic-util.h"
#include "network.h"

typedef struct __PageInfo {
  void * old_page_addr;
  void * new_page_addr;
  struct __PageInfo *next;
} PageInfo;

/** Init / Exit Methods **/
void initialize_client();
void cleanup_client();

/** Client arrived at barrier. Blocks until barrier is clear. */
void arrived_at_barrier(barrier_id barrier);

/** Client released from barrier. Should be called from network code. */
void released_from_barrier(barrier_id barrier);

/** Wait for server to come up, and conect to it. Block until that is done. */
void wait_for_server();

/** Run the client main loop waiting for network traffic from the server */
void * runclient(void * args);

int sic_id();

void dispatch(char* msg, int id, int code, int value);

void mark_read_only(void *start, size_t length);

void initialize_memory_manager();

/** 
 * Clone a page within the shared virtual address space into the local address
 * space and memcpy the old page contents there. 
 *
 * Remove write protections on the old page. 
 *
 * Return the address of the twin.
 */
void *twin_page(void * va);

/** 
 * Register a just-twinned page in the list of currently invalid pages.
 */

void register_page(void *old_va, void *new_va);

/** Logs the current state of affairs **/
void memstat();

/** Actually does a malloc on our shared memory space **/
void * alloc(size_t len);
