#ifndef OBS_AGE_H
#define OBS_AGE_H

#include <vector>

#include "Utils/structures.h"


class Aging
{
protected:
    // The pi 1 and 2 grids
    std::vector<long long> grid_pi1;
    std::vector<long long> grid_pi2;
    int length_pi1, length_pi2;
    long long max_time_pi1, max_time_pi2;

    // Define the pointers
    int pointer1 = 0;
    int pointer2 = 0;

    // The outstream for this tracker
    FILE *outfile_pi1;
    FILE *outfile_pi2;
public:
    Aging(const FileNames);
    ~Aging();
};


class AgingConfig : public Aging
{
private:
    // Helpers
    void _help_step_1_(const long double, const long long, const Vals);
    void _help_step_2_(const long double, const long long, const Vals);

public:
    AgingConfig(const FileNames);
    void step_(const long double, const long long, const Vals);
};


class AgingBasin : public Aging
{
private:
    RuntimeParameters rtp;
    // The basin indexes reference the last basin that a tracer was in, or the
    // basin the tracer is currently in. Whether or not a tracer is currently
    // in a basin or not can be determined by comparing the energies to the
    // thresholds in the RuntimeParameters, rtp.
    long long bi1_E = 0;  // First basin index
    long long bi1_E_IS = 0;  // First basin index for the inherent structure
    long long bi2_E = 0;  // Second basin index
    long long bi2_E_IS = 0;  // Second basin index for the inherent structure
    long long bi1_S = 0;
    long long bi1_S_IS = 0;
    long long bi2_S = 0;
    long long bi2_S_IS = 0;

    // Helpers
    void _help_step_1_(const long double, const Vals, const Vals);
    void _help_step_2_(const long double, const Vals, const Vals);

public:
    AgingBasin(const FileNames, const RuntimeParameters);
    void step_(const long double, const Vals, const Vals);
};


#endif
