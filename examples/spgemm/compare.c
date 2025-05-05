// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdlib.h>

// typedef struct {
//     int *arr1;
//     int *arr2;
// } CompareContext;

// int compare(const void *a, const void *b, void *context) {
//     int i = *(const int *)a;
//     int j = *(const int *)b;
//     CompareContext *ctx = (CompareContext *)context;

//     if (ctx->arr1[i] != ctx->arr1[j])
//         return ctx->arr1[i] - ctx->arr1[j]; // Primary sort by arr1
//     return ctx->arr2[i] - ctx->arr2[j];     // Secondary sort by arr2
// }

// int main() {
//     int arr1[] = {30, 10, 20, 10};
//     int arr2[] = {3, 2, 1, 4};   // used as secondary sort key
//     int arr3[] = {300, 200, 100, 400}; // aligned array

//     int n = sizeof(arr1) / sizeof(arr1[0]);
//     int idx[n];
//     for (int i = 0; i < n; i++) idx[i] = i;

//     CompareContext ctx = {arr1, arr2};
//     qsort_r(idx, n, sizeof(int), compare, &ctx);

//     // Create sorted arrays
//     int sorted1[n], sorted2[n], sorted3[n];
//     for (int i = 0; i < n; i++) {
//         sorted1[i] = arr1[idx[i]];
//         sorted2[i] = arr2[idx[i]];
//         sorted3[i] = arr3[idx[i]];
//     }

//     // Copy back if needed
//     for (int i = 0; i < n; i++) {
//         arr1[i] = sorted1[i];
//         arr2[i] = sorted2[i];
//         arr3[i] = sorted3[i];
//     }

//     // Print final result
//     for (int i = 0; i < n; i++) {
//         printf("%d %d %d\n", arr1[i], arr2[i], arr3[i]);
//     }

