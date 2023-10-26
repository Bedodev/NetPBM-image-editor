// Copyright Andrei-Dragos Badoiu 315CAa 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "commands.h"

int command_id(char *command)
{
	// not enough time to implement a decent command hander but this will do
	if(strcmp(command, "LOAD") == 0) return 1;
	else if(strcmp(command, "SELECT") == 0) return 2;
	else if(strcmp(command, "HISTOGRAM") == 0) return 3;
	else if(strcmp(command, "EQUALIZE") == 0) return 4;
	else if(strcmp(command, "ROTATE") == 0) return 5;
	else if(strcmp(command, "CROP") == 0) return 6;
	else if(strcmp(command, "SAVE") == 0) return 7;
	else if(strcmp(command, "APPLY") == 0) return 8;
	else if(strcmp(command, "EXIT") == 0) return 9;

	else if(strcmp(command, "test") == 0) return 77; // printing image FOR TESTING PURPOSES
	else if(strcmp(command, "sel") == 0) return 88; // printing selection FOR TESTING PURPOSES

	else return 0;
}



int is_image_loaded(netpbm **image)
{
	if(image == NULL){
		printf("No image loaded\n");
		return 0;
	}
	else return 1;
}



int check_image_type(char *filename) 
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		return 0;
	}
	else {
		int bool = 0;
		char *c = malloc(15);
		while(bool == 0){
			fgets(c, 15, file);
			if(c[0] == '#') continue;
			else bool = c[1]-'0';
		}
		fclose(file);
		free(c);
		if (bool == '2' || bool == '3') return 1;
		else return 2;
	}
}



netpbm **load_image(char *filename, netpbm **image, int *image_type, int *width, int *height, int *maxval)
{
	FILE *file = NULL;
	if(check_image_type(filename) == 1) file = fopen(filename, "r");
	else file = fopen(filename, "rb");

	if (file == NULL)
	{
		printf("Failed to load %s\n", filename);
		for (int i = 0; i < *height; i++)
		{
			free(image[i]);
		}
		free(image);
		*image_type = 0;
		*width = 0;
		*height = 0;
		*maxval = 0;
		image = NULL;

		return image;
	} else printf("Loaded %s\n", filename);
	
	if(image != NULL){
		for (int i = 0; i < *height; i++)
		{
			free(image[i]);
		}
		free(image);
		image = NULL;
	}
	// saving image data
	char *c = malloc(15);
	int cnt = 0;
	while(cnt<3){
		fgets(c, 15, file);
		if (c[0] == '#') continue;
		else if (cnt == 0) {
			*image_type = c[1]-'0';
		} else if(cnt == 1){
			// atoi with strtok could have worked better for this 
			// width
			*width=0;

			int i=0;
			while(i<(int)strlen(c) && c[i] != ' ' && ( c[i]>='0' && c[i]<='9' ) ){
			*width = *width*10 + (c[i]-'0');
			i++;
			}

			// height
			*height=0;

			i++;
			while(i<(int)strlen(c) && c[i] != ' ' && c[i] != '\0' && ( c[i]>='0' && c[i]<='9' ) ){
			*height = *height*10 + (c[i]-'0');
			i++;
			}
		} else if(cnt == 2){
			// maxval
			*maxval=0;

			int i=0;
			while(i<(int)strlen(c) && c[i] != ' ' && c[i] != '\0' && ( c[i]>='0' && c[i]<='9' ) ){
			*maxval = *maxval*10 + (c[i]-'0');
			i++;
			}
		}
		cnt++;
	}
	// allocating memory for image
	image = (netpbm **)calloc(*height, sizeof(netpbm *));
	for(int i=0; i<*height; i++){
		image[i] = (netpbm *)calloc(*width, sizeof(netpbm));
	}
	if(*image_type <=3)
		for(int i=0; i<*height; i++)
			for(int j=0; j<*width; j++){
				if(*image_type==2) fscanf(file, "%d", &((*(image+i)+j)->r));
				else if(*image_type==3) fscanf(file, "%d %d %d", &((*(image+i)+j)->r), &((*(image+i)+j)->g), &((*(image+i)+j)->b));
			}
	else {
		for(int i=0; i<*height; i++)
			for(int j=0; j<*width; j++){
				unsigned char c;
				if(*image_type==5) {
					fread(&c, 1, 1, file);
					((*(image+i)+j)->r) = (int)c;
				}
				else if(*image_type==6) {
					fread(&c, 1, 1, file);
					((*(image+i)+j)->r) = (int)c;
					fread(&c, 1, 1, file);
					((*(image+i)+j)->g) = (int)c;
					fread(&c, 1, 1, file);
					((*(image+i)+j)->b) = (int)c;
				}
			}
	}
	free(c);
	fclose(file);
	return image;
}



