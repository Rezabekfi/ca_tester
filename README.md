# ca tester


This app is designed to test and have fun with CA (cellular automata) rules. This app was developed as a supporting project for my bachelor thesis which might in the future be linked here.

## Strucuture

If you want to use this app in your own project, all you need to import is the `core` package. This package contains all the logic for creating and simulating cellular automata. 

## Adding new rules

To add new rules to the app you need to create a new class that implements the `Rule` interface and also add it to the `RuleFactory` class, which you can do by adding this line to your rule.

```cpp
inline static AutoRegisterRule<YourClass> auto_register{"YourClassName", "Description describing your class"};
```
