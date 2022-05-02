// Project identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> 
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            Node()
                :child{ nullptr }, sibling{ nullptr }, parent{ nullptr } {}
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE& val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node* parent;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }
    {
        Node();
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } 
    {
        start = start;
        end = end;
        updatePriorities();
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } 
    {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) 
    {
        Node* n = rhs.root;
        if (this != &rhs)
        {
            deleteHeap(n);
            root = copyOver(n);
        }
        numNodes = rhs.numNodes;
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() 
    {
        deleteHeap(root);
        root = nullptr;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() 
    {
        if (numNodes == 0)
        {
            return;
        }

    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This is almost done,
    //              in that you should implement push functionality in the addNode()
    //              function, and this function should call addNode().
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val)
    {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() 
    {
        Node* toBeDeleted = root;
        numNodes = numNodes - 1;
        if (root->child == nullptr)
        {
            root = nullptr;
        }
        else
        {
            root = twoPassMerge(root->child);
        }
        delete toBeDeleted;

    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const
    {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return size_t(numNodes);
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const 
    {
        if (root == nullptr)
        {
            return true;
        }

        else
        {
            return false;
        }
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* *node, const TYPE & new_value)
    {
        //When the node and the root are the same, set their values equal
        if (node == root)
        {
            node->elt = new_value;
        }
        else
        {
            //Loop until the node element is equal to its prev element
            int counter = 0;
            while (node->parent->data[counter]->elt != node->elt)
            {
                counter++;
            }
            node->elt = new_value;
            node->parent->data.erase(node->parent.begin() + 1);
            root = meld(root, node);
        }
 
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val)
    {
        Node* newNode = new Node(val);

        //Adding the first node
        if (root == nullptr)
        {
            root = newNode;
            numNodes++;
        }
        else
        {
            numNodes++;
            //Root has a higher priority
            if (this->compare(val, root->elt))
            {
                newNode->parent = root;
                root->data.push_back(newNode);
            }
            //newNode has a higher priority than root
            else
            {
                data.push_back(root);
                root->prev = newNode;
                root = newNode;
            }
        }
        return newNode;
    } // addNode()


private:
    Node* root = nullptr;
    int numNodes = 0;
    std::vector<TYPE> data;

    //Return a node to the bigger element
    Node* meld(Node* a, Node *b)
    {
        if (a == nullptr)
        {
            return b;
        }

        if (b == nullptr)
        {
            return a;
        }

        //If b has a higher priority that a
        if (this->compare(a->elt, b->elt))
        {
            //Set the parent of a to b because it is higher priority
            a->parent = b;
            //Set b's parent to be null
            b->parent = nullptr;
            return b;
        }
        //If the priority of a is greater than b
        else
        {
            //set a to the parent of b because its greater
            b->parent = a;
            //Make and and b siblings
            a->parent = nullptr;
            return a;
        }
    }

    Node* twoPassMerge(Node* node)
    {
        std::vector <Node*> pass;

        if (node->sibling == nullptr)
        {
            return node;
        }
        int counter = 0, counter2 = 0, counter3 = 0;
        //while node is not null, add it to the vector, set it to null, and then advance node
        for (counter = 0; node != nullptr; counter++)
        {
            pass[counter] = node;
            node->prev->sibling = nullptr;
            node = node->sibling;
        }

        //Iterate by 2 over the vector and combine pairs of nodes
        for (counter2 = 0; counter2 + 1 < counter; counter2 += 2)
        {
            meld(pass[counter2], pass[counter2 + 1]);
        }
        counter3 = counter - 2;
        //Iterate again
        for (counter3; counter2 >= 2; counter -= 2)
        {
            meld(pass[counter3 - 2], pass[counter3]);
        }
        //At the end, all nodes will be mereged together in the first index
        return pass[0];
    }

    void deleteHeap(Node* x)
    {
        if (x != nullptr)
        {
            deleteHeap(x->child);
            deleteHeap(x->sibling);
            delete x;
        }
    }
    Node* copyOver(Node *node)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        Node* newNode = new Node(node->elt);

        for (int i = 0; i < data.size(); i++)
        {
            newNode->data.push_back(node->data[i]);

        }
    }
};


#endif // PAIRINGPQ_H
