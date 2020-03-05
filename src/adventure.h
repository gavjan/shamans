#ifndef SRC_ADVENTURE_H_
#define SRC_ADVENTURE_H_

#include <algorithm>
#include <vector>

#include "../third_party/threadpool/threadpool.h"
#include "./types.h"
#include "./utils.h"

class Adventure {
 public:
  virtual ~Adventure() = default;

  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag &bag) = 0;

  virtual void arrangeSand(std::vector<GrainOfSand> &grains) = 0;

  virtual Crystal selectBestCrystal(std::vector<Crystal> &crystals) = 0;

  virtual int advType() = 0;

  static uint64_t max(uint64_t a, uint64_t b) { return (a > b) ? a : b; }

  static uint64_t min(uint64_t a, uint64_t b) { return a < b ? a : b; }

  static std::vector<GrainOfSand>::iterator partition(
      std::vector<GrainOfSand>::iterator start,
      std::vector<GrainOfSand>::iterator end) {
    auto pivot = std::prev(end, 1);
    auto i = start;
    for (auto j = start; j != pivot; j++)
      if (*j < *pivot) std::swap(*i++, *j);
    std::swap(*i, *pivot);
    return i;
  }
};

class LonesomeAdventure : public Adventure {
 public:
  LonesomeAdventure() {}

  virtual int advType() { return 0; }

  uint64_t knapSack(uint64_t W, std::vector<Egg> &eggs) {
    uint64_t i, w;
    uint64_t n = eggs.size();
    std::vector<std::vector<uint64_t>> K(n + 1, std::vector<uint64_t>(W + 1));
    for (i = 0; i < n + 1; i++)
      for (i = 0; i <= n; i++) {
        for (w = 0; w <= W; w++) {
          if (i == 0)
            K[i][w] = 0;
          else if (eggs[i - 1].getSize() <= w)
            K[i][w] = max(
                eggs[i - 1].getWeight() + K[i - 1][w - eggs[i - 1].getSize()],
                K[i - 1][w]);
          else
            K[i][w] = K[i - 1][w];
        }
      }
    uint64_t ans = K[n][W];
    return ans;
  }

  virtual uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag &bag) {
    return knapSack(bag.getCapacity(), eggs);
  }

  void quickSort(std::vector<GrainOfSand>::iterator start,
                 std::vector<GrainOfSand>::iterator end) {
    if (std::distance(start, end) > 1) {
      auto part = partition(start, end);
      quickSort(start, part);
      quickSort(part + 1, end);
    }
  }

  virtual void arrangeSand(std::vector<GrainOfSand> &grains) {
    quickSort(grains.begin(), grains.end());
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal> &crystals) {
    Crystal max = crystals.front();
    for (auto &&x : crystals)
      if (max < x) {
        max = x;
      }
    return max;
  }
};

class TeamAdventure : public Adventure {
 public:
  virtual int advType() { return numberOfShamans; }

  explicit TeamAdventure(uint64_t numberOfShamansArg)
      : numberOfShamans(numberOfShamansArg) {}

  static void doLine(std::shared_ptr<std::vector<Egg>> eggs,
                     std::shared_ptr<std::vector<std::vector<uint64_t>>> K,
                     uint64_t i, uint64_t w, uint64_t end) {
    for (; w <= end; w++) {
      if (i == 0)
        (*K)[i][w] = 0;
      else if ((*eggs)[i - 1].getSize() <= w)
        (*K)[i][w] = max((*eggs)[i - 1].getWeight() +
                             (*K)[i - 1][w - (*eggs)[i - 1].getSize()],
                         (*K)[i - 1][w]);
      else
        (*K)[i][w] = (*K)[i - 1][w];
    }
  }

