#ifndef _ccadical_h_INCLUDED
#define _ccadical_h_INCLUDED

#ifdef __cplusplus
    /* in case the compiler is a C++ compiler */
    #define DEFAULT_VALUE(value) = value
#else
    /* otherwise, C compiler, do nothing */
    #define DEFAULT_VALUE(value)
#endif

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
bool ccadical_failed (CCaDiCaL *, int lit);

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
bool ccadical_limit (CCaDiCaL *, const char *name, int limit);
void ccadical_read_dimacs (CCaDiCaL *, const char *path, int strict DEFAULT_VALUE(1));
void ccadical_write_dimacs (CCaDiCaL *, const char *path);
bool ccadical_trace_proof (CCaDiCaL *, const char *path);
void ccadical_close_proof (CCaDiCaL *);
void ccadical_print_statistics (CCaDiCaL *);
int64_t ccadical_vars (CCaDiCaL *);
int64_t ccadical_active (CCaDiCaL *);
int64_t ccadical_redundant (CCaDiCaL *);
int64_t ccadical_irredundant (CCaDiCaL *);
int64_t ccadical_conflicts (CCaDiCaL *);
int64_t ccadical_decisions (CCaDiCaL *);
int64_t ccadical_restarts (CCaDiCaL *);
int64_t ccadical_propagations (CCaDiCaL *);
int ccadical_fixed (CCaDiCaL *, int lit);
bool ccadical_is_active (CCaDiCaL *, int lit);
void ccadical_conclude (CCaDiCaL *);
void ccadical_terminate (CCaDiCaL *);
void ccadical_freeze (CCaDiCaL *, int lit);
bool ccadical_frozen (CCaDiCaL *, int lit);
void ccadical_melt (CCaDiCaL *, int lit);
int ccadical_simplify (CCaDiCaL *);
size_t ccadical_traverse_clauses(CCaDiCaL *, bool redundant DEFAULT_VALUE(false));
size_t ccadical_get_clause_length (CCaDiCaL *, size_t i);
void ccadical_get_clause (CCaDiCaL *, size_t i, int *out_clause);
void ccadical_clear_clauses(CCaDiCaL *);

/*------------------------------------------------------------------------*/

// Propcheck

void ccadical_propcheck_begin (CCaDiCaL *);
void ccadical_propcheck_add (CCaDiCaL *, int lit);
bool ccadical_propcheck (CCaDiCaL *, bool restore DEFAULT_VALUE(true), bool save DEFAULT_VALUE(false), uint64_t *num_propagated DEFAULT_VALUE(NULL));
size_t ccadical_propcheck_get_propagated_length (CCaDiCaL *);
void ccadical_propcheck_get_propagated (CCaDiCaL *, int *out_propagated);

/*------------------------------------------------------------------------*/

// Propcheck-tree

void ccadical_propcheck_all_tree_begin (CCaDiCaL *);
void ccadical_propcheck_all_tree_add (CCaDiCaL *, int v);
uint64_t ccadical_propcheck_all_tree (CCaDiCaL *, uint64_t limit, bool save DEFAULT_VALUE(false));
size_t ccadical_propcheck_all_tree_get_valid_length (CCaDiCaL *);
size_t ccadical_propcheck_all_tree_get_cube_length (CCaDiCaL *, size_t i);
void ccadical_propcheck_all_tree_get_cube (CCaDiCaL *, size_t i, int *out_cube);

/*------------------------------------------------------------------------*/

// Internal stuff

bool ccadical_internal_propagate (CCaDiCaL *);
void ccadical_internal_reset_conflict (CCaDiCaL *);
int ccadical_internal_level (CCaDiCaL *);
signed char ccadical_internal_val (CCaDiCaL *, int lit);
void ccadical_internal_assume_decision (CCaDiCaL *, int lit);
void ccadical_internal_backtrack (CCaDiCaL *, int new_level);

void ccadical_add_unit_clause (CCaDiCaL *, int lit);
void ccadical_add_derived (CCaDiCaL *, int lit);

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