void select_area(netpbm **image, selection *sel, int width, int height)
{
	char all[11];
	int x1, y1, x2, y2;
	// check if given string select or values
	scanf("%s", all);
	
	if(strcmp(all, "ALL") == 0){
		if(is_image_loaded(image)==0) return;
		change_selection(sel, 0, 0, width, height);
		// Output
		printf("Selected %s\n", all);
	} else {
		x1 = atoi(all);

		char line[100];
		fgets(line, 100, stdin);
		line[strlen(line)-1] = '\0';
		char *a, *b, *c;
		// if possible, read with strtok
		a = strtok(line, " ");
		if(a==NULL){
			printf("Invalid command\n");
			return;
		} else {
			b = strtok(NULL, " ");
			if(b==NULL){
				printf("Invalid command\n");
				return;
			} else {
				c = strtok(NULL, " ");
				if(c==NULL){
					printf("Invalid command\n");
					return;
				}
			}
		}
		

		int bool_verif=0;
		for(int i=0; i<(int)strlen(a); i++) if((a[i]<'0' || a[i]>'9') && a[i]!='-') bool_verif=1;
		for(int i=0; i<(int)strlen(b); i++) if((b[i]<'0' || b[i]>'9') && b[i]!='-') bool_verif=1;
		for(int i=0; i<(int)strlen(c); i++) if((c[i]<'0' || c[i]>'9') && c[i]!='-') bool_verif=1;
		if(bool_verif==1){
			printf("Invalid command\n");
			return;
		} else {
			y1 = atoi(a);
			x2 = atoi(b);
			y2 = atoi(c);
		}

		if(is_image_loaded(image)==0) return;
		// check if values are in range
		if(1){
			selection tmp = {x1, y1, x2, y2};
			// switch tmp values if needed
			if(tmp.x1>tmp.x2){
				int tmp2 = tmp.x1;
				tmp.x1 = tmp.x2;
				tmp.x2 = tmp2;
			}
			if(tmp.y1>tmp.y2){
				int tmp2 = tmp.y1;
				tmp.y1 = tmp.y2;
				tmp.y2 = tmp2;
			}
			if(tmp.x1<0 || tmp.x2<0 || tmp.y1<0 || tmp.y2<0 || tmp.x1>=width || tmp.x2>width || tmp.y1>=height || tmp.y2>height || tmp.x1==tmp.x2 || tmp.y1==tmp.y2 || tmp.x2-tmp.x1>width || tmp.y2-tmp.y1>height){
				printf("Invalid set of coordinates\n");
				return;
			}
		}	

		// switch values if needed
		if(x1>x2){
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		if(y1>y2){
			int tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		change_selection(sel, x1, y1, x2, y2);

		// Output
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	}
}



void change_selection(selection *sel, int x1, int y1, int x2, int y2)
{
	sel->x1 = x1;
	sel->y1 = y1;
	sel->x2 = x2;
	sel->y2 = y2;
}



void gen_hist_vector(netpbm **image, selection sel, int maxval, int *hist, int *maxfreq)
{
	for(int i=0; i<=maxval; i++) hist[i] = 0;
	for(int i=sel.y1 ; i<sel.y2 ; i++){
		for(int j=sel.x1 ; j<sel.x2 ; j++){
			hist[image[i][j].r]++;
			if(hist[image[i][j].r] > *maxfreq) *maxfreq = hist[image[i][j].r];
		}
	}
}



void histogram(netpbm **image, selection sel, int image_type, int maxval, int width, int height)
{
	// get given values
	int x=-1,y=-1;
	scanf("%d %d", &x, &y);
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;
	// check if we have values
	if(x==-1 || y==-1){
		printf("Invalid command\n");
		return;
	}
	// check if image is black and white
	if(image_type == 3 || image_type == 6){
		printf("Black and white image needed\n");
		return;
	}

	selection tmp;

	// // uncomment this to hitogram only on selection
	tmp = sel;

	tmp.x1 = 0;
	tmp.y1 = 0;
	tmp.x2 = width;
	tmp.y2 = height;

	// find pixel frequency and max frequency
	int maxfreq = 0;
	int *hist = (int*)malloc((maxval+1)*sizeof(int));
	gen_hist_vector(image, tmp, maxval, hist, &maxfreq);

	int buffer = (maxval+1)/y;
	// print histogram
	for(int i=0; i<=maxval; i+=buffer){
		double aux = 0;
		for(int j=i; j<i+buffer; j++) aux += hist[j];
		aux = (aux*x)/(maxfreq*buffer);
		printf("%d\t|\t", (int)round(aux));
		for(int j=0; j<(int)round(aux); j++){
			printf("*");
		}
		printf("\n");
	}
	free(hist);
}



void equalize(netpbm **image, selection sel, int image_type, int maxval, int width, int height)
{
	// check if image is black and white
	if(image_type == 3 || image_type == 6){
		printf("Black and white image needed\n");
		return;
	}
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;

	selection tmp;
	// // uncomment this if you want to equalize only the selection
	tmp = sel;

	tmp.x1 = 0;
	tmp.y1 = 0;
	tmp.x2 = width;
	tmp.y2 = height;

	int maxfreq = 0;
	int *hist = (int*)malloc((maxval+1)*sizeof(int));
	gen_hist_vector(image, tmp, maxval, hist, &maxfreq);
	
	int area = (tmp.x2-tmp.x1)*(tmp.y2-tmp.y1);

	for(int i=tmp.y1; i<tmp.y2; i++){
		for(int j=tmp.x1; j<tmp.x2; j++){
			int sum = 0;
			for(int k=0; k<=image[i][j].r; k++){
				sum += hist[k];
			}
			double aux = (sum*255)/area;
			aux = round(aux);
			if(aux<0) aux = 0;
			else if(aux>255) aux = 255;
			image[i][j].r = (int)aux;
		}
	}
	free(hist);
	printf("Equalize done\n");
}



netpbm **rotate(netpbm **image, selection *sel, int *width, int *height)
{
	int angle;
	scanf("%d", &angle);
	// check if image is loaded
	if(is_image_loaded(image)==0) return image;
	int is_negative = 0;
	if(angle < 0){
		is_negative = 1;
		angle = -angle;
	}
	if(angle!=90 && angle!=180 && angle!=270 && angle!=360 && angle!=0){
		printf("Unsupported rotation angle\n");
		return image;
	}
	int is_whole = 0;
	if(sel->x1==0 && sel->y1==0 && sel->x2==*width && sel->y2==*height) is_whole = 1;
	if(is_whole == 0){
		if(sel->x2-sel->x1 != sel->y2-sel->y1){
			printf("The selection must be square\n");
			return image;
		}
	}
	if(is_negative) printf("Rotated -%d\n", angle);
	else printf("Rotated %d\n", angle);
	if(angle == 90) return rotation_once(image, sel, width, height, is_negative, is_whole);
	else if(angle == 180){
		image = rotation_once(image, sel, width, height, is_negative, is_whole);
		return rotation_once(image, sel, width, height, is_negative, is_whole);
	}
	else if(angle == 270){
		if(is_negative == 0) is_negative = 1;
		else is_negative = 0;
		return rotation_once(image, sel, width, height, is_negative, is_whole);
	}
	else if(angle == 360) return image;
	else if(angle == 0) return image;
	return image;
}



netpbm **rotation_once(netpbm **image, selection *sel, int *width, int *height, int is_negative, int is_whole){
	if(is_whole == 1){
		netpbm **rotated = (netpbm**)calloc(*width, sizeof(netpbm*));
		for(int i=0; i<*width; i++){
			rotated[i] = (netpbm*)calloc(*height, sizeof(netpbm));
			if(is_negative == 0) for(int j=0; j<*height; j++) rotated[i][j] = image[*height-j-1][i];
			else for(int j=0; j<*height; j++) rotated[i][j] = image[j][*width-i-1];
		}
		// free
		for(int i=0; i<*height; i++){
			free(image[i]);
		}
		free(image);
		// swap width and height
		int aux = *width;
		*width = *height;
		*height = aux;
		// set selection
		sel->x1 = 0;
		sel->y1 = 0;
		sel->x2 = *width;
		sel->y2 = *height;
		return rotated;
	}
	else {
		int size = sel->x2-sel->x1;
		netpbm **rotated = (netpbm**)calloc(size, sizeof(netpbm*));
		for(int i=0; i<size; i++){
			rotated[i] = (netpbm*)calloc(size, sizeof(netpbm));
			if(is_negative == 0) for(int j=0; j<size; j++) rotated[i][j] = image[sel->y2-j-1][sel->x1+i];
			else for(int j=0; j<size; j++) rotated[i][j] = image[sel->y1+j][sel->x2-i-1];
		}
		// copy pixels
		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++){
				image[sel->y1+i][sel->x1+j] = rotated[i][j];
			}
		}
		// free
		for(int i=0; i<size; i++){
			free(rotated[i]);
		}
		free(rotated);
		return image;
	}
}



