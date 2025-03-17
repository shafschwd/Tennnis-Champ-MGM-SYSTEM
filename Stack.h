// Stack.h
#ifndef STACK_H
#define STACK_H

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

template <typename T>
void Stack<T>::push(const T& value) {
    Node* newNode = new Node(value);
    newNode->next = top;
    top = newNode;
    size++;
}

template <typename T>
T Stack<T>::pop() {
    if (isEmpty()) {
        // Handle empty stack case (could throw an exception)
        T emptyValue;
        return emptyValue;
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
        // Handle empty stack case (could throw an exception)
        T emptyValue;
        return emptyValue;
    }

    return top->data;
}

template <typename T>
void Stack<T>::clear() {
    while (!isEmpty()) {
        pop();
    }
}

#endif