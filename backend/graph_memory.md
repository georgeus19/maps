# Load graph virtual memory measurements:


| Description   | V/E size | V/E Count | VM size(MB) |
| ------ | ---- | -------- | ----------------------- |
| AdjacencyGraph with `std::unordered_map` as vertices holder | 64/ 56 | 1879000/10914384 | 4956 - 22 - 3045 |
| AdjacencyGraph with`tsl::robin_map` as vertices holder | 64/ 56 | 1879000/10914384 | 5119 - 22 - 3045 |
| AdjacencyGraph with `std::vector` as vertices holder | 72/ 56 | 1879000/10914384 | 4799 - 22 - 3045 |
| AdjacencyGraph with `std::vector` as vertices holder | 64/ 56 | 1879000/10914384 | 4784 - 22 - 3045 |
| AdjacencyGraph with old edges and old vertices in big two vectors | 64/ 56 | 1879000/10914384 | 4326 - 22 - 3045 |
| AdjacencyGraph with edges and vertices in big two vectors | 48/ 48 | 1879000/10914384 | 7320 - 6363 |
| CHSearchGraph with upward and downward graph memory optimization | 48/ 48 | 1879000/10914384 | 6820 - 6363 |








