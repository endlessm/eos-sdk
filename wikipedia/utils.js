const Gio = imports.gi.Gio;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GLib = imports.gi.GLib;

const set_default = function(obj, key, val) {
    // Similar to python's dict.setdefault(key, val)
    if(obj[key] === undefined)
        obj[key] = val;
}

const default_param = function(variable, defaultValue) {
    // Emulate python's default parameter values
    if(variable === undefined)
        return defaultValue;
    else
        return variable;
}

const range = function(a, b, step) {
    // Emulate python's range function
    var A = [];
    if(typeof a == 'number'){
        if(typeof b == 'undefined'){
            b = a - 1;
            a = 0;
        }
        A[0] = a;
        step = step || 1;
        while(a + step < b){
            A[A.length] = a += step;
        }
    }
    return A;
}

/*
 * ...Taken from utils.js from the eos-weather branch...
 */
function load_file_from_resource(filename) {
    // Return the text stored in the file at filename
    var file = Gio.file_new_for_uri(filename);
    var fstream = file.read(null);
    var dstream = new Gio.DataInputStream({
        base_stream: fstream
    });
    var data = dstream.read_until("", null);
    fstream.close(null);
    return data[0];
}

function get_path_for_relative_path(relative_path){
    let file = Gio.file_new_for_path(relative_path);
    return file.get_path();
}

function write_contents_to_file(filename, content){
    let file = Gio.file_new_for_path(filename);
    file.replace_contents(content, null, false, 0, null);
    return file.get_uri();
}

function write_contents_to_temp_file(name, content){
    let file = Gio.file_new_tmp("XXXXXX" + name)[0];
    file.replace_contents(content, null, false, 0, null);
    return file.get_uri();
}

function make_temp_dir(name){
    return GLib.dir_make_tmp(name + "XXXXXX");
}

/*
 * ...Taken from utils.js from the eos-weather branch...
 */
function load_file(filename) {
    // Return the text stored in the file at filename
    var file = Gio.file_new_for_path(filename);
    var fstream = file.read(null);
    var dstream = new Gio.DataInputStream({
        base_stream: fstream
    });
    var data = dstream.read_until("", null);
    fstream.close(null);
    return data[0];
}

/*
 * Compares two lists *a* and *b* to see if they contain
 * the same contents, ignoring order.  Contents are defined
 * to be equal via JSON representation, unless they are
 * primitives, then String() casting is used.
 *
 * If passed non-list objects, this method has undefined
 * behavior. Algorithm is not exceptionally optimized and has
 * O(n^2) performance.
 *
 * Method treats elements that are logically equivalent but
 * not type-equivalent to be the same.  Thus the string of
 * "3" is considered to equal the integer 3 in these comparisons.
 *
 * Returns true if a and b contain the same elements in
 * any order.
 */
const set_equals = function (a, b) {
    if (typeof a != "object" || typeof b != "object") {
        return false;
    }
    if (a.length != b.length) {
        // Optimization
        return false;
    }
    let matched_of_a = 0;
    for (let i = 0; i < a.length; i++) {
        let i_string = typeof a[i] == "object" ? JSON.stringify (a[i]) : String(a[i]);
        for (let j = 0; j < b.length; j++) {
            let j_string = typeof b[j] == "object" ? JSON.stringify (b[j]) : String(b[j]);
            if (j_string == i_string) {
                matched_of_a++;
            }
        }
    }
    let matched_of_b = 0;
    for (let i = 0; i < b.length; i++) {
        let i_string = typeof b[i] == "object" ? JSON.stringify (b[i]) : String(b[i]);
        for (let j = 0; j < a.length; j++) {
            let j_string = typeof a[j] == "object" ? JSON.stringify (a[j]) : String(a[j]);
            if (j_string == i_string) {
                matched_of_b++;
            }
        }
    }
    if (matched_of_a == a.length &&
        matched_of_b == matched_of_a)
    {
        return true;
    }
    return false;
};

/*
 * Checks to see if an array *arr* contains on element *obj*.  If
 * *same_type* is provided (and is true), the objects must
 * actually be the same type as well to be considered part of the
 * array.
 *
 * Somehow reminds the user of a pirate.
 */
const array_contains = function (arr, obj, same_type) {
    if (same_type == true) {
        for (let i = 0; i < arr.length; i++) {
            if (arr[i] === obj) {
              return true;
            }
        }
  } else {
        for (let i = 0; i < arr.length; i++) {
            if (arr[i] == obj) {
                return true;
            }
        }
  }
  return false;
};

/*
 * Loads a pixbuf sized to cover the dest_width and dest_height with the
 * image in res_path, while mataining the aspect ratio of the image
 */
function load_pixbuf_cover(res_path, dest_width, dest_height) {
    let [load_width, load_height] = [dest_width, dest_height];
    // TODO: We need to get the size of the source image, so right now we
    // are loading the image twice, once to get the size, and the again at
    // the proper size. We should eventually use a GdkPixbuf.Loader and
    // connect to the size-prepared signal, as described in the
    // documentation
    let temp_pixbuf = GdkPixbuf.Pixbuf.new_from_resource(res_path);
    let source_aspect = temp_pixbuf.width / temp_pixbuf.height;
    let dest_aspect = dest_width / dest_height;
    if(dest_aspect > source_aspect)
        load_height = -1;
    else
        load_width = -1;
    let source_pixbuf = GdkPixbuf.Pixbuf.new_from_resource_at_scale(res_path,
        load_width, load_height, true);
    let cropped_pixbuf = source_pixbuf;
    if(dest_width < source_pixbuf.width || dest_height < source_pixbuf.height)
        cropped_pixbuf = source_pixbuf.new_subpixbuf(0, 0, dest_width, dest_height);
    return cropped_pixbuf;
}

// Convenience function to convert a resource URI to a resource path, for
// APIs that expect a path rather than an URI
function resourceUriToPath(uri) {
    if(uri.startsWith('resource://'))
        return uri.slice('resource://'.length);
    throw new Error('Resource URI did not start with "resource://"');
}
