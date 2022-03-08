#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "imgdecoder.h"
#include "mex.h"
#include "matrix.h"

// The gateway function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    Node node;
    int i;

//    char filename[256] = "/Users/tannermccoy/Development/Work/Matlab/IMG File Testing/data/imgn39w104_1.img";
    // Check arguments
    if (mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("StringIO:TypeError", "Input is not a string");
    }

    char *filename = mxArrayToString(prhs[0]);

    // Read the IMG file data into the node structure
    imgDecoder(filename, node);

    // Create an array to hold the MAP_INFO node data
    double map_info[3][2] = {
            {node.Map_Info.upperLeftCenter.x, node.Map_Info.upperLeftCenter.y},
            {node.Map_Info.lowerRightCenter.x, node.Map_Info.lowerRightCenter.y},
            {node.Map_Info.pixelSize.x, node.Map_Info.pixelSize.y}
    };

    // Set the output data
    const char* map_names[] = {"upperLeftCenter", "lowerRightCenter", "pixelSize"};
    const char* stats_names[] = {"minimum", "maximum", "mean", "median", "mode", "stddev"};
    mwSize dims[2] = {1, 2};

    plhs[0] = mxCreateStructArray((mwSize)2, dims, (mwSize)3, map_names);
//    plhs[1] = mxCreateStructArray((mwSize)2, dims, (mwSize)6, stats_names);

    // Set the values for the MAP_INFO structure
    for (i = 0; i < 3; i++) {
        mxArray *field_value_x, *field_value_y;

        field_value_x = mxCreateDoubleMatrix(1, 1, mxREAL);
        field_value_y = mxCreateDoubleMatrix(1, 1, mxREAL);

#if MX_HAS_INTERLEAVED_COMPLEX
        *mxGetDoubles(field_value_x) = map_info[i][0];
        *mxGetDoubles(field_value_y) = map_info[i][1];
#else
        *mxGetPr(field_value_x) = map_info[i][0];
        *mxGetPr(field_value_y) = map_info[i][1];
#endif

        // Set the values
        mxSetFieldByNumber(plhs[0], 0, i, field_value_x);
        mxSetFieldByNumber(plhs[0], 1, i, field_value_y);
    }
}

// Computational routine
void imgDecoder(char filename[256], Node node) {
    FILE *inputFile = NULL;

    if (open_file(filename, &inputFile, 'r') != 0) {
        fprintf(stderr, "ERROR:open_file, main: cannot open file");
    }

    // Read and store all the information for the given IMG file
    if (AllocateNodes(&node, &inputFile) != 0) {
        fprintf(stderr, "ERROR:AllocateNodes, main: cannot allocate nodes");
    }
}

int open_file(char file[], FILE **file_ident, int mode) {
    if (mode == 'w')
        *file_ident = fopen(file, "wb");
    else if (mode == 'r')
        *file_ident = fopen(file, "rb");
    else {
        fprintf(stderr, "ERROR: open_file: Cannot open file %s\n", file);
        return -1;
    }
    return 0;
}