netpbm **crop(netpbm **image, selection *sel, int *width, int *height, int image_type)
{
	// check if image is loaded 
	if(is_image_loaded(image)==0) return NULL;
	// create cropped image
	int crop_width = sel->x2-sel->x1;
	int crop_height = sel->y2-sel->y1;

	netpbm **crop_matrix = (netpbm**)calloc(crop_height, sizeof(netpbm*));
	for(int i = 0 ; i < *height ; i++){
		if(i < sel->y1 || i >= sel->y2) free(image[i]);
		else{
			crop_matrix[i-sel->y1] = (netpbm*)calloc(crop_width, sizeof(netpbm));
			for(int j=0; j<crop_width; j++){
				crop_matrix[i-sel->y1][j].r = image[i][sel->x1+j].r;
				if(image_type == 3 || image_type == 6){
					crop_matrix[i-sel->y1][j].g = image[i][sel->x1+j].g;
					crop_matrix[i-sel->y1][j].b = image[i][sel->x1+j].b;
				}
			}
			free(image[i]);
		}
	}
	free(image);
	// alloc for new image and copy pixels
	image = (netpbm**)calloc(crop_height, sizeof(netpbm*));
	for(int i=0; i<crop_height; i++){
		image[i] = (netpbm*)calloc(crop_width, sizeof(netpbm));
		for(int j=0; j<crop_width; j++){
			image[i][j].r = crop_matrix[i][j].r;
			if(image_type == 3 || image_type == 6){
				image[i][j].g = crop_matrix[i][j].g;
				image[i][j].b = crop_matrix[i][j].b;
			}
		}
		free(crop_matrix[i]);
	}
	free(crop_matrix);
	change_selection(sel, 0, 0, crop_width, crop_height);
	*width = crop_width;
	*height = crop_height;
	printf("Image cropped\n");
	return image;
}



