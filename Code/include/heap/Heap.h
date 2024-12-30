/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Heap.h
 * Author: LTSACH
 *
 * Created on 22 August 2020, 18:18
 */

#ifndef HEAP_H
#define HEAP_H
#include <memory.h>
#include "heap/IHeap.h"
#include <sstream>
#include<iostream>
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 *
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 *
 */
template <class T>
class Heap : public IHeap<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *elements;                            // a dynamic array to contain user's data
    int capacity;                           // size of the dynamic array
    int count;                              // current count of elements stored in this heap
    int (*comparator)(T &lhs, T &rhs);      // see above
    void (*deleteUserData)(Heap<T> *pHeap); // see above

public:
    Heap(int (*comparator)(T &, T &) = 0,
         void (*deleteUserData)(Heap<T> *) = 0);

    Heap(const Heap<T> &heap);               // copy constructor
    Heap<T> &operator=(const Heap<T> &heap); // assignment operator

    ~Heap();

    // Inherit from IHeap: BEGIN
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T) = 0);
    bool contains(T item);
    int size();
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IHeap: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }

    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

public:
    /* if T is pointer type:
     *     pass the address of method "free" to Heap<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  Heap<Point*> heap(&Heap<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(Heap<T> *pHeap)
    {
        for (int idx = 0; idx < pHeap->count; idx++)
            delete pHeap->elements[idx];
    }

private:
    bool aLTb(T &a, T &b)
    {
        return compare(a, b) < 0;
    }
    int compare(T &a, T &b)
    {
        if (comparator != 0)
            return comparator(a, b);
        else
        {
            if (a < b)
                return -1;
            else if (a > b)
                return 1;
            else
                return 0;
        }
    }

    void ensureCapacity(int minCapacity);
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);

    void removeInternalData();
    void copyFrom(const Heap<T> &heap);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////

public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        Heap<T> *heap;
        int cursor;

    public:
        Iterator(Heap<T> *heap = 0, bool begin = 0)
        {
            this->heap = heap;
            if (begin && (heap != 0))
                cursor = 0;
            if (!begin && (heap != 0))
                cursor = heap->size();
        }
        Iterator &operator=(const Iterator &iterator)
        {
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }

        T &operator*()
        {
            return this->heap->elements[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return this->cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            cursor++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
Heap<T>::Heap(
    int (*comparator)(T &, T &),
    void (*deleteUserData)(Heap<T> *))
{
    // YOUR CODE IS HERE
    this->capacity = 10;                    // khởi tạo dung lượng mặc định
    this->count = 0;                        // khởi tạo số lượng phần tử
    this->elements = new T[this->capacity]; // cấp phát bộ nhớ cho mảng phần tử
    this->comparator = comparator;          // gán hàm so sánh
    this->deleteUserData = deleteUserData;  // gán hàm xóa dữ liệu người dùng
}
template <class T>
Heap<T>::Heap(const Heap<T> &heap)
{
    // YOUR CODE IS HERE
    this->capacity = heap.capacity;         // sao chép dung lượng
    this->count = heap.count;               // sao chép số lượng phần tử
    this->elements = new T[this->capacity]; // cấp phát bộ nhớ cho mảng phần tử
    for (int i = 0; i < count; i++)
    {
        this->elements[i] = heap.elements[i]; // sao chép từng phần tử
    }
    this->comparator = heap.comparator;         // sao chép hàm so sánh
    this->deleteUserData = nullptr; // sao chép hàm xóa dữ liệu người dùng
}

template <class T>
Heap<T> &Heap<T>::operator=(const Heap<T> &heap)
{
    if (this != &heap) // kiểm tra tự gán
    {
        this->removeInternalData();
        this->capacity = heap.capacity;         // sao chép dung lượng
        this->count = heap.count;               // sao chép số lượng phần tử
        this->elements = new T[this->capacity]; // cấp phát bộ nhớ cho mảng phần tử
        for (int i = 0; i < count; i++)
        {
            this->elements[i] = heap.elements[i]; // sao chép từng phần tử
        }
        this->comparator = heap.comparator;         // sao chép hàm so sánh
        this->deleteUserData = nullptr; // sao chép hàm xóa dữ liệu người dùng
    }
    return *this; // trả về đối tượng hiện tại
}

template <class T>
Heap<T>::~Heap()
{
    // YOUR CODE IS HERE
    this->removeInternalData();
}

template <class T>
void Heap<T>::push(T item)
{ // item  = 25
    // YOUR CODE IS HERE
    //  Bước 1: Kiểm tra và đảm bảo dung lượng của mảng đủ để chứa phần tử mới
    ensureCapacity(count + 1);

    // Bước 2: Thêm phần tử mới vào cuối mảng
    elements[count] = item;
    count++; // Tăng số lượng phần tử trong heap

    // Bước 3: Thực hiện thao tác "reheap-up" để đảm bảo cấu trúc heap
    reheapUp(count - 1); // Đưa phần tử mới thêm lên vị trí đúng trong heap
}
/*
      18
     /  \
    15   13
    /
  25
 =>
      25
     /  \
    18   13
    /
  15
=> array: [25, 18, 13, 15, , ]
           0   1    2   3

 */
