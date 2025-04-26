#include <iostream>
#include <memory>
#include <iostream>
#include <list>

template<typename T, size_t NodeMaxSize>
struct Node {
    Node<T, NodeMaxSize> *next;
    Node<T, NodeMaxSize> *prev;
    int element_count;
    T *arr;
    std::allocator<T> alloc;

    Node(int n, std::allocator<T> alloc = std::allocator<T>()) {
        next = nullptr;
        prev = nullptr;
        element_count = 0;
        arr = std::allocator_traits<std::allocator<T> >::allocate(alloc, n);
    }
};


template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T> >
class unrolled_list {
public:
    template<bool is_const>
    class Iterator {
        template<bool>
        friend class Iterator;
        friend class unrolled_list;

    private:
        Node<T, NodeMaxSize> *current;
        size_t index;

    public:
        Iterator() : current(nullptr), index(0) {
        }

        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<is_const, const T *, const T *>;
        using reference = std::conditional_t<is_const, const T &, T &>;
        using iterator_concept = std::bidirectional_iterator_tag;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(Node<T, NodeMaxSize> *curr, int ind) : current(curr), index(ind) {
        }

        Iterator(const Iterator &other) {
            current = other.current;
            index = other.index;
        }

        Iterator &operator=(const Iterator &other) {
            if (this != &other) {
                current = other.current;
                index = other.index;
            }
            return *this;
        }

        reference operator*() const {
            if (current == nullptr) {
                throw std::runtime_error("curr == nullptr at *");
            }
            return current->arr[index];
        }

        pointer operator->() {
            if (current == nullptr) {
                throw std::runtime_error("curr == nullptr at ->");
            }
            return &current->arr[index];
        }


