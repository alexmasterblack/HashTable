#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <algorithm>

namespace fefu
{
template<typename T>
class allocator {
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = typename std::add_lvalue_reference<T>::type;
    using const_reference = typename std::add_lvalue_reference<const T>::type;
    using value_type = T;

    allocator() noexcept = default;
	
    allocator(const allocator&) noexcept = default;
	
    template <class U>
    allocator(const allocator<U>&) noexcept {};
	
    ~allocator() = default;

    //выделяю память
    pointer allocate(size_type n = 1) {
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }
	//освобождаю память
    void deallocate(pointer p, size_type n) noexcept {
        ::operator delete(p, n);
    }
};

template<typename ValueType>
class hash_map_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using reference = ValueType&;
    using pointer = ValueType*;

    hash_map_iterator() noexcept = default;

    hash_map_iterator(pointer data, std::vector<int> status_table, size_t position) : data(data), status_table(status_table), position(position) {}

    hash_map_iterator(const hash_map_iterator& other) noexcept : data(other.data), status_table(other.status_table), position(other.position) {}

    reference operator*() const {
        //смещение для элемента, имя массива - это указатель на адрес его первого элемента
        return *(data + position);
    }

    pointer operator->() const {
        return data + position;
    }

    // prefix ++
    hash_map_iterator& operator++() {
        //конец таблица
        if (position == status_table.size()) {
            //
            //throw std::runtime_error("cannot increment end map iterator");
            return *this;
            //break;
        }
        //ищем корзину с элементом
        while (position != status_table.size() && status_table[position] == 0) {
            position ++;
        }
        return *this;
    }
    // postfix ++
    hash_map_iterator operator++(int) {
        //конец таблицы
        if (position == status_table.size()) {
            //break;
            throw std::runtime_error("cannot increment end map iterator");
        }
        //ищем корзину с элементом
        while (position != status_table.size() && status_table[position] == 0) {
            ++position;
        }
        return *this;
    }

    friend bool operator==(const hash_map_iterator<ValueType>& our, const hash_map_iterator<ValueType>& other) {
        return our.data == other.data && our.status_table == other.status_table && our.position == other.position;
    }

    friend bool operator!=(const hash_map_iterator<ValueType>& our, const hash_map_iterator<ValueType>& other) {
        return !(our == other);
    }
private:
    size_t position = 0;
    pointer data;
    std::vector<int> status_table;
};

template<typename ValueType>
class hash_map_const_iterator {
// Shouldn't give non const references on value
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using reference = const ValueType&;
    using pointer = const ValueType*;

    hash_map_const_iterator() noexcept = default;

    hash_map_const_iterator(const pointer data, const std::vector<int> status_table, const size_t position) : data(data), status_table(status_table), position(position) {}

    hash_map_const_iterator(const hash_map_const_iterator& other) noexcept : data(other.data), status_table(other.status_table), position(other.position) {}

    hash_map_const_iterator(const hash_map_iterator<ValueType>& other) noexcept : data(other.data), status_table(other.status_table), position(other.position) {}

    reference operator*() const {
        return *(data + position);
    }

    pointer operator->() const {
        return data + position;
    }

    // prefix ++
    hash_map_const_iterator& operator++() {
        //конец таблица
        if (position == status_table.size()) {
            //
            break;
        }
        //ищем корзину с элементом
        while (position != status_table.size() && status_table[position] == 0) {
            position++;
        }
        return *this;
    }
    // postfix ++
    hash_map_const_iterator operator++(int) {
        return ++(*this);
    }

    friend bool operator==(const hash_map_const_iterator<ValueType>& our, const hash_map_const_iterator<ValueType>& other) {
        return our.data == other.data && our.status_table == other.status_table && our.position == other.position;
    }

    friend bool operator!=(const hash_map_const_iterator<ValueType>& our, const hash_map_const_iterator<ValueType>& other) {
        return !(our == other);
    }
private:
    size_t position = 0;
    pointer data;
    const std::vector<int> status_table;
};
template<typename K, typename T,
	   typename Hash = std::hash<K>,
	   typename Pred = std::equal_to<K>,
	   typename Alloc = allocator<std::pair<const K, T>>>
