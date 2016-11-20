/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#define BLOCKSIZE 32


/* 
 * Please fill in the following team struct 
 */
team_t team = {
  "HunkydoryN",              /* Team name */

  "Neha Telhan",     /* First member full name */
  "nt7ab@virginia.edu",  /* First member email address */

  "John Weber",            /* Second member full name (leave blank if none) */
  "jgw4sq@virginia.edu"   /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Another version of rotate
 */
void block_rotate_one(int dim, pixel *src, pixel *dst) {

  int i=0;
  int j=0;
  int k=0;
  int l=0;

  for (i=0; i<dim; i+=BLOCKSIZE)
    for (j=0; j<dim; j+=BLOCKSIZE)
      for (k=i; k<i+BLOCKSIZE; k++)
	for (l=j; l<j+BLOCKSIZE; l++)
	  dst[RIDX(dim-1-l, k, dim)] = src[RIDX(k,l,dim)];
}
char block_rotate_one_descr[] = "attempt one at rotating block";

char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
  naive_rotate(dim, src, dst);
}

void block_rotate_two(int dim, pixel *src, pixel *dst) {
  /* switched the i and j's and took out some computation inside fourth loop*/
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  int A = dim*dim - dim;

  for (j=0; j<dim; j+=BLOCKSIZE) {
    for (i=0; i<dim; i+=BLOCKSIZE) {
      for (k=j; k<j+BLOCKSIZE; k++) {
	int tempy = A-k*dim;
	for (l=i; l<i+BLOCKSIZE; l++) {
	  dst[tempy + l] = src[l*dim + k];
	}
      }
    }
  }
}
char block_rotate_two_descr[] = "attempt TWO at rotating block";



/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
  add_rotate_function(&naive_rotate, naive_rotate_descr);   
  add_rotate_function(&rotate, rotate_descr);   
  /* ... Register additional test functions here */
  add_rotate_function(&block_rotate_one, block_rotate_one_descr);
  add_rotate_function(&block_rotate_two, block_rotate_two_descr);

}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
  int red;
  int green;
  int blue;
  int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
  sum->red = sum->green = sum->blue = 0;
  sum->num = 0;
  return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
  sum->red += (int) p.red;
  sum->green += (int) p.green;
  sum->blue += (int) p.blue;
  sum->num++;
  return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
  current_pixel->red = (unsigned short) (sum.red/sum.num);
  current_pixel->green = (unsigned short) (sum.green/sum.num);
  current_pixel->blue = (unsigned short) (sum.blue/sum.num);
  return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
  int ii, jj;
  pixel_sum sum;
  pixel current_pixel;

  initialize_pixel_sum(&sum);
  for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
    for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
      accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

  assign_sum_to_pixel(&current_pixel, sum);
  return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth p
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

