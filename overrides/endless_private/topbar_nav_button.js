const Gdk = imports.gi.Gdk;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

var TopbarNavButton = new Lang.Class({
    Name: 'TopbarNavButton',
    GTypeName: 'EosTopbarNavButton',
    Extends: Gtk.Box,
    Properties: {
        'back-button': GObject.ParamSpec.object('back-button', 'Back button',
            'Topbar back button widget',
            GObject.ParamFlags.READABLE,
            Gtk.Button.$gtype),
        'forward-button': GObject.ParamSpec.object('forward-button',
            'Forward button', 'Topbar forward button widget',
            GObject.ParamFlags.READABLE,
            Gtk.Button.$gtype)
    },

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        this.parent(props);

        this._back_button = Gtk.Button.new_from_icon_name('go-previous-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);
        this._forward_button = Gtk.Button.new_from_icon_name('go-next-symbolic',
            Gtk.IconSize.SMALL_TOOLBAR);

        this.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);

        let is_rtl = (Gtk.Widget.get_default_direction() === Gtk.TextDirection.RTL);
        if (is_rtl) {
            this._back_button.get_style_context().add_class('rtl');
            this._forward_button.get_style_context().add_class('rtl');
        }

        this.add(this._back_button);
        this.add(this._forward_button);
        this._back_button.show();
        this._forward_button.show();
    },

    get back_button() {
        return this._back_button;
    },

    get forward_button() {
        return this._forward_button;
    }
});