class hash_map
{
public:
    using key_type = K;
    using mapped_type = T;
    using hasher = Hash;
    using key_equal = Pred;
    using allocator_type = Alloc;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = hash_map_iterator<value_type>;
    using const_iterator = hash_map_const_iterator<value_type>;
    using size_type = std::size_t;

    /// Default constructor.
    hash_map() : data(nullptr), status_table(0), count_buckets(0) {}

    /**
     *  @brief  Default constructor creates no elements.
     *  @param n  Minimal initial number of buckets.
     */

    explicit hash_map(size_type n) : data(alloc.allocate(n)), status_table(n), count_buckets(n) {}


    /**
     *  @brief  Builds an %hash_map from a range.
     *  @param  first  An input iterator.
     *  @param  last  An input iterator.
     *  @param  n  Minimal initial number of buckets.
     *
     *  Create an %hash_map consisting of copies of the elements from
     *  [first,last).  This is linear in N (where N is
     *  distance(first,last)).
     */
    template<typename InputIterator>
    hash_map(InputIterator first, InputIterator last, size_type n = 0) : data(alloc.allocate(n)), status_table(n), count_buckets(n) {
        insert(first, last);
    }

    /// Copy constructor.
    hash_map(const hash_map& other) : data(other.data), status_table(other.status_table),
        count_buckets(other.count_buckets), non_empty_buckets(other.non_empty_buckets), alloc(alloc), hash(other.hash), equal(equal), max_load(other.max_load) {
        for (size_type i = 0; i < count_buckets; i++) {
            if (status_table[i] != 0)
                insert(std::make_pair(other.data[i].first, other.data[i].second));
        }
    }

    /// Move constructor.
    hash_map(hash_map&& other) : hash_map() {
        swap(other);
    }

    /**
     *  @brief Creates an %hash_map with no elements.
     *  @param a An allocator object.
     */
    explicit hash_map(const allocator_type& a) {

    }

    /*
    *  @brief Copy constructor with allocator argument.
    * @param  uset  Input %hash_map to copy.
    * @param  a  An allocator object.
    */
    hash_map(const hash_map& umap, const allocator_type& a) {

    }

    /*
    *  @brief  Move constructor with allocator argument.
    *  @param  uset Input %hash_map to move.
    *  @param  a    An allocator object.
    */
    hash_map(hash_map&& umap, const allocator_type& a) {

    }

    /**
     *  @brief  Builds an %hash_map from an initializer_list.
     *  @param  l  An initializer_list.
     *  @param n  Minimal initial number of buckets.
     *
     *  Create an %hash_map consisting of copies of the elements in the
     *  list. This is linear in N (where N is @a l.size()).
     */
    //hash_map(std::initializer_list<value_type> l, size_type n = 0) : data(alloc.allocate(n)), status_table(n), count_buckets(n) {
    //    insert(l);
    //}
    hash_map(std::initializer_list<value_type> l, size_type n = 0) : hash_map((n != 0) ? n : (l.size() * 2)) {
        insert(l);
    }

    /// Copy assignment operator.
    hash_map& operator=(const hash_map& other) {
        hash_map support(other);
        swap(support);
        return *this;
    }

    /// Move assignment operator.
    hash_map& operator=(hash_map&&) {

    }

    /**
     *  @brief  %hash_map list assignment operator.
     *  @param  l  An initializer_list.
     *
     *  This function fills an %hash_map with copies of the elements in
     *  the initializer list @a l.
     *
     *  Note that the assignment completely changes the %hash_map and
     *  that the resulting %hash_map's size is the same as the number
     *  of elements assigned.
     */
    hash_map& operator=(std::initializer_list<value_type> l) {

    }

    ///  Returns the allocator object used by the %hash_map.
    allocator_type get_allocator() const noexcept {
        return alloc;
    }

    // size and capacity:

    ///  Returns true if the %hash_map is empty.
    bool empty() const noexcept {
        return non_empty_buckets == 0;
    }

    ///  Returns the size of the %hash_map.
    size_type size() const noexcept {
        return non_empty_buckets;
    }

    ///  Returns the maximum size of the %hash_map.
    size_type max_size() const noexcept {
        //возвращает предел типа
        return std::numeric_limits<size_type>::max();
    }

    // iterators.

