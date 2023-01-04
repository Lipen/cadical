#include <cstdlib>
#include <cstring>

#include "cadical.hpp"

namespace CaDiCaL {

struct CCaDiCaL : Learner, Terminator {
    Solver *solver;
    struct {
        void *state;
        int (*function)(void *);
    } terminator;

    struct {
        void *state;
        int max_length;
        int *begin_clause, *end_clause, *capacity_clause;
        void (*function)(void *, int *);
    } learner;

    bool terminate() {
        if (!terminator.function)
            return false;
        return terminator.function(terminator.state);
    }

    bool learning(int size) {
        if (!learner.function)
            return false;
        return size <= learner.max_length;
    }

    void learn(int lit) {
        if (learner.end_clause == learner.capacity_clause) {
            size_t count = learner.end_clause - learner.begin_clause;
            size_t size = count ? 2 * count : 1;
            learner.begin_clause = (int *)realloc(learner.begin_clause, size * sizeof(int));
            learner.end_clause = learner.begin_clause + count;
            learner.capacity_clause = learner.begin_clause + size;
        }
        *learner.end_clause++ = lit;
        if (lit)
            return;
        learner.function(learner.state, learner.begin_clause);
        learner.end_clause = learner.begin_clause;
    }

    CCaDiCaL() : solver(new Solver()) {
        memset(&terminator, 0, sizeof terminator);
        memset(&learner, 0, sizeof learner);
    }

    ~CCaDiCaL() {
        terminator.function = 0;
        if (learner.begin_clause) free(learner.begin_clause);
        delete solver;
    }
};

}  // namespace CaDiCaL

using namespace CaDiCaL;

extern "C" {

#include "ccadical.h"

const char *ccadical_signature(void) {
    return Solver::signature();
}

CCaDiCaL *ccadical_init(void) {
    return new CCaDiCaL();
}

void ccadical_release(CCaDiCaL *ptr) {
    delete ptr;
}

void ccadical_constrain(CCaDiCaL *ptr, int lit) {
    ptr->solver->constrain(lit);
}

int ccadical_constraint_failed(CCaDiCaL *ptr) {
    return ptr->solver->constraint_failed();
}

void ccadical_set_option(CCaDiCaL *ptr,
                         const char *name, int val) {
    ptr->solver->set(name, val);
}

void ccadical_limit(CCaDiCaL *ptr,
                    const char *name, int val) {
    ptr->solver->limit(name, val);
}

int ccadical_get_option(CCaDiCaL *ptr, const char *name) {
    return ptr->solver->get(name);
}

void ccadical_add(CCaDiCaL *ptr, int lit) {
    ptr->solver->add(lit);
}

void ccadical_assume(CCaDiCaL *ptr, int lit) {
    ptr->solver->assume(lit);
}

int ccadical_solve(CCaDiCaL *ptr) {
    return ptr->solver->solve();
}

int ccadical_simplify(CCaDiCaL *ptr) {
    return ptr->solver->simplify();
}

int ccadical_val(CCaDiCaL *ptr, int lit) {
    return ptr->solver->val(lit);
}

int ccadical_failed(CCaDiCaL *ptr, int lit) {
    return ptr->solver->failed(lit);
}

void ccadical_print_statistics(CCaDiCaL *ptr) {
    ptr->solver->statistics();
}

void ccadical_terminate(CCaDiCaL *ptr) {
    ptr->solver->terminate();
}

int ccadical_vars(CCaDiCaL *ptr) {
    return ptr->solver->vars();
}

int ccadical_active(CCaDiCaL *ptr) {
    return ptr->solver->active();
}

int64_t ccadical_irredundant(CCaDiCaL *ptr) {
    return ptr->solver->irredundant();
}

int64_t ccadical_conflicts(CCaDiCaL *ptr) {
    return ptr->solver->conflicts();
}

int64_t ccadical_decisions(CCaDiCaL *ptr) {
    return ptr->solver->decisions();
}

int64_t ccadical_restarts(CCaDiCaL *ptr) {
    return ptr->solver->restarts();
}

int64_t ccadical_propagations(CCaDiCaL *ptr) {
    return ptr->solver->propagations();
}

int ccadical_fixed(CCaDiCaL *ptr, int lit) {
    return ptr->solver->fixed(lit);
}

void ccadical_set_terminate(CCaDiCaL *ptr, void *state, int (*terminate)(void *)) {
    ptr->terminator.state = state;
    ptr->terminator.function = terminate;
    if (terminate)
        ptr->solver->connect_terminator(ptr);
    else
        ptr->solver->disconnect_terminator();
}

void ccadical_set_learn(CCaDiCaL *ptr, void *state, int max_length, void (*learn)(void *state, int *clause)) {
    ptr->learner.state = state;
    ptr->learner.max_length = max_length;
    ptr->learner.function = learn;
    if (learn)
        ptr->solver->connect_learner(ptr);
    else
        ptr->solver->disconnect_learner();
}

void ccadical_freeze(CCaDiCaL *ptr, int lit) {
    ptr->solver->freeze(lit);
}

void ccadical_melt(CCaDiCaL *ptr, int lit) {
    ptr->solver->melt(lit);
}

int ccadical_frozen(CCaDiCaL *ptr, int lit) {
    return ptr->solver->frozen(lit);
}
}
