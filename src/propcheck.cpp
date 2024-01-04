#include "internal.hpp"

#include <iostream>

namespace CaDiCaL {

bool Solver::propcheck (const std::vector<int> &assumptions) {
    assert(!internal->conflict);
    if (internal->conflict) {
        return false;
    }

    // Save the original decision level:
    int level = internal->level;
    bool no_conflicting_assignment = true;
    bool no_conflict = true;

    for (int lit : assumptions) {
        const signed char b = internal->val (lit);
        if (b > 0) {
            // Literal already assigned, do nothing.
        } else if (b < 0) {
            // Conflict during assignment.
            no_conflicting_assignment = false;
            break;
        } else {
            // Assign and propagate the assumption:
            internal->search_assume_decision (lit);
            if (!internal->propagate ()) {
                // Conflict.
                no_conflict = false;
                internal->conflict = 0;
                break;
            }
        }
    }

    // Backtrack to the original decision level:
    internal->backtrack (level);

    // Return `true` if there were no conflicts:
    return no_conflict && no_conflicting_assignment;
}

uint64_t Solver::propcheck_tree (const vector<int>& variables, uint64_t limit) {
    assert(level == 0);

    // Trivial case:
    if (variables.empty ()) {
        return 0;
    }

    // Propagate everything that needs to be propagated:
    if (!internal->propagate ()) {
        // Conflict.
        internal->conflict = 0;
        return 0;
    }

    // Signs:
    vector<int> cube (variables.size(), -1);

    // Number of valid cubes:
    uint64_t total = 0;

    // Number of propagations:
    uint64_t checked = 0;

    // State machine:
    //  state = 0 -- Descending
    //  state = 1 -- Ascending
    //  state = 2 -- Propagating
    //
    int state = 0; // state = Descending

    while (1) {
        assert(internal->level <= (int)variables.size());
        // std::cout << "state = " << state << ", level = " << level << std::endl;

        if (state == 0) { // Descending
            if (internal->level == (int)variables.size()) {
                total++;
                // std::cout << "total++ = " << total << std::endl;
                if (limit && total > limit) {
                    // std::cout << "reached the limit: " << total << " > " << limit << std::endl;
                    break;
                }
                state = 1; // state = Ascending
            } else {
                int v = variables[internal->level];
                const signed char s = cube[internal->level];
                int lit = s * v;
                const signed char b = internal->val (lit);
                // std::cout << "assigning " << lit << std::endl;

                if (b > 0) {
                    // Dummy level:
                    internal->new_trail_level (0);

                    // Keep descending:
                    state = 0; // state = Descending

                } else if (b < 0) {
                    // Dummy level:
                    internal->new_trail_level (0);

                    // Need to ascend:
                    state = 1; // state = Ascending

                } else {
                    // Enqueue the assumption:
                    internal->search_assume_decision (lit);

                    // Need to propagate:
                    state = 2; // state = Propagating
                }
            }

        } else if (state == 1) { // Ascending
            assert(level > 0);

            // Find the last "false":
            size_t i = internal->level;  // 1-based index
            while (i > 0 && cube[i - 1] > 0) {
                i--;
            }
            // std::cout << "i = " << i << std::endl;
            if (i == 0) {
                // Finish.
                // std::cout << "i = 0 for cube = [";
                // for (int k = i; k < level; k++) {
                //     if (k > 0) std::cout << " ";
                //     std::cout << cube[k] * variables[k];
                // }
                // std::cout << "] with total = " << total << std::endl;
                break;
            }

            // Increment the cube:
            assert(cube[i - 1] == -1);
            cube[i - 1] = 1;
            for (size_t j = i; j < variables.size(); j++) {
                cube[j] = -1;
            }

            // Backtrack before the i-th level:
            // std::cout << "backtracking to " << (i - 1) << std::endl;
            internal->backtrack (i - 1);

            // Switch the state:
            state = 0;  // state = Descending

        } else if (state == 2) { // Propagating
            checked++;
            // std::cout << "propagating [";
            // for (int i = 0; i < level; i++) {
            //     if (i > 0) std::cout << " ";
            //     std::cout << cube[i] * variables[i];
            // }
            // std::cout << "]" << std::endl;
            if (!internal->propagate ()) {
                // Conflict.
                internal->conflict = 0;
                state = 1; // state = Ascending
            } else {
                // No conflict.
                state = 0; // state = Descending
            }

        } else { // Bad state
            std::cerr << "Bad state: " << state << std::endl;
            exit(1);
            break;
        }
    }

    internal->backtrack (0);

    // std::cout << "Checked:  " << checked << ", found valid: " << total << std::endl;
    return total;
}

}  // namespace CaDiCaL