        Iterator &operator++() {
            if (current == nullptr) {
                return *this;
            }
            if (index + 1 < current->element_count) {
                ++index;
            } else {
                current = current->next;
                index = 0;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator &operator--() {
            if (current == nullptr) {
                throw("you cant");
            }

            if (index > 0) {
                --index;
            } else {
                current = current->prev;
                if (current) {
                    index = current->element_count - 1;
                } else {
                    index = 0;
                }
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const Iterator &other) const {
            if (current == other.current && index == other.index) {
                return true;
            } else {
                return false;
            }
        }

        bool operator!=(const Iterator &other) const {
            if (current != other.current || index != other.index) {
                return true;
            } else {
                return false;
            }
        }

        template<bool B = is_const, typename = std::enable_if_t<B> >
        Iterator(const Iterator<false> &other)
            : current(other.current), index(other.index) {
        }


        ~Iterator() {
        }
    };

    bool operator==(const unrolled_list &other) const {
        if (curr == other.curr) {
            return true;
        } else {
            return false;
        }
    }

    bool operator!=(const unrolled_list &other) const {
        if (curr != other.curr) {
            return true;
        } else {
            return false;
        }
    }


    unrolled_list() : first(nullptr), last(nullptr), curr(nullptr), total_size(0), node_size(NodeMaxSize),
                      node_alloc() {
    }

    unrolled_list(const unrolled_list &other) {
        node_alloc = allocator_traits::select_on_container_copy_construction(other.node_alloc);
        curr = other.curr;
        first = other.first;
        last = other.last;
        node_size = other.node_size;
        total_size = other.total_size;
    }

    unrolled_list(unrolled_list &&other) {
        curr = other.curr;
        other.curr = nullptr;
        first = other.first;
        other.first = nullptr;
        last = other.last;
        other.last = nullptr;
        node_size = other.node_size;
        other.node_size = 0;
        total_size = other.total_size;
        other.total_size = 0;
    }


    unrolled_list &operator=(const unrolled_list &other) {
        if (this != &other) {
            clear();
            node_alloc = other.node_alloc;
            node_size = other.node_size;
            Node<T, NodeMaxSize> *current = other.first;
            while (current) {
                for (int i = 0; i < current->element_count; ++i) {
                    push_back(current->arr[i]);
                }
                current = current->next;
            }
        }
        return *this;
    }

    unrolled_list &operator=(unrolled_list &&other) noexcept {
        if (*this != other) {
            clear();
            first = other.first;
            last = other.last;
            node_size = other.node_size;
            curr = other.curr;
            total_size = other.total_size;
            node_alloc = std::move(other.node_alloc);
            other.first = nullptr;
            other.last = nullptr;
            other.node_size = 0;
            other.curr = nullptr;
        }
        return *this;
    }

    bool empty() const {
        if (first == nullptr) {
            return 1;
        } else {
            return 0;
        }
    }

    size_t size() const {
        return total_size;
    }

    size_t max_size() const noexcept {
        return std::numeric_limits<size_t>::max() / sizeof(T);
    }


    void swap(unrolled_list &other) {
        std::swap(first, other.first);
        std::swap(total_size, other.total_size);
        std::swap(curr, other.curr);
        std::swap(last, other.last);
        std::swap(node_size, other.node_size);
        std::swap(node_alloc, other.node_alloc);
    }

    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using const_reverse_iterator = Iterator<true>;
    using reverse_iterator = Iterator<false>;
    using allocator_type = Allocator;
    using allocator_traits = std::allocator_traits<Allocator>;
    using node_allocator = typename allocator_traits::template rebind_alloc<Node<T, NodeMaxSize> >;
    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>> >
    unrolled_list(InputIt first, InputIt last, const allocator_type &alloc) : unrolled_list(alloc) {
        while (first != last) {
            push_back(*first);
            ++first;
        }
    }

    iterator begin() {
        return iterator(first, 0);
    }

    const_iterator begin() const {
        return const_iterator(first, 0);
    }

    iterator end() {
        return iterator(nullptr, 0);
    }

    const_iterator end() const {
        return const_iterator(nullptr, 0);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(last, last ? last->element_count - 1 : 0);
    }

    const_reverse_iterator crbegin() const {
        return reverse_iterator(last, last ? last->element_count - 1 : 0);
    }

    reverse_iterator rend() {
        return reverse_iterator(first, first ? -1 : 0);
    }

    const_reverse_iterator crend() const {
        return reverse_iterator(first, first ? -1 : 0);
    }

    const_reverse_iterator rbegin() const {
        return reverse_iterator(last, last ? last->element_count - 1 : 0);
    }

    const_reverse_iterator rend() const {
        return reverse_iterator(first, first ? -1 : 0);
    }

    reference front() {
        return first->arr[0];
    }

    const_reference front() const {
        return first->arr[0];
    }

    reference back() {
        return last->arr[last->element_count - 1];
    }

    const_reference back() const {
        return last->arr[last->element_count - 1];
    }

    explicit unrolled_list(const allocator_type &alloc)
        : node_alloc(alloc), first(nullptr), total_size(0), curr(nullptr), last(nullptr), node_size(0) {
    }

    unrolled_list(unrolled_list &&other, const allocator_type alloc)
        : first(other.first), last(other.last), curr(other.curr), total_size(other.total_size),
          node_size(other.node_size), node_alloc(alloc) {
    }

    unrolled_list(std::initializer_list<T> init_list, const allocator_type &alloc = allocator_type())
        : unrolled_list(alloc) {
        for (const T &elem: init_list) {
            push_back(elem);
        }
    }

    Node<T, NodeMaxSize> *make_node() {
        Node<T, NodeMaxSize> *new_node = std::allocator_traits<node_allocator>::allocate(node_alloc, 1);
        std::allocator_traits<node_allocator>::construct(node_alloc, new_node, NodeMaxSize);
        return new_node;
    }

    void delete_node(Node<T, NodeMaxSize> *node) {
        std::allocator_traits<node_allocator>::destroy(node_alloc, node);
        std::allocator_traits<node_allocator>::deallocate(node_alloc, node, 1);
    }

    allocator_type get_allocator() {
        return node_alloc;
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    unrolled_list(size_type count, const T &value, const Allocator alloc = Allocator()) {
        for (int i = 0; i < count; i++) {
            push_back(value);
        }
    }

    void push_back(T elem) {
        if (first == nullptr) {
            first = make_node();
            last = first;
            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &first->arr[0],
                std::move(elem)
            );
            first->element_count = 1;
            total_size++;
            return;
        }
        if (last->element_count < NodeMaxSize) {
            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &last->arr[last->element_count],
                std::move(elem)
            );
            last->element_count++;
            total_size++;
        } else {
            Node<T, NodeMaxSize> *new_node = make_node();
            const size_t half = NodeMaxSize / 2;

            for (size_t i = 0; i < half; ++i) {
                std::allocator_traits<node_allocator>::construct(
                    node_alloc,
                    &new_node->arr[i],
                    std::move(last->arr[half + i])
                );
            }
            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &new_node->arr[half],
                std::move(elem)
            );
            new_node->element_count = half + 1;
            last->element_count = half;
            new_node->prev = last;
            last->next = new_node;
            last = new_node;
            total_size++;
        }
    }

