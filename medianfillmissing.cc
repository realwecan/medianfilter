#include <stdio.h>
#include <string.h>
#include "mex.h"
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

void CopyImage(double* src, double* dst, const int* dims)
{
  memcpy(dst, src, dims[0]*dims[1]*sizeof(double));
}

double GetPixel(double* thisim, const int* dims, int col, int row)
{
  return thisim[dims[0]*col+row]; //number of rows*col+row
}

int NumberOfMissingValues(double *im, const int* dims)
{
  int countMissingValues = 0;
  for (int i=0; i<dims[0]; i++) //for each row
  {
    for (int j=0; j<dims[1]; j++) //for each col
    {
      if ( GetPixel(im, dims, j, i) == 0 )
        countMissingValues++;
    }
  }
  return countMissingValues;
}

//set any pixel with depth < 100mm to zero (count towards missing)
void Preprocess(double *im, const int* dims)
{
  for (int i=0; i<dims[0]; i++) //for each row
  {
    for (int j=0; j<dims[1]; j++) //for each col
    {
      if ( GetPixel(im, dims, j, i) < 100 )
        im[dims[0]*j+i] = 0;
    }
  }
}

mxArray *process(const mxArray *mximage)
{
  double *im = (double *)mxGetPr(mximage);
  const int *dims = mxGetDimensions(mximage);

  int out[2];
  out[0] = dims[0]; out[1] = dims[1];
  mxArray *tmpimage = mxCreateNumericArray(2, out, mxDOUBLE_CLASS, mxREAL);
  double *tmpim = (double *)mxGetPr(tmpimage);
  mxArray *retimage = mxCreateNumericArray(2, out, mxDOUBLE_CLASS, mxREAL);
  double *retim = (double *)mxGetPr(retimage);

  Preprocess(im, dims);

  int tt = NumberOfMissingValues(im, dims);

  if ( NumberOfMissingValues(im, dims) == dims[0] * dims[1] )
  {
    CopyImage(im, retim, dims);
    return retimage;
  }

  CopyImage(im, retim, dims);

  double tempValue, newValue;
  int newValueCount;

  while ( NumberOfMissingValues(retim, dims) > 0 )
  {
    CopyImage(retim, tmpim, dims);
    for (int i=0; i<dims[0]; i++) //for each row
    {
      for (int j=0; j<dims[1]; j++) //for each col
      {
        if ( GetPixel(retim, dims, j, i) == 0 ) //use median filter to fill-in
        {
          newValue = 0; newValueCount = 0;
          for (int k=max(0,i-2); k<min(i+2,dims[0]); k++)
          {
            for (int l=max(0,j-2); l<min(j+2,dims[1]); l++)
            {
              tempValue = GetPixel(tmpim, dims, l, k);
              if ( tempValue != 0 )
              {
                newValue += tempValue;
                newValueCount++;
              }
            }
          }
          if (newValueCount > 0)
          {
            retim[dims[0]*j+i] = (int)((double)newValue / (double)newValueCount);
          }
        }
      }
    }
  }
  mxDestroyArray(tmpimage);
  return retimage;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 1)
    mexErrMsgTxt("Wrong number of inputs"); 
  if (nlhs != 1)
    mexErrMsgTxt("Wrong number of outputs");
  plhs[0] = process(prhs[0]);
}

