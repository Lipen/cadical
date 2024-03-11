#include "cadical.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

namespace CaDiCaL {

struct Wrapper : Learner, Terminator {

  Solver *solver;
  struct {
    void *state;
    int (*function) (void *);
  } terminator;

  struct {
    void *state;
    int max_length;
    int *begin_clause, *end_clause, *capacity_clause;
    void (*function) (void *, int *);
  } learner;

  bool terminate () {
    if (!terminator.function)
      return false;
    return terminator.function (terminator.state);
  }

  bool learning (int size) {
    if (!learner.function)
      return false;
    return size <= learner.max_length;
  }

  void learn (int lit) {
    if (learner.end_clause == learner.capacity_clause) {
      size_t count = learner.end_clause - learner.begin_clause;
      size_t size = count ? 2 * count : 1;
      learner.begin_clause =
          (int *) realloc (learner.begin_clause, size * sizeof (int));
      learner.end_clause = learner.begin_clause + count;
      learner.capacity_clause = learner.begin_clause + size;
    }
    *learner.end_clause++ = lit;
    if (lit)
      return;
    learner.function (learner.state, learner.begin_clause);
    learner.end_clause = learner.begin_clause;
  }

  Wrapper () : solver (new Solver ()) {
    memset (&terminator, 0, sizeof terminator);
    memset (&learner, 0, sizeof learner);
  }

  ~Wrapper () {
    terminator.function = 0;
    if (learner.begin_clause)
      free (learner.begin_clause);
    delete solver;
  }

  std::vector<int> propcheck_assumptions;
  std::vector<int> propcheck_propagated;
  uint64_t propcheck_num_propagated;
  std::vector<int> propcheck_all_tree_variables;
  std::vector<std::vector<int>> propcheck_all_tree_valid;

  std::vector<std::vector<int>> clauses;
};

} // namespace CaDiCaL

using namespace CaDiCaL;

extern "C" {

#include "ccadical.h"

const char *ccadical_signature (void) { return Solver::signature (); }

CCaDiCaL *ccadical_init (void) { return (CCaDiCaL *) new Wrapper (); }

void ccadical_release (CCaDiCaL *wrapper) { delete (Wrapper *) wrapper; }

void ccadical_constrain (CCaDiCaL *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->constrain (lit);
}

int ccadical_constraint_failed (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->constraint_failed ();
}

bool ccadical_configure (CCaDiCaL *wrapper, const char *name) {
  return ((Wrapper *) wrapper)->solver->configure (name);
}

bool ccadical_set_option (CCaDiCaL *wrapper, const char *name, int val) {
  return ((Wrapper *) wrapper)->solver->set (name, val);
}

int ccadical_get_option (CCaDiCaL *wrapper, const char *name) {
  return ((Wrapper *) wrapper)->solver->get (name);
}

bool ccadical_limit (CCaDiCaL *wrapper, const char *name, int val) {
  return ((Wrapper *) wrapper)->solver->limit (name, val);
}

void ccadical_add (CCaDiCaL *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->add (lit);
}

void ccadical_assume (CCaDiCaL *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->assume (lit);
}

int ccadical_solve (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->solve ();
}

int ccadical_simplify (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->simplify ();
}

int ccadical_val (CCaDiCaL *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->val (lit);
}

int ccadical_failed (CCaDiCaL *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->failed (lit);
}

void ccadical_print_statistics (CCaDiCaL *wrapper) {
  ((Wrapper *) wrapper)->solver->statistics ();
}

void ccadical_terminate (CCaDiCaL *wrapper) {
  ((Wrapper *) wrapper)->solver->terminate ();
}

int64_t ccadical_vars (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->vars ();
}

int64_t ccadical_active (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->active ();
}

int64_t ccadical_irredundant (CCaDiCaL *wrapper) {
  return ((Wrapper *) wrapper)->solver->irredundant ();
}

int64_t ccadical_conflicts (CCaDiCaL *wrapper) {
  return ((Wrapper*) wrapper)->solver->conflicts ();
}

int64_t ccadical_decisions (CCaDiCaL *wrapper) {
  return ((Wrapper*) wrapper)->solver->decisions ();
}

int64_t ccadical_restarts (CCaDiCaL *wrapper) {
  return ((Wrapper*) wrapper)->solver->restarts ();
}

int64_t ccadical_propagations (CCaDiCaL *wrapper) {
  return ((Wrapper*) wrapper)->solver->propagations ();
}

int ccadical_fixed (CCaDiCaL *wrapper, int lit) {
  return ((Wrapper*) wrapper)->solver->fixed (lit);
}

bool ccadical_active_lit (CCaDiCaL *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->active (lit);
}

void ccadical_set_terminate (CCaDiCaL *ptr, void *state,
                             int (*terminate) (void *)) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->terminator.state = state;
  wrapper->terminator.function = terminate;
  if (terminate)
    wrapper->solver->connect_terminator (wrapper);
  else
    wrapper->solver->disconnect_terminator ();
}

