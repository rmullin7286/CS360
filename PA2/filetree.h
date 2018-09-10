#pragma once

//Type of node. Either Directory or File
typedef enum nodetype
{
    FILENODE,
    DIRECTORYNODE
}NodeType;

//C structure for file node for filesystem tree
typedef struct node
{
    char name[64];
    NodeType type;
    struct node *child, *sibling, *parent;
}Node;

typedef struct filetree
{
    Node *root, *cwd;
}FileTree;

FileTree initFileTree(void);
Node * makeNode(char * name, NodeType type);
void closeFileTree(FileTree * ft);

int mkdir(FileTree * ft, char * pathname);
int rmdir(FileTree * ft, char * pathname);
int cd(FileTree * ft, char * pathname);
int creat(FileTree * ft, char * pathname);
int ls(FileTree * ft, char * pathname);
int pwd(FileTree * ft);
int rm(FileTree * ft, char * pathname);

void save(FileTree * ft);
void reload(FileTree * ft);