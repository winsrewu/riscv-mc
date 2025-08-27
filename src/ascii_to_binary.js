function char_to_num(char) {
    return char.charAt(0).charCodeAt();
}

function get_corresponding_char(num) {
    return String.fromCharCode(num);
}

function get_should_print(num) {
    let c = num;
    if (c == 9) return "    ";
    if (c < 32 || c == 126) return "";
    if (c > 126) return "?";
    return get_corresponding_char(num);
}

module.exports = {
    char_to_num: char_to_num,
    get_should_print: get_should_print
};