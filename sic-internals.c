#include "sic-internals.h"

client_id sic_client_id = -1;

bool blocked;

static PageInfo *invalid_pages;

/** Synchronization **/

void arrived_at_barrier(barrier_id id) {
  blocked = true;
  
  printf("Hitting barrier: %d\n", id);
  char resp[1024];
  printf("Sending packet.\n");
  send_packet("127.0.0.1", 30000, "hello there", resp);
  printf("%s", resp);
  // TODO: jlynch send message to server
  //

  while(blocked) {
    sched_yield();
  }
}

void released_from_barrier(barrier_id id) {
  assert(!blocked);
  blocked = false;
  sic_logf("Client released from barrier %d", id);
}

/** Networking **/

void wait_for_server() {
  sic_client_id = 0;
}

int sic_id() {
  return sic_client_id;
}

void *runclient(void * args) {
  int listener_d = open_listener_socket();
  int port = CLIENT_BASE_PORT + sic_id();
  bind_to_port(listener_d, port); 
  listen(listener_d, 10);
  while (1) {
    struct sockaddr_storage server_addr;
    unsigned int address_size = sizeof(server_addr);
    int connect_d = accept(listener_d, (struct sockaddr*) &server_addr, &address_size);
    char * msg = "ack from client";
    send(connect_d, msg, strlen(msg), 0);
    close(connect_d);
  }
}

/** Memory Management **/

static void handler(int sig, siginfo_t *si, void *unused) {
  sic_logf("Got SIGSEGV at address: 0x%lx",(long) si->si_addr);
  sic_logf("Marking it writeable");
  void * failing_page = ROUNDDOWN(si->si_addr, PGSIZE);
  if(mprotect(failing_page, PGSIZE, PROT_READ | PROT_WRITE) < 0) {
    printf("mprotect failed.");
  }
  sic_logf("Cloning page");
  register_page(twin_page(failing_page), failing_page);
}

void initialize_memory_manager() {
  // Setup sigaction handler
  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = handler;
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
    printf("Failure creating handler\n");

}

void mark_read_only(void *start, size_t length) {
  mprotect(start, length, PROT_READ);
}

void *twin_page(void *va) {
  void *new_page = memalign(PGSIZE, PGSIZE);
  memcpy(new_page, va, PGSIZE);
  return new_page;
}

/** Appends to the head of the list of invalid pages */
void register_page(void *oldva, void *newva) {
  if (invalid_pages == NULL) {
    invalid_pages = (PageInfo *)malloc(sizeof(PageInfo));
  } else { 
    PageInfo *new = (PageInfo *)malloc(sizeof(PageInfo));
    new->next = invalid_pages;
    invalid_pages = new;
  }
  invalid_pages->old_page_addr = oldva;
  invalid_pages->new_page_addr = newva;
  
  invalid_pages->next = NULL;
  sic_logf("Registered %p cloned at %p", newva, oldva);
}

/** Logs the current state off memory affairs **/
void memstat() {
  sic_logf(" ----- Computing memstat ---- ");
  PageInfo *w = invalid_pages;
  int num_pages = 0;
  while(w) {
    num_pages++; 
    w = w->next;
  }
  sic_logf("Num cloned pages: %d", num_pages);
  w = invalid_pages;
  while(w) {
    sic_logf("about to make diff");
    RegionDiff diff = memdiff(w->old_page_addr, w->new_page_addr, PGSIZE);
    sic_logf("about to print");
    print_diff(diff);
    w = w->next;
  }
}