    /**
     *  Returns a read/write iterator that points to the first element in the
     *  %hash_map.
     */
    iterator begin() noexcept {
        //первый элемент, у которого в статусе единица, искомый
        for (size_type i = 0; i < status_table.size(); i++) {
            if (status_table[i] == 1) {
                return iterator(data, status_table, i);
            }
        }
        return iterator(data, status_table, status_table.size());
    }

    //@{
    /**
     *  Returns a read-only (constant) iterator that points to the first
     *  element in the %hash_map.
     */
    const_iterator begin() const noexcept {
        for (size_type i = 0; i < status_table.size(); i++) {
            if (status_table[i] == 1) {
                return const_iterator(data, status_table, i);
            }
        }
        return cend();
    }

    const_iterator cbegin() const noexcept {
        for (size_type i = 0; i < status_table.size(); i++) {
            if (status_table[i] == 1) {
                return const_iterator(data, status_table, i);
            }
        }
        return cend();
    }

    /**
     *  Returns a read/write iterator that points one past the last element in
     *  the %hash_map.
     */
    iterator end() noexcept {
        return iterator(data, status_table, status_table.size());
    }

    //@{
    /**
     *  Returns a read-only (constant) iterator that points one past the last
     *  element in the %hash_map.
     */
    const_iterator end() const noexcept {
        return cend();
    }

    const_iterator cend() const noexcept {
        return const_iterator(data, status_table, status_table.size());
    }
    //@}

    // modifiers.

    /**
     *  @brief Attempts to build and insert a std::pair into the
     *  %hash_map.
     *
     *  @param args  Arguments used to generate a new pair instance (see
     *	        std::piecewise_contruct for passing arguments to each
    *	        part of the pair constructor).
    *
    *  @return  A pair, of which the first element is an iterator that points
    *           to the possibly inserted pair, and the second is a bool that
    *           is true if the pair was actually inserted.
    *
    *  This function attempts to build and insert a (key, value) %pair into
    *  the %hash_map.
    *  An %hash_map relies on unique keys and thus a %pair is only
    *  inserted if its first element (the key) is not already present in the
    *  %hash_map.
    *
    *  Insertion requires amortized constant time.
    */
    template<typename... _Args>
    std::pair<iterator, bool> emplace(_Args&&... args) {

    }

    /**
     *  @brief Attempts to build and insert a std::pair into the
     *  %hash_map.
     *
     *  @param k    Key to use for finding a possibly existing pair in
     *                the hash_map.
     *  @param args  Arguments used to generate the .second for a
     *                new pair instance.
     *
     *  @return  A pair, of which the first element is an iterator that points
     *           to the possibly inserted pair, and the second is a bool that
     *           is true if the pair was actually inserted.
     *
     *  This function attempts to build and insert a (key, value) %pair into
     *  the %hash_map.
     *  An %hash_map relies on unique keys and thus a %pair is only
     *  inserted if its first element (the key) is not already present in the
     *  %hash_map.
     *  If a %pair is not inserted, this function has no effect.
     *
     *  Insertion requires amortized constant time.
     */
    template <typename... _Args>
    std::pair<iterator, bool> try_emplace(const key_type& k, _Args&&... args) {

    }

    // move-capable overload
    template <typename... _Args>
    std::pair<iterator, bool> try_emplace(key_type&& k, _Args&&... args) {

    }

    //@{
    /**
     *  @brief Attempts to insert a std::pair into the %hash_map.
     *  @param x Pair to be inserted (see std::make_pair for easy
     *	     creation of pairs).
    *
    *  @return  A pair, of which the first element is an iterator that
    *           points to the possibly inserted pair, and the second is
    *           a bool that is true if the pair was actually inserted.
    *
    *  This function attempts to insert a (key, value) %pair into the
    *  %hash_map. An %hash_map relies on unique keys and thus a
    *  %pair is only inserted if its first element (the key) is not already
    *  present in the %hash_map.
    *
    *  Insertion requires amortized constant time.
    */
    std::pair<iterator, bool> insert(const value_type& x) {
        iterator element = find(x.first);
        if (element != end()) {
            return std::make_pair(element, false);
        }
        else {
            size_type index = hash(x.first) % count_buckets;
            while (status_table[index] == 1) {
                index = (index + hash(x.first)) % count_buckets;
            }
            if (status_table[index] == 0) {
                new (data + index) value_type(x);
                status_table[index] = 1;
                non_empty_buckets++;
            }
            if (load_factor() >= max_load) {
                rehash(2 * count_buckets);
                return std::make_pair(find(x.first), true);
            } else return std::make_pair(iterator(data, status_table, index), true);
        }
    }

