/* Core local spin system algorithm.
 *
 * Matthew Carbone, Columbia University 2020
 *
 */

#include <math.h>
#include <string>
#include <fstream>      // std::ofstream
#include <assert.h>
#include <stdio.h>

#include "utils/init_utils.h"
#include "utils/general_utils.h"
#include "utils/grid_utils.h"


void gillespie(EnergyGrid &energy_grid, const int log_N_timesteps,
    const int N_spins, const double beta, const double beta_critical,
    const int landscape)
{

    /* We first initialize counters, trackers and grids for the various
    observables we are interested in recording.*/

    // ========================================================================
    // Random =================================================================
    // ========================================================================

    // Initialize the MT random number generator and seed with random_device
    std::mt19937 generator;
    unsigned int seed = std::random_device{}();
    generator.seed(seed);

    // ========================================================================
    // Spin system & simulation ===============================================
    // ========================================================================

    // The config is an N_spins-length integer binary array indexing whether a
    // spin is up or down.
    int config[N_spins];
    initialize_spin_system(config, N_spins);

    // Initialize the energy dictionary or array for faster lookups. Note that
    // the energy array is huge and need to be explicitly allocated on the
    // heap else we will get a stackoverflow error for N ~ 20 or so.
    const long long n_configs = ipow(2, N_spins);
    const long long N_timesteps = ipow(10, log_N_timesteps);
    double *energy_arr = new double[n_configs];
    if (landscape == 0)
    {
        initialize_energy_mapping_exponential_arr(energy_arr, n_configs,
            beta_critical);
    }
    else
    {
        initialize_energy_mapping_gaussian_arr(energy_arr, n_configs, N_spins,
            beta_critical);
    }

    // The current energy is the energy of the current configuraiton BEFORE
    // stepping to the next one at the end of each algorithm step.
    double current_energy;

    // Vector of the neighboring energies which is rewritten at every step of
    // the while loop. Also a vector of the dE values, exit rates...
    double neighboring_energies[N_spins];
    double exit_rates[N_spins];
    double delta_E[N_spins];
    double total_exit_rate;
    double waiting_time;

    // Initialize an array for tracking the inherent structures. This is
    // basically a mapping between the index of the array (configuration) and
    // the inherent structure configuration, the value.
    long long *inherent_structure_mapping = new long long[n_configs];

    // Store every entry as -1 (to indicate that none exists yet)
    for (long long ii=0; ii<n_configs; ii++){
        inherent_structure_mapping[ii] = -1;
    }

    // ========================================================================
    // Run the simulation =====================================================
    // ========================================================================

    double current_time = 0.0;
    while (true)
    {

        current_energy = energy_arr[binary_vector_to_int(config, N_spins)];

        // Step 1: get the neighboring energies by filling the relevant object
        get_neighboring_energies(config, energy_arr, neighboring_energies,
            N_spins);

        // Step 2: get the exit rates and dE values
        get_exit_rates(current_energy, beta, neighboring_energies, exit_rates, 
            delta_E, N_spins);
        total_exit_rate = 0.0;
        for (int ii=0; ii<N_spins; ii++){total_exit_rate += exit_rates[ii];}

        // Step 3: initialize an exponential distribution to sample the
        // waiting time from
        std::exponential_distribution<double> tmp_exp_dist(total_exit_rate);
        waiting_time = tmp_exp_dist(generator);

        // At this point: we understand that the tracer is in the configuration
        // it was in at the beginning of this step, and that it is in that
        // configuration for `waiting_time` time, and the new current_time
        // after stepping is `current_time` + `waiting_time`.

        // Step 4: update the current time of the simulation clock
        current_time += waiting_time;

        energy_grid.step(current_time, current_energy, config, N_spins,
            energy_arr, inherent_structure_mapping);

        // Step 5: step to the next state and store the proposed (new) energy
        step_next_state_(config, exit_rates, total_exit_rate, N_spins,
            generator);

        if (current_time >= N_timesteps){break;}

    }

    delete[] energy_arr;
    delete[] inherent_structure_mapping;
}