char attemp1_smooth_descr[] = "attemp1 no ifs";
void attempt1_smooth(int dim, pixel *src, pixel *dst) 
{
  int i, j,ii,jj;
  pixel_sum  sum;
  initialize_pixel_sum(&sum);
  pixel zeroZero;

  //top left
  initialize_pixel_sum(&sum);
  accumulate_sum(&sum,  src[RIDX(1,1,dim)]);
  accumulate_sum(&sum,  src[RIDX(0,0,dim)]);
  accumulate_sum(&sum,  src[RIDX(1,0,dim)]);
  accumulate_sum(&sum,  src[RIDX(0,1,dim)]);

  assign_sum_to_pixel(&zeroZero, sum);
  dst[RIDX(0,0,dim)]= zeroZero;

  //top right
  initialize_pixel_sum(&sum);
  accumulate_sum(&sum,  src[RIDX(0,dim-1,dim)]);
  accumulate_sum(&sum,  src[RIDX(1,dim-1,dim)]);
  accumulate_sum(&sum,  src[RIDX(1,dim-2,dim)]);
  accumulate_sum(&sum,  src[RIDX(0,dim-2,dim)]);

  assign_sum_to_pixel(&zeroZero, sum);
  dst[RIDX(0,dim-1,dim)]= zeroZero;

  //bottom right
  initialize_pixel_sum(&sum);
  accumulate_sum(&sum,  src[RIDX(dim-1,dim-1,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-2,dim-1,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-1,dim-2,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-2,dim-2,dim)]);

  assign_sum_to_pixel(&zeroZero, sum);
  dst[RIDX(dim-1,dim-1,dim)]= zeroZero;
  //bottom left
  initialize_pixel_sum(&sum);
  accumulate_sum(&sum,  src[RIDX(dim-1,0,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-2,0,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-1,1,dim)]);
  accumulate_sum(&sum,  src[RIDX(dim-2,1,dim)]);

  assign_sum_to_pixel(&zeroZero, sum);
  dst[RIDX(dim-1,0,dim)]= zeroZero;



    
  initialize_pixel_sum(&sum);

  //middle pixels
  for (i = 1; i < (dim-1); i++){
    for (j = 1; j < (dim-1); j++){
      for(ii=(i-1); ii<(i+2); ii++){
	for(jj=(j-1);jj<(j+2);jj++){
	  
	  accumulate_sum(&sum,  src[RIDX(ii,jj,dim)]);
	}
      }
      assign_sum_to_pixel(&zeroZero, sum);
      dst[RIDX(i,j,dim)]= zeroZero;
    }
  }


  //top edge

  initialize_pixel_sum(&sum);

  for(i=0; i<1; i++){
    for(j=1; j<dim-1;j++){
      for(ii=i; ii<i+2;ii++){
	for(jj=j-1; jj<j+2; jj++){
	  accumulate_sum(&sum,  src[RIDX(ii,jj,dim)]);
	}
      }
      assign_sum_to_pixel(&zeroZero, sum);
      dst[RIDX(i,j,dim)]= zeroZero;
    }
  }
  initialize_pixel_sum(&sum);

  //bottom edge
  for(i=dim-1; i<dim; i++){
    for(j=1; j<dim-1;j++){
      for(ii=i-1; ii<dim;ii++){
	for(jj=j-1; jj<j+2; jj++){
	   
	  accumulate_sum(&sum,  src[RIDX(ii,jj,dim)]);
	}
      }
      assign_sum_to_pixel(&zeroZero, sum);
      dst[RIDX(i,j,dim)]= zeroZero;
    }
  }
  initialize_pixel_sum(&sum);

  //left edge
  for(i=1; i<dim-1; i++){
    for(j=0; j<1;j++){
      for(ii=i-1; ii<i+2;ii++){
	for(jj=j; jj<j+2; jj++){
        
	  accumulate_sum(&sum,  src[RIDX(ii,jj,dim)]);
	}
      }
      assign_sum_to_pixel(&zeroZero, sum);
      dst[RIDX(i,j,dim)]= zeroZero;
    }
  }
  initialize_pixel_sum(&sum);

  //right edge
  for(i=1; i<dim-1; i++){
    for(j=0; j<1;j++){
      for(ii=i-1; ii<i+2;ii++){
	for(jj=j-1; jj<j+1; jj++){
	          
	  accumulate_sum(&sum,  src[RIDX(ii,jj,dim)]);
	}
      }
      assign_sum_to_pixel(&zeroZero, sum);
      dst[RIDX(i,j,dim)]= zeroZero;
    }
  }




}

/*
 * smooth - Another version of smooth. 
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
  naive_smooth(dim, src, dst);
}


void attempt_two_smooth(int dim, pixel *src, pixel *dst) {

  pixel top_left;
  pixel top_right;
  pixel bottom_left;
  pixel bottom_right;
  pixel middle;
  pixel edge;

  int i, j;
  pixel_sum  sum;
  initialize_pixel_sum(&sum);


 /*Four corners*/
  //top left corner
  sum->red +=(int)top_left.red;
  sum->green += (int) top_left.green;
  sum->blue += (int) top_left.blue;
  //       sum->num++;
  //top right corner
  sum->red +=(int)top_right.red;
  sum->green += (int) top_right.green;
  sum->blue += (int) top_right.blue;
  //        sum->num++;
  //bottom left corner
  sum->red +=(int)bottom_left.red;
  sum->green += (int) bottom_left.green;
  sum->blue += (int) bottom_left.blue;
  //        sum->num++;
  //bottom right corner
  sum->red +=(int)bottom_right.red;
  sum->green += (int) bottom_right.green;
  sum->blue += (int) bottom_right.blue;
  //        sum->num++;




}







/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
  add_smooth_function(&smooth, smooth_descr);
  add_smooth_function(&naive_smooth, naive_smooth_descr);
  add_smooth_function(&attempt1_smooth, attemp1_smooth_descr);
  /* ... Register additional test functions here */
}
 