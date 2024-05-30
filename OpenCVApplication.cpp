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

typedef struct {
	float red;
	float green;
	float blue;
}color;

typedef struct {
	color* buildings;
	color* forests;
	color* mountains;
	color* glacier;
	color* street;
	color* sea;
}average_class;

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

char* get_path() {
	printf("Enter the path: \n ");

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
	fgets(line, sizeof(line), file);
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

void showHistogram_poza(float** procente,char* path) {
	int histSize = 256;
	Mat img = imread(path);
	if (img.empty()) {
		printf("Nu am putut deschide imaginea\n");
		(*procente)[0] = 0.0f;
		(*procente)[1] = 0.0f;
		(*procente)[2] = 0.0f;
		return;
	}

	int countRed = 0; 
	int countGreen = 0;
	int countBlue = 0;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			Vec3b pixel = img.at<Vec3b>(i, j);
			if (pixel[1] > pixel[0] && pixel[1] > pixel[2] && pixel[1] > 100) {
				countGreen++;
			}
			if (pixel[2] > pixel[0] && pixel[2] > pixel[1] && pixel[2] > 100) {
				countRed++;
			}
			if (pixel[0] > pixel[1] && pixel[0] > pixel[2] && pixel[0] > 100) {
				countBlue++;
			}
		}
	}

	float total_pixels = img.rows * img.cols;

	(*procente)[0] = (countBlue / total_pixels) * 100;
	(*procente)[1] = (countGreen / total_pixels) * 100;
	(*procente)[2] = (countRed / total_pixels) * 100;

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

int* generare_etichete_smart(Train_Element* original,int size_list, char* copied_path) {
	srand(time(NULL));

	//facem un nou vector pentru noile etichete generate random
	int* etichete_generate = NULL;
	int size_etichete_generate = size_list;
	etichete_generate = (int*)malloc(size_etichete_generate * sizeof(int));
	int lungime_path = strlen(copied_path);
	float* procentaje = NULL;
	procentaje = (float*)calloc(3, sizeof(float));

	for (int i = 0; i < size_etichete_generate; i++) {
		//generare eticheta random 1-6
		char* poza = original[i].nume_poza;
		int lungime_poza = strlen(poza) + lungime_path + 1;
		char* path_poza = new char[lungime_poza];
		strcpy(path_poza, copied_path);
		strcat(path_poza, poza);
		printf("%s\n", path_poza);
		showHistogram_poza(&procentaje, path_poza);
		int random_number = 1 + rand() % (2 - 1 + 1);
		int culoare = -1;
		float maxim = -1.0f;
		for (int j = 0; j < 3; j++) {
			if (procentaje[j] > maxim) {
				culoare = j;
				maxim = procentaje[j];
			}
		}
		if (culoare == 0) {
			if (random_number == 1) {
				etichete_generate[i] = 4;
			}
			else {
				etichete_generate[i] = 6;
			}
		}
		else {
			if (culoare == 1) {
				if (random_number == 1) {
					etichete_generate[i] = 2;
				}
				else {
					etichete_generate[i] = 3;
				}
			}
			else {
				if (random_number == 1) {
					etichete_generate[i] = 1;
				}
				else {
					etichete_generate[i] = 5;
				}
			}
		}
		printf("Albastru:%.2f%%,Verde:%.2f%%,Rosu:%.2f%% --Eticheta:%d\n", procentaje[0], procentaje[1], procentaje[2], etichete_generate[i]);
	}
	free(procentaje);
	return etichete_generate;
}

int clasificaScena(float* procentaje) {
	int eticheta = 0;
	const float prag_albastru = 30.0;
	const float prag_verde = 50.0;
	const float prag_rosu = 20.0;

	float suma = procentaje[0] + procentaje[1] + procentaje[2];

	float procent_albastru = procentaje[0] / suma * 100;
	float procent_verde = procentaje[1] / suma * 100;
	float procent_rosu = procentaje[2] / suma * 100;

	if (procent_albastru > prag_albastru) {
		eticheta = (procent_verde > 20) ? 4 : 6;
	}
	else if (procent_verde > prag_verde) {
		eticheta = 2;
	}
	else if (procent_rosu > prag_rosu) {
		eticheta = (procent_verde > 15) ? 1 : 5;
	}
	else {
		eticheta = 1;
	}
	return eticheta;
}

float calculateDistance(color a, color b) {
	return sqrt(pow(a.red - b.red, 2) + pow(a.green - b.green, 2) + pow(a.blue - b.blue, 2));
}

// Function to classify an image based on its RGB percentages
int clasificaScena2(float* procente, average_class* class_average) {
	color image_avg = { procente[2], procente[1], procente[0] }; // Assuming procente contains RGB in order B, G, R

	float minDistance = calculateDistance(image_avg, *(class_average->buildings));
	int eticheta = 1;

	float distance = calculateDistance(image_avg, *(class_average->forests));
	if (distance < minDistance) {
		minDistance = distance;
		eticheta = 2;
	}

	distance = calculateDistance(image_avg, *(class_average->glacier));
	if (distance < minDistance) {
		minDistance = distance;
		eticheta = 3;
	}

	distance = calculateDistance(image_avg, *(class_average->mountains));
	if (distance < minDistance) {
		minDistance = distance;
		eticheta = 4;
	}

	distance = calculateDistance(image_avg, *(class_average->street));
	if (distance < minDistance) {
		minDistance = distance;
		eticheta = 5;
	}

	distance = calculateDistance(image_avg, *(class_average->sea));
	if (distance < minDistance) {
		minDistance = distance;
		eticheta = 6;
	}

	return eticheta;
}



