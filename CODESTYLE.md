С++ Codestyle
-------------
_This guide is not completed_

#### Tabs  
Do not use tabs. 4 spaces should be used instead of them.

#### Braces  
Braces should be places on the same column. Braces should be always placed after `if`, `for` and `while`.
```cpp  
if (true)
{
    // Do something
}
while (pointer != nullptr)
{
    pointer = GetNextPointer();
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

#### Overall naming
Avoid short names. No acronyms or abbreviations.
```cpp
int damage = CalculateDamage(armor_);
if (id_ptr_on<Carrot> carrot = GetObject())
{
    return;
}
```
#### Spaces
Spaces are needed after `if`, `while`, `for`. Spaces are needed between function agruments (declaration and definition). Spaces are needed between the binary operator and arguments. Space is not needed when the operation is unary. Spaces are not needed near brackets.
```cpp
while (true)
{
    if (variable)
    {
        return 10 * (variable + 2);
    }
    else
    {
        ++counter;
    }
}
```
