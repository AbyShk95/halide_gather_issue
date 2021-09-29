#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "dsp/generated/resize.h"

int main(int argc, char **argv) {
    #pragma weak remote_session_control
    if (remote_session_control) { 
        struct remote_rpc_control_unsigned_module data; 
        data.enable = 1; 
        data.domain = CDSP_DOMAIN_ID; 
        remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)); 
    }
    /** 
     *  DSP Halide: Set iterations;
     */
    unsigned int timeTaken;
    int iterations = 100;
    if (resize_dspHalide_run(iterations, &timeTaken) == 0) {
        printf("Time taken for Halide execution is %d us\n", timeTaken);
    } else {
        printf("Halide failure\n");
    }

    return 0;
}
