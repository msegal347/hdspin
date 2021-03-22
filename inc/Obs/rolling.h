#ifndef OBS_ROLLING_H
#define OBS_ROLLING_H

#include "Obs/base.h"
#include "Utils/structures.h"

struct ridge_tracker
{
    double mu0 = 0.0;
    double mu1 = 0.0;
    double S0 = 0.0;
    double S1 = 0.0;
    long long counter = 1;
    double current_min = 1e15;
    double current_max = -1e15;
};

class Rolling : public Base
{
private:

    RuntimeParameters rtp;

    // Private data for the rolling means and variances for the ridge energy
    // calculation.
    ridge_tracker E_e_same, E_e_diff, S_e_same, S_e_diff;
    ridge_tracker E_IS_e_same, E_IS_e_diff, S_IS_e_same, S_IS_e_diff;

    // Last energies that were under the threshold
    double E_last_energy = 0.0;
    double E_current_ridge = 0.0;

    double E_IS_last_energy = 0.0;
    double E_IS_current_ridge = 0.0;

    double S_last_energy = 0.0;
    double S_current_ridge = 0.0;

    double S_IS_last_energy = 0.0;
    double S_IS_current_ridge = 0.0;

    void _log_ridge_E(const Vals);
    void _log_ridge_E_IS(const Vals);
    void _log_ridge_S(const Vals);
    void _log_ridge_S_IS(const Vals);

public:

    // Constructor: reads in the grid from the specified grid directory
    Rolling(const FileNames, const RuntimeParameters);

    // Step the grid by performing the following steps:
    // 1) Stepping the pointer
    // 2) Saving the configuration/energy information to disk
    void step_(const Vals, const Vals);

    ~Rolling();
};


#endif
