#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>  
#include <memory.h>

#define max(x, y) ((x>y) ? (x):(y))
#define min(x, y) ((x<y) ? (x):(y))

int xdim;
int ydim;
int maxraw;
unsigned char* image;

void ReadPGM(FILE*);
void WritePGM(FILE*);
void GaussianFilter(unsigned char* inputImage, unsigned char* outputImage, int xdim, int ydim);
void BilateralFilter(unsigned char* inputImage, unsigned char* outputImage, int xdim, int ydim);

int main() {
    int i, j;
    FILE* fp;

    /* begin reading PGM.... */
    printf("begin reading PGM.... \n");
    if ((fp = fopen("C:\\Users\\siddh\\Downloads\\proj_2\\test-img.pgm", "rb")) == NULL) {
        printf("read error...\n");
        exit(0);
    }
    ReadPGM(fp);

    // Perform Gaussian filtering
    unsigned char* gaussianFiltered = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
    GaussianFilter(image, gaussianFiltered, xdim, ydim);

    /* Begin writing Gaussian filtered PGM.... */
    printf("Begin writing Gaussian filtered PGM.... \n");
    if ((fp = fopen("C:\\Users\\siddh\\Downloads\\proj_2\\gaussian_filtered.pgm", "wb")) == NULL) {
        printf("write pgm error....\n");
        exit(0);
    }
    WritePGM(fp, gaussianFiltered);

    // Perform bilateral filtering
    unsigned char* bilateralFiltered = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
    BilateralFilter(image, bilateralFiltered, xdim, ydim);

    /* Begin writing Bilateral filtered PGM.... */
    printf("Begin writing Bilateral filtered PGM.... \n");
    if ((fp = fopen("C:\\Users\\siddh\\Downloads\\proj_2\\bilateral_filtered.pgm", "wb")) == NULL) {
        printf("write pgm error....\n");
        exit(0);
    }
    WritePGM(fp, bilateralFiltered);

    free(image);
    free(gaussianFiltered);
    free(bilateralFiltered);

    return (1);
}

void ReadPGM(FILE* fp) {
    int c;
    int i, j;
    int val;
    unsigned char* line;
    char buf[1024];


    while ((c = fgetc(fp)) == '#')
        fgets(buf, 1024, fp);
    ungetc(c, fp);
    if (fscanf(fp, "P%d\n", &c) != 1) {
        printf("read error ....");
        exit(0);
    }
    if (c != 5 && c != 2) {
        printf("read error ....");
        exit(0);
    }

    if (c == 5) {
        while ((c = fgetc(fp)) == '#')
            fgets(buf, 1024, fp);
        ungetc(c, fp);
        if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
            printf("failed to read width/height/max\n");
            exit(0);
        }
        printf("Width=%d, Height=%d \nMaximum=%d\n", xdim, ydim, maxraw);

        image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
        getc(fp);

        line = (unsigned char*)malloc(sizeof(unsigned char) * xdim);
        for (j = 0; j < ydim; j++) {
            fread(line, 1, xdim, fp);
            for (i = 0; i < xdim; i++) {
                image[j * xdim + i] = line[i];
            }
        }
        free(line);

    }

    else if (c == 2) {
        while ((c = fgetc(fp)) == '#')
            fgets(buf, 1024, fp);
        ungetc(c, fp);
        if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
            printf("failed to read width/height/max\n");
            exit(0);
        }
        printf("Width=%d, Height=%d \nMaximum=%d,\n", xdim, ydim, maxraw);

        image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
        getc(fp);

        for (j = 0; j < ydim; j++)
            for (i = 0; i < xdim; i++) {
                fscanf(fp, "%d", &val);
                image[j * xdim + i] = val;
            }

    }

    fclose(fp);
}

void WritePGM(FILE* fp, unsigned char* image) {
    int i, j;


    fprintf(fp, "P5\n%d %d\n%d\n", xdim, ydim, 255);
    for (j = 0; j < ydim; j++)
        for (i = 0; i < xdim; i++) {
            fputc(image[j * xdim + i], fp);
        }

    fclose(fp);
}

void GaussianFilter(unsigned char* inputImage, unsigned char* outputImage, int xdim, int ydim) {
    // Implement Gaussian filter
    // Define Gaussian kernel
    double kernel[5][5] = {
        {1,  4,  7,  4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1,  4,  7,  4, 1}
    };
    double sumKernel = 273.0; // Sum of all kernel elements

    // Apply Gaussian filter to each pixel
    for (int y = 2; y < ydim - 2; y++) {
        for (int x = 2; x < xdim - 2; x++) {
            double sum = 0.0;
            for (int j = -2; j <= 2; j++) {
                for (int i = -2; i <= 2; i++) {
                    sum += inputImage[(y + j) * xdim + (x + i)] * kernel[j + 2][i + 2];
                }
            }
            outputImage[y * xdim + x] = (unsigned char)(sum / sumKernel);
        }
    }
}

void BilateralFilter(unsigned char* inputImage, unsigned char* outputImage, int xdim, int ydim) {
    // Implement Bilateral filter
    // Define Bilateral parameters
    double sigmaS = 1.0; // Spatial standard deviation
    double sigmaI = 50.0; // Intensity standard deviation

    // Apply Bilateral filter to each pixel
    for (int y = 0; y < ydim; y++) {
        for (int x = 0; x < xdim; x++) {
            double sumIntensity = 0.0;
            double sumWeight = 0.0;
            double centerPixel = inputImage[y * xdim + x];

            // Iterate over the neighborhood of the current pixel
            for (int j = -2; j <= 2; j++) {
                for (int i = -2; i <= 2; i++) {
                    int newX = x + i;
                    int newY = y + j;
                    if (newX >= 0 && newX < xdim && newY >= 0 && newY < ydim) {
                        double neighborPixel = inputImage[newY * xdim + newX];
                        double spatialDist = sqrt(i * i + j * j);
                        double intensityDiff = abs(neighborPixel - centerPixel);
                        double spatialWeight = exp(-0.5 * (spatialDist * spatialDist) / (sigmaS * sigmaS));
                        double intensityWeight = exp(-0.5 * (intensityDiff * intensityDiff) / (sigmaI * sigmaI));
                        double weight = spatialWeight * intensityWeight;
                        sumWeight += weight;
                        sumIntensity += weight * neighborPixel;
                    }
                }
            }

            // Normalize and assign the filtered value to the output image
            outputImage[y * xdim + x] = (unsigned char)(sumIntensity / sumWeight);
        }
    }
}
