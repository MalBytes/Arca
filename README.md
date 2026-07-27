# Arca

Arca is a generic C library that provides reusable data structures, algorithms, and utilities for modern C projects.

Instead of rewriting the same containers and helper functions for every project, Arca offers production-quality implementations with a simple, consistent API while staying lightweight and portable.

## Features

Current and planned modules include:

- Dynamic Arrays (Vectors)
- Hash Maps
- Hash Sets
- Linked Lists
- Stack
- Queue
- Deque
- String Views
- Sized Strings
- Arena Allocator
- Generic algorithms and utilities

Designed with:

- Generic `void *` containers
- Consistent APIs
- Minimal overhead
- C23 compatibility
- Cross-platform support

## Example

```c
#include <arca/vector.h>

ArcaVector *numbers = arca_vector_new(sizeof(int), 0);

for (int i = 0; i < 10; ++i)
    arca_vector_push(numbers, &i);

arca_vector_sort(numbers, int_compare);

arca_vector_free(numbers);
```

## Algorithms

Arca uses efficient implementations where appropriate, including:

- Introsort (PDQsort + Heapsort + Insertion Sort)
- Binary Search
- Linear Search

## Project Status

Arca is currently under active development.

### Planned

- [ ] Vector
- [ ] Hash Map
- [ ] Hash Set
- [ ] Linked List
- [ ] Stack
- [ ] Queue
- [ ] Deque
- [ ] Arena Allocator
- [ ] String View
- [ ] Sized String

## Building

```bash
git clone https://github.com/MalBytes/arca.git
cd arca

make
```

## Contributing

Issues, feature requests, and pull requests are always welcome.

## License

Licensed under the [MIT License](LICENSE).