void save_image(netpbm **image, int image_type, int maxval, int width, int height)
{
	char *filename = (char*)malloc(25*sizeof(char));
	scanf("%s", filename);
	int ascii_mode = 0;
	char ascii[8];
	fgets(ascii, 8, stdin);
	ascii[6] = '\0';
	for(int i=0; i<8; i++) ascii[i] = ascii[i+1];
	if(strcmp(ascii, "ascii") == 0) ascii_mode = 1;

	// check if image is loaded 
	if(is_image_loaded(image)==0) {
		free(filename);
		return;
	}
	if(ascii_mode == 1){
		// open file
		FILE *file = fopen(filename, "w");
		// write header
		if(image_type > 3) image_type = image_type - 3;
		fprintf(file,"P%d\n%d %d\n%d\n", image_type, width, height, maxval);
		// write pixels
		for(int i=0 ; i < height ; i++){
			for(int j=0 ; j < width ; j++){
				fprintf(file, "%d", image[i][j].r);
				if(image_type == 3){
					fprintf(file, " %d %d", image[i][j].g, image[i][j].b);
				}
				if(j!=width-1) fprintf(file, " ");
			}
			if(i!=height-1) fprintf(file, "\n");
		}
		fclose(file);
	} else {
		// open file
		FILE *file = fopen(filename, "wb");
		// write header
		if(image_type < 4) image_type = image_type + 3;
		fprintf(file,"P%d\n%d %d\n%d\n", image_type, width, height, maxval);
		// write pixels
		for(int i=0 ; i < height ; i++){
			for(int j=0 ; j < width ; j++){
				fwrite(&image[i][j].r, sizeof(unsigned char), 1, file);
				if(image_type == 6){
					fwrite(&image[i][j].g, sizeof(unsigned char), 1, file);
					fwrite(&image[i][j].b, sizeof(unsigned char), 1, file);
				}
			}
		}
		fclose(file);
	}
	printf("Saved %s\n", filename);
	free(filename);
}

