Codestyle
----------
_This guide is not completed_

#### Tabs  
Do not use tabs. 4 spaces should be used instead of them.

#### Braces  
Braces should be places on the same column. 
```cpp  
if (true)
{
    // Do something
}
```

#### Classes naming  
Classes names should be capitalized.  
```cpp
class FooBar
{
    // Do something
};
```
**Exception:** template classes which mimic sdl classes (e.g., the `id_ptr_on` class) 

#### Class member functions naming  
Class member functions names should be capitalized.  
```cpp
class FooBar
{
public:
    void DoSomething();
};
```
**Exception:** signals and slots   

#### Variables naming
Variables names should be low-cased.  
```cpp
int buttons_amount = 42;
```
Private variables names should be ended with an underline.
```cpp
class FooBar
{
private:
    int value_;
};
```