//
//  main.c
//  CX-4010 Assignment2
//
//
//  Student name: Xiaofan Wu
//  Student username: xwu319

#include <stdio.h>
#include <stdlib.h>

/**
 * Fill the matrix with data in the file
 *
 * @param row the number of rows
 * @param col the number of columns
 * @param matrix The matrix to fill
 * @param steam the file stream to scan from
 * @return a filled matrix
 */
int fill_matrix(int row, int col, double** matrix, FILE* stream){
    if(row<=0|| col<=0|| matrix == NULL|| stream == NULL){
    	return 1;
    }
    for(int i=0; i<row; i++){
    	for(int j=1; j<col; j++){
    		double currentdata;
    		fscanf(stream, "%lf", &currentdata);
    		matrix[i][j] = currentdata;
    	}
    }
    return 0;
}

/**
 * Allocate memory for a matrix
 *
 * @param row the number of rows
 * @param col the number of columns
 * @return a pointer of pointer of type double
 */
double**  malloc_matrix(int row, int col){
	double** matrix = NULL;
	if(row<=0|| col<=0){
		return NULL;
	}
	matrix = malloc(sizeof(double*)*row);
    for (int i=0; i<row; i++){
    	matrix[i] = malloc(sizeof(double)*col);
    }
    return matrix;
}

/**
 * Free a matrix's memory
 *
 * @param row the number of rows
 * @param col the number of columns
 * @param matrix the matrix to free
 */
void free_matrix(int row, int col, double** matrix){
	if(row>0 && col>0 && matrix != NULL){
		for(int i=0; i<row; i++){
			free(matrix[i]);
		}
		free(matrix);
	}
}

/**
 * Calculate the squared distance between data points in Kmeans output files and
 * a specific target data point, update the distances in the 0th column of the
 * matrix
 * @param data the matrix of data points in Kmeans output
 * @param point the target data point
 * @param num_items the number of data points in Kmeans output
 * @param num_attrs the dimension of the data in Kmeans output
 * @return 0 if executed successfully otherwise 1
 */
int Calculate_Distance(double** data, double* point, int num_items, int num_attrs){
    if (num_items<=0 || num_attrs<=0|| data==NULL|| point==NULL){
    	return 1;
    }
    for(int i = 0; i<num_items; i++){
    	double distance = 0;
    	for (int j=0; j<num_attrs; j++){
            distance += (data[i][j+2] - point[j+1])*(data[i][j+2] - point[j+1]);
    	}
        data[i][0] = distance;
    }
    return 0;
}

/**
 * Compare two double arrays based on their 0 th term
 * @param a the first double array
 * @param b the second double array
 * @return 1 if the first array is larger otherwise 0
 */
int Compare(double* a, double* b){
	if (a == NULL|| b==NULL){
		exit(1);
	}
	if(a[0] > b[0]){
		return 1;
	} else {
        return 0;
	}
}

/**
 * Sort the data points from Kmeans output based on their distances to the
 * target data point
 * @param data the matrix of data points in Kmeans output
 * @param num_items the number of data points in Kmeans output
 * @param num_attrs the dimension of the data in Kmeans output
 * @return 0 if executed successfully otherwise 1
 */
int Sort_by_Distance(double** data, int num_items, int num_attrs){
	if (num_items<=0 || num_attrs<=0|| data==NULL){
	    	return 1;
	    }
	for(int ed=0; ed<num_items - 1; ed++){
		for (int i=0; i<num_items - ed - 1; i++){
			if (Compare(data[i],data[i+1])){
				double* swap = data[i+1];
				data[i+1] = data[i];
				data[i] = swap;
			}
		}
	}
    return 0;
}

/**
 * Get the group assignment for the target data point
 * @param data the matrix of data points in Kmeans output
 * @param num_groups the total number of group assignments for data points in
 * Kmeans output.
 * @param k the first k number of closest data points to refer when assigning
 * groups to the target data point
 * @return 0 if executed successfully otherwise 1
 */
