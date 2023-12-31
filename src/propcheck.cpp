#include "internal.hpp"

#include <iostream>

namespace CaDiCaL {

uint64_t Internal::propcheck_all_tree (const vector<int>& variables, uint64_t limit) {
    assert(level == 0);

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
        assert(level <= variables.size());
        // std::cout << "state = " << state << ", level = " << level << std::endl;

        if (state == 0) { // Descending
            if (level == variables.size()) {
                total++;
                if (limit && total > limit) {
                    // std::cout << "reached the limit: " << total << " > " << limit << std::endl;
                    break;
                }
                state = 1; // state = Ascending
            } else {
                int v = variables[level];
                const signed char s = cube[level];
                int lit = s * v;
                const signed char b = val(lit);

                if (b > 0) {
                    // Dummy level:
                    new_trail_level (0);

                    // Keep descending:
                    state = 0; // state = Descending

                } else if (b < 0) {
                    // Dummy level:
                    new_trail_level (0);

                    // Need to ascend:
                    state = 1; // state = Ascending

                } else {
                    // Enqueue the assumption:
                    search_assume_decision (lit);

                    // Need to propagate:
                    state = 2; // state = Propagating
                }
            }

        } else if (state == 1) { // Ascending
            assert(level > 0);

            // Find the last "0":
            int i = level;  // 1-based index
            while (i > 0 && cube[i - 1] > 0) {
                i--;
            }
            // std::cout << "i = " << i << std::endl;
            if (i == 0) {
                // Finish.
                break;
            }

            // Increment the cube:
            assert(cube[i - 1] == -1);
            cube[i - 1] = 1;
            for (int j = i; j < variables.size(); j++) {
                cube[j] = -1;
            }

            // Backtrack before the i-th level:
            backtrack(i - 1);

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
            if (!propagate()) {
                // Conflict.
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

    backtrack();

    // std::cout << "Checked:  " << checked << ", found valid: " << total << std::endl;
    return total;
}

}  // namespace CaDiCaL