    std::pair<iterator, bool> insert(value_type&& x) {
        iterator element = find(x.first);
        if (element != end()) {
            return std::make_pair(element, false);
        }
        else {
            size_type index = hash(x.first) % count_buckets;
            while (status_table[index] == 1) {
                index = (index + hash(x.first)) % count_buckets;
            }
            if (status_table[index] == 0) {
                //placement new, создание объекта в памяти
                new (data + index) value_type(x);
                status_table[index] = 1;
                non_empty_buckets++;
            }
            if (load_factor() >= max_load) {
                rehash(2 * count_buckets);
                return std::make_pair(find(x.first), true);
            }
            else return std::make_pair(iterator(data, status_table, index), true);
        }
    }

    //@}

    /**
     *  @brief A template function that attempts to insert a range of
     *  elements.
     *  @param  first  Iterator pointing to the start of the range to be
     *                   inserted.
     *  @param  last  Iterator pointing to the end of the range.
     *
     *  Complexity similar to that of the range constructor.
     */
    template<typename _InputIterator>
    void insert(_InputIterator first, _InputIterator last) {
        for (auto i = first; i != last; i++) {
            insert(*i);
        }
    }

    /**
     *  @brief Attempts to insert a list of elements into the %hash_map.
     *  @param  l  A std::initializer_list<value_type> of elements
     *               to be inserted.
     *
     *  Complexity similar to that of the range constructor.
     */
    void insert(std::initializer_list<value_type> l) {
        insert(l.begin(), l.end());
    }


    /**
     *  @brief Attempts to insert a std::pair into the %hash_map.
     *  @param k    Key to use for finding a possibly existing pair in
     *                the map.
     *  @param obj  Argument used to generate the .second for a pair
     *                instance.
     *
     *  @return  A pair, of which the first element is an iterator that
     *           points to the possibly inserted pair, and the second is
     *           a bool that is true if the pair was actually inserted.
     *
     *  This function attempts to insert a (key, value) %pair into the
     *  %hash_map. An %hash_map relies on unique keys and thus a
     *  %pair is only inserted if its first element (the key) is not already
     *  present in the %hash_map.
     *  If the %pair was already in the %hash_map, the .second of
     *  the %pair is assigned from obj.
     *
     *  Insertion requires amortized constant time.
     */
    template <typename _Obj>
    std::pair<iterator, bool> insert_or_assign(const key_type& k, _Obj&& obj) {

    }

    // move-capable overload
    template <typename _Obj>
    std::pair<iterator, bool> insert_or_assign(key_type&& k, _Obj&& obj) {

    }

    //@{
    /**
     *  @brief Erases an element from an %hash_map.
     *  @param  position  An iterator pointing to the element to be erased.
     *  @return An iterator pointing to the element immediately following
     *          @a position prior to the element being erased. If no such
     *          element exists, end() is returned.
     *
     *  This function erases an element, pointed to by the given iterator,
     *  from an %hash_map.
     *  Note that this function only erases the element, and that if the
     *  element is itself a pointer, the pointed-to memory is not touched in
     *  any way.  Managing the pointer is the user's responsibility.
     */
    iterator erase(const_iterator position) {
        size_type index = position.position;
        if (index != status_table.size() && status_table[index] == 1) {
            non_empty_buckets--;
            status_table[index] == -1;
            return position++;
        }
        else {
            return end();
        }
    }

    // LWG 2059.
    iterator erase(iterator position) {
        size_type index = position.position;
        if (index != status_table.size() && status_table[index] == 1) {
            non_empty_buckets--;
            status_table[index] == -1;
            return position++;
        }
        else {
            return end();
        }
    }
    //@}

    /**
     *  @brief Erases elements according to the provided key.
     *  @param  x  Key of element to be erased.
     *  @return  The number of elements erased.
     *
     *  This function erases all the elements located by the given key from
     *  an %hash_map. For an %hash_map the result of this function
     *  can only be 0 (not present) or 1 (present).
     *  Note that this function only erases the element, and that if the
     *  element is itself a pointer, the pointed-to memory is not touched in
     *  any way.  Managing the pointer is the user's responsibility.
     */
    size_type erase(const key_type& x) {
        auto element = find(x);
        if (element != end()) {

        } else 
    }