  uint64_t knapSack(uint64_t W, std::vector<Egg> &eggs, uint64_t N) {
    if (W < 50)
      N = std::min(N, W / 2);
    else
      N = std::min(N, W);
    N = (N == 0) ? 1 : N;
    ThreadPool p(N);
    uint64_t i;
    uint64_t n = eggs.size();
    if (n == 0) return 0;
    std::shared_ptr<std::vector<Egg>> eggs_ptr =
        std::make_shared<std::vector<Egg>>(eggs);
    std::shared_ptr<std::vector<std::vector<uint64_t>>> K =
        std::make_shared<std::vector<std::vector<uint64_t>>>(
            n + 1, std::vector<uint64_t>(W + 1));
    std::vector<std::future<void>> col(N);
    for (i = 0; i < N; i++) {
      for (uint64_t j = 0; j <= i; j++)
        col[j] = p.enqueue(doLine, eggs_ptr, K, i - j, W * j / N,
                           W * (j + 1) / N - (j != (N - 1)));
      for (uint64_t j = 0; j <= i; j++) col[j].wait();
    }
    for (i = 1; i <= n; i++) {
      for (uint64_t j = N - 1; j <= N - 1; j--)
        if ((i + (N - 1 - j)) <= n)
          col[j] = p.enqueue(doLine, eggs_ptr, K, i + (N - 1 - j), W * j / N,
                             W * (j + 1) / N - (j != (N - 1)));
      for (uint64_t j = N - 1; j <= N - 1; j--)
        if ((i + (N - 1 - j)) <= n) col[j].wait();
    }
    return (*K)[n][W];
  }

  uint64_t packEggs(std::vector<Egg> eggs, BottomlessBag &bag) {
    uint64_t N = min(std::thread::hardware_concurrency(), numberOfShamans);
    return knapSack(bag.getCapacity(), eggs, N);
  }

  static Crystal maxInSegment(std::vector<Crystal>::iterator begin,
                              std::vector<Crystal>::iterator end) {
    Crystal max = *begin;
    for (auto it = begin; it <= end; it++)
      if (max < *it) {
        max = *it;
      }
    return max;
  }

  Crystal maxMT(std::vector<Crystal> &crystals, uint64_t N) {
    N = min(std::thread::hardware_concurrency(), numberOfShamans);
    uint64_t last = crystals.size() - 1;
    std::vector<std::future<Crystal>> res(N);
    ThreadPool p(N);
    auto begin = crystals.begin();
    for (uint64_t i = 0; i < N; i++) {
      res[i] = p.enqueue(maxInSegment, begin + last * i / N,
                         begin + last * (i + 1) / N - (i != (N - 1)));
    }
    Crystal max = crystals.front();
    for (uint64_t i = 0; i < N; i++) res[i].wait();
    Crystal result;
    for (uint64_t i = 0; i < N; i++) {
      result = res[i].get();
      if (max < result) max = result;
    }
    return max;
  }

  virtual Crystal selectBestCrystal(std::vector<Crystal> &crystals) {
    if (crystals.size() <= 1) return crystals.front();
    uint64_t N = min(std::thread::hardware_concurrency(), numberOfShamans);
    return maxMT(crystals, N);
  }

  static void quickSortMT(std::vector<GrainOfSand>::iterator start,
                          std::vector<GrainOfSand>::iterator end,
                          std::shared_ptr<ThreadPool> p, int64_t N) {
    int dist = std::distance(start, end);
    if (dist > 1) {
      auto part = partition(start, end);
      if (N > 0 && dist > 150) {
        std::future<void> res = p->enqueue(quickSortMT, start, part, p, N - 2);
        quickSortMT(part + 1, end, p, N - 2);
        res.wait();
      } else {
        quickSortMT(start, part, p, 0);
        quickSortMT(part + 1, end, p, 0);
      }
    }
  }

  virtual void arrangeSand(std::vector<GrainOfSand> &grains) {
    if (grains.size() <= 1) return;
    int64_t N = min(std::thread::hardware_concurrency(), numberOfShamans);
    N = std::min(N, (int64_t)grains.size());
    std::shared_ptr<ThreadPool> p = std::make_shared<ThreadPool>(N);
    quickSortMT(grains.begin(), grains.end(), p, N - 1);
  }

 private:
  uint64_t numberOfShamans;
};

#endif  // SRC_ADVENTURE_H_
