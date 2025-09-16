// "sth:tmp@test.a.b" => "sth:tmp test.a"
// "sth:tmp@test" => "sth:tmp"
function as_storage(str) {
    let parts = str.split("@");

    let res = parts[0] + " ";

    let paths = parts[1].split(".");

    if (paths.length > 1) {
        for (let p of paths.slice(0, -1)) {
            res += p + ".";
        }
    }

    return res.slice(0, -1);
}

// "sth:tmp@test.a.b" => "b"
// "sth:tmp@test" => "test"
function as_name(str) {
    let parts = str.split("@");

    let paths = parts[1].split(".");

    return paths[paths.length - 1];
}

function generate_to_str_format(len) {
    let res = "";
    for (let i = 0; i < len; i++) {
        res += "$(" + i + ")";
    }
    return res;
}

module.exports = {
    as_storage: as_storage,
    as_name: as_name,
    generate_to_str_format: generate_to_str_format
};