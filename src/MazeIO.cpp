#include "MazeIO.h"
#include "Direction.h"
#include "Node.h"

void print_maze_mouse(Maze maze, Mouse mouse){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		char *str = (char *)malloc((MAZE_SIZE * 2 + 1) * sizeof(char));

		char *s=str;
		for (j=0;j<MAZE_SIZE;j++){
      Node *n;
      maze.get_node(&n, i, j);
			if (n->neighbor(Direction::W) == NULL){
				strcpy(s++,"|");
			}
			else {
				strcpy(s++,"_");
			}

			if (mouse.row == i && mouse.col == j){
					strcpy(s++,"o");
			}
			else if (n->neighbor(Direction::S) == NULL){
					strcpy(s++,"_");
			}
			else {
				strcpy(s++," ");
			}
		}
		*(s++) = '|';
		*s = '\0';
		printf("%s\n",str);
		free(str);
	}
}

void print_maze(Maze maze){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		char *str = (char *)malloc((MAZE_SIZE * 2 + 1) * sizeof(char));

		char *s=str;
		for (j=0;j<MAZE_SIZE;j++){
      Node *n;
      maze.get_node(&n, i, j);
			if (n->neighbor(Direction::W) == NULL){
				strcpy(s++,"|");
				if (n->neighbor(Direction::S) == NULL){
					strcpy(s++,"_");
				}
				else {
					strcpy(s++," ");
				}
			}
			else {
				strcpy(s++,"_");
				if (n->neighbor(Direction::S) == NULL){
					strcpy(s++,"_");
				}
				else {
					strcpy(s++," ");
				}
			}
		}
		*(s++) = '|';
		*s = '\0';
		printf("%s\n",str);
		free(str);
	}
}

void print_neighbor_maze(Maze maze){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		for (j=0;j<MAZE_SIZE;j++){
			int d;
			for (d=0;d<4;d++){
				bool wall = (maze.nodes[i][j]->neighbors[d] == NULL);
				printf("%i",wall);
			}
			printf(" ");
		}
		printf("\n");
	}
}

void print_weight_maze(Maze maze){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		for (j=0;j<MAZE_SIZE;j++){
			int w = maze.nodes[i][j]->weight;
			printf("%03d ",w);
		}
		printf("\n");
	}
}

void print_dist_maze(Maze maze){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		for (j=0;j<MAZE_SIZE;j++){
      Node *n;
      maze.get_node(&n, i, j);
			int d = n->distance;
			if (d<10){
				printf("  %d ",d);
			}
			else if (d<100){
				printf(" %d ",d);
			}
			else {
				printf("%d ",d);
			}
		}
		printf("\n");
	}
}

void print_pointer_maze(Maze maze){
	int i,j;
	for (i=0;i<MAZE_SIZE;i++){
		for (j=0;j<MAZE_SIZE;j++){
      Node *n;
      maze.get_node(&n, i, j);
			printf("%p ", n);
		}
		printf("\n");
	}
}
