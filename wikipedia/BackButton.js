const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BackButton = new Lang.Class({
    Name: 'BackButton',
    Extends: Gtk.Button,

    _init: function(props) {
        props = props || {};
        props.image = Gtk.Image.new_from_icon_name('go-previous-symbolic',
            Gtk.IconSize.BUTTON);
        props.always_show_image = true;
        // Don't do that. What should actually happen is the system-wide setting
        // that controls whether buttons show images should be changed.
        this.parent(props);
    }
});
