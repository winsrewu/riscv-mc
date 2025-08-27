function from_num(num, len) {
    let str = "";
    for (let i = 0; i < len; i++) {
        str = (num & 1) + "b," + str;
        num >>= 1;
    }
    return "[" + str.slice(0, -1) + "]";
}

function from_num_to_array(num, len) {
    let arr = [];
    for (let i = 0; i < len; i++) {
        arr.push(num & 1);
        num >>= 1;
    };
    return arr.reverse();
}

function from_num_to_binary_str(num, len) {
    let str = "";
    for (let i = 0; i < len; i++) {
        str = (num & 1) + str;
        num >>= 1;
    }
    return str
}

module.exports = {
    from_num: from_num,
    from_num_to_array: from_num_to_array,
    from_num_to_binary_str: from_num_to_binary_str
};