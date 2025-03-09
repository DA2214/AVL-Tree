#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height (Node *root)
{
    if (root == NULL) return 0;
    int left = height(root->left);
    int right = height(root->right);
    if (root->left && root->right && left != right) return -1;
    if (left == -1 || right == -1) return -1;
    return max(left, right) + 1;
}

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == NULL) return true;
    return height(root) != -1;
}

