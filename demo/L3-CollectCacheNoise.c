/* This file collects cache noise from Linux systems. It probes every 64 sets
 * continuously and repeatedly (with gaps of 64 sets to avoid data prefetching),
 * and uses set i to represent the actual set i*64. It probes each set for 
 * SAMPLES*10(10010*10) times and prints the results - (set, miss number of this 
 * set) into text and CSV files. CSV files are used for more visual observation, 
 * and text files are used for easier use by further exploitation.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <mastik/util.h>
#include <mastik/l3.h>

#define SAMPLES 1010

int main(int ac, char **av)
{
    FILE *fpCsv = fopen("NoiseData/L3-NoiseData.csv", "a+");
    if (fpCsv == NULL)
    {
        return 0;
    }

    FILE *fpTxt = fopen("NoiseData/L3-NoiseData.txt", "a+");
    if (fpTxt == NULL)
    {
        return 0;
    }

    int Round = 10;
    for (int i = 0; i < Round; i++)
    {
        delayloop(3000000000U);

        l3pp_t l3 = l3_prepare(NULL, NULL);

        int nsets = l3_getSets(l3);
        // make sure l3_getSets function returns the right result
        /*
        if(nsets != 8192){
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
        for (int i = 0; i < SAMPLES * nmonitored; i += 4096 / sizeof(uint16_t))
            res[i] = 1;

        // Repeatedly probe sets
        l3_repeatedprobecount(l3, SAMPLES, res, 0);

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
                }
            }
        }

        // Filter out the first ten results of samples to eliminate interference
        for (int i = 10; i < SAMPLES; i++)
        {

            printf("Round %d: ", i);
            fprintf(fpCsv, "Round %d,", i);
            int total = 0;
            for (int j = 0; j < nmonitored - 1; j++)
            {
                printf("(%d,%d) ", j, res[i * nmonitored + j]);
                fprintf(fpCsv, "%d,", res[i * nmonitored + j]);
                fprintf(fpTxt, "%d,", res[i * nmonitored + j]);
                total += res[i * nmonitored + j];
            }

            int j = nmonitored - 1;
            printf("(%d,%d) ", j, res[i * nmonitored + j]);
            fprintf(fpCsv, "%d,", res[i * nmonitored + j]);
            fprintf(fpTxt, "%d\n", res[i * nmonitored + j]);
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

        free(res);
        l3_release(l3);
    }

    fclose(fpCsv);
    fclose(fpTxt);
}
