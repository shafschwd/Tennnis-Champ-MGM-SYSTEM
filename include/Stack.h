// Stack.h
#ifndef STACK_H
#define STACK_H

#include <stdexcept>
#include <string>

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;

        // Constructor
        Node(T value) : data(value), next(nullptr) {}
    };

    Node* top;  // Pointer to the top node
    int size;   // Size of the stack

public:
    // Constructor
    Stack() : top(nullptr), size(0) {}

    // Copy constructor
    Stack(const Stack<T>& other);

    // Assignment operator
    Stack<T>& operator=(const Stack<T>& other);

    // Destructor
    ~Stack();

    // Push an element onto the stack
    void push(const T& value);

    // Pop an element from the stack
    T pop();

    // Check if the stack is empty
    bool isEmpty() const;

    // Get the size of the stack
    int getSize() const;

    // Peek at the top element
    T peek() const;

    // Clear the stack
    void clear();
};

// Implementation of Stack methods
template <typename T>
Stack<T>::~Stack() {
    clear();
}

// Copy constructor
template <typename T>
Stack<T>::Stack(const Stack<T>& other) : top(nullptr), size(0) {
    if (other.isEmpty()) {
        return; // Nothing to copy
    }

    // Create a temporary array to hold elements in correct order
    T* elements = new T[other.size];
    int count = 0;

    // Copy elements from other stack to array (in reverse order)
    Node* current = other.top;
    while (current != nullptr) {
        elements[other.size - 1 - count] = current->data;
        current = current->next;
        count++;
    }

    // Push elements into this stack (in correct order)
    for (int i = 0; i < other.size; i++) {
        push(elements[i]);
    }

    delete[] elements;
}

// Assignment operator
template <typename T>
Stack<T>& Stack<T>::operator=(const Stack<T>& other) {
    if (this == &other) {
        return *this; // Self-assignment check
    }

    // Clear current stack
    clear();

    if (other.isEmpty()) {
        return *this; // Nothing to copy
    }

    // Create a temporary array to hold elements in correct order
    T* elements = new T[other.size];
    int count = 0;

    // Copy elements from other stack to array (in reverse order)
    Node* current = other.top;
    while (current != nullptr) {
        elements[other.size - 1 - count] = current->data;
        current = current->next;
        count++;
    }

    // Push elements into this stack (in correct order)
    for (int i = 0; i < other.size; i++) {
        push(elements[i]);
    }

    delete[] elements;
    return *this;
}

template <typename T>
void Stack<T>::push(const T& value) {
    try {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        size++;
    } catch (const std::bad_alloc&) {
        throw std::runtime_error("Memory allocation failed when pushing to stack.");
    }
}

template <typename T>
T Stack<T>::pop() {
    if (isEmpty()) {
        throw std::runtime_error("Cannot pop from an empty stack.");
    }

    Node* temp = top;
    T value = temp->data;
    top = top->next;
    delete temp;
    size--;

    return value;
}

template <typename T>
bool Stack<T>::isEmpty() const {
    return top == nullptr;
}

template <typename T>
int Stack<T>::getSize() const {
    return size;
}

template <typename T>
T Stack<T>::peek() const {
    if (isEmpty()) {
        throw std::runtime_error("Cannot peek at an empty stack.");
    }

    return top->data;
}

template <typename T>
void Stack<T>::clear() {
    try {
        while (!isEmpty()) {
            pop();
        }
    } catch (const std::exception& e) {
        // This should not happen since we check isEmpty() in the loop condition
        // But adding robust error handling just in case
        throw std::runtime_error(std::string("Error while clearing stack: ") + e.what());
    }
}

#endif