int* generare_etichete_smart2(Train_Element* original, int size_list,average_class *class_average, char* copied_path) {
	srand(time(NULL));

	//facem un nou vector pentru noile etichete generate random
	int* etichete_generate = NULL;
	int size_etichete_generate = size_list;
	etichete_generate = (int*)malloc(size_etichete_generate * sizeof(int));
	int lungime_path = strlen(copied_path);
	float* procentaje = NULL;
	procentaje = (float*)calloc(3, sizeof(float));

	for (int i = 0; i < size_etichete_generate; i++) {
		//generare eticheta random 1-6
		char* poza = original[i].nume_poza;
		int lungime_poza = strlen(poza) + lungime_path + 1;
		char* path_poza = new char[lungime_poza];
		strcpy(path_poza, copied_path);
		strcat(path_poza, poza);
		printf("%s\n", path_poza);
		showHistogram_poza(&procentaje, path_poza);
		int random_number = 1 + rand() % (2 - 1 + 1);
		int culoare = -1;
		float maxim = -1.0f;
		for (int j = 0; j < 3; j++) {
			if (procentaje[j] > maxim) {
				culoare = j;
				maxim = procentaje[j];
			}
		}
		etichete_generate[i] = clasificaScena2(procentaje,class_average);
	}
	free(procentaje);
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

void getAverages(average_class* class_average, Train_Element* train_list, int train_size, char* copied_path) {
	//const char* copied_path = "C:\\Users\\Razvan\\Documents\\utcn\\UTCN\\An 3\\Sem 2\\PI\\kaggle dataset\\train-scene classification\\train\\";
	//const char* copied_path = get_path();
	int lungime_path = strlen(copied_path);
	float* procentaje = (float*)calloc(3, sizeof(float));

	int buildings = 0;
	int forests = 0;
	int mountains = 0;
	int glacier = 0;
	int street = 0;
	int sea = 0;

	for (int i = 0; i < train_size; i++) {
		char* poza = train_list[i].nume_poza;
		int lungime_poza = strlen(poza) + lungime_path + 1;
		char* path_poza = new char[lungime_poza];
		strcpy(path_poza, copied_path);
		strcat(path_poza, poza);

		showHistogram_poza(&procentaje, path_poza);
		switch (train_list[i].eticheta) {
		case 1:
			if (buildings > 0) {
				class_average->buildings->red = (class_average->buildings->red * buildings + procentaje[2]) / (buildings + 1);
				class_average->buildings->green = (class_average->buildings->green * buildings + procentaje[1]) / (buildings + 1);
				class_average->buildings->blue = (class_average->buildings->blue * buildings + procentaje[0]) / (buildings + 1);
			}
			else {
				class_average->buildings->red = procentaje[2];
				class_average->buildings->green = procentaje[1];
				class_average->buildings->blue = procentaje[0];
			}
			buildings++;
			break;
		case 2:
			if (forests > 0) {
				class_average->forests->red = (class_average->forests->red * forests + procentaje[2]) / (forests + 1);
				class_average->forests->green = (class_average->forests->green * forests + procentaje[1]) / (forests + 1);
				class_average->forests->blue = (class_average->forests->blue * forests + procentaje[0]) / (forests + 1);
			}
			else {
				class_average->forests->red = procentaje[2];
				class_average->forests->green = procentaje[1];
				class_average->forests->blue = procentaje[0];
			}
			forests++;
			break;
		case 3:
			if (mountains > 0) {
				class_average->mountains->red = (class_average->mountains->red * mountains + procentaje[2]) / (mountains + 1);
				class_average->mountains->green = (class_average->mountains->green * mountains + procentaje[1]) / (mountains + 1);
				class_average->mountains->blue = (class_average->mountains->blue * mountains + procentaje[0]) / (mountains + 1);
			}
			else {
				class_average->mountains->red = procentaje[2];
				class_average->mountains->green = procentaje[1];
				class_average->mountains->blue = procentaje[0];
			}
			mountains++;
			break;
		case 4:
			if (glacier > 0) {
				class_average->glacier->red = (class_average->glacier->red * glacier + procentaje[2]) / (glacier + 1);
				class_average->glacier->green = (class_average->glacier->green * glacier + procentaje[1]) / (glacier + 1);
				class_average->glacier->blue = (class_average->glacier->blue * glacier + procentaje[0]) / (glacier + 1);
			}
			else {
				class_average->glacier->red = procentaje[2];
				class_average->glacier->green = procentaje[1];
				class_average->glacier->blue = procentaje[0];
			}
			glacier++;
			break;
		case 5:
			if (street > 0) {
				class_average->street->red = (class_average->street->red * street + procentaje[2]) / (street + 1);
				class_average->street->green = (class_average->street->green * street + procentaje[1]) / (street + 1);
				class_average->street->blue = (class_average->street->blue * street + procentaje[0]) / (street + 1);
			}
			else {
				class_average->street->red = procentaje[2];
				class_average->street->green = procentaje[1];
				class_average->street->blue = procentaje[0];
			}
			street++;
			break;
		case 6:
			if (sea > 0) {
				class_average->sea->red = (class_average->sea->red * sea + procentaje[2]) / (sea + 1);
				class_average->sea->green = (class_average->sea->green * sea + procentaje[1]) / (sea + 1);
				class_average->sea->blue = (class_average->sea->blue * sea + procentaje[0]) / (sea + 1);
			}
			else {
				class_average->sea->red = procentaje[2];
				class_average->sea->green = procentaje[1];
				class_average->sea->blue = procentaje[0];
			}
			sea++;
			break;
		default:
			break;
		}
		delete[] path_poza;
	}
	free(procentaje);
}


void normalizeAndPrintConfusionMatrix(float matrix[6][6]) {
	const int numClasses = 6; // Number of classes
	int labelWidth = 2; // Width for class labels
	int dataWidth = 10;   // Width for data columns

	// Calculate the sum of elements for each row
	/*float rowSum[6] = { 0 };
	for (int i = 0; i < numClasses; i++) {
		for (int j = 0; j < numClasses; j++) {
			rowSum[i] += matrix[i][j];
		}
	}*/

	// Normalize each element by the row sum
	//for (int i = 0; i < numClasses; i++) {
	//	for (int j = 0; j < numClasses; j++) {
	//		if (rowSum[i] != 0) // Prevent division by zero
	//			matrix[i][j] /= rowSum[i];
	//	}
	//}

	// Print header with proper alignment
	std::cout << std::right << std::setw(labelWidth) << " "; // Space for the row labels
	for (int j = 0; j < numClasses; j++) {
		std::cout << std::right << std::setw(dataWidth) << "C" << (j + 1);
	}
	std::cout << "\n";

	// Print matrix rows with labels and data
	for (int i = 0; i < numClasses; i++) {
		std::cout << std::left << std::setw(labelWidth) << "Class " + std::to_string(i + 1);
		for (int j = 0; j < numClasses; j++) {
			std::cout << std::right << std::setw(dataWidth) << std::fixed << std::setprecision(2) << matrix[i][j];
		}
		std::cout << "\n";
	}
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


void afisareClase(average_class class_average) {
	printf("Buildings:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.buildings->red, class_average.buildings->green, class_average.buildings->blue);
	printf("Forests:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.forests->red, class_average.forests->green, class_average.forests->blue);
	printf("Glacier:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.glacier->red, class_average.glacier->green, class_average.glacier->blue);
	printf("Mountains:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.mountains->red, class_average.mountains->green, class_average.mountains->blue);
	printf("Street:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.street->red, class_average.street->green, class_average.street->blue);
	printf("Sea:Rosu:%.2f,Green:%.2f,Albastru:%.2f\n", class_average.sea->red, class_average.sea->green, class_average.sea->blue);
}

int main()
{
	printf("Enter the path to the original train.csv file: \n");
	char* path = get_path();
	printf("Enter the path to pictures directory: \n");
	char* path_to_pictures_dir = get_path();
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
	average_class class_average;
	class_average.buildings = (color*)calloc(1, sizeof(color));
	class_average.forests = (color*)calloc(1, sizeof(color));
	class_average.glacier = (color*)calloc(1, sizeof(color));
	class_average.mountains = (color*)calloc(1, sizeof(color));
	class_average.sea = (color*)calloc(1, sizeof(color));
	class_average.street = (color*)calloc(1, sizeof(color));
	getAverages(&class_average, new_train_list, new_train_size, path_to_pictures_dir);
	int* etichete_generate = generare_etichete_smart2(new_test_list,new_test_size,&class_average, path_to_pictures_dir);
	//int* etichete_generate = generare_etichete_smart(new_test_list,new_test_size, path_to_pictures_dir);

	int counter = 0;
	float confusionMatrix[6][6] = { 0 };
	for (int i = 0; i < new_test_size; i++) {
		int actual = new_test_list[i].eticheta - 1;  // Assuming labels are 1-based and need to be 0-based for indexing
		int predicted = etichete_generate[i] - 1;
		//printf("Image %s - has predicted %d but actual %d\n", new_test_list[i].nume_poza, etichete_generate[i], new_test_list[i].eticheta);
		confusionMatrix[actual][predicted]++;
		counter++;
	}

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
		printf(" 7 - Afisare matrice de confuzie\n");
		printf(" 8 - Afisare procente clase\n");
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
		case 7:
			normalizeAndPrintConfusionMatrix(confusionMatrix);
			break;
		case 8:
			afisareClase(class_average);
			break;
		}
	} while (op != 0);

	free(path);
	return 0;
}
