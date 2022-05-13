# C++ GUIDELINE

## Error handling

### Exceptions
Kube Framework doesn't use exception but made to behave correctly with code that may throw everywhere **but NOT in a destructor**.