template <class T>
T Heap<T>::pop()
{
    // Kiểm tra nếu heap rỗng
    if (count == 0)
    { 
        throw std::underflow_error("Calling to peek with an empty heap.");
    }

    // Lưu trữ phần tử gốc (phần tử lớn nhất)
    T root = elements[0];

    // Đặt phần tử cuối cùng lên gốc
    elements[0] = elements[this->count - 1];

    // Giảm kích thước heap
    this->count--;

    // Gọi hàm reheapDown để đảm bảo heap vẫn đúng
    reheapDown(0);

    // Trả về phần tử gốc ban đầu
    return root;
}

/*
      15
     /  \
    18   13
 => ReheapDown
      18
     /  \
    15   13
=> Array: [18, 15, 13, , , ]
 */

template <class T>
const T Heap<T>::peek()
{
    // Kiểm tra nếu heap rỗng
    if (count == 0)
    {
        throw std::underflow_error("Calling peek with an empty heap.");
    }
    return elements[0]; // Trả về phần tử lớn nhất (hoặc nhỏ nhất)
}

template <class T>
void Heap<T>::remove(T item, void (*removeItemData)(T))
{
    // YOUR CODE IS HERE
    //  (a) Gọi phương thức getItem để tìm vị trí của phần tử item trong heap
    int foundIdx = getItem(item);

    // Nếu không tìm thấy phần tử, thoát khỏi phương thức
    if (foundIdx == -1)
    {
        return; // hoặc có thể ném ra ngoại lệ nếu cần
    }

    // (b) Thay thế phần tử tại vị trí foundIdx bằng phần tử cuối cùng trong heap
    elements[foundIdx] = elements[count - 1];

    // (c) Giảm số lượng phần tử (count) đi 1
    count--;

    // (d) Gọi phương thức reheapDown từ vị trí foundIdx để khôi phục tính chất của heap
    reheapDown(foundIdx);

    // (e) Nếu con trỏ hàm removeItemData được cung cấp, gọi hàm này để giải phóng bộ nhớ hoặc xử lý dữ liệu của phần tử đã bị xóa
    if (removeItemData != nullptr)
    {
        removeItemData(item);
    }
}

template <class T>
bool Heap<T>::contains(T item)
{
    return getItem(item) != -1; // Nếu getItem trả về -1, thì item không có trong heap
}

template <class T>
int Heap<T>::size()
{
    return count; // Trả về số lượng phần tử hiện có trong heap
}

template <class T>
void Heap<T>::heapify(T array[], int size)
{
    // Đặt lại kích thước hiện tại của heap
    count = 0;

    // Lặp qua từng phần tử trong mảng
    for (int i = 0; i < size; ++i)
    {
        push(array[i]); // Sử dụng phương thức push để thêm phần tử vào heap
    }
}

template <class T>
void Heap<T>::clear()
{
    // Nếu có một hàm để giải phóng dữ liệu người dùng, gọi hàm đó
    if (deleteUserData != nullptr)
    {
        deleteUserData(this); // Giải phóng dữ liệu nếu kiểu T là con trỏ
    }

    // Đặt lại số lượng phần tử trong heap
    count = 0; // Xóa tất cả các phần tử
}

template <class T>
bool Heap<T>::empty()
{
    // YOUR CODE IS HERE
    return this->count == 0;
}

