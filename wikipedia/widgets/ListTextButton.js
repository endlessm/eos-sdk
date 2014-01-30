const Lang = imports.lang;
const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gtk = imports.gi.Gtk;
const Pango = imports.gi.Pango;

// This is an approximate number of characters that will keep the label from
// going over its specified width
const ARTICLE_LABEL_MAX_WIDTH_CHARS = 20;

const ListTextButton = new Lang.Class({
    Name: 'EndlessListTextButton',
    Extends: Gtk.Button,

    // This is a button for the article list widget. It has a label and an icon image.
    // The icon image will only appear on hover or press of button
    _init: function(hover_icon_path, label_text, params) {
        params.hexpand = true;
        this.parent(params);

        this._hover_icon_pixbuf = GdkPixbuf.Pixbuf.new_from_resource(hover_icon_path);

        this._image = new Gtk.Image({
            opacity: 0,
        });
        this._image.set_from_pixbuf(this._hover_icon_pixbuf);

        this._box = new Gtk.Box({
            orientation: Gtk.Orientation.HORIZONTAL
        });

        this._label = new Gtk.Label({
            label: label_text,
            max_width_chars: ARTICLE_LABEL_MAX_WIDTH_CHARS,
            ellipsize: Pango.EllipsizeMode.END
        });

        this._box.pack_start(this._label, false, false, 0);
        this._box.pack_end(this._image, false, false, 0);

        this.add(this._box);
        this.connect('state-changed', Lang.bind(this, this._update_appearance));
        this.show_all();
    },

    _update_appearance: function(widget, state) {
        // If button is hovered over and/or pressed, then show the arrow icon
        if (widget.get_state_flags() & Gtk.StateFlags.ACTIVE || 
            widget.get_state_flags() & Gtk.StateFlags.PRELIGHT) {
            this._image.set_opacity(1);
            return false;
        }
        // If no hover or press, then hide the arrow icon
        this._image.set_opacity(0);
        return false;
    }
});

