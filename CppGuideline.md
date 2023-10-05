# Naming conventions

## Function arguments
Try to variable with its type name so it is less confusing.
However this rule depend of the context, i.e:
```cpp
// 'Area' is the subject of this member function, it is clear that 'area' argument is the object to test
bool UI::Area::contains(const Area &area) noexcept;

// There is no implicit subject, we must distinguish clearly between the two areas
bool UI::Contains(const Area &parent, const Area &child) noexcept;
```

#

List arguments from most global to most specific