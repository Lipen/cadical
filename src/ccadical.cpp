#include "cadical.hpp"

#include <cstdlib>
#include <cstring>
#include <vector>

namespace CaDiCaL {

struct Wrapper : Learner, Terminator {

  Solver *solver;

  struct {
    void *state = nullptr;
    int (*function) (void *) = nullptr;
  } terminator;

  struct {
    void *state = nullptr;
    void (*function) (void *, int *) = nullptr;
    int max_length = 0;
    std::vector<int> clause; // learnt clause, 0-terminated when `function` is called
  } learner;

  bool terminate () {
    if (!terminator.function)
      return false;
    return terminator.function (terminator.state);
  }

  bool learning (int size) {
    if (!learner.function)
      return false;
    if (!learner.max_length)
      return true;
    return size <= learner.max_length;
  }

  void learn (int lit) {
    learner.clause.push_back (lit);
    if (lit) return;
    learner.function (learner.state, learner.clause.data ());
    learner.clause.clear ();
  }

  Wrapper () : solver (new Solver ()) {}

  ~Wrapper () {
    terminator.function = 0;
    delete solver;
  }

  std::vector<int> propcheck_propagated;
  std::vector<int> propcheck_core;
};

} // namespace CaDiCaL

using namespace CaDiCaL;

extern "C" {

#include "ccadical.h"

const char *ccadical_signature (void) { return Solver::signature (); }

CCaDiCaL *ccadical_init (void) { return (CCaDiCaL *) new Wrapper (); }

void ccadical_release (CCaDiCaL *ptr) { delete (Wrapper *) ptr; }

void ccadical_constrain (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->constrain (lit);
}

int ccadical_constraint_failed (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->constraint_failed ();
}

bool ccadical_configure (CCaDiCaL *ptr, const char *name) {
  return ((Wrapper *) ptr)->solver->configure (name);
}

bool ccadical_set_option (CCaDiCaL *ptr, const char *name, int val) {
  return ((Wrapper *) ptr)->solver->set (name, val);
}

int ccadical_get_option (CCaDiCaL *ptr, const char *name) {
  return ((Wrapper *) ptr)->solver->get (name);
}

bool ccadical_limit (CCaDiCaL *ptr, const char *name, int val) {
  return ((Wrapper *) ptr)->solver->limit (name, val);
}

void ccadical_add (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->add (lit);
}

void ccadical_assume (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->assume (lit);
}

int ccadical_solve (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->solve ();
}

int ccadical_simplify (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->simplify ();
}

int ccadical_val (CCaDiCaL *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->val (lit);
}

bool ccadical_failed (CCaDiCaL *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->failed (lit);
}

void ccadical_reset_assumptions (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->reset_assumptions ();
}

void ccadical_reset_constraint (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->reset_constraint ();
}

void ccadical_print_statistics (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->statistics ();
}

void ccadical_print_resources (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->resources ();
}

void ccadical_terminate (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->terminate ();
}

int64_t ccadical_vars (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->vars ();
}

int64_t ccadical_active (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->active ();
}

int64_t ccadical_redundant (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->redundant ();
}

int64_t ccadical_irredundant (CCaDiCaL *ptr) {
  return ((Wrapper *) ptr)->solver->irredundant ();
}

int64_t ccadical_conflicts (CCaDiCaL *ptr) {
  return ((Wrapper*) ptr)->solver->conflicts ();
}

int64_t ccadical_decisions (CCaDiCaL *ptr) {
  return ((Wrapper*) ptr)->solver->decisions ();
}

int64_t ccadical_restarts (CCaDiCaL *ptr) {
  return ((Wrapper*) ptr)->solver->restarts ();
}

int64_t ccadical_propagations (CCaDiCaL *ptr) {
  return ((Wrapper*) ptr)->solver->propagations ();
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

int ccadical_fixed (CCaDiCaL *ptr, int lit) {
  return ((Wrapper*) ptr)->solver->fixed (lit);
}

bool ccadical_is_active (CCaDiCaL *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->active (lit);
}

void ccadical_freeze (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->freeze (lit);
}

void ccadical_melt (CCaDiCaL *ptr, int lit) {
  ((Wrapper *) ptr)->solver->melt (lit);
}

bool ccadical_frozen (CCaDiCaL *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->frozen (lit);
}

void ccadical_conclude (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->conclude ();
}

bool ccadical_trace_proof (CCaDiCaL *ptr, const char *path) {
  return ((Wrapper *) ptr)->solver->trace_proof (path);
}

void ccadical_close_proof (CCaDiCaL *ptr) {
  ((Wrapper *) ptr)->solver->close_proof_trace ();
}

void ccadical_read_dimacs (CCaDiCaL *ptr, const char *path, int strict) {
  int vars;
  ((Wrapper *) ptr)->solver->read_dimacs (path, vars, strict);
}

void ccadical_write_dimacs(CCaDiCaL *ptr, const char *path) {
  ((Wrapper *) ptr)->solver->write_dimacs (path);
}

bool ccadical_propcheck (
  CCaDiCaL *ptr,
  const int *lits, size_t size,
  bool restore,
  uint64_t *num_propagated,
  bool save_propagated,
  bool save_core
) {
  Wrapper *wrapper = (Wrapper *) ptr;
  const std::vector<int> assumptions (lits, lits + size);
  wrapper->propcheck_propagated.clear();
  wrapper->propcheck_core.clear();
  std::vector<int> *propagated = nullptr;
  std::vector<int> *core = nullptr;
  if (save_propagated)
    propagated = &wrapper->propcheck_propagated;
  if (save_core)
    core = &wrapper->propcheck_core;
  return wrapper->solver->propcheck (assumptions, restore, num_propagated, propagated, core);
}

const int *ccadical_get_propagated (CCaDiCaL *ptr, size_t *size) {
  Wrapper *wrapper = (Wrapper *) ptr;
  *size = wrapper->propcheck_propagated.size ();
  return wrapper->propcheck_propagated.data ();
}

const int *ccadical_propcheck_get_core (CCaDiCaL *ptr, size_t *size) {
  Wrapper *wrapper = (Wrapper *) ptr;
  *size = wrapper->propcheck_core.size ();
  return wrapper->propcheck_core.data ();
}

uint64_t ccadical_propcheck_all_tree (
  CCaDiCaL *ptr,
  const int *lits, size_t size,
  uint64_t limit,
  SliceCallback on_valid,
  void *user_data_valid
) {
  Wrapper *wrapper = (Wrapper *) ptr;
  const std::vector<int> variables (lits, lits + size);
  return wrapper->solver->propcheck_all_tree (variables, limit, on_valid, user_data_valid);
}

class ClauseCallbackIterator : public ClauseIterator {
  ClauseCallback cb;
  void *user_data;

public:
  ClauseCallbackIterator (ClauseCallback cb, void *user_data)
    : cb (cb), user_data (user_data) {}

  bool clause (const std::vector<int> &c) {
    return cb (c.data (), c.size (), user_data);
  }
};

bool ccadical_traverse_clauses (CCaDiCaL *ptr, bool redundant, ClauseCallback cb, void *user_data) {
  Wrapper *wrapper = (Wrapper *) ptr;
  ClauseCallbackIterator it (cb, user_data);
  if (redundant)
    return wrapper->solver->traverse_all_clauses (it);
  else
    return wrapper->solver->traverse_clauses (it);
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

void ccadical_add_unit_clause (CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->add_unit_clause (lit);
}

void ccadical_add_derived (CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->add_derived (lit);
}

}
