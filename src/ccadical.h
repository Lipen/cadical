#ifndef _ccadical_h_INCLUDED
#define _ccadical_h_INCLUDED

/*------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/*------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// C wrapper for CaDiCaL's C++ API following IPASIR.

typedef struct CCaDiCaL CCaDiCaL;

const char *ccadical_signature (void);
CCaDiCaL *ccadical_init (void);
void ccadical_release (CCaDiCaL *);

void ccadical_add (CCaDiCaL *, int lit);
void ccadical_assume (CCaDiCaL *, int lit);
int ccadical_solve (CCaDiCaL *);
int ccadical_val (CCaDiCaL *, int lit);
int ccadical_failed (CCaDiCaL *, int lit);

void ccadical_set_terminate (CCaDiCaL *, void *state,
                             int (*terminate) (void *state));

void ccadical_set_learn (CCaDiCaL *, void *state, int max_length,
                         void (*learn) (void *state, int *clause));

/*------------------------------------------------------------------------*/

// Non-IPASIR conformant 'C' functions.

void ccadical_constrain (CCaDiCaL *, int lit);
int ccadical_constraint_failed (CCaDiCaL *);
bool ccadical_configure (CCaDiCaL *, const char *name);
bool ccadical_set_option (CCaDiCaL *, const char *name, int val);
int ccadical_get_option (CCaDiCaL *, const char *name);
void ccadical_limit (CCaDiCaL *, const char *name, int limit);
void ccadical_print_statistics (CCaDiCaL *);
int64_t ccadical_vars (CCaDiCaL *);
int64_t ccadical_active (CCaDiCaL *);
int64_t ccadical_irredundant (CCaDiCaL *);
int64_t ccadical_conflicts (CCaDiCaL *);
int64_t ccadical_decisions (CCaDiCaL *);
int64_t ccadical_restarts (CCaDiCaL *);
int64_t ccadical_propagations (CCaDiCaL *);
int ccadical_fixed (CCaDiCaL *, int lit);
int ccadical_trace_proof (CCaDiCaL *, FILE *, const char *);
void ccadical_close_proof (CCaDiCaL *);
void ccadical_conclude (CCaDiCaL *);
void ccadical_terminate (CCaDiCaL *);
void ccadical_freeze (CCaDiCaL *, int lit);
int ccadical_frozen (CCaDiCaL *, int lit);
void ccadical_melt (CCaDiCaL *, int lit);
int ccadical_simplify (CCaDiCaL *);
void ccadical_write_dimacs (CCaDiCaL *, const char *);

void ccadical_propcheck_begin (CCaDiCaL *);
void ccadical_propcheck_add (CCaDiCaL *, int lit);
bool ccadical_propcheck (CCaDiCaL *);

void ccadical_propcheck_tree_begin (CCaDiCaL *);
void ccadical_propcheck_tree_add (CCaDiCaL *, int v);
uint64_t ccadical_propcheck_tree (CCaDiCaL *, uint64_t limit);

/*------------------------------------------------------------------------*/

// Support legacy names used before moving to more IPASIR conforming names.

#define ccadical_reset ccadical_release
#define ccadical_sat ccadical_solve
#define ccadical_deref ccadical_val

/*------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*------------------------------------------------------------------------*/

#endif
