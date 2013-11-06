const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const AssetButton = new Lang.Class({
    Name: 'AssetButton',
    Extends: Gtk.Button,
    Properties: {
        'normal-image-uri': GObject.ParamSpec.string('normal-image-uri', 'Normal Image URI', 'Normal Image URI',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'active-image-uri': GObject.ParamSpec.string('active-image-uri', 'Active Image URI', 'Active Image URI',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'prelight-image-uri': GObject.ParamSpec.string('prelight-image-uri', 'Prelight Image URI', 'Prelight Image URI',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'insensitive-image-uri': GObject.ParamSpec.string('disabled-image-uri', 'Disabled Image URI', 'Disabled Image URI',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'focused-image-uri': GObject.ParamSpec.string('focused-image-uri', 'Focused Image URI', 'Focused Image URI',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(params) {
        this._normal_pixbuf = null;
        this._prelight_pixbuf = null;
        this._active_pixbuf = null;
        this._insensitive_pixbuf = null;
        this._focused_pixbuf = null;
        this._label = null;

        this._normal_uri = null;
        this._prelight_uri = null;
        this._active_uri = null;
        this._insensitive_uri = null;
        this._focused_uri = null;

        this._image = new Gtk.Image();

        params.always_show_image = true;
        this.parent(params);

        this.set_image(this._image);
        this.connect('state-flags-changed', Lang.bind(this, this._update_appearance));
    },

    _update_appearance: function() {
        let flags = this.get_state_flags();

        // If the button is insensitive, show its disabled state
        if (this._insensitive_pixbuf !== null && (flags & Gtk.StateFlags.INSENSITIVE)) {
            this._image.set_from_pixbuf(this._insensitive_pixbuf);
            return;
        }
        // Otherwise, if it's prelit (moused-over),
        // show its highlight state
        if (this._active_pixbuf !== null && (flags & Gtk.StateFlags.ACTIVE)) {
            this._image.set_from_pixbuf(this._active_pixbuf);
            return;
        }
        // Otherwise, if it's active (clicked),
        // show its active state
        if (this._prelight_pixbuf !== null && (flags & Gtk.StateFlags.PRELIGHT)) {
            this._image.set_from_pixbuf(this._prelight_pixbuf);
            return;
        }
        // Otherwise, if it's focused,
        // show its focused state
        if (this._focused_pixbuf !== null && (flags & Gtk.StateFlags.FOCUSED)) {
            this._image.set_from_pixbuf(this._focused_pixbuf);
            return;
        }
        // Finally, if neither of the above are true, show the button's normal
        // state by default
        this._image.set_from_pixbuf(this._normal_pixbuf);
    },

    /* Helper function to open and return an input stream into the specified
    URI. Make sure to close() the return value when you are done with it. */
    _open_stream_from_uri: function(uri) {
        let file = Gio.File.new_for_uri(uri);
        return file.read(null);
    },

    get normal_image_uri() {
        return this._normal_uri;
    },

    get prelight_image_uri() {
        return this._prelight_uri;
    },

    get active_image_uri() {
        return this._active_uri;
    },

    get insensitive_image_uri() {
        return this._insensitive_uri;
    },

    get focused_image_uri() {
        return this._focused_uri;
    },

    set normal_image_uri(v) {
        this._set_image_uri(v, 'normal');
    },

    set prelight_image_uri(v) {
        this._set_image_uri(v, 'prelight');
    },

    set active_image_uri(v) {
        this._set_image_uri(v, 'active');
    },

    set insensitive_image_uri(v) {
        this._set_image_uri(v, 'insensitive');
    },

    set focused_image_uri(v) {
        this._set_image_uri(v, 'focused');
    },

    /*
     * This helper method sets the image URI for the specified state.
     * Javascript's array property access allows us to avoid repeated code!
     */
    _set_image_uri: function(uri, state) {
        let uri_property = '_' + state + '_uri';
        let pixbuf_property = '_' + state + '_pixbuf';

        if(this[uri_property] === uri)
            return;
        let img_stream = null;
        try {
            img_stream = this._open_stream_from_uri(uri);
            this[pixbuf_property] = GdkPixbuf.Pixbuf.new_from_stream(img_stream, null);
            this[uri_property] = uri;
        } catch(e if e instanceof Gio.IOErrorEnum) {
            // This catches the instances in which there is no URI for a particular state
            this[pixbuf_property] = null;
            this[uri_property] = null;
        }
        if (img_stream !== null)
            img_stream.close(null);
        this._update_appearance();
        this.notify(state + '-image-uri');
    }
});
