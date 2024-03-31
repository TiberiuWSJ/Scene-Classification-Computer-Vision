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

//Functie care citeste csv-ul orifinal cu test
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

//Functie care citeste csv-ul orifinal cu train
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

//Functia asta proceseaza lista test originala
//Nu mai folosim functia asta
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

//Functia asta proceseaza lista train originala
//nu mai folosim functia asta
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

// Input: o lista de struct cu nume poza si eticheta -> Output: o lista cu etichete generate random
int* generare_etichete(int size_list) {
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

//calcul acuratete
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

void afisare_acuratete(Train_Element* test_list, int* etichete_generate, int size) {
	//calculam acuratetea in functie de cate etichete generate in mod random corect avem (pentru test_list)
	float acc = calcul_acuratete(test_list, etichete_generate, size);
	printf("Acuratete: %f \n", acc);
}


void show_split_train(Train_Element* new_train_list, Train_Element* new_test_list, int new_train_size, int new_test_size) {
	for (int i = 0; i < new_train_size; i++) {
		printf("Train list / Nume: %s, Eticheta: %d\n", new_train_list[i].nume_poza, new_train_list[i].eticheta);
	}
	for (int i = 0; i < new_test_size; i++) {
		printf("Test list / Nume: %s, Eticheta: %d\n", new_test_list[i].nume_poza, new_test_list[i].eticheta);

	}
	printf("Train list size: %d \n", new_train_size);
	printf("Test list size: %d \n", new_test_size);

}

void histograme_test_train(Train_Element* train_list, Train_Element* test_list, int train_size, int test_size) {
	//Nou vector care contine doar etichetele din train
	int* etichete_train = NULL;
	etichete_train = (int*)malloc(train_size * sizeof(int));
	for (int i = 0; i < train_size; i++) {
		etichete_train[i] = train_list[i].eticheta;
	}

	//vector care contine frecventa etichetelor din train
	int size = 6;
	int* frecv_etichete = NULL;
	frecv_etichete = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		frecv_etichete[i] = std::count(etichete_train, etichete_train + train_size, i + 1);
	}
	for (int i = 0; i < size; i++) {
		printf("%d ", frecv_etichete[i]);
	}
	//desenam histograma in functie de frecventa etichetelor
	showHistogram("Train Histogram", frecv_etichete, 200, 250);


	printf("\n");

	//Nou vector care contine doar etichetele din test
	int* etichete_test = NULL;
	etichete_test = (int*)malloc(test_size * sizeof(int));
	for (int i = 0; i < test_size; i++) {
		etichete_test[i] = test_list[i].eticheta;
	}

	int* frecv_etichete_test = NULL;
	frecv_etichete_test = (int*)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++) {
		frecv_etichete_test[i] = std::count(etichete_test, etichete_test + test_size, i + 1);
	}
	for (int i = 0; i < size; i++) {
		printf("%d ", frecv_etichete_test[i]);
	}

	showHistogram("Test Histogram", frecv_etichete_test, 200, 250);

	free(etichete_train);
	free(etichete_test);
}

void afisare_etichete(Train_Element* test_list, int size) {
	int* etichete_generate = generare_etichete(size);
	for (int i = 0; i < size; i++) {
		printf("Poza: %s cu eticheta generata %d \n", test_list[i].nume_poza, etichete_generate[i]);
	}
}


void clearInputBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}


char* get_path() {
	printf("Enter the path to the original train.csv file: ");

	char* path = (char*)malloc(1024 * sizeof(char));
	if (!path) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	if (!fgets(path, 1024, stdin)) {
		free(path);
		perror("Error reading input");
		exit(EXIT_FAILURE);
	}

	// Removing trailing newline character, if any
	size_t len = strlen(path);
	if (len > 0 && path[len - 1] == '\n') {
		path[len - 1] = '\0';
	}

	return path;
}


int main()
{

	char* path = get_path();
	char* copied_path = (char*)malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(copied_path, path);
	//char* path = NULL;
	//char* copied_path = NULL;
	int op;
	char buffer[100];

	
	//Citim CSV-ul original cu train si impartim 50/50 in alte 2 liste: new_train si new_test
	int size_train;
	Train_Element* train_list = read_train(copied_path, &size_train);

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

	//array cu etichetele generate random pentru test 
	int* etichete_generate = generare_etichete(new_test_size);

	do
	{
		//system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Basic image opening...\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - Afisare liste train si test\n");
		printf(" 5 - Afisare etichete generate random (pt test)\n");
		printf(" 6 - Afisare acuratete\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		//scanf("%d", &op);
		fgets(buffer, sizeof(buffer), stdin);
		sscanf(buffer, "%d", &op);
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
			show_split_train(new_train_list, new_test_list, new_train_size, new_test_size);
			break;
		case 5:
			afisare_etichete(new_test_list, new_test_size);
			break;
		case 6:
			afisare_acuratete(new_test_list, etichete_generate, new_test_size);
			break;
		}
	} while (op != 0);

	free(path);
	return 0;
}
