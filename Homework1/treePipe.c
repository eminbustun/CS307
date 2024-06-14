#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
/*
* The tree will be full binary tree which every node 
* has 2 children or none. 
* 
* Input might be coming from descendants or parent process
*
* Process may forward the result of its own computation
* to its children or parent. Hence, processes have to 
* communicate with each other. We have to use Unix pipes 
* for these purpose.
*
* 
*/


struct Node {
    int lr; //* right => 1, left => 0
    int currentDepth;
    int maxDepth;
    int num1;
    struct Node* left;
    struct Node* right;
};


void inOrderTraversal(struct Node* root) {
    if (root == NULL){
        return;
    }
    inOrderTraversal(root->left);
    fprintf(stderr, "Num1 = %d, Depth = %d\n", root->num1, root->currentDepth);
    inOrderTraversal(root->right);
}



struct Node* newNode(int lr, int num1, int currentDepth, int maxDepth){

    struct Node* node = (struct Node*)malloc(sizeof(struct Node));

    node->lr = lr;
    node->num1 = num1;
    node->left = NULL;
    node->right = NULL;
    node->currentDepth = currentDepth;
    node->maxDepth = maxDepth;

    return node;

}

int main(int argc, char *argv[]){

    if (argc != 4){
        printf("Usage: treePipe <currenct depth> <max depth> <left-right>\n");
        exit(1);
    }

    
    // Turning argv items to integers
    int fd[2]; 
    pipe(fd); //* Between parent and child

    int curDepth = atoi(argv[1]);
    //printf("curDepth is = %d\n", curDepth);
    int maxDepth = atoi(argv[2]);
    //printf("maxDepth is = %d\n", maxDepth);
    int lr = atoi(argv[3]);
    //printf("lr is = %d\n", lr);

    struct Node *rootNode = newNode(lr, 0, curDepth, maxDepth);

    int rc = fork();

    if (rc < 0) {
        printf("Fork failed!\n");
        exit(1);
    }

    else if (rc == 0){
        //* This is the left child of the parent.

        if (maxDepth != 0 && curDepth != maxDepth){
             dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
                
            int num1_child;
        
            scanf("%d", &num1_child);
            
            struct Node *childNode = newNode(lr, num1_child, curDepth, maxDepth);

            rootNode->num1 = num1_child;
            rootNode->left = childNode;        

            
            char sNum1[100];
            char sCurDepth[100];
            char sMaxDepth[100];
            char sLr[3];
            char *myargs[5];   
            curDepth = curDepth + 1;
            snprintf(sCurDepth, sizeof(sCurDepth), "%d", curDepth);
            snprintf(sMaxDepth, sizeof(sMaxDepth), "%d", maxDepth);
            snprintf(sLr, sizeof(sLr), "%d", lr);
            snprintf(sNum1, sizeof(sNum1), "%d", num1_child);

            myargs[0] = strdup("./a.out");
            myargs[1] = strdup(sCurDepth);
            myargs[2] = strdup(sMaxDepth);
            myargs[3] = strdup(sLr);
            myargs[4] = NULL;
                
            execvp(argv[0], myargs);
        
        }
        else if(maxDepth == 0){
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            dup2(fd[1], STDOUT_FILENO);
                //close(fd[1]);

            execvp("./left", NULL);
        }

       

        } else if (rc > 0) {
         printf("> current depth: 0, lr: 0\n");

        printf("Please enter num1 for the root: ");


        int num1;

        scanf("%d", &num1);

        if (maxDepth == 0){
           


    

                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);

                int num2 = 1;

                printf("%d\n", num1);
                printf("%d\n", num2);

            
                wait(NULL);
                dup2(fd[0], STDIN_FILENO);
                int nt;

                scanf("%d", &nt);
                //fprintf(stderr,"res = %d\n", nt);
                fprintf(stderr, "> my num1 is: %d\n", num1);
                fprintf(stderr, "> my result is: %d\n", nt);
                fprintf(stderr, "The final result is: %d\n", nt);

                

                close(fd[0]);

                fflush(stdout);
                close(STDOUT_FILENO);
                exit(1);

            

        }


        rootNode->num1 = num1;

        //* Root node
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        

        printf("%d", num1);
        fflush(stdout);
        close(STDOUT_FILENO);

        wait(NULL);

        //inOrderTraversal(rootNode);          
        
    }  
    return 0;

}