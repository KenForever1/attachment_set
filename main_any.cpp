#include <iostream>
#include "attachment_any.hpp"

using namespace std;
struct A {
    A() { cout << "A" << endl; }
    ~A() { cout << "~A" << endl; }
    void print() const { cout << "a is " << a << endl; }
private:
    int a = 1;
};

struct C {
    C() { cout << "C" << endl; }
    ~C() { cout << "~C" << endl; }
    void print() const { cout << "a is " << a << endl; }
private:
    int a = 1;
    std::string b = "ddd";
};


class B {
public:
    B() { cout << "B" << endl; }
    ~B() { cout << "~B" << endl; }
    
    const ken::AnyAttachmentSet& attachments() const {
        return _attachments;
    }
    
    ken::AnyAttachmentSet& attachments() {
        return _attachments;
    }

private:
    ken::AnyAttachmentSet _attachments;
    float b;
};

int main(){
    std::shared_ptr<A> a_ptr = std::make_shared<A>();
    std::cout << "a ref count : " << a_ptr.use_count() << std::endl;

    // std::shared_ptr<void> aa_ptr =  std::static_pointer_cast<void>(a_ptr);
    std::shared_ptr<void> aa_ptr =  a_ptr;
    std::cout << "a ref count set to void : " << a_ptr.use_count() << std::endl;
    {
        std::shared_ptr<B> b_ptr = std::make_shared<B>();
        {
            b_ptr->attachments().set("A", a_ptr);
            std::cout << "a ref count : " << a_ptr.use_count() << std::endl;
        }
        auto A_ptr = b_ptr->attachments().get<A>("A");
        A_ptr->print();
        std::cout << "attachments size : " << b_ptr->attachments().size() << endl;

        std::shared_ptr<int> int_ptr = std::make_shared<int>(1111);
        b_ptr->attachments().set("B", int_ptr);

        // 遍历所有附件
        for (auto it = b_ptr->attachments().begin(); it != b_ptr->attachments().end(); ++it) {
            if (it.is<int>()) {
                auto ptr = it.get<int>();
                std::cout << "int ptr " << it.name() << ": " << *ptr << std::endl;
            }
        }
    }
    std::cout << "a ref count : " << a_ptr.use_count() << std::endl;

    {
        std::shared_ptr<A> tmp_ptr = a_ptr;
        std::cout << "a ref count : " << a_ptr.use_count() << std::endl;
    }

    std::cout << "a ref count : " << a_ptr.use_count() << std::endl;

    return 0;
}
// A
// a ref count : 1
// a ref count set to void : 2
// B
// a ref count : 3
// a is 1
// attachments size : 1
// int ptr B: 1111
// ~B
// a ref count : 2
// a ref count : 3
// a ref count : 2
// ~A