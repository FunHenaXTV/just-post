/**
 * @param {number[]} nums
 * @param {number} target
 * @return {number[]}
 */
var twoSum = function(nums, target) {
    const hash_map = new Map();
    for (let i = 0; i < nums.length; i++) {
        const to_find = target - nums[i];
        if (hash_map.has(to_find)) {
            return [hash_map.get(to_find), i];
        }

        hash_map.set(nums[i], i);
    }
};