template <class T>
string Heap<T>::toString(string (*item2str)(T &))
{
    stringstream os;
    if (item2str != 0)
    {
        os << "[";
        for (int idx = 0; idx < count - 1; idx++)
            os << item2str(elements[idx]) << ",";
        if (count > 0)
            os << item2str(elements[count - 1]);
        os << "]";
    }
    else
    {
        os << "[";
        for (int idx = 0; idx < count - 1; idx++)
            os << elements[idx] << ",";
        if (count > 0)
            os << elements[count - 1];
        os << "]";
    }
    return os.str();
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
void Heap<T>::ensureCapacity(int minCapacity)
{
    if (minCapacity >= capacity)
    {
        // re-allocate
        int old_capacity = capacity;
        capacity = old_capacity + (old_capacity >> 2);
        try
        {
            T *new_data = new T[capacity];
            // OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity * sizeof(T));
            delete[] elements;
            elements = new_data;
        }
        catch (std::bad_alloc e)
        {
            e.what();
        }
    }
}

template <class T>
void Heap<T>::swap(int a, int b)
{
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template <class T>
void Heap<T>::reheapUp(int position) {
    // Lưu trữ vị trí hiện tại
    int current = position;

    // Tiếp tục cho đến khi không còn phần tử cha
    while (current > 0) {
        // Tính toán chỉ số của phần tử cha
        int parent = (current - 1) / 2;

        // So sánh phần tử hiện tại với phần tử cha sử dụng hàm so sánh
        if (compare( elements[parent],elements[current]) > 0) {
            // Nếu phần tử hiện tại nhỏ hơn cha, hoán đổi chúng
          this->swap(current, parent);
            // Cập nhật vị trí hiện tại để tiếp tục kiểm tra
            current = parent;
        } else {
            // Nếu không, dừng lại vì cấu trúc heap đã được duy trì
            break;
        }
    }
}


template <class T>
void Heap<T>::reheapDown(int position) {
    int leftChild = position * 2 + 1;  // Chỉ số của con trái
    int rightChild = position * 2 + 2; // Chỉ số của con phải
    int largest = position;             // Giả định rằng nút hiện tại là lớn nhất

    // Kiểm tra xem con trái có tồn tại và lớn hơn nút hiện tại không
    if (leftChild < count && compare(elements[leftChild], elements[largest]) < 0) {
        largest = leftChild; // Cập nhật largest thành con trái
    }

    // Kiểm tra xem con phải có tồn tại và lớn hơn largest không
    if (rightChild < count && compare(elements[rightChild], elements[largest]) < 0) {
        largest = rightChild; // Cập nhật largest thành con phải
    }

    // Nếu largest không phải là nút hiện tại
    if (largest != position) {
       swap(position, largest); // Hoán đổi
        reheapDown(largest); // Gọi đệ quy với nút largest
    }
}

// template<class T>
// void Heap<T>::reheapDown(int position) {
//     int largest = position; // Giả định rằng nút hiện tại là lớn nhất
//     int leftChild, rightChild;

//     while (true) {
//         leftChild = 2 * largest + 1; // Chỉ số của con trái
//         rightChild = 2 * largest + 2; // Chỉ số của con phải

//         // Kiểm tra xem con trái có tồn tại và lớn hơn nút hiện tại không
//         if (leftChild < count && aLTb(elements[largest], elements[leftChild])) {
//             largest = leftChild; // Cập nhật largest thành con trái
//         }

//         // Kiểm tra xem con phải có tồn tại và lớn hơn largest không
//         if (rightChild < count && aLTb(elements[largest], elements[rightChild])) {
//             largest = rightChild; // Cập nhật largest thành con phải
//         }

//         // Nếu largest không phải là nút hiện tại, hoán đổi và tiếp tục
//         if (largest != position) {
//             swap(position, largest); // Hoán đổi
//             position = largest; // Cập nhật vị trí để tiếp tục kiểm tra
//         } else {
//             break; // Nếu không có sự thay đổi, thoát khỏi vòng lặp
//         }
//     }
// }

template <class T>
int Heap<T>::getItem(T item)
{
    for (int i = 0; i < count; i++)
    {
        if (elements[i] == item)
        {             // So sánh phần tử tại chỉ số i với item
            return i; // Trả về chỉ số của phần tử nếu tìm thấy
        }
    }
    return -1; // Trả về -1 nếu không tìm thấy phần tử
}

template <class T>
void Heap<T>::removeInternalData()
{
    if (this->deleteUserData != 0)
        deleteUserData(this); // clear users's data if they want
    delete[] elements;
}

template <class T>
void Heap<T>::copyFrom(const Heap<T> &heap)
{
    capacity = heap.capacity;
    count = heap.count;
    elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;

    // Copy items from heap:
    for (int idx = 0; idx < heap.size(); idx++)
    {
        this->elements[idx] = heap.elements[idx];
    }
}

#endif /* HEAP_H */
