#include <stdio.h>
// Add your system includes here.
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "ftree.h"
#include "hash.h"

struct TreeNode* head = NULL;

struct TreeNode* ft_node(char* fname, int permiss, char* path){
	struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	FILE *fp;
	fp = fopen(path, "r");
	if(fp == NULL) {
		node -> hash = NULL;
	}else {
		node -> hash = hash(fp);
	}
	fclose(fp);
	node -> fname = fname;
	node -> permissions = permiss;
	node -> next = NULL;
	node -> contents = NULL;
	return node;
}

void Insert_head(char* fname, int permiss, char* path) {
	struct TreeNode* newNode = ft_node(fname, permiss, path);
	struct TreeNode* conts = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	head = newNode;
	head -> contents = conts;
}

char* permission(mode_t mode) {
	char result[7];
	char *permit = malloc(7 * sizeof(char));
	sprintf(result , "%o", mode);
	strcpy(permit, result);
	free(permit);
	return permit;
}





int get_permission(mode_t mode) {
	char *result = malloc(4 * sizeof(char));
	char *permit = permission(mode);
	char temp[7];
	int len;
	strncpy(temp, permit, sizeof(temp) - 1);
	len = strlen(temp);
	if(len == 6) {
		result[0] = temp[3];
		result[1] = temp[4];
		result[2] = temp[5];
		result[3] = '\0';
	}else if(len == 5) {
		result[0] = temp[2];
		result[1] = temp[3];
		result[2] = temp[4];
		result[3] = '\0';
	}
	free(result);
	int permission_int;
	sscanf(result, "%d", &permission_int);
	return permission_int;
}

char* current_path(char* dir_name, char* fold) {
	const char* fs = "/";
	char* path = (char*)malloc(2*sizeof(char) + strlen(dir_name) + strlen(fold) + 1);
	// append folder name to the input path.
	strcpy(path, dir_name);
	strcat(path, fs);
	strcat(path, fold);
	return path;
}

void Insert_h_contents(char* fname, int permiss, char* path) {
	struct TreeNode* temp = head -> contents;
	struct TreeNode* newNode = ft_node(fname, permiss, path);
	if(head -> contents == NULL) {
		head -> contents = newNode;
	}else {
		while(temp -> next != NULL) {
			temp = temp -> next;
		}
		temp -> next = newNode;
	}
}

void Insert_cons(struct TreeNode ***node, char* fname, int permiss, char* path) {
	struct TreeNode** temp = *node;
	struct TreeNode* newNode = ft_node(fname, permiss, path);
	while((*temp) -> next != NULL) {
		if((*temp) -> next == NULL) {
			(*temp) -> next = newNode;
		}
	}
	(*temp) = (*temp) -> next;
}

void Insert_dir(char* fname, int permiss, struct TreeNode **node, char* path) {
	struct stat filestat;
	struct dirent *sd;
	DIR* dir = opendir(path);
	while((sd = readdir(dir)) != NULL) {
		// excluding "." and ".."
		if(strcmp(sd -> d_name, ".") != 0 && strcmp(sd -> d_name, ".") != 0) {
			// excluding files/directories that begin with '.'
			if((sd -> d_name)[0] != '.') {
				char* newpath = current_path(path, sd -> d_name);
				lstat(newpath, &filestat);
				int permission = get_permission(filestat.st_mode);
				struct TreeNode* newNode = ft_node(sd -> d_name, permission, newpath);
				struct TreeNode* temp = *node;
				
				if(S_ISREG(filestat.st_mode)) {
					if((*node) == NULL) {
						(*node) = newNode;
					}else {
						while(temp -> next != NULL) {
							temp = temp -> next;
						}
						temp -> next = newNode;
					}

				}else if(S_ISLNK(filestat.st_mode)) {
					if((*node) == NULL) {
						(*node) = newNode;
					}else {
						while(temp -> next != NULL) {
							temp = temp -> next;
						}
						temp -> next = newNode;
					}
				}else if(sd->d_type == DT_DIR){//else if(S_ISDIR(filestat.st_mode)) {
					int path_len;
					char cp_path[4096];
					path_len = snprintf(cp_path, 4096, "%s/%s", path, sd->d_name);
					struct TreeNode* conts = (struct TreeNode*)malloc(sizeof(struct TreeNode));
					if((*node) == NULL) {
						(*node) = newNode;
						(*node) -> contents = conts;
						Insert_dir(sd -> d_name, permission, &((*node) -> contents), cp_path);
					}else {
						while(temp -> next != NULL) {
							temp = temp -> next;
						}
						struct TreeNode* conts2 = (struct TreeNode*)malloc(sizeof(struct TreeNode));
						temp -> next = newNode;
						temp -> next -> contents = conts2;
						Insert_dir(sd -> d_name, permission, &(temp -> next -> contents), cp_path);
					}
				}		
			}
		}
	}
}

