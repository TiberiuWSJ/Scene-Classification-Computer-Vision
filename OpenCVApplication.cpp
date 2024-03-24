// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iostream>

long int random(void);

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

void process_test_list() {
	const char* test_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\kaggle dataset\\test_WyRytb0.csv";
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
	const char* train_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\kaggle dataset\\train-scene classification\\train.csv";
	int size_train;
	Train_Element* train_list = read_train(train_path, &size_train);
	if (!train_list) {
		fprintf(stderr, "Failed to read CSV file.\n");
	}

	
	for (int i = 0; i < size_train; i++) {
		printf("Nume: %s, Eticheta: %d\n", train_list[i].nume_poza, train_list[i].eticheta);
	}
	printf("TRAIN LIST ROWS: %d\n", size_train - 1);

	freeTrain(train_list, size_train);
	printf("\n");
}

//Functie preluata din laborator
/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	// calculate the width of each bin
	int bin_width = imgHist.cols / hist_cols;


	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x * bin_width, baseline);
		Point p2 = Point(x * bin_width, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
	waitKey(0);
}

int* generare_etichete(Train_Element* list, int size_list) {
	srand(time(NULL));

	//facem un nou vector pentru noile etichete generate random
	int* etichete_generate = NULL;
	int size_etichete_generate = size_list;
	etichete_generate = (int*)malloc(size_etichete_generate * sizeof(int));

	for (int i = 0; i < size_etichete_generate; i++) {
		//generare eticheta random 1-6
		int random_number = 1 + rand() % (6 - 1 + 1);
		etichete_generate[i] = random_number;
	}
	return etichete_generate;
}

float calcul_acuratete(Train_Element* original, int* generate, int size) {
	int ok = 0;
	for (int i = 0; i < size; i++) {
		if (original[i].eticheta == generate[i]) {
			ok++;
		}
	}
	float acc = (float)ok / size;
	return acc;
}

//Functie care imparte train list-ul original in 2 liste: train(first 50%) si test (last 50%)
//Nu am facut inca free la memorie pentru cele 2 liste noi :) sper sa nu uit
//Momentan, histogramele sunt afisate din aceasta functie, ulterior trebuie mutate separat
void split_train() {
	const char* train_path = "C:\\Users\\Razvan\\Documents\\UTCN\\UTCN\\An 3\\Sem 2\\PI\\kaggle dataset\\train-scene classification\\train.csv";
	int size_train;
	Train_Element* train_list = read_train(train_path, &size_train);

	Train_Element* new_train_list = NULL;
	Train_Element* new_test_list = NULL;

	int new_train_size = size_train / 2;
	int new_test_size = size_train - new_train_size;

	new_train_list = (Train_Element*)malloc(new_train_size * sizeof(Train_Element));
	if (!new_train_list) {
		printf("Memory allocation failed for the first list\n");
		exit(1);
	}

	new_test_list = (Train_Element*)malloc(new_test_size * sizeof(Train_Element));
	if (!new_test_list) {
		printf("Memory allocation failed for the second list\n");
		exit(1);
	}

	for (int i = 0; i < new_train_size; i++) {
		new_train_list[i] = train_list[i];
	}

	for (int i = 0; i < new_test_size; i++) {
		new_test_list[i] = train_list[i + new_train_size];
	}


	for (int i = 0; i < new_train_size; i++) {
		printf("Train list / Nume: %s, Eticheta: %d\n", new_train_list[i].nume_poza, new_train_list[i].eticheta);
	}
	for (int i = 0; i < new_test_size; i++) {
		printf("Test list / Nume: %s, Eticheta: %d\n", new_test_list[i].nume_poza, new_test_list[i].eticheta);
	}

	printf("Original list size: %d \n", size_train);
	printf("Train list size: %d \n", new_train_size);
	printf("Test list size: %d \n", new_test_size);

	//test pentru histograma
	//o voi muta separat mai incolo

	//pentru histograma trebuie calculata frecventa etichetelor 
	

	//Nou vector care contine doar etichetele din train
	int* etichete_train = NULL;
	etichete_train = (int*)malloc(new_train_size * sizeof(int));
	for (int i = 0; i < new_train_size; i++) {
		etichete_train[i] = new_train_list[i].eticheta;
	}

	//vector care contine frecventa etichetelor din train
	int size = 6;
	int* frecv_etichete = NULL;
	frecv_etichete = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		frecv_etichete[i] = std::count(etichete_train, etichete_train + new_train_size, i+1);
	}
	for (int i = 0; i < size; i++) {
		printf("%d ", frecv_etichete[i]);
	}
	//desenam histograma in functie de frecventa etichetelor
	//showHistogram("Train Histogram", frecv_etichete, 200, 250);


	printf("\n");

	//Nou vector care contine doar etichetele din test
	int* etichete_test = NULL;
	etichete_test = (int*)malloc(new_test_size * sizeof(int));
	for (int i = 0; i < new_test_size; i++) {
		etichete_test[i] = new_test_list[i].eticheta;
	}

	int* frecv_etichete_test = NULL;
	frecv_etichete_test = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		frecv_etichete_test[i] = std::count(etichete_test, etichete_test + new_test_size, i + 1);
	}
	for (int i = 0; i < size; i++) {
		printf("%d ", frecv_etichete_test[i]);
	}

	//showHistogram("Test Histogram", frecv_etichete_test, 200, 250);



	//generare de etichete aici
	int* etichete_generate = generare_etichete(new_test_list, new_test_size);
	for (int i = 0; i < new_test_size; i++) {
		printf("Poza nr %d cu eticheta generata %d \n", i, etichete_generate[i]);
	}

	//calculam acuratetea in functie de cate etichete generate in mod random corect avem (pentru test_list)
	float acc = calcul_acuratete(new_test_list, etichete_generate, new_test_size);
	printf("Acuratete: %f \n", acc);

	//free cum ne-a invatat Daddy Oprisa
	free(etichete_train);
	free(frecv_etichete);
	free(etichete_test);
	free(frecv_etichete_test);
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
		printf(" 6 - Split train\n");
		printf(" 8 - Test generare etichete\n");
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
		case 6:
			split_train();
		//	break;
		//case 7:
		//	showHistogram();
		//	break;
		}
	} while (op != 0);
	return 0;
}