void apply(netpbm **image, selection sel, int image_type, int *maxval, int width, int height)
{
	char *type = (char*)malloc(16*sizeof(char));
	if(is_image_loaded(image)==0) {
		fgets(type, 16, stdin);
		free(type);
		return;
	}

	// read type
	fgets(type, 16, stdin);
	type[strlen(type)-1] = '\0';
	for(int i=0; i<(int)strlen(type); i++) type[i] = type[i+1];
	if(strcmp(type, "\0") == 0) {
		invalid_command(0);
		return;
	}

	// check if image is color
	if(image_type == 2 || image_type == 5){
		printf("Easy, Charlie Chaplin\n");
		free(type);
		return;
	}
	if(strcmp(type, "EDGE") == 0) edge_detection(image, sel, maxval, width, height);
	else if(strcmp(type, "SHARPEN") == 0) sharpen(image, sel, maxval, width, height);
	else if(strcmp(type, "BLUR") == 0) blur(image, sel, width, height);
	else if(strcmp(type, "GAUSSIAN_BLUR") == 0) gaussian_blur(image, sel, width, height);
	else printf("APPLY parameter invalid\n");
	free(type);
}

void edge_detection(netpbm **image, selection sel, int *maxval, int width, int height)
{
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;
	// create new image
	int new_height = sel.y2 - sel.y1;
	int new_width = sel.x2 - sel.x1;
	netpbm **new_image = (netpbm**)malloc(new_height*sizeof(netpbm*));
	for(int i=0; i<new_height; i++){
		new_image[i] = (netpbm*)malloc(new_width*sizeof(netpbm));
	}
	// apply filter
	for(int i=sel.y1; i<sel.y2; i++){
		for(int j=sel.x1; j<sel.x2; j++){
			int sum_r = 0 , sum_g = 0, sum_b = 0;
			int vecini = 0;
			if(i-1 >= 0 && j-1 >= 0) { // upper-left
				sum_r -= image[i-1][j-1].r;
				sum_g -= image[i-1][j-1].g;
				sum_b -= image[i-1][j-1].b;
				vecini++;
			}
			if(i-1 >= 0) { // upper
				sum_r -= image[i-1][j].r;
				sum_g -= image[i-1][j].g;
				sum_b -= image[i-1][j].b;
				vecini++;
			}
			if(i-1 >= 0 && j+1 < width) { // upper-right
				sum_r -= image[i-1][j+1].r;
				sum_g -= image[i-1][j+1].g;
				sum_b -= image[i-1][j+1].b;
				vecini++;
			}
			if(j-1 >= 0) { // left
				sum_r -= image[i][j-1].r;
				sum_g -= image[i][j-1].g;
				sum_b -= image[i][j-1].b;
				vecini++;
			}
			if(j+1 < width) { // right
				sum_r -= image[i][j+1].r;
				sum_g -= image[i][j+1].g;
				sum_b -= image[i][j+1].b;
				vecini++;
			}
			if(i+1 < height && j-1 >= 0) { // lower-left
				sum_r -= image[i+1][j-1].r;
				sum_g -= image[i+1][j-1].g;
				sum_b -= image[i+1][j-1].b;
				vecini++;
			}
			if(i+1 < height) { // lower
				sum_r -= image[i+1][j].r;
				sum_g -= image[i+1][j].g;
				sum_b -= image[i+1][j].b;
				vecini++;
			}
			if(i+1 < height && j+1 < width) { // lower-right
				sum_r -= image[i+1][j+1].r;
				sum_g -= image[i+1][j+1].g;
				sum_b -= image[i+1][j+1].b;
				vecini++;
			}
			// middle
			sum_r += 8*image[i][j].r;
			sum_g += 8*image[i][j].g;
			sum_b += 8*image[i][j].b;

			if(sum_r < 0) sum_r = 0;
			else if(sum_r > 255) {
				sum_r = 255;
				*maxval = 255;
			}
			if(sum_g < 0) sum_g = 0;
			else if(sum_g > 255) {
				sum_g = 255;
				*maxval = 255;

			}
			if(sum_b < 0) sum_b = 0;
			else if(sum_b > 255) {
				sum_b = 255;
				*maxval = 255;
			}

			if(vecini == 8){
				new_image[i-sel.y1][j-sel.x1].r = sum_r;
				new_image[i-sel.y1][j-sel.x1].g = sum_g;
				new_image[i-sel.y1][j-sel.x1].b = sum_b;
			}
			else{
				new_image[i-sel.y1][j-sel.x1].r = image[i][j].r;
				new_image[i-sel.y1][j-sel.x1].g = image[i][j].g;
				new_image[i-sel.y1][j-sel.x1].b = image[i][j].b;
			}
		}
	}
	// copy new image to old image
	for(int i=0; i<new_height; i++){
		for(int j=0; j<new_width; j++){
			image[i+sel.y1][j+sel.x1].r = new_image[i][j].r;
			image[i+sel.y1][j+sel.x1].g = new_image[i][j].g;
			image[i+sel.y1][j+sel.x1].b = new_image[i][j].b;
		}
	}
	printf("APPLY EDGE done\n");
	// free memory
	for(int i=0; i<new_height; i++){
		free(new_image[i]);
	}
	free(new_image);
}