    /**
     *  @brief Erases a [first,last) range of elements from an
     *  %hash_map.
     *  @param  first  Iterator pointing to the start of the range to be
     *                  erased.
     *  @param last  Iterator pointing to the end of the range to
     *                be erased.
     *  @return The iterator @a last.
     *
     *  This function erases a sequence of elements from an %hash_map.
     *  Note that this function only erases the elements, and that if
     *  the element is itself a pointer, the pointed-to memory is not touched
     *  in any way.  Managing the pointer is the user's responsibility.
     */
    iterator erase(const_iterator first, const_iterator last) {

    }

    /**
     *  Erases all elements in an %hash_map.
     *  Note that this function only erases the elements, and that if the
     *  elements themselves are pointers, the pointed-to memory is not touched
     *  in any way.  Managing the pointer is the user's responsibility.
     */
    void clear() noexcept {
        status_table.assign(bucket_count, 0);
        non_empty_buckets = 0;
    }

    /**
     *  @brief  Swaps data with another %hash_map.
     *  @param  x  An %hash_map of the same element and allocator
     *  types.
     *
     *  This exchanges the elements between two %hash_map in constant
     *  time.
     *  Note that the global std::swap() function is specialized such that
     *  std::swap(m1,m2) will feed to this function.
     */
    void swap(hash_map& other) {
        using std::swap;
        swap(data, other.data);
        swap(alloc, other.alloc);
        swap(status_table, other.status_table);
        swap(non_empty_buckets, other.non_empty_buckets);
        swap(max_load, other.max_load);
        swap(equal, other.equal);
        swap(hash, other.hash);
    }

    template<typename _H2, typename _P2>
    void merge(hash_map<K, T, _H2, _P2, Alloc>& source) {

    }

    template<typename _H2, typename _P2>
    void merge(hash_map<K, T, _H2, _P2, Alloc>&& source) {

    }

    // observers.

    ///  Returns the hash functor object with which the %hash_map was
    ///  constructed.
    Hash hash_function() const {
        return hash;
    }

    ///  Returns the key comparison object with which the %hash_map was
    ///  constructed.
    Pred key_eq() const {
        return equal;
    }

    // lookup.

    //@{
    /**
     *  @brief Tries to locate an element in an %hash_map.
     *  @param  x  Key to be located.
     *  @return  Iterator pointing to sought-after element, or end() if not
     *           found.
     *
     *  This function takes a key and tries to locate the element with which
     *  the key matches.  If successful the function returns an iterator
     *  pointing to the sought after element.  If unsuccessful it returns the
     *  past-the-end ( @c end() ) iterator.
     */
    iterator find(const key_type& x) {
        size_type index = hash(x) % count_buckets;
        //допустим произошла коллизия и элемент оказался в другом месте, ищем
        while (data[index].first != x && status_table[index] != 0) {
            index = (index + hash(x)) % count_buckets;
        }
        //если по этому индексу лежит нужный элемент и в статусе его место равно 1
        if (data[index].first == x && status_table[index] == 1) {
            return iterator(data, status_table, index);
        } else return iterator(data, status_table, count_buckets);
    }

    const_iterator find(const key_type& x) const {
        size_type index = hash(x) % count_buckets;
        //допустим произошла коллизия и элемент оказался в другом месте, ищем
        while (data[index].first != x && status_table[index] != 0) {
            index = (index + hash(x)) % count_buckets;
        }
        //если по этому индексу лежит нужный элемент и в статусе его место равно 1
        if (data[index].first == x && status_table[index] == 1) {
            return const_iterator(data, status_table, index);
        }
        else return cend();
    }
    //@}

    /**
     *  @brief  Finds the number of elements.
     *  @param  x  Key to count.
     *  @return  Number of elements with specified key.
     *
     *  This function only makes sense for %unordered_multimap; for
     *  %hash_map the result will either be 0 (not present) or 1
     *  (present).
     */
    size_type count(const key_type& x) const {
        return (find(x) != end()) ? 1 : 0;
    }

    /**
     *  @brief  Finds whether an element with the given key exists.
     *  @param  x  Key of elements to be located.
     *  @return  True if there is any element with the specified key.
     */
    bool contains(const key_type& x) const {
        return count(x);
    }