void ccadical_set_learn (CCaDiCaL *ptr, void *state, int max_length,
                         void (*learn) (void *state, int *clause)) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->learner.state = state;
  wrapper->learner.max_length = max_length;
  wrapper->learner.function = learn;
  if (learn)
    wrapper->solver->connect_learner (wrapper);
  else
    wrapper->solver->disconnect_learner ();
}

void ccadical_freeze (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->freeze (lit);
}

void ccadical_melt (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->melt (lit);
}

int ccadical_frozen (CCaDiCaL *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->frozen (lit);
}

bool ccadical_trace_proof (CCaDiCaL *ptr, const char *path) {
  return ((Wrapper *) ptr)->solver->trace_proof (path);
}

void ccadical_close_proof (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->close_proof_trace ();
}

void ccadical_conclude (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->conclude ();
}

void ccadical_write_dimacs(CCaDiCaL *ptr, const char *path) {
  ((Wrapper *) ptr)->solver->write_dimacs (path);
}

void ccadical_propcheck_begin (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_assumptions.clear();
}

void ccadical_propcheck_add (CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_assumptions.push_back (lit);
}

bool ccadical_propcheck (CCaDiCaL *ptr, bool restore, bool save) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_propagated.clear();
  if (save) {
    return wrapper->solver->propcheck (wrapper->propcheck_assumptions, restore, &wrapper->propcheck_propagated, &wrapper->propcheck_num_propagated);
  } else {
    return wrapper->solver->propcheck (wrapper->propcheck_assumptions, restore, NULL, &wrapper->propcheck_num_propagated);
  }
}

uint64_t ccadical_propcheck_num_propagated (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->propcheck_num_propagated;
}

size_t ccadical_propcheck_get_propagated_length (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->propcheck_propagated.size ();
}

void ccadical_propcheck_get_propagated (CCaDiCaL *ptr, int *out_propagated) {
  Wrapper *wrapper = (Wrapper *) ptr;
  for (size_t j = 0; j < wrapper->propcheck_propagated.size(); j++) {
    out_propagated[j] = wrapper->propcheck_propagated[j];
  }
}

void ccadical_propcheck_all_tree_begin (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_all_tree_variables.clear ();
}

void ccadical_propcheck_all_tree_add (CCaDiCaL *ptr, int v) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_all_tree_variables.push_back (v);
}

uint64_t ccadical_propcheck_all_tree (CCaDiCaL *ptr, uint64_t limit, bool save) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->propcheck_all_tree_valid.clear ();
  if (save) {
    return wrapper->solver->propcheck_all_tree (wrapper->propcheck_all_tree_variables, limit, &wrapper->propcheck_all_tree_valid);
  } else {
    return wrapper->solver->propcheck_all_tree (wrapper->propcheck_all_tree_variables, limit, NULL);
  }
}

size_t ccadical_propcheck_all_tree_get_valid_length (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->propcheck_all_tree_valid.size ();
}

size_t ccadical_propcheck_all_tree_get_cube_length (CCaDiCaL * ptr, size_t i) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->propcheck_all_tree_valid[i].size ();
}

void ccadical_propcheck_all_tree_get_cube (CCaDiCaL *ptr, size_t i, int *out_cube) {
  Wrapper *wrapper = (Wrapper *) ptr;
  const std::vector<int> &cube = wrapper->propcheck_all_tree_valid[i];
  for (size_t j = 0; j < cube.size(); j++) {
    out_cube[j] = cube[j];
  }
}

class ClauseCloner : public ClauseIterator {
  std::vector<std::vector<int>> &dest;

public:
  ClauseCloner (std::vector<std::vector<int>> &d) : dest (d) {}
  bool clause (const std::vector<int> &c) {
    dest.push_back (c);
    return true;
  }
};

size_t ccadical_traverse_clauses(CCaDiCaL *ptr, bool redundant) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->clauses.clear ();
  ClauseCloner cloner (wrapper->clauses);
  if (redundant) {
    wrapper->solver->traverse_all_clauses (cloner);
  } else {
    wrapper->solver->traverse_clauses (cloner);
  }
  return wrapper->clauses.size ();
}

size_t ccadical_get_clause_length (CCaDiCaL *ptr, size_t i) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->clauses[i].size ();
}

void ccadical_get_clause (CCaDiCaL *ptr, size_t i, int *out_clause) {
  Wrapper *wrapper = (Wrapper *) ptr;
  const std::vector<int> &clause = wrapper->clauses[i];
  for (size_t j = 0; j < clause.size(); j++) {
    out_clause[j] = clause[j];
  }
}

void ccadical_clear_clauses (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->clauses.clear ();
}

bool ccadical_internal_propagate (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->solver->internal_propagate ();
}

void ccadical_internal_reset_conflict (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->internal_reset_conflict ();
}

int ccadical_internal_level (CCaDiCaL *ptr) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->solver->internal_level ();
}

signed char ccadical_internal_val (CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  return wrapper->solver->internal_val (lit);
}

void ccadical_internal_assume_decision (CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->internal_assume_decision (lit);
}

void ccadical_internal_backtrack (CCaDiCaL *ptr, int new_level) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->internal_backtrack (new_level);
}

}
