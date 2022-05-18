/** Adding an array to record each probe's time (cycles) would take up large 
 * memory space, and accessing these memory spaces would cause a lot of extra 
 * noise. We found that the time between each probe was easy to measure and 
 * calculate. This program is used to calculate the noise time between each 
 * probe situation.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <mastik/util.h>
#include <mastik/l3.h>

#define SAMPLES 1010

int main(int ac, char **av)
{
    FILE *fpCsv = fopen("NoiseData/L3-TimeData.csv", "w");
    if (fpCsv == NULL)
    {
        return 0;
    }

    FILE *fpTxt = fopen("NoiseData/L3-TimeData.txt", "w");
    if (fpTxt == NULL)
    {
        return 0;
    }

    FILE *fpTime = fopen("NoiseData/L3-Time.txt", "w");
    if (fpTxt == NULL)
    {
        return 0;
    }

    int Round = 1;
    for (int i = 0; i < Round; i++)
    {
        delayloop(3000000000U);

        l3pp_t l3 = l3_prepare(NULL, NULL);

        int nsets = l3_getSets(l3);
        int associativity = l3_getAssociativity(l3);
        int probeTime[associativity];
        int count[associativity];
        int filteredSets[nsets/64];
        for (size_t i = 0; i < associativity; i++)
        {
            probeTime[i] = 0;
            count[i] = 0;
        }
        for (size_t i = 0; i < nsets/64; i++)
        {
            filteredSets[i] = 0;
        }
        // make sure l3_getSets function returns the right result
        /*
        if (nsets != 8192)
        {
            i--;
            continue;
        }
        */
        int nmonitored = nsets / 64;
        printf("nsets: %d\n", nsets);
        fprintf(fpCsv, "nsets: %d \n", nsets);

        int n = 32;
        for (int i = n; i < nsets; i += 64)
            l3_monitor(l3, i);

        uint16_t *res = calloc(SAMPLES * nmonitored, sizeof(uint16_t));
        uint64_t *time = calloc(SAMPLES * nmonitored, sizeof(uint64_t));
        for (int i = 0; i < SAMPLES * nmonitored; i += 4096 / sizeof(uint16_t))
            res[i] = 1;
        for (int i = 0; i < SAMPLES * nmonitored; i += 4096 / sizeof(uint64_t))
            time[i] = 1;

        // Repeatedly probe sets
        l3_repeatedprobecountTime(l3, SAMPLES, res, time, 0);

        fprintf(fpCsv, "*****************%d*****************\n", i);
        fprintf(fpCsv, " ,");
        for (int j = 0; j < nmonitored - 1; j++)
        {
            fprintf(fpCsv, "Set%d,", j);
        }
        fprintf(fpCsv, "Set%d, Total\n", nmonitored);

        // Filter out fixed noise due to the execution and data access of this noise collection program
        for (int i = 0; i < nmonitored; i++)
        {
            int SetTotal = 0;
            for (int j = 10; j < SAMPLES; j++)
            {
                SetTotal += res[j * nmonitored + i];
            }
            if (SetTotal >= SAMPLES - 10)
            {
                for (int j = 10; j < SAMPLES; j++)
                {
                    res[j * nmonitored + i] = 0;
                    filteredSets[i] = 1;
                }
            }
        }

        // Filter out the first ten results of samples to eliminate interference
        for (int i = 10; i < SAMPLES; i++)
        {

            //printf("Round %d: ", i);
            fprintf(fpCsv, "Round %d,", i);
            int total = 0;
            for (int j = 0; j < nmonitored - 1; j++)
            {
                if (filteredSets[j+1] == 0)
                {
                    probeTime[res[i * nmonitored + j+1]] = (count[res[i * nmonitored + j+1]] * probeTime[res[i * nmonitored + j+1]] + time[i * nmonitored + j+1] - time[i * nmonitored + j]) / (count[res[i * nmonitored + j+1]] + 1);
                    count[res[i * nmonitored + j+1]]++;
                }
                
                //printf("(%d,%d) ", j, res[i * nmonitored + j]);
                fprintf(fpCsv, "%d,", res[i * nmonitored + j]);
                fprintf(fpTxt, "(%d,%ld), ", res[i * nmonitored + j], time[i * nmonitored + j]);
                total += res[i * nmonitored + j];
            }
            
            int j = nmonitored - 1;
            //printf("(%d,%d) ", j, res[i * nmonitored + j]);
            fprintf(fpCsv, "%d,", res[i * nmonitored + j]);
            fprintf(fpTxt, "(%d,%ld)\n", res[i * nmonitored + j], time[i * nmonitored + j]);
            total += res[i * nmonitored + j];
            fprintf(fpCsv, "%d\n", total);
        }

        fprintf(fpCsv, "Total,");
        int SetTotal = 0;
        for (int i = 0; i < nmonitored - 1; i++)
        {
            SetTotal = 0;
            for (int j = 10; j < SAMPLES; j++)
            {
                SetTotal += res[j * nmonitored + i];
            }
            fprintf(fpCsv, "%d,", SetTotal);
        }

        SetTotal = 0;
        for (int i = 10; i < SAMPLES; i++)
        {
            SetTotal += res[i * nmonitored + nmonitored - 1];
        }
        fprintf(fpCsv, "%d\n", SetTotal);
        putchar('\n');
        fprintf(fpCsv, "\n\n");


        for (size_t i = 0; i < associativity; i++)
        {
            fprintf(fpTime, "Miss number of a probe: %d, Time this probe takes(cycles): %d, Counts: %d\n", i, probeTime[i], count[i]);
        }

        free(res);
        l3_release(l3);
    }

    fclose(fpCsv);
    fclose(fpTxt);
}