    //@{
    /**
     *  @brief  Subscript ( @c [] ) access to %hash_map data.
     *  @param  k  The key for which data should be retrieved.
     *  @return  A reference to the data of the (key,data) %pair.
     *
     *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
     *  data associated with the key specified in subscript.  If the key does
     *  not exist, a pair with that key is created using default values, which
     *  is then returned.
     *
     *  Lookup requires constant time.
     */
    mapped_type& operator[](const key_type& k) {
        auto element = find(k);
        if (element == end()) {
            insert(std::make_pair(k, mapped_type()));
        }
        return at(k);
    }

    mapped_type& operator[](key_type&& k) {
        auto element = find(k);
        if (element == end()) {
            insert(std::make_pair(k, mapped_type()));
        }
        return at(k);
    }
    //@}

    //@{
    /**
     *  @brief  Access to %hash_map data.
     *  @param  k  The key for which data should be retrieved.
     *  @return  A reference to the data whose key is equal to @a k, if
     *           such a data is present in the %hash_map.
     *  @throw  std::out_of_range  If no such data is present.
     */
    mapped_type& at(const key_type& k) {
        auto element = find(k);
        if (element != end()) {
            return element->second;
        }
        else throw std::out_of_range("Out of range");
    }

    const mapped_type& at(const key_type& k) const {
        auto element = find(k);
        if (element != end()) {
            return element->second;
        }
        else throw std::out_of_range("Out of range");
    }
    //@}

    // bucket interface.

    /// Returns the number of buckets of the %hash_map.
    size_type bucket_count() const noexcept {
        return count_buckets;
    }

    /*
    * @brief  Returns the bucket index of a given element.
    * @param  _K  A key instance.
    * @return  The key bucket index.
    */
    size_type bucket(const key_type& _K) const {
        auto element = find(_K);
        if (element != end()) {
            return element.position;
        }
        //мейби ошибку кинуть
        else return -1;
    }

    // hash policy.

    /// Returns the average number of elements per bucket.
    float load_factor() const noexcept {
        if (count_buckets == 0) {
            return 0;
        } else return static_cast<float>(non_empty_buckets / count_buckets);
    }

    /// Returns a positive number that the %hash_map tries to keep the
    /// load factor less than or equal to.
    float max_load_factor() const noexcept {
        return max_load;
    }

    /**
     *  @brief  Change the %hash_map maximum load factor.
     *  @param  z The new maximum load factor.
     */
    void max_load_factor(float z) {
        max_load = z;
    }

    /**
     *  @brief  May rehash the %hash_map.
     *  @param  n The new number of buckets.
     *
     *  Rehash will occur only if the new number of buckets respect the
     *  %hash_map maximum load factor.
     */
    void rehash(size_type n) {
        hash_map new_table(n);
        new_table.insert(begin(), end());
        //или своп
        *this = new_table;
    }

    /**
     *  @brief  Prepare the %hash_map for a specified number of
     *          elements.
     *  @param  n Number of elements required.
     *
     *  Same as rehash(ceil(n / max_load_factor())).
     */
    void reserve(size_type n) {
        rehash(round_up(ceil(n / max_load_factor())));
    }

    bool operator==(const hash_map& other) const {
        //if (alloc != other.alloc) {
        //    return false;
        //}
        if (count_buckets != other.count_buckets) {
            return false;
        }
        for (size_type i = 0; i < count_buckets; i++) {
            if (status_table[i] != other.status_table[i]) {
                return false;
            }
            //должны лежать одинаковые элементы
            if (status_table[i] == 1 && other.status_table[i] == status_table[i]) {
                if (!equal(data[i].first, other.data[i].first)) {
                    return false;
                }
                if (!equal(data[i].second, other.data[i].second)) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    //сам контейнер будет здесь
    value_type* data;
    //информация о состоянии таблицы
    std::vector<int> status_table;
    //выделение памяти
    allocator_type alloc;
    //сравнение
    key_equal equal;
    //хеш
    hasher hash;
    //число корзин
    size_type count_buckets = 0;
    //число занятых корзин
    size_type non_empty_buckets = 0;
    //максимум загруженности таблицы
    float max_load = 0.75;
};
} // namespace fefu
