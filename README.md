# Speeding up conventional algorithms with multi-threading

The goal of the task was to try speeding up classic sequential algorithms with the help of multi-threading.

## Sequential algorithms used

Sequential algorithms for the following three problems in C++ will be used:
- 0-1 Knapsack Problem (DP) 
- Quicksort O(NlogN)
- Max in array O(N)

## Performance Gains

Bencmark results and analysis of the multi-thread algorithms can be found in [report.pdf](https://github.com/gavjan/shamans/blob/master/report.pdf).

## Implementation

Implementation of the multi-threaded algorithms can be found in `src/adventure.h` inside the class `Adventure` where sequential implementations are in its sublcass `LonesomeAdventure` while the multi-threaded ones are in `TeamAdventure`.
- fucntion `packEggs()` implements 0-1 Knapsack Problem 
- fucntion `arrangeSand()` implements QuickSort
- fucntion `selectBestCrystal()` implements Max in array
