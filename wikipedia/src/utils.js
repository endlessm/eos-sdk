const Gio = imports.gi.Gio;
const Clutter = imports.gi.Clutter;
const Cogl = imports.gi.Cogl;
const GdkPixbuf = imports.gi.GdkPixbuf;

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

// Convenience function to parse a clutter color from a string
function parse_clutter_color(color_string) {
    let [res, color] = Clutter.Color.from_string(color_string);
    return color;
}

// Convenience function to load a gresource image into a Clutter.Image
function load_clutter_image_from_resource(resource_path) {
    let pixbuf = GdkPixbuf.Pixbuf.new_from_resource(resource_path);
    let image = new Clutter.Image();
    if (pixbuf != null) {
        image.set_data(pixbuf.get_pixels(),
                       pixbuf.get_has_alpha()
                           ? Cogl.PixelFormat.RGBA_8888
                           : Cogl.PixelFormat.RGB_888,
                       pixbuf.get_width(),
                       pixbuf.get_height(),
                       pixbuf.get_rowstride());
    }
    return image;
}

// Private function to format a clutter actors allocation and print it
function _clutter_allocation_printer(actor, box, flag) {
    print("Allocation for", actor);
    print(" Xs:", box.x1, box.x2);
    print(" Ys:", box.y1, box.y2);
}

// Call this function on a clutter actor to have it log it's allocation to
// console
function print_clutter_actor_allocation(actor) {
    actor.connect('allocation-changed', _clutter_allocation_printer);
}

// Convenience function to convert a resource URI to a resource path, for
// APIs that expect a path rather than an URI
function resourceUriToPath(uri) {
    if(uri.startsWith('resource://'))
        return uri.slice('resource://'.length);
    throw new Error('Resource URI did not start with "resource://"');
}