    iterator insert(const_iterator pos, T value) {
        if (pos == end()) {
            push_back(value);
            return iterator(last, last->element_count - 1);
        }

        Node<T, NodeMaxSize> *curr_node = pos.current;
        size_t local_pos = pos.index;

        if (curr_node->element_count < NodeMaxSize) {
            for (int i = curr_node->element_count; i > static_cast<int>(local_pos); --i) {
                std::allocator_traits<node_allocator>::construct(
                    node_alloc,
                    &curr_node->arr[i],
                    std::move(curr_node->arr[i - 1])
                );
                std::allocator_traits<node_allocator>::destroy(
                    node_alloc,
                    &curr_node->arr[i - 1]
                );
            }

            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &curr_node->arr[local_pos],
                std::move(value)
            );

            curr_node->element_count++;
            total_size++;
            return iterator(curr_node, local_pos);
        } else {
            Node<T, NodeMaxSize> *new_node = make_node();
            const size_t split_pos = NodeMaxSize / 2;
            size_t new_local_pos;

            if (local_pos <= split_pos) {
                for (size_t i = split_pos; i < NodeMaxSize; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &new_node->arr[i - split_pos],
                        std::move(curr_node->arr[i])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &curr_node->arr[i]
                    );
                }

                curr_node->element_count = split_pos;

                for (size_t i = curr_node->element_count; i > local_pos; --i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &curr_node->arr[i],
                        std::move(curr_node->arr[i - 1])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &curr_node->arr[i - 1]
                    );
                }

                std::allocator_traits<node_allocator>::construct(
                    node_alloc,
                    &curr_node->arr[local_pos],
                    std::move(value)
                );

                curr_node->element_count++;
                new_node->element_count = NodeMaxSize - split_pos;
                new_local_pos = local_pos;
            } else {
                for (size_t i = split_pos; i < NodeMaxSize; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &new_node->arr[i - split_pos],
                        std::move(curr_node->arr[i])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &curr_node->arr[i]
                    );
                }

                curr_node->element_count = split_pos;
                new_node->element_count = NodeMaxSize - split_pos;
                size_t insert_pos_in_new = local_pos - split_pos;

