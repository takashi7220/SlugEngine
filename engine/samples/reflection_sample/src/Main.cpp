#include <rttr/registration>
#include <iostream>
struct MyStruct
{
    MyStruct(){};
    void func(double){};
    int data = 0;
};

RTTR_REGISTRATION
{
    rttr::registration::class_<MyStruct>("MyStruct")
         .constructor<>()
         .property("data", &MyStruct::data)
         .method("func", &MyStruct::func);
}

int main()
{
    return 0;
}
