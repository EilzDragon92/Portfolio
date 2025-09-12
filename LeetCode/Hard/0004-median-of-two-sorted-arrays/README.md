# 4. Median of Two Sorted Arrays

**Description:** https://leetcode.com/problems/median-of-two-sorted-arrays/description/

**Difficulty:** Hard

**Topics:** Array, Binary Search, Divide and Conquer


## Approach

1\. Core Concept: In a sorted array, the median value must have the same number of elements on its left and right, and the elements on the left of the median must be smaller or equal to those on the right of it.

2\. Conduct binary search to find the index `crs0` on `vec0` and `crs1` on `vec1` which meet the conditions specified in 1.


## Result

**Submission:** https://leetcode.com/problems/median-of-two-sorted-arrays/submissions/1759179837/

**Language:** C++

**Runtime:** 0 ms (Beats 100%)

**Memory Usage:** 94.89 MB (Beats 99.58%)

**Time Complexity:** O(log(min(n, m)))

**Space Complexity:** O(1)
