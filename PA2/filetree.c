#include "filetree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node * makeNode(char * name, NodeType type)
{
    Node * newNode = malloc(sizeof(Node));
    newNode->child = newNode->parent = newNode->sibling = NULL;
    newNode->type = type;
    strcpy(newNode->name, name);
    return newNode;
}

FileTree initFileTree(void)
{
    FileTree newFileTree;
    newFileTree.cwd = newFileTree.root = makeNode("/", DIRECTORYNODE);
    return newFileTree;
}

Node * searchPath(Node * start, char * path)
{
    char * tok = strtok(path, "/");
    do
    {
        if(start->type == FILENODE)
            return NULL;
        start = start->child;
        while(start)
            if(strcmp(start->name, tok) == 0)
                break;
            else
                start = start->sibling;
        if(!start)
            return NULL;
    }while(tok = strtok(NULL, "/"));
    return start;
}

void removeNode(Node * path)
{
    if(path->parent->child == path)
    {
        path->parent->child = path->sibling;
    }
    else
    {
        Node * cur = path->parent->child;
        while(cur->sibling != path)
            cur = cur->sibling;
        cur->sibling = path->sibling;
    }
    free(path);
}

int insertNode(FileTree * ft, char * pathname, NodeType t)
{
    //split up basename and dirname
    char * basename, * dirname;
    {
        int len = strlen(pathname);
        if(pathname[len - 1] == '/')
        {
            pathname[len - 1] = '\0';
            len--;
        }
        for(len; pathname[len] != '/' && len != 0; len--);
        
        if(len == 0)
        {
            dirname = NULL;
            basename = pathname;
        }
        else
        {
            pathname[len] = '\0';
            dirname = pathname;
            basename = pathname + len + 1;
        }
    }

    //check if absolute or relative
    Node * path;
    {
        if(dirname != NULL && dirname[0] == '/')
        {
            dirname = dirname + 1;
            path = ft->root;
        }
        else
        {
            path = ft->cwd;
        }
    }

    //search
    {
        if(dirname != NULL && !(path = searchPath(path, dirname)))
        {
            printf("Invalid path\n");
            return -1;
        }
    }

    //add child
    {
        Node * newNode = makeNode(basename, t);
        newNode->parent = path;
        if(!path->child)
        {
            path->child = newNode;
        }
        else
        {
            path = path->child;
            while(path->sibling != NULL)
                path = path->sibling;
            path->sibling = newNode;
        }
        newNode->parent = path;
    }

    return 0;
}

int creat(FileTree * ft, char * pathname)
{
    return insertNode(ft, pathname, FILENODE);
}

int mkdir(FileTree * ft, char * pathname)
{
    return insertNode(ft, pathname, DIRECTORYNODE);
}

int rmdir(FileTree * ft, char * pathname)
{
    //check if absolute
    Node *path;
    if(pathname[0] == '/')
    {
        pathname = pathname + 1;
        path = ft->root;
    }
    else
    {
        path = ft->cwd;
    }

    //find dir
    if(!(path = searchPath(path, pathname)))
    {
        printf("Invalid path\n");
        return -1;
    }

    else if(path->type != DIRECTORYNODE)
    {
        printf("Path is not a directory\n");
        return -1;
    }

    else if(path->child)
    {
        printf("Directory is not empty\n");
        return -1;
    }

    else
    {
        removeNode(path);
        return 0;
    }
}

int cd(FileTree * ft, char * pathname)
{
    Node* path;
    if(pathname[0] == '/')
    {
        pathname++;
        path = ft->root;
    }
    else
    {
        path = ft->cwd;
    }

    if(pathname[0] == '\0' || (path = searchPath(path, pathname)))
    {
        ft->cwd = path;
        return 0;
    }
    else
    {
        printf("Invalid path\n");
        return -1;
    }

}

int ls(FileTree * ft, char * pathname)
{
    Node * path;
    if(pathname[0] == '/')
    {
        pathname++;
        path = ft->root;
    }
    else
    {
        path = ft->cwd;
    }

    if(pathname[0] == '\0' || (path = searchPath(path, pathname)))
    {
        path = path->child;
        while(path)
        {
            printf("%s\n", path->name);
            path = path->sibling;
        }
        return 0;
    }
    else
    {
        printf("Invalid path\n");
        return -1;
    }
}

int pwdHelper(Node * path, Node * root)
{
    if(path == root)
    {
        return 0;
    }
    pwdHelper(path->parent, root);
    printf("/%s", path->name);
    return 0;
}

int pwd(FileTree * ft)
{
    if(ft->cwd == ft->root)
    {
        printf("/\n");
        return 0;
    }
    int ret =  pwdHelper(ft->cwd, ft->root);
    printf("\n");
    return ret;
}

int rm(FileTree * ft, char * pathname)
{
    Node * path;
    if(pathname[0] == '/')
    {
        pathname = pathname + 1;
        path = ft->root;
    }
    else
    {
        path = ft->cwd;
    }

    if(!(path = searchPath(path, pathname)))
    {
        printf("Invalid path\n");
        return -1;
    }
    else if(path->type != FILENODE)
    {
        printf("Path is not a file\n");
        return -1;
    }
    else
    {
        removeNode(path);
        return 0;
    }
}

void saveHelper(Node * path, FILE * out, char * pathname)
{
    if(!path)
        return;
    fprintf(out, "%d %s%s\n", path->type, pathname, path->name);
    char new_pathname[50];
    strcpy(new_pathname, pathname);
    strcat(new_pathname, path->name);
    if(strcmp(path->name, "/") != 0)
        strcat(new_pathname, "/");
    saveHelper(path->child, out, new_pathname);
    saveHelper(path->sibling, out, pathname);
}

void save(FileTree * ft)
{
    FILE * out = fopen("save.txt", "w+");
    saveHelper(ft->root, out, "");
    fclose(out);
}

void reload(FileTree * ft)
{
    closeFileTree(ft);
    *ft = initFileTree();
    FILE * in = fopen("save.txt", "r");
    if(!in)
        return;
    char line[100];
    char type[10], path[50];
    fgets(line, 100, in);
    while(fgets(line, 100, in))
    {
        sscanf(line, "%s %s", type, path);
        insertNode(ft, path + 1, atoi(type));
    }
}

void closeFileTreeHelper(Node * path)
{
    if(!path)
        return;
    Node * child = path->child, * sibling = path->sibling;
    free(path);
    closeFileTreeHelper(child);
    closeFileTreeHelper(sibling);
}

void closeFileTree(FileTree * ft)
{
    closeFileTreeHelper(ft->root);
}