                for (size_t i = new_node->element_count; i > insert_pos_in_new; --i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &new_node->arr[i],
                        std::move(new_node->arr[i - 1])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &new_node->arr[i - 1]
                    );
                }

                std::allocator_traits<node_allocator>::construct(
                    node_alloc,
                    &new_node->arr[insert_pos_in_new],
                    std::move(value)
                );

                new_node->element_count++;
                new_local_pos = insert_pos_in_new;
            }

            new_node->next = curr_node->next;
            new_node->prev = curr_node;
            if (curr_node->next) {
                curr_node->next->prev = new_node;
            }
            curr_node->next = new_node;

            if (curr_node == last) {
                last = new_node;
            }

            total_size++;

            return (local_pos <= split_pos)
                       ? iterator(curr_node, new_local_pos)
                       : iterator(new_node, new_local_pos);
        }
    }


    iterator insert(const_iterator pos, int n, const T &value) {
        if (n <= 0) return iterator(pos.current, pos.index);

        Node<T, NodeMaxSize> *current_node = pos.current;
        size_t insert_pos = pos.index;

        if (current_node == nullptr) {
            for (int i = 0; i < n; ++i) {
                push_back(value);
            }
            return iterator(last, last->element_count - 1);
        }

        iterator result(current_node, insert_pos);

        while (n > 0) {
            size_t available = NodeMaxSize - current_node->element_count;
            size_t to_insert = std::min(static_cast<size_t>(n), available);

            if (to_insert > 0) {
                for (int i = current_node->element_count - 1; i >= static_cast<int>(insert_pos); --i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &current_node->arr[i + to_insert],
                        std::move(current_node->arr[i])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &current_node->arr[i]
                    );
                }

                for (size_t i = 0; i < to_insert; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &current_node->arr[insert_pos + i],
                        value
                    );
                }

                current_node->element_count += to_insert;
                total_size += to_insert;
                n -= to_insert;
                insert_pos += to_insert;
            }

            if (n > 0) {
                Node<T, NodeMaxSize> *new_node = make_node();
                size_t elements_to_move = current_node->element_count - insert_pos;

                for (size_t i = 0; i < elements_to_move; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &new_node->arr[i],
                        std::move(current_node->arr[insert_pos + i])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &current_node->arr[insert_pos + i]);
                }
                size_t remaining = std::min(static_cast<size_t>(n), NodeMaxSize);
                for (size_t i = 0; i < remaining; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &new_node->arr[elements_to_move + i],
                        value
                    );
                }

                new_node->element_count = elements_to_move + remaining;
                current_node->element_count = insert_pos;
                total_size += remaining;
                n -= remaining;
                new_node->next = current_node->next;
                new_node->prev = current_node;
                if (current_node->next) {
                    current_node->next->prev = new_node;
                }
                current_node->next = new_node;

                if (current_node == last) {
                    last = new_node;
                }

                current_node = new_node;
                insert_pos = remaining;
            }
        }

        return result;
    }

    iterator erase(const_iterator pos) {
        if (empty() || pos > end()) {
            throw std::runtime_error("empty or out of range");
        }
        Node<T, NodeMaxSize> *current_node = pos.current;
        size_t curr_pos = pos.index;

        std::allocator_traits<node_allocator>::destroy(
            node_alloc,
            &current_node->arr[curr_pos]
        );

        for (size_t i = curr_pos; i < current_node->element_count - 1; ++i) {
            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &current_node->arr[i],
                std::move(current_node->arr[i + 1])
            );
            std::allocator_traits<node_allocator>::destroy(
                node_alloc,
                &current_node->arr[i + 1]
            );
        }
        current_node->element_count--;
        total_size--;
        if (current_node->element_count < NodeMaxSize / 2) {
            Node<T, NodeMaxSize> *next_node = current_node->next;
            if (next_node) {
                size_t needed = (NodeMaxSize / 2 + 1) - current_node->element_count;
                size_t can_take = std::min(needed, next_node->element_count);

                for (size_t i = 0; i < can_take; ++i) {
                    size_t current_idx = current_node->element_count + i;
                    size_t next_idx = i;

                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &current_node->arr[current_idx],
                        std::move(next_node->arr[next_idx])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &next_node->arr[next_idx]
                    );
                }

                current_node->element_count += can_take;
                next_node->element_count -= can_take;

                for (size_t i = 0; i < next_node->element_count; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc,
                        &next_node->arr[i],
                        std::move(next_node->arr[i + can_take])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc,
                        &next_node->arr[i + can_take]
                    );
                }

                if (next_node->element_count < NodeMaxSize / 2) {
                    for (size_t i = 0; i < next_node->element_count; ++i) {
                        size_t current_idx = current_node->element_count + i;

                        std::allocator_traits<node_allocator>::construct(
                            node_alloc,
                            &current_node->arr[current_idx],
                            std::move(next_node->arr[i])
                        );
                        std::allocator_traits<node_allocator>::destroy(
                            node_alloc,
                            &next_node->arr[i]
                        );
                    }

                    current_node->element_count += next_node->element_count;
                    current_node->next = next_node->next;
                    if (next_node->next) {
                        next_node->next->prev = current_node;
                    }
                    delete_node(next_node);
                }
            }
        }
        if (curr_pos < current_node->element_count) {
            return iterator(current_node, curr_pos);
        } else if (current_node->next) {
            return iterator(current_node->next, 0);
        } else {
            return end();
        }
    }


    iterator erase(const_iterator first_pos, const_iterator last_pos) {
        if (first_pos == last_pos) return iterator(first_pos.current, first_pos.index);
        if (empty() || first_pos == end()) return end();

        Node<T, NodeMaxSize> *start_node = first_pos.current;
        size_t start_idx = first_pos.index;
        Node<T, NodeMaxSize> *end_node = last_pos.current;
        size_t end_idx = last_pos.index;
        size_t deleted_count = 0;

        if (start_node == end_node) {
            deleted_count = end_idx - start_idx;

            for (size_t i = start_idx; i < start_node->element_count - deleted_count; ++i) {
                start_node->arr[i] = std::move(start_node->arr[i + deleted_count]);
                std::allocator_traits<node_allocator>::destroy(
                    node_alloc,
                    &start_node->arr[i + deleted_count]
                );
            }
            start_node->element_count -= deleted_count;
            total_size -= deleted_count;

            if (start_node->element_count < NodeMaxSize / 2) {
                if (start_node->next &&
                    start_node->element_count + start_node->next->element_count <= NodeMaxSize) {
                    Node<T, NodeMaxSize> *next_node = start_node->next;

                    for (size_t i = 0; i < next_node->element_count; ++i) {
                        start_node->arr[start_node->element_count + i] =
                                std::move(next_node->arr[i]);
                    }
                    start_node->element_count += next_node->element_count;

                    start_node->next = next_node->next;
                    if (next_node->next) {
                        next_node->next->prev = start_node;
                    }
                    if (next_node == last) {
                        last = start_node;
                    }
                    delete_node(next_node);
                }
            }
            return iterator(start_node, start_idx);
        }

        deleted_count = start_node->element_count - start_idx;
        for (size_t i = start_idx; i < start_node->element_count; ++i) {
            std::allocator_traits<node_allocator>::destroy(
                node_alloc,
                &start_node->arr[i]
            );
        }
        start_node->element_count = start_idx;
        total_size -= deleted_count;

        Node<T, NodeMaxSize> *curr_node = start_node->next;
        while (curr_node != end_node) {
            Node<T, NodeMaxSize> *next_node = curr_node->next;
            total_size -= curr_node->element_count;
            delete_node(curr_node);
            curr_node = next_node;
        }
        start_node->next = end_node;
        end_node->prev = start_node;

        deleted_count = end_idx;
        for (size_t i = 0; i < end_idx; ++i) {
            std::allocator_traits<node_allocator>::destroy(
                node_alloc,
                &end_node->arr[i]
            );
        }

        for (size_t i = 0; i < end_node->element_count - end_idx; ++i) {
            end_node->arr[i] = std::move(end_node->arr[end_idx + i]);
            std::allocator_traits<node_allocator>::destroy(
                node_alloc,
                &end_node->arr[end_idx + i]
            );
        }
        end_node->element_count -= end_idx;
        total_size -= end_idx;


        if (start_node->element_count == 0) {
            Node<T, NodeMaxSize> *prev_node = start_node->prev;
            if (prev_node) {
                prev_node->next = start_node->next;
            }
            if (start_node->next) {
                start_node->next->prev = prev_node;
            }
            if (start_node == first) {
                first = start_node->next;
            }
            delete_node(start_node);
        }

        return iterator(end_node, 0);
    }

    void clear() {
        Node<T, NodeMaxSize> *curr_node = first;
        while (curr_node != nullptr) {
            Node<T, NodeMaxSize> *next_node = curr_node->next;
            std::allocator_traits<node_allocator>::destroy(node_alloc, curr_node);
            std::allocator_traits<node_allocator>::deallocate(node_alloc, curr_node, 1);
            curr_node = next_node;
        }
        first = last = nullptr;
        total_size = 0;
    }


    void pop_back() {
        if (last == nullptr) {
            throw std::runtime_error("list is empty");
        }
        std::allocator_traits<node_allocator>::destroy(node_alloc, &last->arr[last->element_count - 1]);
        last->element_count--;
        total_size--;

        if (last->element_count == 0) {
            Node<T, NodeMaxSize> *prev_node = last->prev;
            delete_node(last);

            if (prev_node) {
                prev_node->next = nullptr;
                last = prev_node;
            } else {
                first = nullptr;
                last = nullptr;
            }
        }
    }

    void push_front(T elem) {
        if (first == nullptr) {
            Node<T, NodeMaxSize> *new_node = make_node();
            try {
                std::allocator_traits<node_allocator>::construct(
                    node_alloc, &new_node->arr[0], std::move(elem)
                );
                new_node->element_count = 1;
                first = last = new_node;
                total_size++;
            } catch (...) {
                delete_node(new_node);
                throw;
            }
            return;
        }

        if (first->element_count < NodeMaxSize) {
            try {
                for (int i = first->element_count; i > 0; --i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc, &first->arr[i], std::move(first->arr[i - 1])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc, &first->arr[i - 1]
                    );
                }
                std::allocator_traits<node_allocator>::construct(
                    node_alloc, &first->arr[0], std::move(elem)
                );
                first->element_count++;
                total_size++;
            } catch (...) {
                for (int i = 1; i <= first->element_count; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc, &first->arr[i - 1], std::move(first->arr[i])
                    );
                    std::allocator_traits<node_allocator>::destroy(
                        node_alloc, &first->arr[i]
                    );
                }
                throw;
            }
        } else {
            Node<T, NodeMaxSize> *new_node = make_node();
            const size_t half = NodeMaxSize / 2;

            try {
                std::allocator_traits<node_allocator>::construct(
                    node_alloc, &new_node->arr[0], std::move(elem)
                );

                for (size_t i = 0; i < half; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc, &new_node->arr[i + 1], std::move(first->arr[i])
                    );
                }
                new_node->element_count = half + 1;

                for (size_t i = 0; i < NodeMaxSize - half; ++i) {
                    std::allocator_traits<node_allocator>::construct(
                        node_alloc, &first->arr[i], std::move(first->arr[half + i])
                    );
                }
                first->element_count = NodeMaxSize - half;
            } catch (...) {
                delete_node(new_node);
                throw;
            }

            new_node->next = first;
            first->prev = new_node;
            first = new_node;
            total_size++;
        }
    }


    void pop_front() {
        if (first == nullptr) {
            throw std::runtime_error("list is empty");
        }

        std::allocator_traits<node_allocator>::destroy(node_alloc, &first->arr[0]);

        for (int i = 1; i < first->element_count; ++i) {
            std::allocator_traits<node_allocator>::construct(
                node_alloc,
                &first->arr[i - 1],
                std::move(first->arr[i])
            );
            std::allocator_traits<node_allocator>::destroy(node_alloc, &first->arr[i]);
        }

        first->element_count--;
        total_size--;

        if (first->element_count == 0) {
            Node<T, NodeMaxSize> *next_node = first->next;
            delete_node(first);

            if (next_node) {
                next_node->prev = nullptr;
                first = next_node;
            } else {
                first = nullptr;
                last = nullptr;
            }
        }
    }


    ~unrolled_list() {
        clear();
    }

    void show() {
        Node<T, NodeMaxSize> *currnt = first;
        while (currnt != nullptr) {
            for (int i = 0; i < currnt->element_count; i++) {
                std::cout << currnt->arr[i] << " ";
            }
            std::cout << " - ";

            currnt = currnt->next;
        }
    }

private:
    Node<T, NodeMaxSize> *first;
    size_t total_size;
    Node<T, NodeMaxSize> *curr;
    Node<T, NodeMaxSize> *last;
    int node_size;
    node_allocator node_alloc;
};