/*
 * Returns the FTree rooted at the path fname.
 */
struct TreeNode *generate_ftree(const char *fname) {
    // Your implementation here.
    struct dirent *sd;
    struct stat filestat;
	DIR* dir = opendir(fname);
	int count = 0;
	lstat((char*)fname, &filestat);
	int temp_p = get_permission(filestat.st_mode);
	char* headname;
	if(S_ISREG(filestat.st_mode) || S_ISLNK(filestat.st_mode)){:
		Insert_head((char*)fname,temp_p,(char*)fname);
		head->next = NULL;
		return head;
	}else if(strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) {
		Insert_head((char*)fname,temp_p, (char*)fname);
		if(dir == NULL) {
			perror("opendir");
		}
			//reading all files from current directory.
   		 while ((sd = readdir(dir)) != NULL) {
			if(strcmp(sd -> d_name, ".") != 0 && strcmp(sd -> d_name, "..") !=  0) {
				if((sd -> d_name)[0] != '.') {
					char* path = current_path((char*) fname, sd -> d_name);
					lstat(path, &filestat);
					int permission = get_permission(filestat.st_mode);

					if(S_ISREG(filestat.st_mode)) {
						count ++;
						Insert_h_contents(sd -> d_name, permission, path);

					}else if(S_ISLNK(filestat.st_mode)) {
						count ++;
						Insert_h_contents(sd -> d_name, permission, path);

					}else if(S_ISDIR(filestat.st_mode)) {
						struct TreeNode* newNode = ft_node(sd -> d_name, permission, path);
						struct TreeNode* temp = head -> contents;
						struct TreeNode* conts = (struct TreeNode*)malloc(sizeof(struct TreeNode));
						if(head -> contents == NULL) {
							head -> contents = newNode;
							head -> contents -> contents = conts;
							Insert_dir(sd -> d_name, permission, &(head -> contents->contents), path);
						}else {
							while(temp -> next != NULL) {
								temp = temp -> next;
							}
							temp -> next = newNode;
							temp -> next -> contents = conts;
							Insert_dir(sd -> d_name, permission, &(temp -> next -> contents), path);
						}
					}
				}
			}
		}
		closedir(dir);
		return head;
	}else{
		headname = basename((char*)fname);
		Insert_head(headname, temp_p,(char*)fname);
			//reading all files from current directory.
	   	 while ((sd = readdir(dir)) != NULL) {
			if(strcmp(sd -> d_name, ".") != 0 && strcmp(sd -> d_name, "..") !=  0) {
				if((sd -> d_name)[0] != '.') {
					char* path = current_path((char*) fname, sd -> d_name);
					lstat(path, &filestat);
					int permission = get_permission(filestat.st_mode);

					if(S_ISREG(filestat.st_mode)) {
						count ++;
						Insert_h_contents(sd -> d_name, permission, path);

					}else if(S_ISLNK(filestat.st_mode)) {
						count ++;
						Insert_h_contents(sd -> d_name, permission, path);

					}else if(S_ISDIR(filestat.st_mode)) {
						struct TreeNode* newNode = ft_node(sd -> d_name, permission, path);
						struct TreeNode* temp = head -> contents;
						struct TreeNode* conts = (struct TreeNode*)malloc(sizeof(struct TreeNode));
						if(head -> contents == NULL) {
							head -> contents = newNode;
							head -> contents -> contents = conts;
							Insert_dir(sd -> d_name, permission, &(head -> contents->contents), path);
						}else {
							while(temp -> next != NULL) {
								temp = temp -> next;
							}
							temp -> next = newNode;
							temp -> next -> contents = conts;
							Insert_dir(sd -> d_name, permission, &(temp -> next -> contents), path);
						}
					}
				}
			}
		}
		closedir(dir);
		return head;
	}
}


/*
 * Prints the TreeNodes encountered on a preorder traversal of an FTree.
 */
void print_ftree(struct TreeNode *root) {
    // Here's a trick for remembering what depth (in the tree) you're at
    // and printing 2 * that many spaces at the beginning of the line.
    static int depth = 0;
    printf("%*s", depth * 2, "");
    // Your implementation here.
	struct TreeNode* temp = root -> contents;
	printf("===== %s (%d) =====\n",root->fname, root->permissions);
	depth++;
	while(temp -> next != NULL) {
		if(temp -> next -> contents != NULL) {
			
			print_ftree(temp->next);
		}else{
			printf("%*s",depth*2,"");
			printf("%s (%d)\n", temp->next->fname, temp->next->permissions);
		}
		temp = temp -> next;
	}
	depth--;
}