void sharpen(netpbm **image, selection sel, int *maxval, int width, int height)
{
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;
	// create new image
	int new_height = sel.y2 - sel.y1;
	int new_width = sel.x2 - sel.x1;
	netpbm **new_image = (netpbm**)malloc(new_height*sizeof(netpbm*));
	for(int i=0; i<new_height; i++){
		new_image[i] = (netpbm*)malloc(new_width*sizeof(netpbm));
	}
	// apply filter
	for(int i=sel.y1; i<sel.y2; i++){
		for(int j=sel.x1; j<sel.x2; j++){
			int sum_r = 0 , sum_g = 0, sum_b = 0;
			int vecini = 0;
			if(i-1 >= 0) { // upper
				sum_r -= image[i-1][j].r;
				sum_g -= image[i-1][j].g;
				sum_b -= image[i-1][j].b;
				vecini++;
			}
			if(j-1 >= 0) { // left
				sum_r -= image[i][j-1].r;
				sum_g -= image[i][j-1].g;
				sum_b -= image[i][j-1].b;
				vecini++;
			}
			if(j+1 < width) { // right
				sum_r -= image[i][j+1].r;
				sum_g -= image[i][j+1].g;
				sum_b -= image[i][j+1].b;
				vecini++;
			}
			if(i+1 < height) { // lower
				sum_r -= image[i+1][j].r;
				sum_g -= image[i+1][j].g;
				sum_b -= image[i+1][j].b;
				vecini++;
			}
			// middle
			sum_r += 5*image[i][j].r;
			sum_g += 5*image[i][j].g;
			sum_b += 5*image[i][j].b;

			if(sum_r < 0) sum_r = 0;
			else if(sum_r > 255) {
				sum_r = 255;
				*maxval = 255;
			}
			if(sum_g < 0) sum_g = 0;
			else if(sum_g > 255) {
				sum_g = 255;
				*maxval = 255;

			}
			if(sum_b < 0) sum_b = 0;
			else if(sum_b > 255) {
				sum_b = 255;
				*maxval = 255;
			}
		
			if(vecini == 4){
				new_image[i-sel.y1][j-sel.x1].r = sum_r;
				new_image[i-sel.y1][j-sel.x1].g = sum_g;
				new_image[i-sel.y1][j-sel.x1].b = sum_b;
			}
			else{
				new_image[i-sel.y1][j-sel.x1].r = image[i][j].r;
				new_image[i-sel.y1][j-sel.x1].g = image[i][j].g;
				new_image[i-sel.y1][j-sel.x1].b = image[i][j].b;
			}
		}
	}
	// copy new image to old image
	for(int i=0; i<new_height; i++){
		for(int j=0; j<new_width; j++){
			image[i+sel.y1][j+sel.x1].r = new_image[i][j].r;
			image[i+sel.y1][j+sel.x1].g = new_image[i][j].g;
			image[i+sel.y1][j+sel.x1].b = new_image[i][j].b;
		}
	}
	printf("APPLY SHARPEN done\n");
	// free memory
	for(int i=0; i<new_height; i++){
		free(new_image[i]);
	}
	free(new_image);
}



