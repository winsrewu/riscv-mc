function char_to_num(char) {
    return char.charAt(0).charCodeAt();
}

function get_corresponding_char(num) {
    return String.fromCharCode(num);
}

function get_should_print(num) {
    let c = num;
    if (c == 10) return "\\n";
    if (c == 9) return "    ";
    if (c < 32 || c == 126) return "";
    if (c == 34) return "\\\"";
    if (c == 39) return "\\'";
    if (c == 92) return "\\\\";
    if (c > 126) return "?";
    return get_corresponding_char(num);
}

function is_printable(num) {
    return (num >= 32 && num <= 126) || num == 10 || num == 9;
}

module.exports = {
    char_to_num: char_to_num,
    get_should_print: get_should_print,
    is_printable: is_printable
};