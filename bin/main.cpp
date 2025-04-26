#include "lib/unrolled_list.h"
#include <iostream>

int main() {
    unrolled_list<int, 4> list; // Create a list with node capacity of 4 elements

    // Push elements to the back
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    list.push_back(40);
    list.push_back(50); // A new node will be created here

    // Print the list
    std::cout << "After push_back:" << std::endl;
    list.show(); // Expected output: 10 20 30 40 - 50 -

    std::cout << "\n\n";

    // Push an element to the front
    list.push_front(5);
    std::cout << "After push_front 5:" << std::endl;
    list.show(); // Expected output: 5 10 20 30 - 40 50 -

    std::cout << "\n\n";

    // Insert an element into the list
    auto it = list.begin();
    ++it; // Move to the second element
    list.insert(it, 7);
    std::cout << "After inserting 7 at the second position:" << std::endl;
    list.show(); // Expected output: 5 7 10 20 - 30 40 50 -

    std::cout << "\n\n";

    // Erase an element
    it = list.begin();
    ++it; // Move to the second element again
    list.erase(it);
    std::cout << "After erasing the second element:" << std::endl;
    list.show(); // Expected output: 5 10 20 30 - 40 50 -

    std::cout << "\n\n";

    // Pop element from the front
    list.pop_front();
    std::cout << "After pop_front:" << std::endl;
    list.show(); // Expected output: 10 20 30 - 40 50 -

    std::cout << "\n\n";

    // Pop element from the back
    list.pop_back();
    std::cout << "After pop_back:" << std::endl;
    list.show(); // Expected output: 10 20 30 - 40 -

    std::cout << "\n\n";

    return 0;
}