void blur(netpbm **image, selection sel, int width, int height)
{
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;
	// create new image
	int new_height = sel.y2 - sel.y1;
	int new_width = sel.x2 - sel.x1;
	netpbm **new_image = (netpbm**)malloc(new_height*sizeof(netpbm*));
	for(int i=0; i<new_height; i++){
		new_image[i] = (netpbm*)malloc(new_width*sizeof(netpbm));
	}
	// apply filter
	for(int i=sel.y1; i<sel.y2; i++){
		for(int j=sel.x1; j<sel.x2; j++){
			int sum_r = 0 , sum_g = 0, sum_b = 0;
			int vecini = 0;
			if(i-1 >= 0 && j-1 >= 0) { // upper-left
				sum_r += image[i-1][j-1].r;
				sum_g += image[i-1][j-1].g;
				sum_b += image[i-1][j-1].b;
				vecini++;
			}
			if(i-1 >= 0) { // upper
				sum_r += image[i-1][j].r;
				sum_g += image[i-1][j].g;
				sum_b += image[i-1][j].b;
				vecini++;
			}
			if(i-1 >= 0 && j+1 < width) { // upper-right
				sum_r += image[i-1][j+1].r;
				sum_g += image[i-1][j+1].g;
				sum_b += image[i-1][j+1].b;
				vecini++;
			}
			if(j-1 >= 0) { // left
				sum_r += image[i][j-1].r;
				sum_g += image[i][j-1].g;
				sum_b += image[i][j-1].b;
				vecini++;
			}
			if(j+1 < width) { // right
				sum_r += image[i][j+1].r;
				sum_g += image[i][j+1].g;
				sum_b += image[i][j+1].b;
				vecini++;
			}
			if(i+1 < height && j-1 >= 0) { // lower-left
				sum_r += image[i+1][j-1].r;
				sum_g += image[i+1][j-1].g;
				sum_b += image[i+1][j-1].b;
				vecini++;
			}
			if(i+1 < height) { // lower
				sum_r += image[i+1][j].r;
				sum_g += image[i+1][j].g;
				sum_b += image[i+1][j].b;
				vecini++;
			}
			if(i+1 < height && j+1 < width) { // lower-right
				sum_r += image[i+1][j+1].r;
				sum_g += image[i+1][j+1].g;
				sum_b += image[i+1][j+1].b;
				vecini++;
			}
			// middle
			sum_r += image[i][j].r;
			sum_g += image[i][j].g;
			sum_b += image[i][j].b;

			sum_r /= 9;
			sum_g /= 9;
			sum_b /= 9;

			if(vecini == 8){
				new_image[i-sel.y1][j-sel.x1].r = sum_r;
				new_image[i-sel.y1][j-sel.x1].g = sum_g;
				new_image[i-sel.y1][j-sel.x1].b = sum_b;
			}
			else{
				new_image[i-sel.y1][j-sel.x1].r = image[i][j].r;
				new_image[i-sel.y1][j-sel.x1].g = image[i][j].g;
				new_image[i-sel.y1][j-sel.x1].b = image[i][j].b;
			}
		}
	}
	// copy new image to old image
	for(int i=0; i<new_height; i++){
		for(int j=0; j<new_width; j++){
			image[i+sel.y1][j+sel.x1].r = new_image[i][j].r;
			image[i+sel.y1][j+sel.x1].g = new_image[i][j].g;
			image[i+sel.y1][j+sel.x1].b = new_image[i][j].b;
		}
	}
	printf("APPLY BLUR done\n");
	// free memory
	for(int i=0; i<new_height; i++){
		free(new_image[i]);
	}
	free(new_image);
}



