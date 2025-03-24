#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parentNode, AVLNode<Key, Value>* node);//Done
    void removeFix(AVLNode<Key, Value>* node, int diff); //Done
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* insertedNode = nullptr;
    AVLNode<Key, Value>* parent = nullptr;
    int8_t parentBalance = 0;

    if (this->root_ == nullptr){
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        insertedNode = static_cast<AVLNode<Key, Value>*>(this->root_);
        insertedNode->setBalance(0);
        return;
    } else {
        AVLNode<Key, Value>* find = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
        if (find != nullptr){
            find->setValue(new_item.second); 
            return;
        } else { 
            AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
            while (temp != nullptr){
                if (new_item.first < temp->getKey()){
                    if (temp->getLeft() == nullptr){
                        temp->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
                        insertedNode = static_cast<AVLNode<Key, Value>*>(temp->getLeft());
                        insertedNode->setBalance(0);
                        parent = static_cast<AVLNode<Key, Value>*>(insertedNode->getParent());
                        parentBalance = parent->getBalance();
                        break;
                    } else {
                        temp = static_cast<AVLNode<Key, Value>*>(temp->getLeft());
                    }
                } else {
                    if (temp->getRight() == nullptr){
                        temp->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
                        insertedNode = static_cast<AVLNode<Key, Value>*>(temp->getRight());
                        insertedNode->setBalance(0);
                        parent = static_cast<AVLNode<Key, Value>*>(insertedNode->getParent());
                        parentBalance = parent->getBalance();
                        break;
                    } else {
                        temp = static_cast<AVLNode<Key, Value>*>(temp->getRight());
                    }
                }
            }
        }  
    }

    if (insertedNode != nullptr){
        if (parentBalance == -1) parent->setBalance(0);
        else if (parentBalance == 1) parent->setBalance(0);
        if (parentBalance == 0){
            if (parent->getLeft() == insertedNode){
                parent->updateBalance(-1);
                insertFix(parent, insertedNode);
            } else if (parent->getRight() == insertedNode){
                parent->updateBalance(1);
                insertFix(parent, insertedNode);
            }
        }
    }
    
    return;
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    if (this->root_ == nullptr) return;
    
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (target == nullptr) return;

    AVLNode<Key, Value>* parent = target->getParent();
    int diff = 0;

    if (parent != nullptr){
        if (target == parent ->getLeft()){
            diff = 1;
        } else if (target == parent->getRight()){
            diff = -1;
        }
    }

    //node has 0 children
    if (target->getLeft() == nullptr && target->getRight() == nullptr){
        if (target == this->root_){
            delete this->root_;
            this->root_ = nullptr;
        } else {
            if (target->getParent()->getLeft() == target){
                target->getParent()->setLeft(nullptr);
            } else {
                target->getParent()->setRight(nullptr);
            }
            delete target;  
        }
        return;
    }

    //node has 2 children
    if (target->getLeft() != nullptr && target->getRight() != nullptr){
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
        nodeSwap(target, pred);
        target = pred;
    } 

    //node has 1 child
    AVLNode<Key, Value>* child = nullptr;
    
    if (target->getLeft() != nullptr){
        child = target->getLeft();
    } else {
        child = target->getRight();
    }

    if (parent == nullptr){
        this->root_ = child;
    } else {
        if (parent->getLeft() == target){
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }

    if (child != nullptr){
        child->setParent(parent);
    }

    delete target;
    target = nullptr;

    if (parent != nullptr) removeFix(parent, diff);

    return;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    if (node == nullptr || node->getRight() == nullptr) return nullptr;

    AVLNode<Key, Value>* temp = node->getRight();
    AVLNode<Key, Value>* originalTempLeft = temp->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(originalTempLeft);
    if (originalTempLeft != nullptr) originalTempLeft->setParent(node);
    temp->setLeft(node);
    node->setParent(temp);

    if (parent == nullptr){
        this->root_ = temp;
        temp->setParent(nullptr);
    } else {
        temp->setParent(parent);
        if (parent->getLeft() == node){
            parent->setLeft(temp);
        } else {
            parent->setRight(temp);
        }
    }

    return temp;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    if (node == nullptr || node->getLeft() == nullptr) return nullptr;

    AVLNode<Key, Value>* temp = node->getLeft();
    AVLNode<Key, Value>* originalTempRight = temp->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(originalTempRight);
    if (originalTempRight != nullptr) originalTempRight->setParent(node);
    temp->setRight(node);
    node->setParent(temp);

    if (parent == nullptr){
        this->root_ = temp;
        temp->setParent(nullptr);
    } else {
        temp->setParent(parent);
        if (parent->getLeft() == node){
            parent->setLeft(temp);
        } else {
            parent->setRight(temp);
        }
    }
    
    return temp;
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    if (node == nullptr) return; 
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = nullptr;
    int newDiff = 0;

    if (parent != nullptr) {
        if (node == parent->getLeft()){
            newDiff = 1;
        } else {
            newDiff = -1;
        }
    }

    //diff is -1
    if (node->getBalance() + diff == -2){
        child = node->getLeft();
        if (child->getBalance() == -1){
            rotateRight(node);
            node->setBalance(0);
            child->setBalance(0);
            removeFix(parent, newDiff);
        } else if (child->getBalance() == 0){
            rotateRight(node);
            node->setBalance(-1);
            child->setBalance(1);
            return;
        } else if (child->getBalance() == 1){
            AVLNode<Key, Value>* grandchild = child->getRight();
            rotateLeft(child);
            rotateRight(node);
            if (grandchild->getBalance() == 1){
                node->setBalance(0);
                child->setBalance(-1);
                grandchild->setBalance(0);
            } else if (grandchild->getBalance() == 0){
                node->setBalance(0);
                child->setBalance(0);
                grandchild->setBalance(0);
            } else if (grandchild->getBalance() == -1){
                node->setBalance(1);
                child->setBalance(0);
                grandchild->setBalance(0);
            }
            removeFix(parent, newDiff);
        }
    } else if (node->getBalance() + diff == 2){
        child = node->getRight();
        if (child->getBalance() == 1){
            rotateLeft(node);
            node->setBalance(0);
            child->setBalance(0);
            removeFix(parent, newDiff);
        } else if (child->getBalance() == 0){
            rotateLeft(node);
            node->setBalance(1);
            child->setBalance(-1);
            return;
        } else if (child->getBalance() == -1){
            AVLNode<Key, Value>* grandchild = child->getLeft();
            rotateRight(child);
            rotateLeft(node);
            if (grandchild->getBalance() == 1){
                node->setBalance(-1);
                child->setBalance(0);
                grandchild->setBalance(0);
            } else if (grandchild->getBalance() == 0){
                node->setBalance(0);
                child->setBalance(0);
                grandchild->setBalance(0);
            } else if (grandchild->getBalance() == -1){
                node->setBalance(0);
                child->setBalance(1);
                grandchild->setBalance(0);
            }
            removeFix(parent, newDiff);
        }
    } else if (node->getBalance() + diff == -1){
        node->setBalance(-1);  
        return;
    } else if (node->getBalance() + diff == 0){
        node->setBalance(0);
        removeFix(parent, newDiff);
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == nullptr || p->getParent() == nullptr) return;
    AVLNode<Key, Value>* g = p->getParent();

    //p is left child of gp
    if (p == g->getLeft()){
        g->updateBalance(-1);
        if (g->getBalance() == 0){
            return;
        } else if (g->getBalance() == -1){
            insertFix(g, p);
        } else if (g->getBalance() == -2){
            if (n == p->getLeft()){
                rotateRight(g);
                g->setBalance(0);
                p->setBalance(0);
            } else {
                int8_t oldBalance = n->getBalance();
                rotateLeft(p);
                rotateRight(g);
                
                if (oldBalance == -1){
                    p->setBalance(0);
                    g->setBalance(1);
                } else if (oldBalance == 0){
                    p->setBalance(0);
                    g->setBalance(0);
                } else if (oldBalance == 1){
                    p->setBalance(-1);
                    g->setBalance(0);
                }

                n->setBalance(0);
            }
        }
    } else if (p == g->getRight()){
        g->updateBalance(1);
        if (g->getBalance() == 0){
            return;
        } else if (g->getBalance() == 1){
            insertFix(g, p);
        } else if (g->getBalance() == 2){
            if (n == p->getRight()){
                rotateLeft(g);
                g->setBalance(0);
                p->setBalance(0);
            } else {
                int8_t oldBalance = n->getBalance();
                rotateRight(p);
                rotateLeft(g);
                
                if (oldBalance == 1){
                    p->setBalance(0);
                    g->setBalance(-1);
                } else if (oldBalance == 0){
                    p->setBalance(0);
                    g->setBalance(0);
                } else if (oldBalance == -1){
                    p->setBalance(1);
                    g->setBalance(0);
                }

                n->setBalance(0);
            }
        }
    }
}

#endif