//     return 0;
// }
/*
 * SPDX-FileCopyrightText: 2024 Binsparse Developers
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <binsparse/binsparse.h>
 #include <stdio.h>
 #include <math.h>
 #include <float.h>  // For FLT_EPSILON
//  int check_array_equivalence(bsp_array_t array1, bsp_array_t array2) {
//     if (array1.size != array2.size) {
//       fprintf(stderr, "Array sizes do not match. %zu != %zu\n", array1.size,
//               array2.size);
//       return 1;
//     }
  
//     if (array1.size == 0) {
//       return 0;
//     }
  
//     bsp_matrix_market_type_t mm_type1 = BSP_MM_REAL;
  
//     if ((array1.type >= BSP_UINT8 && array1.type <= BSP_INT64) ||
//         array1.type == BSP_BINT8) {
//       mm_type1 = BSP_MM_INTEGER;
//     } else if (array1.type >= BSP_FLOAT32 && array1.type <= BSP_FLOAT64) {
//       mm_type1 = BSP_MM_REAL;
//     } else if (array1.type == BSP_COMPLEX_FLOAT32 ||
//                array1.type == BSP_COMPLEX_FLOAT64) {
//       mm_type1 = BSP_MM_COMPLEX;
//     } else {
//       fprintf(stderr, "Unhandled array type.\n");
//       return 2;
//     }
  
//     bsp_matrix_market_type_t mm_type2 = BSP_MM_REAL;
  
//     if ((array2.type >= BSP_UINT8 && array2.type <= BSP_INT64) ||
//         array2.type == BSP_BINT8) {
//       mm_type2 = BSP_MM_INTEGER;
//     } else if (array2.type >= BSP_FLOAT32 && array2.type <= BSP_FLOAT64) {
//       mm_type2 = BSP_MM_REAL;
//     } else if (array2.type == BSP_COMPLEX_FLOAT32 ||
//                array2.type == BSP_COMPLEX_FLOAT64) {
//       mm_type2 = BSP_MM_COMPLEX;
//     } else {
//       fprintf(stderr, "Unhandled array type.\n");
//       return 2;
//     }
  
//     if (mm_type1 != mm_type2) {
//       fprintf(stderr, "Array types do not match.\n");
//       return 3;
//     }
  
//     for (size_t i = 0; i < array1.size; i++) {
//       if (mm_type1 == BSP_MM_INTEGER) {
//         size_t value1, value2;
//         bsp_array_read(array1, i, value1);
//         bsp_array_read(array2, i, value2);
//         //print value1
//         printf("value1: %zu\n", value1);
//         if (value1 != value2) {
//           fprintf(stderr, "Array values are not equal. (%zu != %zu)\n", value1,
//                   value2);
//           return 4;
//         }
//       } else if (mm_type1 == BSP_MM_REAL) {
//         double value1, value2;
//         bsp_array_read(array1, i, value1);
//         bsp_array_read(array2, i, value2);
  
//         if (value1 != value2) {
//           fprintf(stderr, "Array values are not equal. (%.17lg != %.17lg)\n",
//                   value1, value2);
//           return 4;
//         }
//       } else if (mm_type1 == BSP_MM_COMPLEX) {
//         double _Complex value1, value2;
//         bsp_array_read(array1, i, value1);
//         bsp_array_read(array2, i, value2);
  
//         if (value1 != value2) {
//           fprintf(stderr,
//                   "Array values are not equal. (%.17lg + i%.17lg != %.17lg + "
//                   "i%.17lg)\n",
//                   __real__ value1, __imag__ value1, __real__ value2,
//                   __imag__ value2);
//           return 4;
//         }
//       }
//     }
//  }
typedef struct {
  int index02;
  int index01;
  double value1;
} Entry;

int compare(const void *a, const void *b) {
  Entry *ea = (Entry *)a;
  Entry *eb = (Entry *)b;
  
  if (ea->index02 != eb->index02)
      return ea->index02 - eb->index02;
  else
      return ea->index01 - eb->index01;
}
 int main(int argc, char** argv) {
    char* file1 = argv[1];
    char* file2 = argv[2];

    bsp_fdataset_info_t info1 = bsp_parse_fdataset_string(argv[1]);
    bsp_fdataset_info_t info2 = bsp_parse_fdataset_string(argv[2]);
    bsp_matrix_t matrix1 = bsp_read_matrix(info1.fname, info1.dataset);
    bsp_matrix_t matrix2 = bsp_read_matrix(info2.fname, info2.dataset);
    printf("Matrix 1 format: %s\n", bsp_get_matrix_format_string(matrix1.format));
    printf("Matrix 2 format: %s\n", bsp_get_matrix_format_string(matrix2.format));
    //print matrix1.values
    bsp_array_t array1 = matrix1.values;
    bsp_array_t array2 = matrix2.values;
    if (array1.size != array2.size) {
        fprintf(stderr, "Array sizes do not match. %zu != %zu\n", array1.size,
                array2.size);
        return 1;
      }
    
      if (array1.size == 0) {
        return 0;
      }
    
      bsp_matrix_market_type_t mm_type1 = BSP_MM_REAL;
    
      if ((array1.type >= BSP_UINT8 && array1.type <= BSP_INT64) ||
          array1.type == BSP_BINT8) {
        mm_type1 = BSP_MM_INTEGER;
      } else if (array1.type >= BSP_FLOAT32 && array1.type <= BSP_FLOAT64) {
        mm_type1 = BSP_MM_REAL;
      } else if (array1.type == BSP_COMPLEX_FLOAT32 ||
                 array1.type == BSP_COMPLEX_FLOAT64) {
        mm_type1 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      bsp_matrix_market_type_t mm_type2 = BSP_MM_REAL;
    
      if ((array2.type >= BSP_UINT8 && array2.type <= BSP_INT64) ||
          array2.type == BSP_BINT8) {
        mm_type2 = BSP_MM_INTEGER;
      } else if (array2.type >= BSP_FLOAT32 && array2.type <= BSP_FLOAT64) {
        mm_type2 = BSP_MM_REAL;
      } else if (array2.type == BSP_COMPLEX_FLOAT32 ||
                 array2.type == BSP_COMPLEX_FLOAT64) {
        mm_type2 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      if (mm_type1 != mm_type2) {
        fprintf(stderr, "Array types do not match.\n");
        return 3;
      }
     
    //create tmp array that of each type is real
    double tmp_values1[array1.size]; 
    double tmp_values2[array2.size]; 
    
    for (size_t i = 0; i < array1.size; i++){
        
        if (mm_type2 == BSP_MM_INTEGER) {
            size_t value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            //print it is integer
            // printf("it is integer\n");
            
          } else if (mm_type2 == BSP_MM_REAL) {
            double value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
           
            tmp_values1[i] = value1;
            tmp_values2[i] = value2;
           
          } else if (mm_type2 == BSP_MM_COMPLEX) {
            double _Complex value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            printf("it is complex\n");
            //print value1 in real number form
            
            
          }
          else{
            printf("bsp_array_read is not working\n");
          }
          //print tmp_array
          
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    array1 = matrix1.indices_1;
    array2 = matrix2.indices_1;
   
    if (array1.size != array2.size) {
        fprintf(stderr, "Array sizes do not match. %zu != %zu\n", array1.size,
                array2.size);
        return 1;
      }
      
      if (array1.size == 0) {
        return 0;
      }
    
      mm_type1 = BSP_MM_REAL;
    
      if ((array1.type >= BSP_UINT8 && array1.type <= BSP_INT64) ||
          array1.type == BSP_BINT8) {
        mm_type1 = BSP_MM_INTEGER;
      } else if (array1.type >= BSP_FLOAT32 && array1.type <= BSP_FLOAT64) {
        mm_type1 = BSP_MM_REAL;
      } else if (array1.type == BSP_COMPLEX_FLOAT32 ||
                 array1.type == BSP_COMPLEX_FLOAT64) {
        mm_type1 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      mm_type2 = BSP_MM_REAL;
    
      if ((array2.type >= BSP_UINT8 && array2.type <= BSP_INT64) ||
          array2.type == BSP_BINT8) {
        mm_type2 = BSP_MM_INTEGER;
      } else if (array2.type >= BSP_FLOAT32 && array2.type <= BSP_FLOAT64) {
        mm_type2 = BSP_MM_REAL;
      } else if (array2.type == BSP_COMPLEX_FLOAT32 ||
                 array2.type == BSP_COMPLEX_FLOAT64) {
        mm_type2 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      if (mm_type1 != mm_type2) {
        fprintf(stderr, "Array types do not match.\n");
        return 3;
      }
    //create tmp array that of each type is real
    // double tmp_array[array1.size]; 
    //print array1.size
    
    int tmp_indices11[array1.size]; 
    int tmp_indices12[array2.size]; 
    for (size_t i = 0; i < array1.size; i++){
        if (mm_type2 == BSP_MM_INTEGER) {
            size_t value1, value2;
            
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            //print it is integer
            // printf("it is integer\n");
            tmp_indices11[i] = value1;
            tmp_indices12[i] = value2;
            
          } else if (mm_type2 == BSP_MM_REAL) {
            //print it is rea;
            // printf("it is real\n");
            double value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            // printf("it is real\n");
            // //print value1 in real number form
            // printf("value1: %.17lg\n", value1);
            
            
          } else if (mm_type2 == BSP_MM_COMPLEX) {
            // printf("it is complex\n");
            double _Complex value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            printf("it is complex\n");
            //print value1 in real number form
            
           
          }
          //print tmp_array
          
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    array1 = matrix1.indices_0;
    array2 = matrix2.indices_0;
    //print array1.size
    if (array1.size != array2.size) {
        fprintf(stderr, "Array sizes do not match. %zu != %zu\n", array1.size,
                array2.size);
        return 1;
      }
      
      if (array1.size == 0) {
        return 0;
      }
    
      mm_type1 = BSP_MM_REAL;
    
      if ((array1.type >= BSP_UINT8 && array1.type <= BSP_INT64) ||
          array1.type == BSP_BINT8) {
        mm_type1 = BSP_MM_INTEGER;
      } else if (array1.type >= BSP_FLOAT32 && array1.type <= BSP_FLOAT64) {
        mm_type1 = BSP_MM_REAL;
      } else if (array1.type == BSP_COMPLEX_FLOAT32 ||
                 array1.type == BSP_COMPLEX_FLOAT64) {
        mm_type1 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      mm_type2 = BSP_MM_REAL;
    
      if ((array2.type >= BSP_UINT8 && array2.type <= BSP_INT64) ||
          array2.type == BSP_BINT8) {
        mm_type2 = BSP_MM_INTEGER;
      } else if (array2.type >= BSP_FLOAT32 && array2.type <= BSP_FLOAT64) {
        mm_type2 = BSP_MM_REAL;
      } else if (array2.type == BSP_COMPLEX_FLOAT32 ||
                 array2.type == BSP_COMPLEX_FLOAT64) {
        mm_type2 = BSP_MM_COMPLEX;
      } else {
        fprintf(stderr, "Unhandled array type.\n");
        return 2;
      }
    
      if (mm_type1 != mm_type2) {
        fprintf(stderr, "Array types do not match.\n");
        return 3;
      }
    //create tmp array that of each type is real
    // double tmp_array[array1.size]; 
    //print array1.size
    // printf("array1.size: %zu\n", array1.size);
    int tmp_indices01[array1.size]; 
    int tmp_indices02[array2.size]; 
    for (size_t i = 0; i < array1.size; i++){
        if (mm_type2 == BSP_MM_INTEGER) {
            size_t value1, value2;
           
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            //printf value1
            tmp_indices01[i] = value1;
            tmp_indices02[i] = value2;
            //print it is integer
            // printf("it is integer\n");
           
            // printf("value1: %i\n",  tmp_indices1[i] );
            // printf("value2: %i\n",  tmp_indices2[i] );
          } else if (mm_type2 == BSP_MM_REAL) {
            //print it is rea;
            // printf("it is real\n");
            double value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            // printf("it is real\n");
            // //print value1 in real number form
            // printf("value1: %.17lg\n", value1);
            
          } else if (mm_type2 == BSP_MM_COMPLEX) {
            // printf("it is complex\n");
            double _Complex value1, value2;
            bsp_array_read(array1, i, value1);
            bsp_array_read(array2, i, value2);
            // printf("it is complex\n");
            //print value1 in real number form
            
          }
          //print tmp_array

    }
   
    int SIZE = array1.size;
    Entry entries[SIZE];
    

    // Pack the data into structs
    for (int i = 0; i < SIZE; i++) {
        entries[i].index02 = tmp_indices02[i];
        entries[i].index01 = tmp_indices01[i];
        entries[i].value1 = tmp_values1[i];
    }

    // Sort using qsort
    qsort(entries, SIZE, sizeof(Entry), compare);

    // Unpack the sorted data
    for (int i = 0; i < SIZE; i++) {
        tmp_indices02[i] = entries[i].index02;
        tmp_indices01[i] = entries[i].index01;
        tmp_values1[i] = entries[i].value1;
    }
    Entry entries1[SIZE];
    

    // Pack the data into structs
    for (int i = 0; i < SIZE; i++) {
        entries1[i].index02 = tmp_indices12[i];
        entries1[i].index01 = tmp_indices11[i];
        entries1[i].value1 = tmp_values2[i];
    }

    // Sort using qsort
    qsort(entries, SIZE, sizeof(Entry), compare);

    // Unpack the sorted data
    for (int i = 0; i < SIZE; i++) {
        tmp_indices02[i] = entries[i].index02;
        tmp_indices01[i] = entries[i].index01;
        tmp_values1[i] = entries[i].value1;
    }
    //compare tmp_values1 and tmp_values2
    for (int i = 0; i < SIZE; i++) {
    //   if (fabsf(a - b) < FLT_EPSILON * fmaxf(fabsf(a), fabsf(b))) {
    //     printf("tmp_values1[2335] and tmp_values2[2335] are equal (within epsilon)\n");
    // } else {
    //     printf("tmp_values1[2335] and tmp_values2[2335] are different\n");
    // }
        if (fabs(tmp_values1[i] - tmp_values2[i]) < FLT_EPSILON) {
            printf("tmp_values1[%d] = %f, tmp_values2[%d] = %f\n", i, tmp_values1[i], i, tmp_values2[i]);
            printf("there is a problem\n");
            return 1;
        }
        
    }
    //compare tmp_indices11 and tmp_indices12
    for (int i = 0; i < SIZE; i++) {
        // if (tmp_indices11[i] != tmp_indices12[i]) {
        //     printf("tmp_indices11[%d] = %d, tmp_indices12[%d] = %d\n", i, tmp_indices11[i], i, tmp_indices12[i]);
        // }
        if (fabs(tmp_indices11[i] - tmp_indices12[i]) > FLT_EPSILON) {
          printf("tmp_values1[%d] = %f, tmp_values2[%d] = %f\n", i, tmp_values1[i], i, tmp_values2[i]);
          printf("there is a problem\n");
          return 1;
      }
    }
    //compare tmp_indices01 and tmp_indices02
    for (int i = 0; i < SIZE; i++) {
        // if (tmp_indices01[i] != tmp_indices02[i]) {
        //     printf("tmp_indices01[%d] = %d, tmp_indices02[%d] = %d\n", i, tmp_indices01[i], i, tmp_indices02[i]);
        // }
        if (fabs(tmp_indices01[i] - tmp_indices02[i]) > FLT_EPSILON) {
          printf("tmp_values1[%d] = %f, tmp_values2[%d] = %f\n", i, tmp_values1[i], i, tmp_values2[i]);
          printf("there is a problem\n");
          return 1;
      }
    }
    printf("two matrix are equal\n");

 }
//  261181.50590463998
//three arrays are created
// 1. tmp_values1
// 2. tmp_values2
// 3. tmp_indices11
// 4. tmp_indices01
// 5. tmp_indices02
// 6. tmp_indices12


// int tmp_indices11[array1.size]; 
// //     int tmp_pointer12[array2.size]; 
// int tmp_indices11[array1.size]; 
//     int tmp_pointer12[array2.size]; 