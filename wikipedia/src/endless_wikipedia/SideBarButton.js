const Lang = imports.lang;
const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gtk = imports.gi.Gtk;

const SideBarButton = new Lang.Class({
    Name: 'EndlessSideBarButton',
    Extends: Gtk.Button,

    // This is a button for the article list widget. It has a label and an icon image.
    // The icon image will only appear on hover or press of button
    _init: function(hover_icon_path, params) {
        this.parent(params);

        this.set_size_request(40, -1);

        this._image = new Gtk.Image({
            resource: hover_icon_path,
            no_show_all: true
        });

        this.add(this._image);

        this.connect('state-changed', Lang.bind(this, this._update_appearance));
    },

    _update_appearance: function(widget, state) {
        // If button is hovered over and/or pressed, then show the arrow icon
        if (widget.get_state_flags() & Gtk.StateFlags.ACTIVE || 
            widget.get_state_flags() & Gtk.StateFlags.PRELIGHT) {
            this._image.show();
            return false; // don't block event
        }
        // If no hover or press, then hide the arrow icon
        this._image.hide();
        return false; // don't block event
    }
});

