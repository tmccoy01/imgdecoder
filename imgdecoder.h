//
// Created by Tanner McCoy on 12/7/21.
//

#pragma once

#include "mex.h"
#include "imgutil.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void imgDecoder(char filename[], Node node);
int open_file(char file[], FILE **file_ident, int mode);