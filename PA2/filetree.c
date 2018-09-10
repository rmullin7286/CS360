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
    char * tok = strtok(path, '/');
    do
    {
        if(start->type == FILENODE)
            return NULL;
        start = start->child;
        while(start)
            if(strcmp(start->name, tok))
                break;
        if(!start)
            return NULL;
    }while(tok = strtok(NULL, '/'));
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
        if(dirname[0] == '/')
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
        if(!(path = searchPath(path, dirname)))
        {
            printf("Invalid path");
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
            newNode = path->sibling;
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
    return insertnode(ft, pathname, DIRECTORYNODE);
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
        printf("Invalid path");
        return -1;
    }

    else if(path->type != DIRECTORYNODE)
    {
        printf("Path is not a directory");
        return -1;
    }

    else if(path->child)
    {
        printf("Directory is not empty");
        return -1;
    }

    else
    {
        removeNode(path);
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

    if(path = searchPath(path, pathname))
    {
        ft->cwd = path;
        return 0;
    }
    else
    {
        printf("Invalid path");
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

    if(path = searchPath(path, pathname))
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
        printf("Invalid path");
        return -1;
    }
}

int pwd(FileTree * ft)
{
    char save[100];
    strcpy(save, pat
}