void gaussian_blur(netpbm **image, selection sel, int width, int height)
{
	// check if image is loaded 
	if(is_image_loaded(image)==0) return;
	// create new image
	int new_height = sel.y2 - sel.y1;
	int new_width = sel.x2 - sel.x1;
	netpbm **new_image = (netpbm**)malloc(new_height*sizeof(netpbm*));
	for(int i=0; i<new_height; i++){
		new_image[i] = (netpbm*)malloc(new_width*sizeof(netpbm));
	}
	// apply filter
	for(int i=sel.y1; i<sel.y2; i++){
		for(int j=sel.x1; j<sel.x2; j++){
			int sum_r = 0 , sum_g = 0, sum_b = 0;
			int vecini = 0;
			if(i-1 >= 0 && j-1 >= 0) { // upper-left
				sum_r += image[i-1][j-1].r;
				sum_g += image[i-1][j-1].g;
				sum_b += image[i-1][j-1].b;
				vecini++;
			}
			if(i-1 >= 0) { // upper
				sum_r += 2*image[i-1][j].r;
				sum_g += 2*image[i-1][j].g;
				sum_b += 2*image[i-1][j].b;
				vecini++;
			}
			if(i-1 >= 0 && j+1 < width) { // upper-right
				sum_r += image[i-1][j+1].r;
				sum_g += image[i-1][j+1].g;
				sum_b += image[i-1][j+1].b;
				vecini++;
			}
			if(j-1 >= 0) { // left
				sum_r += 2*image[i][j-1].r;
				sum_g += 2*image[i][j-1].g;
				sum_b += 2*image[i][j-1].b;
				vecini++;
			}
			if(j+1 < width) { // right
				sum_r += 2*image[i][j+1].r;
				sum_g += 2*image[i][j+1].g;
				sum_b += 2*image[i][j+1].b;
				vecini++;
			}
			if(i+1 < height && j-1 >= 0) { // lower-left
				sum_r += image[i+1][j-1].r;
				sum_g += image[i+1][j-1].g;
				sum_b += image[i+1][j-1].b;
				vecini++;
			}
			if(i+1 < height) { // lower
				sum_r += 2*image[i+1][j].r;
				sum_g += 2*image[i+1][j].g;
				sum_b += 2*image[i+1][j].b;
				vecini++;
			}
			if(i+1 < height && j+1 < width) { // lower-right
				sum_r += image[i+1][j+1].r;
				sum_g += image[i+1][j+1].g;
				sum_b += image[i+1][j+1].b;
				vecini++;
			}
			// middle
			sum_r += 4*image[i][j].r;
			sum_g += 4*image[i][j].g;
			sum_b += 4*image[i][j].b;

			sum_r /= 16;
			sum_g /= 16;
			sum_b /= 16;

			if(vecini == 8){
				new_image[i-sel.y1][j-sel.x1].r = sum_r;
				new_image[i-sel.y1][j-sel.x1].g = sum_g;
				new_image[i-sel.y1][j-sel.x1].b = sum_b;
			}
			else{
				new_image[i-sel.y1][j-sel.x1].r = image[i][j].r;
				new_image[i-sel.y1][j-sel.x1].g = image[i][j].g;
				new_image[i-sel.y1][j-sel.x1].b = image[i][j].b;
			}
		}
	}
	// copy new image to old image
	for(int i=0; i<new_height; i++){
		for(int j=0; j<new_width; j++){
			image[i+sel.y1][j+sel.x1].r = new_image[i][j].r;
			image[i+sel.y1][j+sel.x1].g = new_image[i][j].g;
			image[i+sel.y1][j+sel.x1].b = new_image[i][j].b;
		}
	}
	printf("APPLY GAUSSIAN_BLUR done\n");
	// free memory
	for(int i=0; i<new_height; i++){
		free(new_image[i]);
	}
	free(new_image);
}



void invalid_command(int need_to_clear) {
	if (need_to_clear == 1)
	{
		char *trash = malloc(100);
		fgets(trash, 100, stdin);
		free(trash);
	}
	printf("Invalid command\n");
}
