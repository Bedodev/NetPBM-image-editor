// Copyright Andrei-Dragos Badoiu 315CAa 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// struct image with attributes: r, g, b
typedef struct netpbm{
	int r;
	int g;
	int b;
}netpbm;

// struct selection with attributes: x1, y1, x2, y2
typedef struct selection{
	int x1; // lower width
	int y1; // lower height
	int x2; // higher width
	int y2; // higher height
}selection;


int command_id(char *command);
int is_image_loaded(netpbm **image);
int check_image_type(char *filename);
netpbm **load_image(char *filename, netpbm **image, int *image_type, int *width, int *height, int *maxval);
void select_area(netpbm **image, selection *sel, int width, int height);
void change_selection(selection *sel, int x1, int y1, int x2, int y2);
void histogram(netpbm **image, selection sel, int image_type, int maxval, int width, int height);
void equalize(netpbm **image, selection sel, int image_type, int maxval, int width, int height);
netpbm **rotate(netpbm **image, selection *sel, int *width, int *height);
netpbm **crop(netpbm **image, selection *sel, int *width, int *height, int image_type);
void save_image(netpbm **image, int image_type, int maxval, int width, int height);
void apply(netpbm **image, selection sel, int image_type, int *maxval, int width, int height);

void edge_detection(netpbm **image, selection sel, int *maxval, int width, int height);
void sharpen(netpbm **image, selection sel, int *maxval, int width, int height);
void blur(netpbm **image, selection sel, int width, int height);
void gaussian_blur(netpbm **image, selection sel, int width, int height);

void invalid_command(int need_to_clear);
netpbm **rotation_once(netpbm **image, selection *sel, int *width, int *height, int is_negative, int is_whole);