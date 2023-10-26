// Copyright Andrei-Dragos Badoiu 315CAa 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "commands.h"


int main()
{
	// main photo vars
	int image_type=0, width=0, height=0, maxval=0;
	netpbm **image = NULL;
	selection sel = {0,0,0,0};

	char *command = malloc(10);
	char *filename = malloc(20);
	while(1){
		scanf("%s", command);
		// printf("command is: %s\n", command);
		switch (command_id(command))
		{
			case 1:
				scanf(" %s", filename);
				image = load_image(filename,image,&image_type,&width,&height,&maxval);
				change_selection(&sel, 0, 0, width, height);
				break;
			case 2:
				select_area(image,&sel,width,height);
				break;
			case 3:
				histogram(image,sel,image_type,maxval,width,height);
				break;
			case 4:
				equalize(image,sel,image_type,maxval,width,height);
				break;
			case 5:
				image = rotate(image,&sel,&width,&height);
				break;
			case 6:
				image = crop(image,&sel,&width,&height,image_type);
				break;
			case 7:
				save_image(image,image_type,maxval, width, height);
				break;
			case 8:
				apply(image,sel,image_type,&maxval, width, height);
				break;
			case 9:
				is_image_loaded(image);
				for(int i=0; i<height; i++) free(image[i]);
				free(image);
				free(command);
				free(filename);
				return 0;
				break;
			case 77:
				// read testing
				for(int i=0; i<height; i++){
					for(int j=0; j<width; j++){
						printf("%d ", image[i][j].r);
					}
					printf("\n");
				}
				break;
			case 88:
				// selection testing
				printf("sel: %d %d %d %d\n", sel.x1, sel.y1, sel.x2, sel.y2);
				break;
			default:
				invalid_command(1);
		}
	}
	for(int i=0; i<height; i++){
		free(image[i]);
	}
	free(image);
	free(command);
	free(filename);
	return 0;
}