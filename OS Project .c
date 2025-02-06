#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include<time.h>
#include <windows.h>

#define ROWS 30
#define COLS 60
#define MAX_NODES 100
#define NUM_THREADS 4

int grid[ROWS][COLS];
int source_row, source_col, dest_row, dest_col;
bool visited[ROWS][COLS];

typedef struct node {
    int row;
    int col;
    int f_score;
    int g_score;
    struct node* parent;
} node_t;

node_t* open_list[MAX_NODES];
int open_list_size = 0;

pthread_mutex_t lock;

int heuristic(node_t* n) {
    return abs(n->row - dest_row) + abs(n->col - dest_col);
}

bool is_valid_node(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS &&
           (grid[row][col] == 0 || (row == source_row && col == source_col) || (row == dest_row && col == dest_col)) &&
           !visited[row][col];
}


bool is_valid_neighbor(node_t* curr, int row, int col) {
    if (row == curr->row && col == curr->col) {
        return false;
    }

    if (!is_valid_node(row, col)) {
        return false;
    }

    if (abs(curr->row - row) + abs(curr->col - col) > 1) {
        return false;
    }	
    return true;
}

void add_to_open_list(node_t* n) {
    pthread_mutex_lock(&lock);

    if (open_list_size >= MAX_NODES) {
        printf("Error: Open list overflow\n");
        exit(1);
    }

    open_list[open_list_size] = n;
    open_list_size++;

    pthread_mutex_unlock(&lock);
}

node_t* get_from_open_list() {
    pthread_mutex_lock(&lock);

    if (open_list_size <= 0) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    int min_f_score = open_list[0]->f_score;
    int min_index = 0;

    for (int i = 1; i < open_list_size; i++) {
        if (open_list[i]->f_score < min_f_score) {
            min_f_score = open_list[i]->f_score;
            min_index = i;
        }
    }

    node_t* result = open_list[min_index];

    for (int i = min_index + 1; i < open_list_size; i++) {
        open_list[i - 1] = open_list[i];
    }

    open_list_size--;

    pthread_mutex_unlock(&lock);

    return result;
}

void construct_path(node_t* n) {
    // traverse the path again to find nodes in the shortest path
    while (n != NULL) {
        int row = n->row;
        int col = n->col;
        grid[row][col] = '+';
        n = n->parent;
    }
}

void* astar(void* arg) {
    int thread_num = *(int*)arg;
    srand(time(NULL));

    while (true) {
        node_t* curr = get_from_open_list();

        if (curr == NULL) {
            break;
        }

        visited[curr->row][curr->col] = true;

        if (curr->row == dest_row && curr->col == dest_col) {
            printf("Thread %d found a path!\n", thread_num);
            construct_path(curr);
            pthread_exit(NULL);
        }

        int neighbors_row[4] = { curr->row - 1, curr->row, curr->row, curr->row + 1 };
        int neighbors_col[4] = { curr->col, curr->col - 1, curr->col + 1, curr->col };

        for (int i = 0; i < 4; i++) {
            int neighbor_row = neighbors_row[i];
            int neighbor_col = neighbors_col[i];

            if (!is_valid_neighbor(curr, neighbor_row, neighbor_col)) {
                continue;
            }

            int tentative_g_score = curr->g_score + 1;

            node_t* neighbor = NULL;

            for (int j = 0; j < open_list_size; j++) {
                if (open_list[j]->row == neighbor_row && open_list[j]->col == neighbor_col) {
                    neighbor = open_list[j];
                    break;
                }
            }

            if (neighbor == NULL) {
                neighbor = malloc(sizeof(node_t));
                neighbor->row = neighbor_row;
                neighbor->col = neighbor_col;
                neighbor->parent = curr;
                neighbor->g_score = tentative_g_score;
                neighbor->f_score = tentative_g_score + heuristic(neighbor);

                add_to_open_list(neighbor);
            } else if (tentative_g_score < neighbor->g_score) {
                neighbor->parent = curr;
                neighbor->g_score = tentative_g_score;
                neighbor->f_score = tentative_g_score + heuristic(neighbor);
            }
        }

        if (curr->row == dest_row && curr->col == dest_col) {
            construct_path(curr);
            printf("Thread %d found a path!\n", thread_num);
            pthread_exit(NULL);
        }
    }
	printf("Thread %d unable to find path :(( \n",thread_num);
    pthread_exit(NULL);
}

void print_grid() {
	printf("Raw grid is: \n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == -1) {
                printf("X "); // obstacle
            } else if (i == source_row && j == source_col) {
                printf("S "); // start node
            } else if (i == dest_row && j == dest_col) {
                printf("D "); // destination node
            } else {
                // check if this node is in the path
                bool in_path = false;
                node_t* curr = open_list[0];
                while (curr != NULL) {
                    if (curr->row == i && curr->col == j) {
                        in_path = true;
                        break;
                    }
                    curr = curr->parent;
                }

                if (in_path) {
                    printf(". "); // node in path
                } else {
                    printf(". "); // empty node
                }
            }
        }
    
		printf("\n");
    }
}


int main() {
    
	printf("Meaning of symbols used: \n");
	printf("X : Obstacles\n. : Path unexplored\n+ : Shortest Path\n\n");
    int obstacle_threshold = 30; // Percentage chance of an obstacle appearing at a position

    // Randomly generate obstacles
    srand(time(NULL));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int obstacle = rand() % 100; // Generate a random number between 0 and 99
            if (obstacle < obstacle_threshold) {
                grid[i][j] = -1; // Add an obstacle
            }
        }
    }

    printf("Enter the source row: ");
    scanf("%d", &source_row); 
    printf("Enter the source col: ");
    scanf("%d", &source_col); 
	printf("Enter the destination row: ");
    scanf("%d", &dest_row); 
    printf("Enter the destinaton column: ");
    scanf("%d", &dest_col); 
    printf("\n");

    node_t* start_node = malloc(sizeof(node_t));
    start_node->row = source_row;
    start_node->col = source_col;
    start_node->parent = NULL;
    start_node->g_score = 0;
    start_node->f_score = heuristic(start_node);
    add_to_open_list(start_node);

    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i;
        pthread_create(&threads[i], NULL, astar, &thread_nums[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    node_t* curr = open_list[0];

    while (curr != NULL) {
        visited[curr->row][curr->col] = true;
        curr = curr->parent;
    }


    for (int i = 0; i < open_list_size; i++) {
        free(open_list[i]);
    }
  printf("\n");
  
  printf("Shortest path(Represented by +) is:\n\n\n");
    for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
    	if(i == source_row && j == source_col){ 
			printf("S " );
			continue;
    	}
    	if(i == dest_row && j == dest_col){
    		printf("D " );
    		continue;
    	}
    	if(grid[i][j] == -1) {
    		printf("X ");
		}
		else if(grid[i][j] == '+') {
    		printf("\x1b[34m+ \x1b[0m");
        }
	 	else{
    		printf(". ");
		}
    }
    printf("\n");
}

    return 0;
}