int Get_GroupNum(int num_groups, int k, double** data, int* groupFreq){
	if (num_groups<=0|| k<=0|| data==NULL|| groupFreq == NULL){
		exit(1);
	}
    for(int j=0; j<num_groups; j++){
    	groupFreq[j] = 0;
    }
    for (int i=0; i<k; i++){
    	int groupnum = (int)data[i][1];
    	groupFreq[groupnum] += 1;
    }
    int group_number = 0;
    for (int h=0; h<num_groups; h++){
    	if(groupFreq[group_number] < groupFreq[h]){
    		group_number = h;
    	}
    }
    return group_number;
}

//Main
//Scan the target data from a file
//Group the data with KNN algorithm
//Write the data along with their computed group assignments to a new file
int main(int argc, char** argv){
    if(argc < 5){
    	return 1;
    }
    const char* data_file = argv[1]; //non_target grouped data from Kmeans
    const char* points_file = argv[2]; //target data to be classified
    const char* output_file = argv[3];
    int k = atoi(argv[4]); //The number of closest points to pick in KNN
    if(k<=0){
        printf("Please enter a k greater than zero.");
    	return 1;
    }

    FILE* data_stream = fopen(data_file, "r");
    if(data_stream == NULL){
    	return 1;
    }
    int num_items, num_attrs, num_groups;
    fscanf(data_stream, "%d", &num_items);
    fscanf(data_stream, "%d", &num_attrs);
    fscanf(data_stream, "%d", &num_groups);
    double** data = malloc_matrix(num_items, num_attrs+2);
    if (data == NULL){
    	return 1;
    }
    if (fill_matrix(num_items, num_attrs+2, data, data_stream)){
    	return 1;
    }
    if(k>num_items){
        printf("Please enter a k smaller or equal to items available.");
        return 1;
    }
    fclose(data_stream); //write non-target grouped data to double** data

    FILE* points_stream = fopen(points_file, "r");
    if(points_stream == NULL){
    	return 1;
    }
    int num_items_p, num_attrs_p;
    fscanf(points_stream, "%d", &num_items_p);
    fscanf(points_stream, "%d", &num_attrs_p);
    if(num_attrs != num_attrs_p){
    	return 1;
    }
    double** points = malloc_matrix(num_items_p, num_attrs_p + 1);
    if(points == NULL){
    	return 1;
    }
    if(fill_matrix(num_items_p, num_attrs_p + 1, points, points_stream)){
        return 1;
    }
    fclose(points_stream); //write target data to double** points

    int* groupFreq = malloc(sizeof(int)*num_groups);
    //generate a 1-d array to record the frequency for each group/label
    if(groupFreq == NULL){
    	return 1;
    }
    for (int i=0; i<num_items_p; i++){
    	double* point = points[i];
    	if(Calculate_Distance(data, point, num_items, num_attrs)){
    		return 1;
    	}
    	if(Sort_by_Distance(data, num_items, num_attrs)){
    		return 1;
    	}
    	int groupnum = Get_GroupNum(num_groups, k, data, groupFreq);
        points[i][0] = groupnum;
        printf("Data Point: %d, ", i+1);
        for (int j=1; j<num_attrs_p + 1; j++){
            printf("%lf ", points[i][j]);
        }
        printf("Group Num: %d\n", groupnum);
		for (int i = 0; i < k; i++) {
			for (int j = 0; j < num_attrs + 1; j++) {
				printf("%lf ", data[i][j]);
			}
			printf("%lf\n", data[i][num_attrs + 1]);
        }
        printf("\n");
    }

    FILE* out = fopen(output_file, "w");
    if(out == NULL){
    	return 1;
    }
    for(int i=0; i<num_items_p; i++){
    	fprintf(out, "%.0lf ", points[i][0]);
    	for(int j=1; j<num_attrs_p+1; j++){
            fprintf(out, "%lf ", points[i][j]);
    	}
    	fprintf(out, "\n");
    }
    fclose(out);
    free(groupFreq);
    free_matrix(num_items, num_attrs+2, data);
    free_matrix(num_items_p, num_attrs_p+1, points);
    return 0;
}


