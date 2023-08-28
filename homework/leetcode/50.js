/**
 * @param {number} x
 * @param {number} n
 * @return {number}
 */
var myPow = function(x, n) {
    if (n === 1) {
        return x;
    } else if (n === -1) {
        return 1 / x;
    } else if (n === 0)  {
        return 1;
    }

    if (n % 2 === 0) {
        const ret = myPow(x, n / 2);
        return ret * ret;
    }

    return x * myPow(x, n - 1);
};
