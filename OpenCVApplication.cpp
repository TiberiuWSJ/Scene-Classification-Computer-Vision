// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
	char* nume_poza;
	int eticheta;
}Train_Element;

#define MAX_LINE_LENGTH 1024

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i, j) = (r + g + b) / 3;
			}
		}

		imshow("original image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

char** read_test(const char* path, int* numRows) {
	FILE* file = fopen(path, "r"); 
	if (!file) {
		fprintf(stderr, "Error opening file.\n");
		return NULL;
	}

	char** list = NULL;
	int capacity = 10; 
	*numRows = 0;

	list = (char**)malloc(capacity * sizeof(char*));
	if (!list) {
		fclose(file);
		fprintf(stderr, "Memory allocation failed.\n");
		return NULL;
	}
	char line[MAX_LINE_LENGTH];
	fgets(line, sizeof(line), file); 
	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = '\0';
		//memorie pentru randul nou
		list[*numRows] = _strdup(line);
		(*numRows)++;

		//resize daca e necesar
		if (*numRows >= capacity) {
			capacity *= 2;
			char** newList = (char**)realloc(list, capacity * sizeof(char*));
			if (!newList) {
				fclose(file);
				fprintf(stderr, "Memory reallocation failed.\n");
				return NULL;
			}
			list = newList;
		}
	}

	fclose(file);
	return list;
}

Train_Element* read_train(const char* path, int* numRows) {
	FILE* file = fopen(path, "r"); 
	if (!file) {
		fprintf(stderr, "Error opening file.\n");
		return NULL;
	}

	Train_Element* list = NULL;
	int capacity = 10;
	*numRows = 0;

	list = (Train_Element*)malloc(capacity * sizeof(Train_Element));
	if (!list) {
		fclose(file);
		fprintf(stderr, "Memory allocation failed.\n");
		return NULL;
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file)) {

		line[strcspn(line, "\n")] = '\0';


		char* token = strtok(line, ",");
		if (!token) continue;  


		list[*numRows].nume_poza = _strdup(token);


		token = strtok(NULL, ",");
		if (!token) {
			free(list[*numRows].nume_poza); 
			continue;  
		}
		list[*numRows].eticheta = atoi(token);
		//schimbam etichetele de la 0-5 la 1-6
		list[*numRows].eticheta++;

		(*numRows)++;

		if (*numRows >= capacity) {
			capacity *= 2;
			Train_Element* newList = (Train_Element*)realloc(list, capacity * sizeof(Train_Element));
			if (!newList) {
				fclose(file);
				fprintf(stderr, "Memory reallocation failed.\n");
				return NULL;
			}
			list = newList;
		}
	}

	fclose(file);
	return list;
}


void freeTest(char** list, int size) {
	if (!list) return;

	for (int i = 0; i < size; i++) {
		free(list[i]);
	}
	free(list);
}

void freeTrain(Train_Element* list, int size) {
	if (!list) return;
	for (int i = 0; i < size; i++) {
		free(list[i].nume_poza);
	}
	free(list);
}

//am spart functia asta in 2 functii mai mici
//void splitLists() {
//	const char* test_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\Proiect\\test_WyRytb0.csv";
//	const char* train_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\Proiect\\train-scene classification\\train.csv";
//
//	int size_test;
//	int size_train;
//
//	char** test_list = read_test(test_path, &size_test);
//	Train_Element* train_list = read_train(train_path, &size_train);
//
//	if (!test_list || !train_list) {
//		fprintf(stderr, "Failed to read CSV file.\n");
//	}
//	printf("TEST LIST ROWS: %d\n", size_test);
//	for (int i = 0; i < size_test; i++) {
//		printf("%s\n", test_list[i]);
//	}
//	
//	freeTest(test_list, size_test);
//
//	printf("\n");
//
//	printf("TRAIN LIST ROWS: %d\n", size_train);
//	for (int i = 0; i < size_train; i++) {
//		printf("Nume: %s, Eticheta: %d\n", train_list[i].nume_poza, train_list[i].eticheta);
//	}
//
//	freeTrain(train_list, size_train);  
//
//
//}

void process_test_list() {
	const char* test_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\Proiect\\test_WyRytb0.csv";
	int size_test;
	char** test_list = read_test(test_path, &size_test);
	if (!test_list) {
		fprintf(stderr, "Failed to read CSV file.\n");
	}
	printf("TEST LIST ROWS: %d\n", size_test);
	for (int i = 0; i < size_test; i++) {
		printf("%s\n", test_list[i]);
	}

	freeTest(test_list, size_test);

	printf("\n");

}

void process_train_list() {
	const char* train_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\Proiect\\train-scene classification\\train.csv";
	int size_train;
	Train_Element* train_list = read_train(train_path, &size_train);
	if (!train_list) {
		fprintf(stderr, "Failed to read CSV file.\n");
	}

	printf("TRAIN LIST ROWS: %d\n", size_train);
	for (int i = 0; i < size_train; i++) {
		printf("Nume: %s, Eticheta: %d\n", train_list[i].nume_poza, train_list[i].eticheta);
	}

	freeTrain(train_list, size_train);
	printf("\n");
}


int main()
{
	int op;
	do
	{
		//system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Basic image opening...\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - Process and show Test List\n");
		printf(" 5 - Process and show Train List\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testColor2Gray();
			break;
		case 4:
			process_test_list();
			break;
		case 5:
			process_train_list();
			break;
		}
	} while (op != 0);
	return 0;
}
